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
#include "pagetimbre.h"
#include "dialogbuttonbox.h"
#include "contextmanager.h"
#include "sampleutils.h"
#include "synth.h"
#include "soundfontmanager.h"
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QPointer>
#include <QMutexLocker>
#include <cmath>

TimbreCorrectionParameters DialogTimbreCorrection::s_lastParameters;
bool DialogTimbreCorrection::s_lastParametersSet = false;

static void scaleData(QList<TimbreCorrectedSample> &samples, double factor)
{
    if (std::fabs(factor - 1) < 1e-6)
        return;
    for (int i = 0; i < samples.count(); i++)
    {
        float * data = samples[i].data.data();
        for (int j = 0; j < samples[i].data.size(); j++)
            data[j] = static_cast<float>(data[j] * factor);
    }
}

DialogTimbreCorrection::DialogTimbreCorrection(TimbreModel * model, const QList<int> &targets,
                                               const TimbreSettings &settings, QWidget * parent) : QDialog(parent),
    _model(model),
    _targets(targets),
    _settings(settings),
    _synth(ContextManager::audio()->getSynth()),
    _generation(0),
    _latestGeneration(new QAtomicInt(0)),
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

    QHBoxLayout * layout = new QHBoxLayout(this);
    QVBoxLayout * left = new QVBoxLayout();
    QFormLayout * form = new QFormLayout();
    left->addLayout(form);

    // Target: the reference, with the choice of the view preselected, or another root key played at the key (the root
    // keys to correct excepted, the closest one being proposed first)
    _comboReference = new QComboBox(this);
    PageTimbre::addReferenceItems(_comboReference);
    _comboAligned = new QComboBox(this);
    int closest = -1;
    foreach (int key, _model->getPositions().keys())
    {
        if (_targets.contains(key))
            continue;
        _comboAligned->addItem(PageTimbre::keyName(key), key);
        if (closest == -1 || std::abs(key - _targets.first()) < std::abs(closest - _targets.first()))
            closest = key;
    }
    if (_comboAligned->count() > 0)
        _comboReference->addItem(tr("Aligned on a root key"), -1);
    int index = _comboAligned->findData(parameters.alignedPosition);
    _comboAligned->setCurrentIndex(index >= 0 ? index : _comboAligned->findData(closest));
    _comboReference->setCurrentIndex(_comboReference->findData(
        _settings.reference == TimbreSettings::REFERENCE_REGRESSION ? qBound(2, _settings.regressionCount, 8) : 0));
    form->addRow(tr("Reference"), _comboReference);
    form->addRow("", _comboAligned);

    // Strength, the deviations within the dead zone being left
    _spinStrength = new QSpinBox(this);
    _spinStrength->setRange(10, 100);
    _spinStrength->setSingleStep(10);
    _spinStrength->setSuffix(" %");
    _spinStrength->setValue(qBound(10, qRound(100 * parameters.strength), 100));
    _spinStrength->setToolTip(tr("Part of the deviation of each row corrected beyond %1 (a third of the tolerance)")
                              .arg(PageTimbre::formatDb(_settings.deadZone())));
    form->addRow(tr("Strength"), _spinStrength);

    // Rows corrected: all of them, or up to one of them
    _comboRanks = new QComboBox(this);
    _comboRanks->addItem(tr("All"), 0);
    const QList<TimbreRow> &rows = _model->getRows();
    for (int row = 0; row < rows.count() - 1; row++)
        _comboRanks->addItem(tr("Up to %1").arg(rows[row].label), rows[row].lastRank);
    index = _comboRanks->findData(parameters.maxRank);
    _comboRanks->setCurrentIndex(index >= 0 ? index : 0);
    form->addRow(tr("Ranks corrected"), _comboRanks);

    _checkLevel = new QCheckBox(tr("Correct the level (attenuation)"), this);
    _checkLevel->setChecked(parameters.correctLevel);
    _checkLevel->setToolTip(tr("Otherwise the level heard is kept: the correction of the timbre doesn't change it"));
    form->addRow(_checkLevel);

    // Comparison
    QGroupBox * groupComparison = new QGroupBox(tr("Listening"), this);
    QVBoxLayout * comparisonLayout = new QVBoxLayout(groupComparison);
    _comboPreviewed = new QComboBox(groupComparison);
    foreach (int key, _targets)
        _comboPreviewed->addItem(tr("Root key %1").arg(PageTimbre::keyName(key)), key);
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
    foreach (QComboBox * combo, QList<QComboBox *>() << _comboReference << _comboAligned << _comboRanks)
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview()));
    connect(_comboPreviewed, SIGNAL(currentIndexChanged(int)), this, SLOT(onPreviewedChanged()));
    connect(_spinStrength, SIGNAL(valueChanged(int)), this, SLOT(updatePreview()));
    connect(_checkLevel, SIGNAL(toggled(bool)), this, SLOT(updatePreview()));
    connect(_radioCorrected, SIGNAL(toggled(bool)), this, SLOT(applyComparison()));
    connect(_buttonPlay, SIGNAL(toggled(bool)), this, SLOT(onPlayToggled(bool)));
    updatePreview();
}

