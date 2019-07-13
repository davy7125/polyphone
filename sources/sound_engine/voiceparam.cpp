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

#include "voiceparam.h"
#include "qmath.h"
#include "modulatedparameter.h"
#include "soundfontmanager.h"

VoiceParam::VoiceParam(EltID idPrstInst, EltID idInstSmpl, EltID idSmpl, int key, int vel) :
    _sm(SoundfontManager::getInstance()),
    _modulatorGroupInst(&_parameters, false),
    _modulatorGroupPrst(&_parameters, true)
{
    // Prepare the parameters (everything to default)
    prepareParameters();

    // Read sample properties and specify the default key / vel
    readSmpl(idSmpl);
    AttributeValue value;
    if (key < 0)
        value.wValue = static_cast<quint16>(_parameters[champ_overridingRootKey]->getIntValue());
    else
        value.wValue = static_cast<quint16>(key);
    _parameters[champ_keynum]->initValue(value, false);
    if (vel < 0)
        value.wValue = 127;
    else
        value.wValue = static_cast<quint16>(vel);
    _parameters[champ_velocity]->initValue(value, false);

    // Possibly add the configuration of the instrument level
    if (idInstSmpl.typeElement != elementUnknown)
        readDivisionAttributes(idInstSmpl);

    // Possibly add the configuration of the preset level
    if (idPrstInst.typeElement != elementUnknown)
    {
        readDivisionAttributes(idPrstInst);
        _wPresetNumber = _sm->get(EltID(elementPrst, idPrstInst.indexSf2, idPrstInst.indexElt), champ_wPreset).wValue;
    }
    else
        _wPresetNumber = -1;

    // Initialize the modulator groups
    int keyForComputation = _parameters[champ_keynum]->getIntValue();
    int velForComputation = _parameters[champ_velocity]->getIntValue();
    _modulatorGroupInst.initialize(key, keyForComputation, velForComputation);
    _modulatorGroupPrst.initialize(key, keyForComputation, velForComputation);

    // Load modulators from the instrument and preset levels, if possible
    if (idInstSmpl.typeElement != elementUnknown)
        readDivisionModulators(idInstSmpl);
    if (idPrstInst.typeElement != elementUnknown)
        readDivisionModulators(idPrstInst);
}

VoiceParam::~VoiceParam()
{
    QList<AttributeType> attributeTypes = _parameters.keys();
    foreach (AttributeType attributeType, attributeTypes)
        delete _parameters.take(attributeType);
}

