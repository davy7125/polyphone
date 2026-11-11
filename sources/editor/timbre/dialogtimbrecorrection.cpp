/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/


#include "dialogtimbrecorrection.h"
#include "timbrebarswidget.h"
#include "dialogbuttonbox.h"
#include "contextmanager.h"
#include "sampleutils.h"
#include "synth.h"
#include "soundfontmanager.h"
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QPointer>
#include <cmath>

TimbreCorrectionParameters DialogTimbreCorrection::s_lastParameters;
bool DialogTimbreCorrection::s_lastParametersSet = false;

DialogTimbreCorrection::DialogTimbreCorrection(TimbreModel * model, const QList<int> &targets,
                                               const TimbreSettings &settings, QWidget * parent) : QDialog(parent),
    _model(model),
    _targets(targets),
    _settings(settings),
    _synth(ContextManager::audio()->getSynth()),
    _generation(0),
    _playedKey(-1)
{
    this->setWindowTitle(tr("Correct the timbre"));
    this->setWindowFlags((this->windowFlags() | Qt::Tool) & ~Qt::WindowContextHelpButtonHint);
    _pool.setMaxThreadCount(1);
    _previewTimer.setSingleShot(true);
    _previewTimer.setInterval(300);
    connect(&_previewTimer, SIGNAL(timeout()), this, SLOT(computeAudioPreview()));

    TimbreCorrectionParameters parameters;
    if (s_lastParametersSet)
        parameters = s_lastParameters;
    KeyNameManager * keyNames = ContextManager::keyName();

    QHBoxLayout * layout = new QHBoxLayout(this);
    QVBoxLayout * left = new QVBoxLayout();
    QFormLayout * form = new QFormLayout();
    left->addLayout(form);

    _comboReference = new QComboBox(this);
    _comboReference->addItem(tr("Interpolation between the neighbors"));
    _comboReference->addItem(tr("Local regression"));
    _comboReference->addItem(tr("Aligned on a root key"));
    _comboReference->setCurrentIndex(parameters.reference);
    form->addRow(tr("Reference"), _comboReference);
    _spinRegression = new QSpinBox(this);
    _spinRegression->setRange(2, 8);
    _spinRegression->setValue(parameters.regressionCount);
    _spinRegression->setSuffix(" " + tr("neighbors on each side"));
    form->addRow("", _spinRegression);
    _comboAligned = new QComboBox(this);
    foreach (int key, _model->getPositions().keys())
        _comboAligned->addItem(keyNames->getKeyName(static_cast<unsigned int>(key)), key);
    int aligned = _model->getPositions().keys().indexOf(parameters.alignedPosition);
    _comboAligned->setCurrentIndex(aligned >= 0 ? aligned : 0);
    form->addRow("", _comboAligned);

    _checkTimbre = new QCheckBox(tr("Correct the timbre (sample data)"), this);
    _checkTimbre->setChecked(parameters.correctTimbre);
    form->addRow(_checkTimbre);
    _checkLevel = new QCheckBox(tr("Correct the level (attenuation)"), this);
    _checkLevel->setChecked(parameters.correctLevel);
    form->addRow(_checkLevel);

    auto spin = [&](double minimum, double maximum, double value, int decimals, QString suffix) -> QDoubleSpinBox * {
        QDoubleSpinBox * box = new QDoubleSpinBox(this);
        box->setRange(minimum, maximum);
        box->setDecimals(decimals);
        box->setValue(value);
        box->setSuffix(suffix);
        return box;
    };
    QString db = " " + tr("dB", "unit for decibels");
    _spinTolerance = spin(0, 20, parameters.tolerance, 1, db);
    _spinTolerance->setToolTip(tr("A smaller deviation of a row is not corrected, only the part beyond is"));
    form->addRow(tr("Tolerance"), _spinTolerance);
    _spinStrength = spin(0, 100, 100 * parameters.strength, 0, " %");
    form->addRow(tr("Strength"), _spinStrength);
    _spinMaximum = spin(0, 24, parameters.maxCorrection, 1, db);
    form->addRow(tr("Maximum correction"), _spinMaximum);
    _comboRanks = new QComboBox(this);
    _comboRanks->addItem(tr("Ranks and bands"));
    _comboRanks->addItem(tr("Ranks from 1 to"));
    _comboRanks->setCurrentIndex(parameters.maxRank > 0 ? 1 : 0);
    form->addRow(tr("Ranks corrected"), _comboRanks);
    _spinRanks = new QSpinBox(this);
    _spinRanks->setRange(1, 64);
    _spinRanks->setValue(parameters.maxRank > 0 ? parameters.maxRank : settings.detailedRanks);
    form->addRow("", _spinRanks);
    _spinBrightness = spin(-6, 6, parameters.brightness, 1, " " + tr("dB / octave"));
    _spinBrightness->setToolTip(tr("Slope added to the target: positive for a brighter sound"));
    form->addRow(tr("Brightness"), _spinBrightness);
    _spinOddEven = spin(-12, 12, parameters.oddEven, 1, db);
    _spinOddEven->setToolTip(tr("Level added to the odd ranks (3, 5, 7...) of the target"));
    form->addRow(tr("Odd ranks"), _spinOddEven);

    // Comparison
    QGroupBox * groupComparison = new QGroupBox(tr("Listening"), this);
    QVBoxLayout * comparisonLayout = new QVBoxLayout(groupComparison);
    _comboPreviewed = new QComboBox(groupComparison);
    foreach (int key, _targets)
        _comboPreviewed->addItem(tr("Root key %1").arg(keyNames->getKeyName(static_cast<unsigned int>(key))), key);
    _comboPreviewed->setVisible(_targets.count() > 1);
    comparisonLayout->addWidget(_comboPreviewed);
    _radioOriginal = new QRadioButton(tr("A: original"), groupComparison);
    _radioOriginal->setChecked(true);
    _radioCorrected = new QRadioButton(tr("B: corrected"), groupComparison);
    QHBoxLayout * radioLayout = new QHBoxLayout();
    radioLayout->addWidget(_radioOriginal);
    radioLayout->addWidget(_radioCorrected);
    comparisonLayout->addLayout(radioLayout);
    _buttonPlay = new QPushButton(tr("Play"), groupComparison);
    _buttonPlay->setCheckable(true);
    comparisonLayout->addWidget(_buttonPlay);
    left->addWidget(groupComparison);
    left->addStretch();
    layout->addLayout(left);

    // Levels
    QVBoxLayout * right = new QVBoxLayout();
    _bars = new TimbreBarsWidget(this);
    _bars->setMinimumSize(420, 240);
    right->addWidget(_bars, 1);
    _labelSummary = new QLabel(this);
    _labelSummary->setWordWrap(true);
    right->addWidget(_labelSummary);
    DialogButtonBox * buttonBox = new DialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    right->addWidget(buttonBox);
    layout->addLayout(right, 1);

    // Connections
    foreach (QComboBox * combo, QList<QComboBox *>() << _comboReference << _comboAligned << _comboRanks << _comboPreviewed)
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
    foreach (QDoubleSpinBox * box, QList<QDoubleSpinBox *>() << _spinTolerance << _spinStrength << _spinMaximum
             << _spinBrightness << _spinOddEven)
        connect(box, SIGNAL(valueChanged(double)), this, SLOT(updatePreview()));
    connect(_spinRegression, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(_spinRanks, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(_checkTimbre, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(_checkLevel, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(_radioCorrected, SIGNAL(toggled(bool)), this, SLOT(onComparisonChanged()));
    connect(_buttonPlay, SIGNAL(toggled(bool)), this, SLOT(onPlayToggled(bool)));
    updatePreview();
}

DialogTimbreCorrection::~DialogTimbreCorrection()
{
    clearAudioPreview();
    _pool.waitForDone();
}

TimbreCorrectionParameters DialogTimbreCorrection::getParameters()
{
    TimbreCorrectionParameters parameters;
    parameters.reference = static_cast<TimbreCorrectionParameters::Reference>(_comboReference->currentIndex());
    parameters.regressionCount = _spinRegression->value();
    parameters.alignedPosition = _comboAligned->currentData().toInt();
    parameters.correctTimbre = _checkTimbre->isChecked();
    parameters.correctLevel = _checkLevel->isChecked();
    parameters.tolerance = _spinTolerance->value();
    parameters.strength = _spinStrength->value() / 100;
    parameters.maxCorrection = _spinMaximum->value();
    parameters.maxRank = _comboRanks->currentIndex() == 1 ? _spinRanks->value() : 0;
    parameters.brightness = _spinBrightness->value();
    parameters.oddEven = _spinOddEven->value();
    return parameters;
}

void DialogTimbreCorrection::done(int result)
{
    if (result == QDialog::Accepted)
    {
        s_lastParameters = getParameters();
        s_lastParametersSet = true;
    }
    if (_buttonPlay->isChecked())
        _buttonPlay->setChecked(false);
    clearAudioPreview();
    QDialog::done(result);
}

void DialogTimbreCorrection::updatePreview()
{
    _spinRegression->setVisible(_comboReference->currentIndex() == TimbreCorrectionParameters::REFERENCE_REGRESSION);
    _comboAligned->setVisible(_comboReference->currentIndex() == TimbreCorrectionParameters::REFERENCE_POSITION);
    _spinRanks->setVisible(_comboRanks->currentIndex() == 1);

    // Plan of the root key previewed
    int key = _targets.count() > 1 ? _comboPreviewed->currentData().toInt() : _targets.first();
    _plan = TimbreActions::planCorrection(*_model, key, getParameters());
    TimbrePalette palette;
    QList<TimbreBarsWidget::Series> series;
    if (_model->getPositions().contains(key))
        series << TimbreBarsWidget::Series { tr("Heard"), _model->getRowLevels(_model->getPositions()[key].spectrum),
                                             palette.grid(), TimbreBarsWidget::STYLE_BARS };
    if (!_plan.target.isEmpty())
        series << TimbreBarsWidget::Series { tr("Target"), _model->getRowLevels(_plan.target), palette.text(),
                                             TimbreBarsWidget::STYLE_MARKERS };
    if (!_plan.after.isEmpty())
        series << TimbreBarsWidget::Series { tr("Result"), _model->getRowLevels(_plan.after), palette.highlight(),
                                             TimbreBarsWidget::STYLE_OUTLINE };
    _bars->setData(_model->getRows(), series, _settings.dynamicRange);

    QLocale locale;
    auto db = [&](double value) { return locale.toString(value, 'f', 1) + " " + tr("dB", "unit for decibels"); };
    QStringList lines;
    if (_plan.before.valid)
    {
        lines << tr("Before: level %1, timbre %2").arg(db(_plan.before.levelDifference), db(_plan.before.timbreDifference));
        if (_plan.expected.valid)
            lines << tr("Expected: level %1, timbre %2").arg(db(_plan.expected.levelDifference),
                                                             db(_plan.expected.timbreDifference));
        if (_plan.levelGain != 0)
            lines << tr("Level change through the attenuation: %1").arg(db(_plan.levelGain));
        if (!_plan.valid)
            lines << tr("Nothing to correct with these parameters.");
    }
    else
        lines << tr("No reference: nothing can be computed.");
    if (_targets.count() > 1)
        lines << tr("The parameters will be applied to %n root keys, from the worst to the least bad.", "",
                    static_cast<int>(_targets.count()));
    _labelSummary->setText(lines.join("<br/>"));

    if (_radioCorrected->isChecked())
        _previewTimer.start();
}

void DialogTimbreCorrection::onPlayToggled(bool isOn)
{
    int key = _targets.count() > 1 ? _comboPreviewed->currentData().toInt() : _targets.first();
    if (isOn)
    {
        _playedKey = key;
        ContextManager::midi()->processKeyOn(-1, key, _settings.velocity);
    }
    else if (_playedKey >= 0)
    {
        ContextManager::midi()->processKeyOff(-1, _playedKey);
        _playedKey = -1;
    }
}

void DialogTimbreCorrection::onComparisonChanged()
{
    if (_radioCorrected->isChecked())
        computeAudioPreview();
    else
        clearAudioPreview();
}

void DialogTimbreCorrection::computeAudioPreview()
{
    _previewTimer.stop();
    int generation = ++_generation;
    TimbreCorrectionPlan plan = _plan;
    if (!plan.valid)
    {
        clearAudioPreview();
        return;
    }

    // Data computed in another thread, the level change being included in the data
    int indexSf2 = _model->getInstrument().indexSf2;
    QPointer<DialogTimbreCorrection> dialog(this);
    _pool.start(QRunnable::create([dialog, generation, plan, indexSf2]() {
        double compensation = 0;
        QList<TimbreCorrectedSample> samples = TimbreActions::computeCorrection(indexSf2, plan, compensation);
        if (samples.isEmpty())
        {
            // Level only: the original data with the gain
            SoundfontManager * sm = SoundfontManager::getInstance();
            foreach (int sampleIndex, plan.samples)
            {
                TimbreCorrectedSample sample;
                sample.sampleIndex = sampleIndex;
                sample.data = sm->getDataFloat(EltID(elementSmpl, indexSf2, sampleIndex));
                samples << sample;
            }
        }
        double gain = std::pow(10.0, (plan.levelGain + compensation) / 20);
        QMetaObject::invokeMethod(qApp, [dialog, generation, samples, gain]() {
            if (!dialog.isNull())
                dialog->applyAudioPreview(generation, samples, gain);
        }, Qt::QueuedConnection);
    }));
}

void DialogTimbreCorrection::applyAudioPreview(int generation, const QList<TimbreCorrectedSample> &samples, double gain)
{
    if (generation != _generation || !_radioCorrected->isChecked())
        return;
    clearAudioPreview();
    int indexSf2 = _model->getInstrument().indexSf2;
    foreach (TimbreCorrectedSample sample, samples)
    {
        QVector<float> data = sample.data;
        if (std::fabs(gain - 1) > 1e-6)
            for (int i = 0; i < data.size(); i++)
                data[i] = qBound(-1.0f, static_cast<float>(data[i] * gain), 1.0f);
        qint16 * data16;
        quint8 * data24;
        SampleUtils::floatToInt24(data, data16, data24);
        EltID id(elementSmpl, indexSf2, sample.sampleIndex);
        _synth->setSampleData(id, data16, data24, static_cast<quint32>(data.size()), true);
        delete [] data16;
        delete [] data24;
        _previewIds << id;
    }
}

void DialogTimbreCorrection::clearAudioPreview()
{
    foreach (EltID id, _previewIds)
        _synth->setSampleData(id, nullptr, nullptr, 0, true);
    _previewIds.clear();
}
