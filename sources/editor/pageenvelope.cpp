/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "pageenvelope.h"
#include "ui_pageenvelope.h"
#include "contextmanager.h"
#include <qmath.h>

PageEnvelope::PageEnvelope(QWidget *parent) :
    Page(parent, "page:env"),
    ui(new Ui::PageEnvelope),
    _isVolume(true)
{
    ui->setupUi(this);
    ui->graphicsView->linkSliderX(ui->sliderGraph);
    connect(ui->sliderGraph, SIGNAL(valueChanged(int)), ui->graphicsView, SLOT(setPosX(int)));

    QColor redColor = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    QColor greenColor = ContextManager::theme()->getFixedColor(ThemeManager::GREEN, ThemeManager::LIST_BACKGROUND);

    QImage imageG(32, 16, QImage::Format_ARGB32);
    QImage imageR(32, 16, QImage::Format_ARGB32);
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
            imageG.setPixelColor(i, j, greenColor);
            imageG.setPixelColor(16 + i, j, Qt::transparent);
            imageR.setPixelColor(i, j, redColor);
            imageR.setPixelColor(16 + i, j, Qt::transparent);
#else
            imageG.setPixel(i, j, greenColor.rgba());
            imageG.setPixel(16 + i, j, Qt::transparent);
            imageR.setPixel(i, j, redColor.rgba());
            imageR.setPixel(16 + i, j, Qt::transparent);
#endif
        }
    }
    ui->pushVolume->setIcon(QPixmap::fromImage(imageG));
    ui->pushModulation->setIcon(QPixmap::fromImage(imageR));

    QPixmap revertIcon = ContextManager::theme()->getColoredSvg(":/icons/edit-undo.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT);
    ui->pushAttack->setIcon(revertIcon);
    ui->pushDecay->setIcon(revertIcon);
    ui->pushDelay->setIcon(revertIcon);
    ui->pushHold->setIcon(revertIcon);
    ui->pushKeyDecay->setIcon(revertIcon);
    ui->pushKeyHold->setIcon(revertIcon);
    ui->pushRelease->setIcon(revertIcon);
    ui->pushSustain->setIcon(revertIcon);
}

PageEnvelope::~PageEnvelope()
{
    delete ui;
}

void PageEnvelope::updateInterface(QString editingSource)
{
    Q_UNUSED(editingSource)
    enableEditor(_currentIds.count() == 1);
    populate();
}

void PageEnvelope::on_pushVolume_clicked()
{
    if (!_isVolume)
    {
        _isVolume = true;
        stopSignals(true);
        ui->doubleSpinSustain->setSuffix(" " + tr("dB", "unit for decibels"));
        ui->doubleSpinSustain->setMaximum(144);
        stopSignals(false);
        populate();
    }
}

void PageEnvelope::on_pushModulation_clicked()
{
    if (_isVolume)
    {
        _isVolume = false;
        stopSignals(true);
        ui->doubleSpinSustain->setSuffix(tr("%", "percentage"));
        ui->doubleSpinSustain->setMaximum(100);
        stopSignals(false);
        populate();
    }
}

