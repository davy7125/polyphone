/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "envelopeditor.h"
#include "ui_envelopeditor.h"
#include "contextmanager.h"
#include <qmath.h>

EnvelopEditor::EnvelopEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnvelopEditor),
    _sf2(SoundfontManager::getInstance()),
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

EnvelopEditor::~EnvelopEditor()
{
    delete ui;
}

void EnvelopEditor::display(QList<EltID> ids, bool justSelection)
{
    Q_UNUSED(justSelection)

    // Reinitialization
    _displayedElt = ids;

    enableEditor(_displayedElt.count() == 1);
    populate();
}

void EnvelopEditor::on_pushVolume_clicked()
{
    if (!_isVolume)
    {
        _isVolume = true;
        stopSignals(true);
        ui->doubleSpinSustain->setSuffix(" " + trUtf8("dB", "unit for decibels"));
        ui->doubleSpinSustain->setMaximum(144);
        stopSignals(false);
        populate();
    }
}

void EnvelopEditor::on_pushModulation_clicked()
{
    if (_isVolume)
    {
        _isVolume = false;
        stopSignals(true);
        ui->doubleSpinSustain->setSuffix(trUtf8("%", "percentage"));
        ui->doubleSpinSustain->setMaximum(100);
        stopSignals(false);
        populate();
    }
}

void EnvelopEditor::populate()
{
    stopSignals(true);

    // Prepare fonts
    QFont font = this->font();
    QFont fontB = font;
    fontB.setBold(true);

    // Initialize the editor values
    if (_displayedElt.count() == 1)
    {
        EltID id = _displayedElt[0];
        bool isOverriden;

        // Delay
        ui->doubleSpinDelay->setValue(computeValue(id, _isVolume ? champ_delayVolEnv : champ_delayModEnv, isOverriden));
        ui->labelDelay->setFont(isOverriden ? fontB : font);
        ui->pushDelay->setEnabled(isOverriden);

        // Attack
        ui->doubleSpinAttack->setValue(computeValue(id, _isVolume ? champ_attackVolEnv : champ_attackModEnv, isOverriden));
        ui->labelAttack->setFont(isOverriden ? fontB : font);
        ui->pushAttack->setEnabled(isOverriden);

        // Hold
        ui->doubleSpinHold->setValue(computeValue(id, _isVolume ? champ_holdVolEnv : champ_holdModEnv, isOverriden));
        ui->labelHold->setFont(isOverriden ? fontB : font);
        ui->pushHold->setEnabled(isOverriden);

        // Decay
        ui->doubleSpinDecay->setValue(computeValue(id, _isVolume ? champ_decayVolEnv : champ_decayModEnv, isOverriden));
        ui->labelDecay->setFont(isOverriden ? fontB : font);
        ui->pushDecay->setEnabled(isOverriden);

        // Sustain
        ui->doubleSpinSustain->setValue(computeValue(id, _isVolume ? champ_sustainVolEnv : champ_sustainModEnv, isOverriden));
        ui->labelSustain->setFont(isOverriden ? fontB : font);
        ui->pushSustain->setEnabled(isOverriden);

        // Release
        ui->doubleSpinRelease->setValue(computeValue(id, _isVolume ? champ_releaseVolEnv : champ_releaseModEnv, isOverriden));
        ui->labelRelease->setFont(isOverriden ? fontB : font);
        ui->pushRelease->setEnabled(isOverriden);

        // Key -> Hold
        ui->spinKeyHold->setValue(computeValue(id, _isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold, isOverriden));
        ui->labelKeyHold->setFont(isOverriden ? fontB : font);
        ui->pushKeyHold->setEnabled(isOverriden);

        // Key -> Decay
        ui->spinKeyDecay->setValue(computeValue(id, _isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay, isOverriden));
        ui->labelKeyDecay->setFont(isOverriden ? fontB : font);
        ui->pushKeyDecay->setEnabled(isOverriden);
    }

    // Graphics
    ui->graphicsView->clearEnvelops();
    if (_isVolume)
    {
        for (int i = 0; i < _displayedElt.count(); i++)
        {
            EltID id = _displayedElt[i];

            // Volume envelop
            addEnvelop(id, true, true);
        }
    }
    else
    {
        for (int i = 0; i < _displayedElt.count(); i++)
        {
            EltID id = _displayedElt[i];

            // Corresponding volume envelop in secondary
            addEnvelop(id, true, false);

            // Modulation envelop
            addEnvelop(id, false, true);
        }
    }
    ui->graphicsView->drawEnvelops();

    // Sample
    if (_displayedElt.count() == 1 && _displayedElt[0].typeElement == elementInstSmpl)
        addSample(_displayedElt[0]);

    ui->graphicsView->repaint();
    ui->graphicsView->zoomDrag();

    stopSignals(false);
}

double EnvelopEditor::computeValue(EltID id, AttributeType champ, bool &isOverriden)
{
    if (_sf2->isSet(id, champ))
    {
        isOverriden = true;
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
        isOverriden = false;
        return value;
    }
}

