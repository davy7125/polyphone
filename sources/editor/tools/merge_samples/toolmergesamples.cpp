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

#include "toolmergesamples.h"
#include "toolmergesamples_parameters.h"
#include "toolmergesamples_gui.h"
#include "soundfontmanager.h"
#include "runnablemerger.h"
#include "voice.h"
#include "contextmanager.h"

ToolMergeSamples::ToolMergeSamples() : AbstractTool(new ToolMergeSamples_parameters(), new ToolMergeSamples_gui()),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(elementProcessed(EltID,EltID,quint32,quint32)),
            this, SLOT(onElementProcessed(EltID,EltID,quint32,quint32)), Qt::QueuedConnection);
}

ToolMergeSamples::~ToolMergeSamples()
{
    delete _waitingDialog;
}

bool ToolMergeSamples::isCompatible(IdList ids)
{
    // Ok if there is only one preset
    return ids.getSelectedIds(elementPrst).count() == 1;
}

void ToolMergeSamples::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    ToolMergeSamples_parameters * params = dynamic_cast<ToolMergeSamples_parameters *>(parameters);
    _canceled = false;
    _warning = "";
    _presetName = params->getPresetName();

    // Current preset
    EltID idPrst = ids.getSelectedIds(elementPrst)[0];
    idPrst.typeElement = elementPrst;

    // Keyrange
    RangesType range = this->getPresetRange(sm, idPrst);
    if (range.byLo > range.byHi)
    {
        _warning = tr("The preset doesn't trigger any samples.");
        emit finished(true);
        return;
    }

    // Create a new instrument
    bool loopEnabled = params->getLoopSample();
    _idNewInst = createInstrumentInPreset(sm, params->getPresetName(), idPrst.indexSf2, range, loopEnabled);

    // Number of keys per sample
    quint32 keyNumber = 1;
    switch (params->getDensityType())
    {
    case 0: keyNumber = 1; break;
    case 1: keyNumber = 3; break;
    case 2: keyNumber = 6; break;
    default:
        return;
    }

    // Compute the number of steps
    _currentStep = 0;
    _steps = static_cast<quint32>(qAbs(range.byHi - range.byLo)) / keyNumber + 1;

    // Create and open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    initVoices();

    // For each key / division
    quint32 noteStart2 = range.byLo;
    quint32 noteEnd = range.byHi;
    quint32 noteStart = noteStart2 + (noteEnd - noteStart2) % keyNumber;

    int sustainDuration = params->getSampleSustainDuration();
    int releaseDuration = params->getSampleReleaseDuration();
    bool stereoSamples = params->getStereoSample();
    for (quint32 key = noteStart; key <= noteEnd; key += keyNumber)
    {
        RunnableMerger * rcc = new RunnableMerger(
            this, idPrst, key,
            (key + 1 < noteStart2 + keyNumber) ? noteStart2 : (key - keyNumber + 1), // min key
            loopEnabled, stereoSamples,
            sustainDuration, releaseDuration);
        QThreadPool::globalInstance()->start(rcc);
    }
}