void PageEnvelope::populate()
{
    stopSignals(true);

    // Prepare fonts
    QFont font = this->font();
    QFont fontB = font;
    fontB.setBold(true);

    // Initialize the editor values
    if (_currentIds.count() == 1)
    {
        EltID id = _currentIds[0];
        bool isOverridden;

        // Delay
        ui->doubleSpinDelay->setValue(computeValue(id, _isVolume ? champ_delayVolEnv : champ_delayModEnv, isOverridden));
        ui->labelDelay->setFont(isOverridden ? fontB : font);
        ui->pushDelay->setEnabled(isOverridden);

        // Attack
        ui->doubleSpinAttack->setValue(computeValue(id, _isVolume ? champ_attackVolEnv : champ_attackModEnv, isOverridden));
        ui->labelAttack->setFont(isOverridden ? fontB : font);
        ui->pushAttack->setEnabled(isOverridden);

        // Hold
        ui->doubleSpinHold->setValue(computeValue(id, _isVolume ? champ_holdVolEnv : champ_holdModEnv, isOverridden));
        ui->labelHold->setFont(isOverridden ? fontB : font);
        ui->pushHold->setEnabled(isOverridden);

        // Decay
        ui->doubleSpinDecay->setValue(computeValue(id, _isVolume ? champ_decayVolEnv : champ_decayModEnv, isOverridden));
        ui->labelDecay->setFont(isOverridden ? fontB : font);
        ui->pushDecay->setEnabled(isOverridden);

        // Sustain
        ui->doubleSpinSustain->setValue(computeValue(id, _isVolume ? champ_sustainVolEnv : champ_sustainModEnv, isOverridden));
        ui->labelSustain->setFont(isOverridden ? fontB : font);
        ui->pushSustain->setEnabled(isOverridden);

        // Release
        ui->doubleSpinRelease->setValue(computeValue(id, _isVolume ? champ_releaseVolEnv : champ_releaseModEnv, isOverridden));
        ui->labelRelease->setFont(isOverridden ? fontB : font);
        ui->pushRelease->setEnabled(isOverridden);

        // Key -> Hold
        ui->spinKeyHold->setValue(computeValue(id, _isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold, isOverridden));
        ui->labelKeyHold->setFont(isOverridden ? fontB : font);
        ui->pushKeyHold->setEnabled(isOverridden);

        // Key -> Decay
        ui->spinKeyDecay->setValue(computeValue(id, _isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay, isOverridden));
        ui->labelKeyDecay->setFont(isOverridden ? fontB : font);
        ui->pushKeyDecay->setEnabled(isOverridden);
    }

    // Graphics
    ui->graphicsView->clearEnvelops();
    if (_isVolume)
    {
        for (int i = 0; i < _currentIds.count(); i++)
        {
            EltID id = _currentIds[i];

            // Volume envelop
            addEnvelop(id, true, true);
        }
    }
    else
    {
        for (int i = 0; i < _currentIds.count(); i++)
        {
            EltID id = _currentIds[i];

            // Corresponding volume envelop in secondary
            addEnvelop(id, true, false);

            // Modulation envelop
            addEnvelop(id, false, true);
        }
    }
    ui->graphicsView->computeEnvelops();

    // Sample
    if (_currentIds.count() == 1 && _currentIds[0].typeElement == elementInstSmpl)
        addSample(_currentIds[0]);

    ui->graphicsView->repaint();

    stopSignals(false);
}

double PageEnvelope::computeValue(EltID id, AttributeType champ, bool &isOverridden)
{
    if (_sf2->isSet(id, champ))
    {
        isOverridden = true;
        return Attribute::toRealValue(champ, false, _sf2->get(id, champ));
    }
    else
    {
        // Default value
        double value = Attribute::getDefaultRealValue(champ, false);
        if (id.typeElement == elementInstSmpl)
        {
            // Global value?
            EltID id2 = id;
            id2.typeElement = elementInst;
            if (_sf2->isSet(id2, champ))
                value = Attribute::toRealValue(champ, false, _sf2->get(id2, champ));
        }
        isOverridden = false;
        return value;
    }
}

