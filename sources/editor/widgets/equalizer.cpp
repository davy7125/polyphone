/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "equalizer.h"
#include "ui_equalizer.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "synth.h"
#include <QMessageBox>
#include <QPointer>
#include <QApplication>
#include <QLocale>
#include <algorithm>

const double Equalizer::FREQUENCIES[10] = { 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000 };
const double Equalizer::RATIOS[10] = { 0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256 };

Equalizer::Equalizer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Equalizer),
    _synth(ContextManager::audio()->getSynth()),
    _initialization(false),
    _previewGeneration(0)
{
    ui->setupUi(this);

    // Style
    ui->pushEgaliser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushEgalRestore->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reset_eq.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    // Sliders and their labels by band
    _sliders << ui->verticalSlider_1 << ui->verticalSlider_2 << ui->verticalSlider_3 << ui->verticalSlider_4
             << ui->verticalSlider_5 << ui->verticalSlider_6 << ui->verticalSlider_7 << ui->verticalSlider_8
             << ui->verticalSlider_9 << ui->verticalSlider_10;
    _labels << ui->label_8 << ui->label_9 << ui->label_10 << ui->label_11 << ui->label_12 << ui->label_13
            << ui->label_14 << ui->label_15 << ui->label_16 << ui->label_17;

    // The preview is computed when the settings stop changing
    _previewTimer.setSingleShot(true);
    _previewTimer.setInterval(100);
    connect(&_previewTimer, SIGNAL(timeout()), this, SLOT(computePreview()));
    _previewThreadPool.setMaxThreadCount(1);
    ui->spinnerPreview->hide();

    // Initialize EQ
    loadEQ();
    updateBands();
}

Equalizer::~Equalizer()
{
    clearPreview();
    _previewThreadPool.waitForDone();
    delete ui;
}

void Equalizer::setCurrentIds(IdList ids)
{
    _currentIds = ids;
    updateSamples();
}

void Equalizer::updateSamples()
{
    updateBands();
    if (ui->checkEqualizerPreview->isChecked())
        schedulePreview();
}

void Equalizer::on_pushEgalRestore_clicked()
{
    _initialization = true;
    foreach (QSlider * slider, _sliders)
        slider->setValue(0);
    _initialization = false;
    saveEQ();
    updateSamples();
}

void Equalizer::on_pushEgaliser_clicked()
{
    if (_initialization)
        return;
    saveEQ();

    GainCurve curve = getCurve();
    if (curve.isNeutral())
        return;

    // Soundfont editing
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<EltID> ids = getProcessedIds();
    foreach (EltID id, ids)
    {
        QVector<float> data = sm->getDataFloat(id);
        data = curve.apply(data, sm->get(id, champ_dwSampleRate).dwValue,
                           GainCurve::getFundamental(sm->get(id, champ_byOriginalPitch).bValue,
                                                     sm->get(id, champ_chPitchCorrection).cValue),
                           sm->get(id, champ_dwStartLoop).dwValue, sm->get(id, champ_dwEndLoop).dwValue);
        sm->setData(id, data);
    }

    int processedSampleNumber = ids.count();
    if (processedSampleNumber == 0)
        return;

    sm->endEditing("equalizer");
    ui->checkEqualizerPreview->setChecked(false);
    QMessageBox::information(this, tr("Information"), processedSampleNumber == 1 ?
                                 tr("Equalization successfully applied on 1 sample.") :
                                 tr("Equalization successfully applied on %1 samples.").arg(processedSampleNumber));
}

void Equalizer::loadEQ()
{
    _initialization = true;
    for (int i = 0; i < _sliders.count(); i++)
        _sliders[i]->setValue(ContextManager::configuration()->getValue(
            ConfManager::SECTION_TOOLS, "sample_EQ", QString("%1").arg(i + 1, 2, 10, QChar('0')), 0).toInt());
    ui->checkNoteTracking->setChecked(ContextManager::configuration()->getValue(
        ConfManager::SECTION_TOOLS, "sample_EQ", "note_tracking", false).toBool());
    _initialization = false;
}

void Equalizer::saveEQ()
{
    for (int i = 0; i < _sliders.count(); i++)
        ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, "sample_EQ",
                                                  QString("%1").arg(i + 1, 2, 10, QChar('0')), _sliders[i]->value());
    ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, "sample_EQ", "note_tracking",
                                              ui->checkNoteTracking->isChecked());
}