void VoiceParam::prepareParameters()
{
    // Offsets
    _parameters[champ_startAddrsOffset] = new ModulatedParameter(champ_startAddrsOffset);
    _parameters[champ_startAddrsCoarseOffset] = new ModulatedParameter(champ_startAddrsCoarseOffset);
    _parameters[champ_endAddrsOffset] = new ModulatedParameter(champ_endAddrsOffset);
    _parameters[champ_endAddrsCoarseOffset] = new ModulatedParameter(champ_endAddrsCoarseOffset);
    _parameters[champ_startloopAddrsOffset] = new ModulatedParameter(champ_startloopAddrsOffset);
    _parameters[champ_startloopAddrsCoarseOffset] = new ModulatedParameter(champ_startloopAddrsCoarseOffset);
    _parameters[champ_endloopAddrsOffset] = new ModulatedParameter(champ_endloopAddrsOffset);
    _parameters[champ_endloopAddrsCoarseOffset] = new ModulatedParameter(champ_endloopAddrsCoarseOffset);

    // Volume envelop
    _parameters[champ_delayVolEnv] = new ModulatedParameter(champ_delayVolEnv);
    _parameters[champ_attackVolEnv] = new ModulatedParameter(champ_attackVolEnv);
    _parameters[champ_holdVolEnv] = new ModulatedParameter(champ_holdVolEnv);
    _parameters[champ_decayVolEnv] = new ModulatedParameter(champ_decayVolEnv);
    _parameters[champ_sustainVolEnv] = new ModulatedParameter(champ_sustainVolEnv);
    _parameters[champ_releaseVolEnv] = new ModulatedParameter(champ_releaseVolEnv);

    _parameters[champ_keynumToVolEnvHold] = new ModulatedParameter(champ_keynumToVolEnvHold);
    _parameters[champ_keynumToVolEnvDecay] = new ModulatedParameter(champ_keynumToVolEnvDecay);

    // Modulation envelop
    _parameters[champ_delayModEnv] = new ModulatedParameter(champ_delayModEnv);
    _parameters[champ_attackModEnv] = new ModulatedParameter(champ_attackModEnv);
    _parameters[champ_holdModEnv] = new ModulatedParameter(champ_holdModEnv);
    _parameters[champ_decayModEnv] = new ModulatedParameter(champ_decayModEnv);
    _parameters[champ_sustainModEnv] = new ModulatedParameter(champ_sustainModEnv);
    _parameters[champ_releaseModEnv] = new ModulatedParameter(champ_releaseModEnv);

    _parameters[champ_keynumToModEnvHold] = new ModulatedParameter(champ_keynumToModEnvHold);
    _parameters[champ_keynumToModEnvDecay] = new ModulatedParameter(champ_keynumToModEnvDecay);

    _parameters[champ_modEnvToFilterFc] = new ModulatedParameter(champ_modEnvToFilterFc);
    _parameters[champ_modEnvToPitch] = new ModulatedParameter(champ_modEnvToPitch);

    // Modulation LFO
    _parameters[champ_delayModLFO] = new ModulatedParameter(champ_delayModLFO);
    _parameters[champ_freqModLFO] = new ModulatedParameter(champ_freqModLFO);
    _parameters[champ_modLfoToPitch] = new ModulatedParameter(champ_modLfoToPitch);
    _parameters[champ_modLfoToFilterFc] = new ModulatedParameter(champ_modLfoToFilterFc);
    _parameters[champ_modLfoToVolume] = new ModulatedParameter(champ_modLfoToVolume);

    // Vibrato LFO
    _parameters[champ_delayVibLFO] = new ModulatedParameter(champ_delayVibLFO);
    _parameters[champ_freqVibLFO] = new ModulatedParameter(champ_freqVibLFO);
    _parameters[champ_vibLfoToPitch] = new ModulatedParameter(champ_vibLfoToPitch);

    // Low pass filter and attenuation
    _parameters[champ_initialFilterFc] = new ModulatedParameter(champ_initialFilterFc);
    _parameters[champ_initialFilterQ] = new ModulatedParameter(champ_initialFilterQ);
    _parameters[champ_initialAttenuation] = new ModulatedParameter(champ_initialAttenuation);

    // Effects, pan
    _parameters[champ_chorusEffectsSend] = new ModulatedParameter(champ_chorusEffectsSend);
    _parameters[champ_reverbEffectsSend] = new ModulatedParameter(champ_reverbEffectsSend);
    _parameters[champ_pan] = new ModulatedParameter(champ_pan);

    // Tuning
    _parameters[champ_coarseTune] = new ModulatedParameter(champ_coarseTune);
    _parameters[champ_fineTune] = new ModulatedParameter(champ_fineTune);
    _parameters[champ_scaleTuning] = new ModulatedParameter(champ_scaleTuning);

    // Other
    _parameters[champ_overridingRootKey] = new ModulatedParameter(champ_overridingRootKey);
    _parameters[champ_keynum] = new ModulatedParameter(champ_keynum);
    _parameters[champ_velocity] = new ModulatedParameter(champ_velocity);
    _parameters[champ_sampleModes] = new ModulatedParameter(champ_sampleModes);
    _parameters[champ_exclusiveClass] = new ModulatedParameter(champ_exclusiveClass);
}

void VoiceParam::readSmpl(EltID idSmpl)
{
    // Read sample properties
    _parameters[champ_overridingRootKey]->initValue(_sm->get(idSmpl, champ_byOriginalPitch), false);
    _sampleFineTune = _sm->get(idSmpl, champ_chPitchCorrection).cValue;
    _sampleLength = static_cast<qint32>(_sm->get(idSmpl, champ_dwLength).dwValue);
    _sampleLoopStart = static_cast<qint32>(_sm->get(idSmpl, champ_dwStartLoop).dwValue);
    _sampleLoopEnd = static_cast<qint32>(_sm->get(idSmpl, champ_dwEndLoop).dwValue);
}

void VoiceParam::readDivisionAttributes(EltID idDivision)
{
    bool isPrst = idDivision.isPrst();

    // Load global attributes
    EltID id(isPrst ? elementPrst : elementInst, idDivision.indexSf2, idDivision.indexElt);
    QList<AttributeType> globalAttributeTypes;
    QList<AttributeValue> globalAttributeValues;
    _sm->getAllAttributes(id, globalAttributeTypes, globalAttributeValues);

    // Configure with the global attributes
    for (int i = 0; i < globalAttributeTypes.count(); i++)
        if (_parameters.contains(globalAttributeTypes[i]))
            _parameters[globalAttributeTypes[i]]->initValue(globalAttributeValues[i], isPrst);

    // Load division attributes
    QList<AttributeType> divisionAttributeTypes;
    QList<AttributeValue> divisionAttributeValues;
    _sm->getAllAttributes(idDivision, divisionAttributeTypes, divisionAttributeValues);

    // Configure with the division attributes (possibly overriding it)
    for (int i = 0; i < divisionAttributeTypes.count(); i++)
        if (_parameters.contains(divisionAttributeTypes[i]))
            _parameters[divisionAttributeTypes[i]]->initValue(divisionAttributeValues[i], isPrst);
}