void ToolMergeSamples::onElementProcessed(EltID idSmplR, EltID idSmplL, quint32 key, quint32 minKey)
{
    if (_waitingDialog == nullptr)
        return; // Just in case
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Sample name
    QString name = QString("%1").arg(key, 3, 10, QChar('0'));
    if (idSmplL.typeElement == elementSmpl)
    {
        name = _presetName.left(15) + " " + name;
        sm->set(idSmplR, champ_name, name + "R");
        sm->set(idSmplL, champ_name, name + "L");
    }
    else
    {
        name = _presetName.left(16) + " " + name;
        sm->set(idSmplR, champ_name, name);
    }

    // Possible link samples
    AttributeValue value;
    if (idSmplL.typeElement == elementSmpl)
    {
        value.wValue = static_cast<quint16>(idSmplL.indexElt);
        sm->set(idSmplR, champ_wSampleLink, value);
        value.wValue = static_cast<quint16>(idSmplR.indexElt);
        sm->set(idSmplL, champ_wSampleLink, value);
    }

    // Add sample R into the instrument
    EltID idInstSmplR = EltID(elementInstSmpl, _idNewInst.indexSf2, _idNewInst.indexElt);
    idInstSmplR.indexElt2 = sm->add(idInstSmplR);

    // Configuration of the division
    value.wValue = static_cast<quint16>(idSmplR.indexElt);
    sm->set(idInstSmplR, champ_sampleID, value);
    value.rValue.byLo = static_cast<quint8>(minKey);
    value.rValue.byHi = static_cast<quint8>(key);
    sm->set(idInstSmplR, champ_keyRange, value);
    value.shValue = idSmplL.typeElement == elementSmpl ? 500 : 0;
    sm->set(idInstSmplR, champ_pan, value);

    // Possibly add sample L into the instrument
    if (idSmplL.typeElement == elementSmpl)
    {
        EltID idInstSmplL = EltID(elementInstSmpl, _idNewInst.indexSf2, _idNewInst.indexElt);
        idInstSmplL.indexElt2 = sm->add(idInstSmplL);

        // Configuration of the division
        value.wValue = static_cast<quint16>(idSmplL.indexElt);
        sm->set(idInstSmplL, champ_sampleID, value);
        value.rValue.byLo = static_cast<quint8>(minKey);
        value.rValue.byHi = static_cast<quint8>(key);
        sm->set(idInstSmplL, champ_keyRange, value);
        value.shValue = -500;
        sm->set(idInstSmplL, champ_pan, value);
    }

    // Update the waiting dialog
    _waitingDialog->setValue(static_cast<int>(++_currentStep));
    if (_currentStep >= _steps)
    {
        restoreVoices();
        delete _waitingDialog;
        _waitingDialog = nullptr;
        if (_canceled)
        {
            SoundfontManager::getInstance()->revertNewEditing();
            emit finished(false);
        }
        else
            emit finished(true);
    }
}

void ToolMergeSamples::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

QString ToolMergeSamples::getWarning()
{
    return _warning;
}

RangesType ToolMergeSamples::getPresetRange(SoundfontManager * sm, EltID idPrst)
{
    RangesType result;
    result.byLo = 127;
    result.byHi = 0;

    // Default preset range
    RangesType presetDefaultRange;
    if (sm->isSet(idPrst, champ_keyRange))
        presetDefaultRange = sm->get(idPrst, champ_keyRange).rValue;
    else
    {
        presetDefaultRange.byLo = 0;
        presetDefaultRange.byHi = 127;
    }

    // Check the range of all divisions
    EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
    foreach (int index, sm->getSiblings(idPrstInst))
    {
        idPrstInst.indexElt2 = index;

        // Division max range
        RangesType prstInstMaxRange;
        if (sm->isSet(idPrstInst, champ_keyRange))
            prstInstMaxRange = sm->get(idPrstInst, champ_keyRange).rValue;
        else
            prstInstMaxRange = presetDefaultRange;

        // Range of the instrument
        RangesType instRange = getInstrumentRange(sm, EltID(elementInst, idPrstInst.indexSf2, sm->get(idPrstInst, champ_instrument).wValue));
        if (instRange.byLo > instRange.byHi)
            continue; // Nothing in the instrument

        if (instRange.byLo < prstInstMaxRange.byLo)
            instRange.byLo = prstInstMaxRange.byLo;
        if (instRange.byHi > prstInstMaxRange.byHi)
            instRange.byHi = prstInstMaxRange.byHi;

        if (instRange.byLo > instRange.byHi)
            continue; // The instrument key range is not included in the division range

        if (instRange.byLo < result.byLo)
            result.byLo = instRange.byLo;
        if (instRange.byHi > result.byHi)
            result.byHi = instRange.byHi;
    }

    return result;
}