DialogTimbreCorrection::~DialogTimbreCorrection()
{
    stopAudioPreview();
    _pool.waitForDone();
}

TimbreCorrectionParameters DialogTimbreCorrection::getParameters()
{
    // Reference: interpolation (0), regression on this number of neighbors on each side, or aligned on a root key (-1)
    TimbreCorrectionParameters parameters;
    int count = _comboReference->currentData().toInt();
    parameters.reference = count > 0 ? TimbreSettings::REFERENCE_REGRESSION : TimbreSettings::REFERENCE_INTERPOLATION;
    parameters.regressionCount = count > 0 ? count : _settings.regressionCount;
    parameters.alignedPosition = count < 0 ? _comboAligned->currentData().toInt() : -1;
    parameters.correctLevel = _checkLevel->isChecked();
    parameters.deadZone = _settings.deadZone();
    parameters.strength = 0.01 * _spinStrength->value();
    parameters.maxRank = _comboRanks->currentData().toInt();
    return parameters;
}

void DialogTimbreCorrection::done(int result)
{
    if (result == QDialog::Accepted)
    {
        s_lastParameters = getParameters();
        s_lastParametersSet = true;
    }
    stopAudioPreview();
    QDialog::done(result);
}

int DialogTimbreCorrection::getPreviewedKey()
{
    return _targets.count() > 1 ? _comboPreviewed->currentData().toInt() : _targets.first();
}

void DialogTimbreCorrection::updatePreview()
{
    _comboAligned->setVisible(_comboReference->currentData().toInt() < 0);

    // Plan of the root key previewed, same colors as in the view: heard in bars, target in markers, computed result in
    // outline
    int key = getPreviewedKey();
    _plan = TimbreActions::planCorrection(*_model, key, getParameters());
    TimbrePalette palette(TimbrePalette::SURFACE_WINDOW);
    QList<TimbreBarsWidget::Series> series;
    if (_model->getPositions().contains(key))
        series << TimbreBarsWidget::Series { tr("Heard"), _model->getRowLevels(_model->getPositions()[key].spectrum),
                                             palette.highlight(), TimbreBarsWidget::STYLE_BARS };
    if (!_plan.target.isEmpty())
        series << TimbreBarsWidget::Series { tr("Target"), _model->getRowLevels(_plan.target), palette.text(),
                                             TimbreBarsWidget::STYLE_MARKERS };
    if (!_plan.after.isEmpty())
        series << TimbreBarsWidget::Series { tr("Result"), _model->getRowLevels(_plan.after), palette.breakColor(),
                                             TimbreBarsWidget::STYLE_OUTLINE };
    _bars->setData(_model->getRows(), series, TimbreSettings::DYNAMIC_RANGE);

    // Levels with their sign, as in the view
    QStringList lines;
    if (_plan.before.valid)
    {
        lines << tr("Before: level %1, timbre %2").arg(PageTimbre::formatDb(_plan.before.levelDifference, 1, true),
                                                      PageTimbre::formatDb(_plan.before.timbreDifference));
        if (_plan.expected.valid)
            lines << tr("Expected: level %1, timbre %2").arg(PageTimbre::formatDb(_plan.expected.levelDifference, 1, true),
                                                             PageTimbre::formatDb(_plan.expected.timbreDifference));
        if (_plan.levelGain != 0)
            lines << tr("Level change through the attenuation: %1").arg(PageTimbre::formatDb(_plan.levelGain, 1, true));
        if (!_plan.valid)
            lines << tr("Nothing to correct with these parameters.");
    }
    else
        lines << tr("No reference: nothing can be computed.");
    if (_targets.count() > 1)
        lines << tr("The parameters will be applied to %n root keys, from the worst to the least bad.", "",
                    static_cast<int>(_targets.count()));
    _labelSummary->setText(lines.join("<br/>"));

    // Sounds A and B computed once the parameters stop changing
    _previewTimer.start();
}

void DialogTimbreCorrection::onPreviewedChanged()
{
    // Data of the previous root key back, then the note played again at the new one
    resetAudioPreview();
    updatePreview();
    if (_buttonPlay->isChecked())
        onPlayToggled(true);
}

