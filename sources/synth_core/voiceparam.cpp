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

#include "voiceparam.h"
#include <cmath>
#include <cstdio>
#include "synthmath.h"

VoiceParam::VoiceParam(const VoiceContext * context) :
    _context(context),
    _version(1), // The parameters have no value in cache (version 0)
    _modulatorGroupInst(_parameters, false, context),
    _modulatorGroupPrst(_parameters, true, context)
{
    // Prepare the parameters (everything to default)
    prepareParameters();
}

VoiceParam::~VoiceParam()
{

}

void VoiceParam::initialize(int sf2Id, ISynthInstPrst * prst, ISynthZone * prstZone, ISynthInstPrst * inst, ISynthZone * instZone,
                            ISynthSample * smpl, int8_t channel, int8_t key, int8_t vel, int8_t type)
{
    _channel = channel;
    _key = key;
    _type = type;
    _sf2Id = sf2Id;
    _presetId = (prst != nullptr ? prst->getIndex() : -1);
    _wPresetNumber = (prst != nullptr ? prst->getPresetNumber() : -1);

    // Reset the parameters
    for (int i = 0; i < champ_endOper; i++)
        _parameters[i].resetComputation();

    // Read sample properties and specify the default key / vel
    readSmpl(smpl);
    AttributeValue value;
    if (_key < 0)
        value.wValue = static_cast<uint16_t>(_parameters[champ_overridingRootKey].getIntValue());
    else
        value.wValue = static_cast<uint16_t>(_key);
    _parameters[champ_keynum].initValue(value, false);
    if (vel < 0)
        value.wValue = 127;
    else
        value.wValue = static_cast<uint16_t>(vel);
    _parameters[champ_velocity].initValue(value, false);

    // Possibly add the configuration of the instrument level
    ISynthZone * instGlobalZone = (inst != nullptr ? inst->getGlobalZone() : nullptr);
    if (instZone)
        readZoneAttributes(instGlobalZone, instZone, false);

    // Possibly add the configuration of the preset level
    ISynthZone * prstGlobalZone = (prst != nullptr ? prst->getGlobalZone() : nullptr);
    if (prstZone)
        readZoneAttributes(prstGlobalZone, prstZone, true);

    // Initialize the modulator groups
    if (instZone)
    {
        int keyForComputation = _parameters[champ_keynum].getIntValue();
        int velForComputation = _parameters[champ_velocity].getIntValue();
        _modulatorGroupInst.initialize(_channel, _key, keyForComputation, velForComputation);
        _modulatorGroupPrst.initialize(_channel, _key, keyForComputation, velForComputation);
    }
    else
    {
        // Sample level: no modulators
        _modulatorGroupInst.clear();
        _modulatorGroupPrst.clear();
    }

    // Possibly load modulators from the instrument and preset levels
    if (instZone)
        readZoneModulators(instGlobalZone, instZone, false);
    if (prstZone)
        readZoneModulators(prstGlobalZone, prstZone, true);

    if (type != 0) // Smpl level
        this->prepareForSmpl(key, smpl->getSampleType());

    // Initialize inputs and compute all modulations
    _modulatorGroupInst.initializeModulatorInputs();
    _modulatorGroupPrst.initializeModulatorInputs();
    computeModulations();
}

