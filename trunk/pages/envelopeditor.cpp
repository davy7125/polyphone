/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "envelopeditor.h"
#include "ui_envelopeditor.h"
#include "parameter.h"
#include <qmath.h>

EnvelopEditor::EnvelopEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnvelopEditor),
    _sf2(NULL),
    _isVolume(true)
{
    ui->setupUi(this);
}

EnvelopEditor::~EnvelopEditor()
{
    delete ui;
}

void EnvelopEditor::init(Pile_sf2 * sf2)
{
    _sf2 = sf2;
}

void EnvelopEditor::display(QList<EltID> ids)
{
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
        ui->doubleSpinSustain->setSuffix(" dB");
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
        ui->doubleSpinSustain->setSuffix("%");
        ui->doubleSpinSustain->setMaximum(100);
        stopSignals(false);
        populate();
    }
}

void EnvelopEditor::populate()
{
    stopSignals(true);

    // Clear all envelops
    ui->graphicsView->clearEnvelops();

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

    // Display the envelop
    /*EltID child = _baseElt;
    if (child.typeElement == elementInst)
        child.typeElement = elementInstSmpl;
    else if (child.typeElement == elementPrst)
        child.typeElement = elementPrstInst;

    addEnvelop(_baseElt, -1);
    for (int i = 0; i < _sf2->count(child); i++)
    {
        child.indexElt2 = i;
        if (!_sf2->get(child, champ_hidden).bValue)
            addEnvelop(child, i);
    }*/

    stopSignals(false);
}

double EnvelopEditor::computeValue(EltID id, Champ champ, bool &isOverriden)
{
    if (_sf2->isSet(id, champ))
    {
        isOverriden = true;
        return Parameter::computeRealValue(champ, false, _sf2->get(id, champ).genValue);
    }
    else
    {
        // Default value
        double value = Parameter::getDefaultRealValue(champ, false);
        if (id.typeElement == elementInstSmpl)
        {
            // Global value?
            EltID id2 = id;
            id2.typeElement = elementInst;
            if (_sf2->isSet(id2, champ))
                value = Parameter::computeRealValue(champ, false, _sf2->get(id2, champ).genValue);
        }
        isOverriden = false;
        return value;
    }
}

void EnvelopEditor::addEnvelop(EltID id, int index)
{
    ui->graphicsView->addEnvelop(index);

    if (_isVolume)
    {
        if (_sf2->isSet(id, champ_delayVolEnv))
            ui->graphicsView->setValue(index, Envelop::DELAY,
                                       qPow(2., (double)_sf2->get(id, champ_delayVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::ATTACK,
                                       qPow(2., (double)_sf2->get(id, champ_attackVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_holdVolEnv))
            ui->graphicsView->setValue(index, Envelop::HOLD,
                                       qPow(2., (double)_sf2->get(id, champ_holdVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_decayVolEnv))
            ui->graphicsView->setValue(index, Envelop::DECAY,
                                       qPow(2., (double)_sf2->get(id, champ_decayVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_sustainVolEnv))
            ui->graphicsView->setValue(index, Envelop::SUSTAIN,
                                       (double)_sf2->get(id, champ_sustainVolEnv).genValue.shAmount / 1440.);
        else if (_sf2->isSet(id, champ_releaseVolEnv))
            ui->graphicsView->setValue(index, Envelop::RELEASE,
                                       qPow(2., (double)_sf2->get(id, champ_releaseVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_keynumToVolEnvHold))
            ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD,
                                       _sf2->get(id, champ_keynumToVolEnvHold).genValue.shAmount);
        else if (_sf2->isSet(id, champ_keynumToVolEnvDecay))
            ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY,
                                       _sf2->get(id, champ_keynumToVolEnvDecay).genValue.shAmount);
    }
    else
    {
        if (_sf2->isSet(id, champ_attackModEnv))
            ui->graphicsView->setValue(index, Envelop::DELAY,
                                       qPow(2., (double)_sf2->get(id, champ_attackModEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::ATTACK,
                                       qPow(2., (double)_sf2->get(id, champ_attackModEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::HOLD,
                                       qPow(2., (double)_sf2->get(id, champ_attackVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::DECAY,
                                       qPow(2., (double)_sf2->get(id, champ_attackVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::SUSTAIN,
                                       (double)_sf2->get(id, champ_attackVolEnv).genValue.shAmount / 1000.);
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::RELEASE,
                                       qPow(2., (double)_sf2->get(id, champ_attackVolEnv).genValue.shAmount / 1200.));
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_DECAY,
                                       _sf2->get(id, champ_attackVolEnv).genValue.shAmount);
        else if (_sf2->isSet(id, champ_attackVolEnv))
            ui->graphicsView->setValue(index, Envelop::KEYNUM_TO_HOLD,
                                       _sf2->get(id, champ_attackVolEnv).genValue.shAmount);
    }
}

void EnvelopEditor::enableEditor(bool isEnabled)
{
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        QWidget *widget = ui->gridLayout_2->itemAt(i)->widget();
        if (widget != NULL)
            widget->setEnabled(isEnabled);
    }
}

void EnvelopEditor::stopSignals(bool isStopped)
{
    for (int i = 0; i < ui->gridLayout_2->count(); ++i)
    {
        QWidget *widget = ui->gridLayout_2->itemAt(i)->widget();
        if (widget != NULL)
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

void EnvelopEditor::processEdit(Champ champ, double value)
{
    bool isOverriden = true;
    double oldValue = computeValue(_displayedElt[0], champ, isOverriden);
    if (qAbs(value - oldValue) > 0.0005)
    {
        _sf2->prepareNewActions(false);
        Valeur val;
        val.genValue = Parameter::computeStoredValue(champ, false, value);
        _sf2->set(_displayedElt[0], champ, val);
        emit(valueChanged());
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

void EnvelopEditor::processClear(Champ champ)
{
    if (_sf2->isSet(_displayedElt[0], champ))
    {
        _sf2->prepareNewActions(false);
        _sf2->reset(_displayedElt[0], champ);
        emit(valueChanged());
        populate();
    }
}