void Equalizer::enableApply(bool isEnabled)
{
    ui->pushEgaliser->setEnabled(isEnabled);
}

GainCurve Equalizer::getCurve()
{
    // Gain in dB of each band, interpolated on a logarithmic scale of the frequencies
    bool noteTracking = ui->checkNoteTracking->isChecked();
    QList<QPointF> points;
    for (int i = 0; i < _sliders.count(); i++)
        points << QPointF(noteTracking ? RATIOS[i] : FREQUENCIES[i], _sliders[i]->value());
    return GainCurve(GainCurve::MODE_GAIN, noteTracking, points);
}

QList<EltID> Equalizer::getProcessedIds()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool stereoEditing = ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", true).toBool();
    QList<EltID> ids;
    foreach (EltID id, _currentIds.getSelectedIds(elementSmpl))
    {
        if (!sm->isValid(id))
            continue;
        if (!ids.contains(id))
            ids << id;

        // Linked sample (as PageSmpl::getRepercussionID)
        SFSampleLink link = sm->get(id, champ_sfSampleType).sfLinkValue;
        if (stereoEditing && link != monoSample && link != RomMonoSample)
        {
            EltID id2 = id;
            id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
            if (sm->isValid(id2) && !ids.contains(id2))
                ids << id2;
        }
    }
    return ids;
}

void Equalizer::updateBands()
{
    // Fundamental frequency and Nyquist frequency of the samples to process
    SoundfontManager * sm = SoundfontManager::getInstance();
    QList<double> fundamentals, nyquistFrequencies;
    foreach (EltID id, getProcessedIds())
    {
        fundamentals << GainCurve::getFundamental(sm->get(id, champ_byOriginalPitch).bValue,
                                                  sm->get(id, champ_chPitchCorrection).cValue);
        nyquistFrequencies << 0.5 * sm->get(id, champ_dwSampleRate).dwValue;
    }

    bool noteTracking = ui->checkNoteTracking->isChecked();
    QString hz = " " + tr("Hz", "unit for Herz");
    static const QStringList frequencyLabels = { "32", "64", "125", "250", "500", "1k", "2k", "4k", "8k", "16k" };
    const QChar times(0x00D7);
    for (int i = 0; i < _sliders.count(); i++)
    {
        QString tooltip;
        if (noteTracking)
        {
            _labels[i]->setText(times + (i == 0 ? QString(QChar(0x00BD)) : QString::number(RATIOS[i])));

            // Frequencies of the band for the samples to process
            tooltip = times + (i == 0 ? QString(QChar(0x00BD)) : QString::number(RATIOS[i]));
            if (!fundamentals.isEmpty())
            {
                double minFrequency = RATIOS[i] * *std::min_element(fundamentals.begin(), fundamentals.end());
                double maxFrequency = RATIOS[i] * *std::max_element(fundamentals.begin(), fundamentals.end());
                tooltip += ": " + QLocale::system().toString(minFrequency, 'f', 0);
                if (maxFrequency > 1.001 * minFrequency)
                    tooltip += QString::fromUtf8(" – ") + QLocale::system().toString(maxFrequency, 'f', 0);
                tooltip += hz;
            }
        }
        else
        {
            _labels[i]->setText(frequencyLabels[i]);
            tooltip = QString::number(FREQUENCIES[i]) + hz;
        }
        _sliders[i]->setToolTip(tooltip);

        // A band has no effect if the previous one is above the Nyquist frequency of every sample
        bool hasEffect = (i == 0 || fundamentals.isEmpty());
        for (int j = 0; j < fundamentals.count() && !hasEffect; j++)
            hasEffect = (noteTracking ? RATIOS[i - 1] * fundamentals[j] : FREQUENCIES[i - 1]) < nyquistFrequencies[j];
        _sliders[i]->setEnabled(hasEffect);
        _labels[i]->setEnabled(hasEffect);
    }
}

void Equalizer::on_checkEqualizerPreview_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    if (_initialization)
        return;

    if (ui->checkEqualizerPreview->isChecked())
        computePreview();
    else
        clearPreview();
}