void PageEnvelope::addEnvelop(EltID id, bool isVolume, bool isMain)
{
    int index = ui->graphicsView->addEnvelop();
    ui->graphicsView->setEnvelopStyle(index, id.typeElement == elementInst, isVolume, isMain);

    bool isOverridden = false;
    if (isVolume)
    {
        ui->graphicsView->setValue(index, Envelop::DELAY, computeValue(id, champ_delayVolEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::ATTACK, computeValue(id, champ_attackVolEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::HOLD, computeValue(id, champ_holdVolEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::DECAY, computeValue(id, champ_decayVolEnv, isOverridden), isOverridden);
        //ui->graphicsView->setValue(index, Envelop::SUSTAIN, qPow(10, -0.05 * static_cast<double>(computeValue(id, champ_sustainVolEnv, isOverridden))), isOverridden); // Gain computed
        ui->graphicsView->setValue(index, Envelop::SUSTAIN, 1. - computeValue(id, champ_sustainVolEnv, isOverridden) / 144., isOverridden);
        ui->graphicsView->setValue(index, Envelop::RELEASE, computeValue(id, champ_releaseVolEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD, computeValue(id, champ_keynumToVolEnvHold, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY, computeValue(id, champ_keynumToVolEnvDecay, isOverridden), isOverridden);
    }
    else
    {
        ui->graphicsView->setValue(index, Envelop::DELAY, computeValue(id, champ_delayModEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::ATTACK, computeValue(id, champ_attackModEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::HOLD, computeValue(id, champ_holdModEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::DECAY, computeValue(id, champ_decayModEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::SUSTAIN, 1. - computeValue(id, champ_sustainModEnv, isOverridden) / 100., isOverridden);
        ui->graphicsView->setValue(index, Envelop::RELEASE, computeValue(id, champ_releaseModEnv, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD, computeValue(id, champ_keynumToModEnvHold, isOverridden), isOverridden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY, computeValue(id, champ_keynumToModEnvDecay, isOverridden), isOverridden);
    }

    // Keyrange of the envelop
    int keyMin = 0;
    int keyMax = 127;
    if (_sf2->isSet(id, champ_keyRange))
    {
        AttributeValue val = _sf2->get(id, champ_keyRange);
        keyMin = val.rValue.byLo;
        keyMax = val.rValue.byHi;
    }
    else if (id.typeElement == elementInstSmpl)
    {
        // Global value?
        EltID id2 = id;
        id2.typeElement = elementInst;
        if (_sf2->isSet(id2, champ_keyRange))
        {
            AttributeValue val = _sf2->get(id2, champ_keyRange);
            keyMin = val.rValue.byLo;
            keyMax = val.rValue.byHi;
        }
    }

    ui->graphicsView->setKeyRange(index, keyMin, keyMax);
}

void PageEnvelope::addSample(EltID idInstSmpl)
{
    // Index of the sample
    EltID idSmpl = idInstSmpl;
    idSmpl.typeElement = elementSmpl;
    idSmpl.indexElt = _sf2->get(idInstSmpl, champ_sampleID).wValue;

    // Data
    QVector<float> vData = _sf2->getDataFloat(idSmpl);

    // Parameters
    int sampleRate = _sf2->get(idSmpl, champ_dwSampleRate).dwValue;
    int startLoop = _sf2->get(idSmpl, champ_dwStartLoop).dwValue;
    int endLoop = _sf2->get(idSmpl, champ_dwEndLoop).dwValue;
    int loopMode = 0;
    if (_sf2->isSet(idInstSmpl, champ_sampleModes))
        loopMode = _sf2->get(idInstSmpl, champ_sampleModes).wValue;
    else
    {
        EltID idInst = idInstSmpl;
        idInst.typeElement = elementInst;
        if (_sf2->isSet(idInst, champ_sampleModes))
            loopMode = _sf2->get(idInst, champ_sampleModes).wValue;
    }

    // Adjust values
    if (startLoop < 0 || endLoop > vData.size())
        loopMode = 0;

    ui->graphicsView->setSample(vData, sampleRate, loopMode, startLoop, endLoop);
}

void PageEnvelope::enableEditor(bool isEnabled)
{
    ui->labelDelay->setEnabled(isEnabled);
    ui->doubleSpinDelay->setEnabled(isEnabled);
    ui->pushDelay->setEnabled(isEnabled);

    ui->labelAttack->setEnabled(isEnabled);
    ui->doubleSpinAttack->setEnabled(isEnabled);
    ui->pushAttack->setEnabled(isEnabled);

    ui->labelHold->setEnabled(isEnabled);
    ui->doubleSpinHold->setEnabled(isEnabled);
    ui->pushHold->setEnabled(isEnabled);

    ui->labelDecay->setEnabled(isEnabled);
    ui->doubleSpinDecay->setEnabled(isEnabled);
    ui->pushDecay->setEnabled(isEnabled);

    ui->labelSustain->setEnabled(isEnabled);
    ui->doubleSpinSustain->setEnabled(isEnabled);
    ui->pushSustain->setEnabled(isEnabled);

    ui->labelRelease->setEnabled(isEnabled);
    ui->doubleSpinRelease->setEnabled(isEnabled);
    ui->pushRelease->setEnabled(isEnabled);

    ui->labelKeyDecay->setEnabled(isEnabled);
    ui->spinKeyDecay->setEnabled(isEnabled);
    ui->pushKeyDecay->setEnabled(isEnabled);

    ui->labelKeyHold->setEnabled(isEnabled);
    ui->spinKeyHold->setEnabled(isEnabled);
    ui->pushKeyHold->setEnabled(isEnabled);
}

void PageEnvelope::stopSignals(bool isStopped)
{
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        QWidget *widget = ui->gridLayout_2->itemAt(i)->widget();
        if (widget != nullptr)
            widget->blockSignals(isStopped);
    }
}

void PageEnvelope::on_doubleSpinDelay_editingFinished()
{
    processEdit(_isVolume ? champ_delayVolEnv : champ_delayModEnv, ui->doubleSpinDelay->value());
}

void PageEnvelope::on_doubleSpinAttack_editingFinished()
{
    processEdit(_isVolume ? champ_attackVolEnv : champ_attackModEnv, ui->doubleSpinAttack->value());
}

void PageEnvelope::on_doubleSpinHold_editingFinished()
{
    processEdit(_isVolume ? champ_holdVolEnv : champ_holdModEnv, ui->doubleSpinHold->value());
}

void PageEnvelope::on_doubleSpinDecay_editingFinished()
{
    processEdit(_isVolume ? champ_decayVolEnv : champ_decayModEnv, ui->doubleSpinDecay->value());
}

void PageEnvelope::on_doubleSpinSustain_editingFinished()
{
    processEdit(_isVolume ? champ_sustainVolEnv : champ_sustainModEnv, ui->doubleSpinSustain->value());
}

void PageEnvelope::on_doubleSpinRelease_editingFinished()
{
    processEdit(_isVolume ? champ_releaseVolEnv : champ_releaseModEnv, ui->doubleSpinRelease->value());
}

void PageEnvelope::on_spinKeyHold_editingFinished()
{
    processEdit(_isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold, ui->spinKeyHold->value());
}

void PageEnvelope::on_spinKeyDecay_editingFinished()
{
    processEdit(_isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay, ui->spinKeyDecay->value());
}

void PageEnvelope::processEdit(AttributeType champ, double value)
{
    bool isOverridden = true;
    double oldValue = computeValue(_currentIds[0], champ, isOverridden);
    if (qAbs(value - oldValue) > 0.0005)
    {
        AttributeValue val = Attribute::fromRealValue(champ, false, value);
        _sf2->set(_currentIds[0], champ, val);
        _sf2->endEditing(_editingSource);
        populate();
    }
}

void PageEnvelope::on_pushDelay_clicked()
{
    processClear(_isVolume ? champ_delayVolEnv : champ_delayModEnv);
}

void PageEnvelope::on_pushAttack_clicked()
{
    processClear(_isVolume ? champ_attackVolEnv : champ_attackModEnv);
}

void PageEnvelope::on_pushHold_clicked()
{
    processClear(_isVolume ? champ_holdVolEnv : champ_holdModEnv);
}

void PageEnvelope::on_pushSustain_clicked()
{
    processClear(_isVolume ? champ_sustainVolEnv : champ_sustainModEnv);
}

void PageEnvelope::on_pushRelease_clicked()
{
    processClear(_isVolume ? champ_releaseVolEnv : champ_releaseModEnv);
}

void PageEnvelope::on_pushDecay_clicked()
{
    processClear(_isVolume ? champ_decayVolEnv : champ_decayModEnv);
}

void PageEnvelope::on_pushKeyHold_clicked()
{
    processClear(_isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold);
}

void PageEnvelope::on_pushKeyDecay_clicked()
{
    processClear(_isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay);
}

void PageEnvelope::processClear(AttributeType champ)
{
    if (_sf2->isSet(_currentIds[0], champ))
    {
        _sf2->reset(_currentIds[0], champ);
        _sf2->endEditing(_editingSource);
        populate();
    }
}