RangesType ToolMergeSamples::getInstrumentRange(SoundfontManager * sm, EltID idInst)
{
    RangesType result;
    result.byLo = 127;
    result.byHi = 0;

    // Check the range of all divisions
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    bool useDefault = false;
    bool atLeastOneDivision = false;
    foreach (int index, sm->getSiblings(idInstSmpl))
    {
        atLeastOneDivision = true;
        idInstSmpl.indexElt2 = index;

        if (sm->isSet(idInstSmpl, champ_keyRange))
        {
            RangesType rangeTmp = sm->get(idInstSmpl, champ_keyRange).rValue;
            if (rangeTmp.byLo < result.byLo)
                result.byLo = rangeTmp.byLo;
            if (rangeTmp.byHi > result.byHi)
                result.byHi = rangeTmp.byHi;
        }
        else
            useDefault = true;
    }

    if (atLeastOneDivision)
    {
        // Check the range of the instrument, if needed
        if (useDefault && sm->isSet(idInst, champ_keyRange))
        {
            RangesType defaultRange = sm->get(idInst, champ_keyRange).rValue;
            if (defaultRange.byLo < result.byLo)
                result.byLo = defaultRange.byLo;
            if (defaultRange.byHi > result.byHi)
                result.byHi = defaultRange.byHi;
        }

        // Final check
        if (result.byHi < result.byLo)
        {
            quint8 tmp = result.byLo;
            result.byLo = result.byHi;
            result.byHi = tmp;
        }
    }

    return result;
}

EltID ToolMergeSamples::createInstrumentInPreset(SoundfontManager * sm, QString name, int indexSf2, RangesType range, bool withLoop)
{
    AttributeValue val;

    // Create an instrument
    EltID idNewInst = EltID(elementInst, indexSf2);
    idNewInst.indexElt = sm->add(idNewInst);
    sm->set(idNewInst, champ_name, name.left(20));
    if (withLoop)
    {
        val.wValue = 1;
        sm->set(idNewInst, champ_sampleModes, val);
    }

    // Create a new preset
    EltID idNewPrst = EltID(elementPrst, indexSf2);
    idNewPrst.indexElt = sm->add(idNewPrst);
    sm->set(idNewPrst, champ_name, name.left(20));

    int numBank, numPreset;
    sm->firstAvailablePresetBank(idNewPrst, numBank, numPreset);
    val.wValue = numBank;
    sm->set(idNewPrst, champ_wBank, val);
    val.wValue = numPreset;
    sm->set(idNewPrst, champ_wPreset, val);

    // Add the instrument to the preset
    EltID idNewPrstInst = EltID(elementPrstInst, indexSf2, idNewPrst.indexElt);
    idNewPrstInst.indexElt2 = sm->add(idNewPrstInst);

    val.wValue = idNewInst.indexElt;
    sm->set(idNewPrstInst, champ_instrument, val);

    // Range of the division
    val.rValue = range;
    sm->set(idNewPrstInst, champ_keyRange, val);

    return idNewInst;
}

void ToolMergeSamples::initVoices()
{
    // Remove the temperament and the tuning fork
    float temperament[12];
    memset(temperament, 0, 12 * sizeof(float));
    Voice::setTemperament(temperament, 0);
    Voice::setTuningFork(440);
}

void ToolMergeSamples::restoreVoices()
{
    // Restore the temperament and the tuning fork
    SynthConfig * configuration = ContextManager::configuration()->getSynthConfig();
    if (configuration->temperament.count() == 14)
    {
        float temperament[12];
        for (int i = 1; i < 13; i++)
            temperament[i - 1] = configuration->temperament[i].toFloat();
        int temperamentRelativeKey = configuration->temperament[13].toInt() % 12;
        Voice::setTemperament(temperament, temperamentRelativeKey);
    }

    Voice::setTuningFork(configuration->tuningFork);
}