void Equalizer::on_checkNoteTracking_toggled(bool checked)
{
    Q_UNUSED(checked)
    if (_initialization)
        return;

    saveEQ();
    updateSamples();
}

void Equalizer::on_verticalSliderMoved(int position)
{
    Q_UNUSED(position)
    if (_initialization)
        return;

    saveEQ();
    if (ui->checkEqualizerPreview->isChecked())
        schedulePreview();
}

void Equalizer::schedulePreview()
{
    // The spinner is displayed while the sound heard doesn't match the settings: a sample is going to play processed
    // data, or its own data again
    if (!_previewIds.isEmpty() || (_currentIds.getSelectedIds(elementSmpl).count() == 1 && !getCurve().isNeutral()))
        setPreviewPending(true);
    _previewTimer.start();
}

void Equalizer::setPreviewPending(bool isPending)
{
    if (isPending == !ui->spinnerPreview->isHidden())
        return;

    if (isPending)
    {
        ui->spinnerPreview->show();
        ui->spinnerPreview->startAnimation();
    }
    else
    {
        ui->spinnerPreview->hide();
        ui->spinnerPreview->stopAnimation();
    }
}

void Equalizer::computePreview()
{
    _previewTimer.stop();
    int generation = ++_previewGeneration;

    // Samples that can be played alone in the page: one selected sample, with its linked sample
    QList<EltID> ids;
    if (_currentIds.getSelectedIds(elementSmpl).count() == 1)
        ids = getProcessedIds();
    GainCurve curve = getCurve();
    if (curve.isNeutral())
        ids.clear();

    // The samples not concerned anymore play their own data again
    foreach (EltID id, _previewIds)
        if (!ids.contains(id))
            _synth->setSampleData(id, nullptr, nullptr, 0);
    _previewIds = ids;
    setPreviewPending(!ids.isEmpty());
    if (ids.isEmpty())
        return;

    // Data processed in another thread, as "apply" does
    struct Input
    {
        EltID id;
        QVector<float> data;
        double sampleRate, fundamental;
        quint32 loopStart, loopEnd;
    };
    QList<Input> inputs;
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (EltID id, ids)
    {
        Input input;
        input.id = id;
        input.data = sm->getDataFloat(id);
        input.sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
        input.fundamental = GainCurve::getFundamental(sm->get(id, champ_byOriginalPitch).bValue,
                                                      sm->get(id, champ_chPitchCorrection).cValue);
        input.loopStart = sm->get(id, champ_dwStartLoop).dwValue;
        input.loopEnd = sm->get(id, champ_dwEndLoop).dwValue;
        inputs << input;
    }

    QPointer<Equalizer> equalizer(this);
    _previewThreadPool.start(QRunnable::create([equalizer, generation, curve, inputs]() {
        QList<PreviewData> previewData;
        foreach (const Input &input, inputs)
        {
            PreviewData data;
            data.id = input.id;
            data.data = curve.apply(input.data, input.sampleRate, input.fundamental, input.loopStart, input.loopEnd);
            previewData << data;
        }

        // Back to the main thread
        QMetaObject::invokeMethod(qApp, [equalizer, generation, previewData]() {
            if (!equalizer.isNull())
                equalizer->applyPreview(generation, previewData);
        }, Qt::QueuedConnection);
    }));
}

void Equalizer::applyPreview(int generation, const QList<PreviewData> &previewData)
{
    // Only the last computation is used
    if (generation != _previewGeneration)
        return;

    foreach (const PreviewData &data, previewData)
    {
        if (!_previewIds.contains(data.id))
            continue;
        qint16 * data16;
        quint8 * data24;
        SampleUtils::floatToInt24(data.data, data16, data24);
        _synth->setSampleData(data.id, data16, data24, static_cast<quint32>(data.data.size()));
        delete [] data16;
        delete [] data24;
    }

    // Unless the settings changed again in the meantime, the sound heard is now up to date
    if (!_previewTimer.isActive())
        setPreviewPending(false);
}

void Equalizer::clearPreview()
{
    _previewTimer.stop();
    _previewGeneration++;
    foreach (EltID id, _previewIds)
        _synth->setSampleData(id, nullptr, nullptr, 0);
    _previewIds.clear();
    setPreviewPending(false);
}