void VoiceParam::readDivisionModulators(EltID idDivision)
{
    bool isPrst = idDivision.isPrst();

    // Load global modulators
    EltID id(isPrst ? elementPrst : elementInst, idDivision.indexSf2, idDivision.indexElt);
    QList<ModulatorData> globalModulators;
    _sm->getAllModulators(id, globalModulators);
    if (isPrst)
        _modulatorGroupPrst.loadModulators(globalModulators);
    else
        _modulatorGroupInst.loadModulators(globalModulators);

    // Load division modulators
    QList<ModulatorData> divisionModulators;
    _sm->getAllModulators(idDivision, divisionModulators);
    if (isPrst)
        _modulatorGroupPrst.loadModulators(divisionModulators);
    else
        _modulatorGroupInst.loadModulators(divisionModulators);
}

void VoiceParam::prepareForSmpl(int key, SFSampleLink link)
{
    // Calling a second time the same sample mute the first one
    AttributeValue value;
    value.wValue = static_cast<quint16>(key); // Not a problem if -1 is translated into an unsigned
    _parameters[champ_exclusiveClass]->initValue(value, false);

    // Default release
    value.shValue = static_cast<qint16>(qRound(1200. * qLn(0.2) / 0.69314718056));
    _parameters[champ_releaseVolEnv]->initValue(value, false);

    // Pan
    switch (link)
    {
    case leftSample: case RomLeftSample:
        value.shValue = -500;
        _parameters[champ_pan]->initValue(value, false);
        break;
    case rightSample: case RomRightSample:
        value.shValue = 500;
        _parameters[champ_pan]->initValue(value, false);
        break;
    default:
        value.shValue = 0;
        _parameters[champ_pan]->initValue(value, false);
        break;
    }
}

void VoiceParam::setPan(double val)
{
    AttributeValue value;
    value.shValue = static_cast<qint16>(qRound(val * 10.));
    _parameters[champ_pan]->initValue(value, false);
}

void VoiceParam::setLoopMode(quint16 val)
{
    AttributeValue value;
    value.wValue = val;
    _parameters[champ_sampleModes]->initValue(value, false);
}

void VoiceParam::setLoopStart(quint32 val)
{
    _sampleLoopStart = static_cast<qint32>(val);
}

void VoiceParam::setLoopEnd(quint32 val)
{
    _sampleLoopEnd = static_cast<qint32>(val);
}

void VoiceParam::setFineTune(qint16 val)
{
    AttributeValue value;
    value.shValue = val;
    _parameters[champ_fineTune]->initValue(value, false);
}

double VoiceParam::getDouble(AttributeType type)
{
    if (_parameters.contains(type))
        return _parameters[type]->getRealValue();

    qDebug() << "VoiceParam: type" << type << "-" << Attribute::getDescription(type, false) << "not found";
    return 0.0;
}

qint32 VoiceParam::getInteger(AttributeType type)
{
    // Notes:
    // * if fineTune is required: add the finetune from the sample level
    // * if wPreset is required, it will be stored in a special variable
    if (_parameters.contains(type))
        return (type == champ_fineTune ? _sampleFineTune : 0) + _parameters[type]->getIntValue();
    if (type == champ_wPreset)
        return _wPresetNumber;

    qDebug() << "VoiceParam: type" << type << "-" << Attribute::getDescription(type, false) << "not found";
    return 0;
}

quint32 VoiceParam::getPosition(AttributeType type)
{
    quint32 result = 0;

    switch (type)
    {
    case champ_dwStart16: { // Used here for computing the beginning
        qint32 offset = _parameters[champ_startAddrsOffset]->getIntValue() +
                32768 * _parameters[champ_startAddrsCoarseOffset]->getIntValue();
        if (offset < 0)
            result = 0;
        else
        {
            result = static_cast<quint32>(offset);
            quint32 currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    case champ_dwLength: {
        qint32 offset = _parameters[champ_endAddrsOffset]->getIntValue() +
                32768 * _parameters[champ_endAddrsCoarseOffset]->getIntValue();
        if (_sampleLength + offset < 0)
            result = 0;
        else if (offset > 0)
            result = static_cast<quint32>(_sampleLength);
        else
            result = static_cast<quint32>(_sampleLength + offset);
    } break;
    case champ_dwStartLoop: {
        qint32 offset = _parameters[champ_startloopAddrsOffset]->getIntValue() +
                32768 * _parameters[champ_startloopAddrsCoarseOffset]->getIntValue();
        if (_sampleLoopStart + offset < 0)
            result = 0;
        else
        {
            result = static_cast<quint32>(_sampleLoopStart + offset);
            quint32 currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    case champ_dwEndLoop: {
        qint32 offset = _parameters[champ_endloopAddrsOffset]->getIntValue() +
                32768 * _parameters[champ_endloopAddrsCoarseOffset]->getIntValue();
        if (_sampleLoopEnd + offset < 0)
            result = 0;
        else
        {
            result = static_cast<quint32>(_sampleLoopEnd + offset);
            quint32 currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    default:
        qDebug() << "VoiceParam: type" << type << "is not readable with 'getPosition'";
    }

    return result;
}

void VoiceParam::computeModulations()
{
    // First clear all modulations
    foreach (ModulatedParameter * parameter, _parameters)
        parameter->clearModulations();

    // Process modulators
    _modulatorGroupInst.process();
    _modulatorGroupPrst.process();
}