void EnvelopEditor::addEnvelop(EltID id, bool isVolume, bool isMain)
{
    int index = ui->graphicsView->addEnvelop();
    ui->graphicsView->setEnvelopStyle(index, id.typeElement == elementInst, isVolume, isMain);

    bool isOverriden = false;
    if (isVolume)
    {
        ui->graphicsView->setValue(index, Envelop::DELAY, computeValue(id, champ_delayVolEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::ATTACK, computeValue(id, champ_attackVolEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::HOLD, computeValue(id, champ_holdVolEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::DECAY, computeValue(id, champ_decayVolEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::SUSTAIN, 1. - computeValue(id, champ_sustainVolEnv, isOverriden) / 144., isOverriden);
        ui->graphicsView->setValue(index, Envelop::RELEASE, computeValue(id, champ_releaseVolEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD, computeValue(id, champ_keynumToVolEnvHold, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY, computeValue(id, champ_keynumToVolEnvDecay, isOverriden), isOverriden);
    }
    else
    {
        ui->graphicsView->setValue(index, Envelop::DELAY, computeValue(id, champ_delayModEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::ATTACK, computeValue(id, champ_attackModEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::HOLD, computeValue(id, champ_holdModEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::DECAY, computeValue(id, champ_decayModEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::SUSTAIN, 1. - computeValue(id, champ_sustainModEnv, isOverriden) / 100., isOverriden);
        ui->graphicsView->setValue(index, Envelop::RELEASE, computeValue(id, champ_releaseModEnv, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD, computeValue(id, champ_keynumToModEnvHold, isOverriden), isOverriden);
        ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY, computeValue(id, champ_keynumToModEnvDecay, isOverriden), isOverriden);
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

void EnvelopEditor::addSample(EltID idInstSmpl)
{
    // Index of the sample
    EltID idSmpl = idInstSmpl;
    idSmpl.typeElement = elementSmpl;
    idSmpl.indexElt = _sf2->get(idInstSmpl, champ_sampleID).wValue;

    // Data
    QByteArray data = _sf2->getData(idSmpl, champ_sampleData32);
    const qint32 * dataSmpl = (qint32 *)data.constData();
    QVector<double> dataD(data.length() / 4);
    for (int i = 0; i < data.length() / 4; i++)
        dataD[i] = (double)dataSmpl[i] / 2147483648.;

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
    if (startLoop < 0 || endLoop > dataD.size())
        loopMode = 0;

    ui->graphicsView->setSample(dataD, sampleRate, loopMode, startLoop, endLoop);
}

void EnvelopEditor::enableEditor(bool isEnabled)
{
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        QWidget *widget = ui->gridLayout_2->itemAt(i)->widget();
        if (widget != nullptr)
            widget->setEnabled(isEnabled);
    }
}

void EnvelopEditor::stopSignals(bool isStopped)
{
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        QWidget *widget = ui->gridLayout_2->itemAt(i)->widget();
        if (widget != nullptr)
            widget->blockSignals(isStopped);
    }
}

void EnvelopEditor::on_doubleSpinDelay_editingFinished()
{
    processEdit(_isVolume ? champ_delayVolEnv : champ_delayModEnv, ui->doubleSpinDelay->value());
}

void EnvelopEditor::on_doubleSpinAttack_editingFinished()
{
    processEdit(_isVolume ? champ_attackVolEnv : champ_attackModEnv, ui->doubleSpinAttack->value());
}

void EnvelopEditor::on_doubleSpinHold_editingFinished()
{
    processEdit(_isVolume ? champ_holdVolEnv : champ_holdModEnv, ui->doubleSpinHold->value());
}

void EnvelopEditor::on_doubleSpinDecay_editingFinished()
{
    processEdit(_isVolume ? champ_decayVolEnv : champ_decayModEnv, ui->doubleSpinDecay->value());
}

void EnvelopEditor::on_doubleSpinSustain_editingFinished()
{
    processEdit(_isVolume ? champ_sustainVolEnv : champ_sustainModEnv, ui->doubleSpinSustain->value());
}

void EnvelopEditor::on_doubleSpinRelease_editingFinished()
{
    processEdit(_isVolume ? champ_releaseVolEnv : champ_releaseModEnv, ui->doubleSpinRelease->value());
}

void EnvelopEditor::on_spinKeyHold_editingFinished()
{
    processEdit(_isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold, ui->spinKeyHold->value());
}

void EnvelopEditor::on_spinKeyDecay_editingFinished()
{
    processEdit(_isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay, ui->spinKeyDecay->value());
}

void EnvelopEditor::processEdit(AttributeType champ, double value)
{
    bool isOverriden = true;
    double oldValue = computeValue(_displayedElt[0], champ, isOverriden);
    if (qAbs(value - oldValue) > 0.0005)
    {
        AttributeValue val = Attribute::fromRealValue(champ, false, value);
        _sf2->set(_displayedElt[0], champ, val);
        _sf2->endEditing("envelopEditor");
        populate();
    }
}

void EnvelopEditor::on_pushDelay_clicked()
{
    processClear(_isVolume ? champ_delayVolEnv : champ_delayModEnv);
}

void EnvelopEditor::on_pushAttack_clicked()
{
    processClear(_isVolume ? champ_attackVolEnv : champ_attackModEnv);
}

void EnvelopEditor::on_pushHold_clicked()
{
    processClear(_isVolume ? champ_holdVolEnv : champ_holdModEnv);
}

void EnvelopEditor::on_pushSustain_clicked()
{
    processClear(_isVolume ? champ_sustainVolEnv : champ_sustainModEnv);
}

void EnvelopEditor::on_pushRelease_clicked()
{
    processClear(_isVolume ? champ_releaseVolEnv : champ_releaseModEnv);
}

void EnvelopEditor::on_pushDecay_clicked()
{
    processClear(_isVolume ? champ_decayVolEnv : champ_decayModEnv);
}

void EnvelopEditor::on_pushKeyHold_clicked()
{
    processClear(_isVolume ? champ_keynumToVolEnvHold : champ_keynumToModEnvHold);
}

void EnvelopEditor::on_pushKeyDecay_clicked()
{
    processClear(_isVolume ? champ_keynumToVolEnvDecay : champ_keynumToModEnvDecay);
}

void EnvelopEditor::processClear(AttributeType champ)
{
    if (_sf2->isSet(_displayedElt[0], champ))
    {
        _sf2->reset(_displayedElt[0], champ);
        _sf2->endEditing("envelopEditor");
        populate();
    }
}