void DialogTimbreCorrection::onPlayToggled(bool isOn)
{
    if (_playedKey >= 0)
    {
        ContextManager::midi()->processKeyOff(-1, _playedKey);
        _playedKey = -1;
    }
    if (isOn)
    {
        // Played where the root key is heard: the closest key covered by its divisions for an extension
        int key = getPreviewedKey();
        if (_model->getPositions().contains(key) && _model->getPositions()[key].anchor >= 0)
            key = _model->getPositions()[key].anchor;
        _playedKey = key;
        ContextManager::midi()->processKeyOn(-1, key, _settings.velocity);
    }
}

void DialogTimbreCorrection::computeAudioPreview()
{
    _previewTimer.stop();
    int generation = ++_generation;
    _latestGeneration->storeRelaxed(generation);
    _pool.clear();
    TimbreCorrectionPlan plan = _plan;
    if (!plan.valid)
    {
        _previewOriginal.clear();
        _previewCorrected.clear();
        applyComparison();
        return;
    }

    // Data computed in another thread, the level change being included in the corrected data
    int indexSf2 = _model->getInstrument().indexSf2;
    QPointer<DialogTimbreCorrection> dialog(this);
    QSharedPointer<QAtomicInt> latest = _latestGeneration;
    _pool.start(QRunnable::create([dialog, latest, generation, plan, indexSf2]() {
        if (latest->loadRelaxed() != generation)
            return;
        double compensation = 0;
        QList<TimbreCorrectedSample> corrected = TimbreActions::computeCorrection(indexSf2, plan, compensation);
        QList<TimbreCorrectedSample> original;
        SoundfontManager * sm = SoundfontManager::getInstance();
        foreach (int sampleIndex, plan.samples)
        {
            TimbreCorrectedSample sample;
            sample.sampleIndex = sampleIndex;
            {
                QMutexLocker locker(sm->getMutex());
                sample.data = sm->getDataFloat(EltID(elementSmpl, indexSf2, sampleIndex));
            }
            original << sample;
        }
        if (corrected.isEmpty())
            corrected = original; // Level only

        // The data of the synth cannot exceed the full scale, unlike the attenuation applied afterwards: if the
        // corrected sound would, both sounds are lowered by the same amount so that the comparison stays fair
        double gain = std::pow(10.0, (plan.levelGain + compensation) / 20);
        double peak = 0;
        foreach (TimbreCorrectedSample sample, corrected)
            peak = qMax(peak, gain * SampleUtils::peak(sample.data.constData(), static_cast<quint32>(sample.data.size())));
        double headroom = qMax(1.0, peak);
        scaleData(corrected, gain / headroom);
        if (headroom > 1.0 + 1e-6)
            scaleData(original, 1.0 / headroom);
        else
            original.clear(); // Not replaced
        if (latest->loadRelaxed() != generation)
            return;
        QMetaObject::invokeMethod(qApp, [dialog, generation, original, corrected]() {
            if (!dialog.isNull())
                dialog->setAudioPreview(generation, original, corrected);
        }, Qt::QueuedConnection);
    }));
}

void DialogTimbreCorrection::setAudioPreview(int generation, const QList<TimbreCorrectedSample> &original,
                                             const QList<TimbreCorrectedSample> &corrected)
{
    if (generation != _generation)
        return;
    _previewOriginal = original;
    _previewCorrected = corrected;
    applyComparison();
}

void DialogTimbreCorrection::applyComparison()
{
    // B: the corrected data, A: the original data (replaced only if lowered with B)
    clearAudioPreview();
    int indexSf2 = _model->getInstrument().indexSf2;
    foreach (TimbreCorrectedSample sample, _radioCorrected->isChecked() ? _previewCorrected : _previewOriginal)
    {
        qint16 * data16;
        quint8 * data24;
        SampleUtils::floatToInt24(sample.data, data16, data24);
        EltID id(elementSmpl, indexSf2, sample.sampleIndex);
        _synth->setSampleData(id, data16, data24, static_cast<quint32>(sample.data.size()), true);
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

void DialogTimbreCorrection::stopAudioPreview()
{
    // The note stops, then the sounds are reset
    if (_buttonPlay->isChecked())
        _buttonPlay->setChecked(false);
    _previewTimer.stop();
    resetAudioPreview();
}

void DialogTimbreCorrection::resetAudioPreview()
{
    // The computations in progress are ignored and the synth plays the original data again
    _latestGeneration->storeRelaxed(++_generation);
    _pool.clear();
    _previewOriginal.clear();
    _previewCorrected.clear();
    clearAudioPreview();
}