void VoiceParam::prepareParameters()
{
    // Offsets
    _parameters[champ_startAddrsOffset].setType(champ_startAddrsOffset);
    _parameters[champ_startAddrsCoarseOffset].setType(champ_startAddrsCoarseOffset);
    _parameters[champ_endAddrsOffset].setType(champ_endAddrsOffset);
    _parameters[champ_endAddrsCoarseOffset].setType(champ_endAddrsCoarseOffset);
    _parameters[champ_startloopAddrsOffset].setType(champ_startloopAddrsOffset);
    _parameters[champ_startloopAddrsCoarseOffset].setType(champ_startloopAddrsCoarseOffset);
    _parameters[champ_endloopAddrsOffset].setType(champ_endloopAddrsOffset);
    _parameters[champ_endloopAddrsCoarseOffset].setType(champ_endloopAddrsCoarseOffset);

    // Volume envelop
    _parameters[champ_delayVolEnv].setType(champ_delayVolEnv);
    _parameters[champ_attackVolEnv].setType(champ_attackVolEnv);
    _parameters[champ_holdVolEnv].setType(champ_holdVolEnv);
    _parameters[champ_decayVolEnv].setType(champ_decayVolEnv);
    _parameters[champ_sustainVolEnv].setType(champ_sustainVolEnv);
    _parameters[champ_releaseVolEnv].setType(champ_releaseVolEnv);

    _parameters[champ_keynumToVolEnvHold].setType(champ_keynumToVolEnvHold);
    _parameters[champ_keynumToVolEnvDecay].setType(champ_keynumToVolEnvDecay);

    // Modulation envelop
    _parameters[champ_delayModEnv].setType(champ_delayModEnv);
    _parameters[champ_attackModEnv].setType(champ_attackModEnv);
    _parameters[champ_holdModEnv].setType(champ_holdModEnv);
    _parameters[champ_decayModEnv].setType(champ_decayModEnv);
    _parameters[champ_sustainModEnv].setType(champ_sustainModEnv);
    _parameters[champ_releaseModEnv].setType(champ_releaseModEnv);

    _parameters[champ_keynumToModEnvHold].setType(champ_keynumToModEnvHold);
    _parameters[champ_keynumToModEnvDecay].setType(champ_keynumToModEnvDecay);

    _parameters[champ_modEnvToFilterFc].setType(champ_modEnvToFilterFc);
    _parameters[champ_modEnvToPitch].setType(champ_modEnvToPitch);

    // Modulation LFO
    _parameters[champ_delayModLFO].setType(champ_delayModLFO);
    _parameters[champ_freqModLFO].setType(champ_freqModLFO);
    _parameters[champ_modLfoToPitch].setType(champ_modLfoToPitch);
    _parameters[champ_modLfoToFilterFc].setType(champ_modLfoToFilterFc);
    _parameters[champ_modLfoToVolume].setType(champ_modLfoToVolume);

    // Vibrato LFO
    _parameters[champ_delayVibLFO].setType(champ_delayVibLFO);
    _parameters[champ_freqVibLFO].setType(champ_freqVibLFO);
    _parameters[champ_vibLfoToPitch].setType(champ_vibLfoToPitch);

    // Low pass filter and attenuation
    _parameters[champ_initialFilterFc].setType(champ_initialFilterFc);
    _parameters[champ_initialFilterQ].setType(champ_initialFilterQ);
    _parameters[champ_initialAttenuation].setType(champ_initialAttenuation);

    // Effects, pan
    _parameters[champ_chorusEffectsSend].setType(champ_chorusEffectsSend);
    _parameters[champ_reverbEffectsSend].setType(champ_reverbEffectsSend);
    _parameters[champ_pan].setType(champ_pan);

    // Tuning
    _parameters[champ_coarseTune].setType(champ_coarseTune);
    _parameters[champ_fineTune].setType(champ_fineTune);
    _parameters[champ_scaleTuning].setType(champ_scaleTuning);

    // Other
    _parameters[champ_overridingRootKey].setType(champ_overridingRootKey);
    _parameters[champ_keynum].setType(champ_keynum);
    _parameters[champ_velocity].setType(champ_velocity);
    _parameters[champ_sampleModes].setType(champ_sampleModes);
    _parameters[champ_exclusiveClass].setType(champ_exclusiveClass);
}

void VoiceParam::readSmpl(ISynthSample * smpl)
{
    // Read sample properties
    AttributeValue val;
    val.bValue = smpl->getRootKey();
    _parameters[champ_overridingRootKey].initValue(val, false);
    _sampleFineTune = smpl->getPitchCorrection();
    _sampleLength = static_cast<int32_t>(smpl->getLength());
    _sampleLoopStart = static_cast<int32_t>(smpl->getLoopStart());
    _sampleLoopEnd = static_cast<int32_t>(smpl->getLoopEnd());
}

void VoiceParam::readZoneAttributes(ISynthZone * globalZone, ISynthZone * zone, bool isPrst)
{
    // Configure with the global attributes
    bool * attributeSet = globalZone->getAttributeSet();
    AttributeValue * attributeValues = globalZone->getAttributeValues();
    for (int i = 0; i < champ_endOper; i++)
    {
        if (i == champ_sampleModes)
            i = champ_sampleModes;
        if (attributeSet[i])
            _parameters[i].initValue(attributeValues[i], isPrst);
    }

    // Configure with the zone attributes (possibly overriding it)
    attributeSet = zone->getAttributeSet();
    attributeValues = zone->getAttributeValues();
    for (int i = 0; i < champ_endOper; i++)
    {
        if (i == champ_sampleModes)
            i = champ_sampleModes;
        if (attributeSet[i])
            _parameters[i].initValue(attributeValues[i], isPrst);
    }
}

void VoiceParam::readZoneModulators(ISynthZone * globalZone, ISynthZone * zone, bool isPrst)
{
    ModulatorGroup * modulatorGroup = isPrst ? &_modulatorGroupPrst : &_modulatorGroupInst;

    // Load global modulators
    ModulatorData * modulatorData;
    for (int i = 0; i < globalZone->getModulatorCount(); i++)
        if ((modulatorData = globalZone->getModulatorData(i)) != nullptr)
            modulatorGroup->loadModulators(modulatorData, 1);

    // Load zone modulators
    for (int i = 0; i < zone->getModulatorCount(); i++)
        if ((modulatorData = zone->getModulatorData(i)) != nullptr)
            modulatorGroup->loadModulators(modulatorData, 1);
}

void VoiceParam::prepareForSmpl(int key, SFSampleLink link)
{
    // Calling a second time the same sample mute the first one
    AttributeValue value;
    value.wValue = static_cast<uint16_t>(key); // Not a problem if -1 is translated into an unsigned
    _parameters[champ_exclusiveClass].initValue(value, false);

    // Default release
    value.shValue = static_cast<int16_t>(SynthMath::round(1200. * std::log(0.2) / M_LN2));
    _parameters[champ_releaseVolEnv].initValue(value, false);

    // Pan
    switch (link)
    {
    case leftSample: case RomLeftSample:
        value.shValue = -500;
        _parameters[champ_pan].initValue(value, false);
        break;
    case rightSample: case RomRightSample:
        value.shValue = 500;
        _parameters[champ_pan].initValue(value, false);
        break;
    default:
        value.shValue = 0;
        _parameters[champ_pan].initValue(value, false);
        break;
    }
}

void VoiceParam::setPan(float val)
{
    AttributeValue value;
    value.shValue = static_cast<int16_t>(SynthMath::round(val * 10.));
    _parameters[champ_pan].initValue(value, false);
    _version.fetch_add(1, std::memory_order_release);
}

void VoiceParam::setLoopMode(uint16_t val)
{
    AttributeValue value;
    value.wValue = val;
    _parameters[champ_sampleModes].initValue(value, false);
    _version.fetch_add(1, std::memory_order_release);
}

void VoiceParam::setLoopStart(uint32_t val)
{
    _sampleLoopStart = static_cast<int32_t>(val);
}

void VoiceParam::setLoopEnd(uint32_t val)
{
    _sampleLoopEnd = static_cast<int32_t>(val);
}

void VoiceParam::setFineTune(int16_t val)
{
    _sampleFineTune = val;
}

float VoiceParam::getCurrentFloat(AttributeType type)
{
    return _parameters[type].getRealValue();
}

int32_t VoiceParam::getCurrentInteger(AttributeType type)
{
    // Same notes as getInteger
    if (type == champ_wPreset)
        return _wPresetNumber;
    return (type == champ_fineTune ? _sampleFineTune : 0) + _parameters[type].getIntValue();
}

uint32_t VoiceParam::getPosition(AttributeType type)
{
    uint32_t result = 0;
    uint32_t version = _version.load(std::memory_order_acquire);

    switch (type)
    {
    case champ_dwStart16: { // Used here for computing the beginning
        int32_t offset = _parameters[champ_startAddrsOffset].getIntValue(version) +
                32768 * _parameters[champ_startAddrsCoarseOffset].getIntValue(version);
        if (offset < 0)
            result = 0;
        else
        {
            result = static_cast<uint32_t>(offset);
            uint32_t currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    case champ_dwLength: {
        int32_t offset = _parameters[champ_endAddrsOffset].getIntValue(version) +
                32768 * _parameters[champ_endAddrsCoarseOffset].getIntValue(version);
        if (_sampleLength + offset < 0)
            result = 0;
        else if (offset > 0)
            result = static_cast<uint32_t>(_sampleLength);
        else
            result = static_cast<uint32_t>(_sampleLength + offset);
    } break;
    case champ_dwStartLoop: {
        int32_t offset = _parameters[champ_startloopAddrsOffset].getIntValue(version) +
                32768 * _parameters[champ_startloopAddrsCoarseOffset].getIntValue(version);
        if (_sampleLoopStart + offset < 0)
            result = 0;
        else
        {
            result = static_cast<uint32_t>(_sampleLoopStart + offset);
            uint32_t currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    case champ_dwEndLoop: {
        int32_t offset = _parameters[champ_endloopAddrsOffset].getIntValue(version) +
                32768 * _parameters[champ_endloopAddrsCoarseOffset].getIntValue(version);
        if (_sampleLoopEnd + offset < 0)
            result = 0;
        else
        {
            result = static_cast<uint32_t>(_sampleLoopEnd + offset);
            uint32_t currentLength = this->getPosition(champ_dwLength);
            if (result > currentLength)
                result = currentLength;
        }
    } break;
    default:
        fprintf(stderr, "VoiceParam: type %d is not readable with 'getPosition'\n", static_cast<int>(type));
    }

    return result;
}

void VoiceParam::computeModulations()
{
    // First clear all modulations
    for (int i = 0; i < champ_endOper; i++)
        _parameters[i].clearModulations();

    // Process modulators
    _modulatorGroupInst.process();
    _modulatorGroupPrst.process();

    // The values are computed again by the thread computing the voice
    _version.fetch_add(1, std::memory_order_release);
}

void VoiceParam::processPolyPressureChanged(int pressure)
{
    if (_modulatorGroupInst.processPolyPressureChanged(pressure) ||
        _modulatorGroupPrst.processPolyPressureChanged(pressure))
        computeModulations();
}

void VoiceParam::processMonoPressureChanged(int value)
{
    if (_modulatorGroupInst.processMonoPressureChanged(value) ||
        _modulatorGroupPrst.processMonoPressureChanged(value))
        computeModulations();
}

void VoiceParam::processControllerChanged(int num, int value)
{
    if (_modulatorGroupInst.processControllerChanged(num, value) ||
        _modulatorGroupPrst.processControllerChanged(num, value))
        computeModulations();
}

void VoiceParam::processBendChanged(float value)
{
    if (_modulatorGroupInst.processBendChanged(value) ||
        _modulatorGroupPrst.processBendChanged(value))
        computeModulations();
}

void VoiceParam::processBendSensitivityChanged(float semitones)
{
    if (_modulatorGroupInst.processBendSensitivityChanged(semitones) ||
        _modulatorGroupPrst.processBendSensitivityChanged(semitones))
        computeModulations();
}
