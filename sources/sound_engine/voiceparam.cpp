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
#include "division.h"
#include "smpl.h"
#include "modulator.h"

VoiceParam::VoiceParam(Division * prstGlobalDiv, Division * prstDiv, Division * instGlobalDiv, Division * instDiv,
                       Smpl * smpl, int presetId, int presetNumber, int channel, int key, int vel) :
    _channel(channel),
    _key(key),
    _sf2Id(smpl->getId().indexSf2),
    _presetId(presetId),
    _modulatorGroupInst(_parameters, false),
    _modulatorGroupPrst(_parameters, true),
    _wPresetNumber(presetNumber)
{
    memset(_parameters, 0, 140 * sizeof(ModulatedParameter *));

    // Prepare the parameters (everything to default)
    prepareParameters();

    // Read sample properties and specify the default key / vel
    readSmpl(smpl);
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
    if (instDiv)
        readDivisionAttributes(instGlobalDiv, instDiv, false);

    // Possibly add the configuration of the preset level
    if (prstDiv)
        readDivisionAttributes(prstGlobalDiv, prstDiv, true);

    // Initialize the modulator groups
    int keyForComputation = _parameters[champ_keynum]->getIntValue();
    int velForComputation = _parameters[champ_velocity]->getIntValue();
    _modulatorGroupInst.initialize(channel, key, keyForComputation, velForComputation);
    _modulatorGroupPrst.initialize(channel, key, keyForComputation, velForComputation);

    // Load modulators from the instrument and preset levels, if possible
    if (instDiv)
        readDivisionModulators(instGlobalDiv, instDiv, false);
    if (prstDiv)
        readDivisionModulators(prstGlobalDiv, prstDiv, true);
}

VoiceParam::~VoiceParam()
{
    for (int i = 0; i < 140; i++)
        delete _parameters[i];
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

void VoiceParam::readSmpl(Smpl *smpl)
{
    // Read sample properties
    AttributeValue val;
    val.bValue = smpl->_sound.getUInt32(champ_byOriginalPitch);
    _parameters[champ_overridingRootKey]->initValue(val, false);
    _sampleFineTune = smpl->_sound.getInt32(champ_chPitchCorrection);
    _sampleLength = static_cast<qint32>(smpl->_sound.getUInt32(champ_dwLength));
    _sampleLoopStart = static_cast<qint32>(smpl->_sound.getUInt32(champ_dwStartLoop));
    _sampleLoopEnd = static_cast<qint32>(smpl->_sound.getUInt32(champ_dwEndLoop));
}

void VoiceParam::readDivisionAttributes(Division * globalDivision, Division * division, bool isPrst)
{
    // Configure with the global attributes
    bool * attributeSet = globalDivision->getAttributeSet();
    AttributeValue * attributeValues = globalDivision->getAttributeValues();
    for (int i = 0; i < END_OF_GEN; i++)
        if (attributeSet[i] && _parameters[i])
            _parameters[i]->initValue(attributeValues[i], isPrst);

    // Configure with the division attributes (possibly overriding it)
    attributeSet = division->getAttributeSet();
    attributeValues = division->getAttributeValues();
    for (int i = 0; i < END_OF_GEN; i++)
        if (attributeSet[i] && _parameters[i])
            _parameters[i]->initValue(attributeValues[i], isPrst);
}

void VoiceParam::readDivisionModulators(Division * globalDivision, Division * division, bool isPrst)
{
    // Load global modulators
    QVector<Modulator *> modulators = globalDivision->getMods().values();
    QList<ModulatorData> list;
    foreach (Modulator * mod, modulators)
        if (!mod->isHidden())
            list << mod->_data;

    if (isPrst)
        _modulatorGroupPrst.loadModulators(list);
    else
        _modulatorGroupInst.loadModulators(list);

    // Load division modulators
    modulators = division->getMods().values();
    list.clear();
    foreach (Modulator * mod, modulators)
        if (!mod->isHidden())
            list << mod->_data;

    if (isPrst)
        _modulatorGroupPrst.loadModulators(list);
    else
        _modulatorGroupInst.loadModulators(list);
}

void VoiceParam::prepareForSmpl(int key, SFSampleLink link)
{
    // Calling a second time the same sample mute the first one
    AttributeValue value;
    value.wValue = static_cast<quint16>(key); // Not a problem if -1 is translated into an unsigned
    _parameters[champ_exclusiveClass]->initValue(value, false);

    // Default release
    value.shValue = static_cast<qint16>(qRound(1200. * qLn(0.2) / M_LN2));
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
    _sampleFineTune = val;
}

double VoiceParam::getDouble(AttributeType type)
{
    if (_parameters[type])
        return _parameters[type]->getRealValue();

    qWarning() << "VoiceParam: type" << type << "-" << Attribute::getDescription(type, false) << "not found";
    return 0.0;
}

qint32 VoiceParam::getInteger(AttributeType type)
{
    // Notes:
    // * if fineTune is required: add the finetune from the sample level
    // * if wPreset is required, it will be stored in a special variable
    if (type == champ_wPreset)
        return _wPresetNumber;
    if (_parameters[type])
        return (type == champ_fineTune ? _sampleFineTune : 0) + _parameters[type]->getIntValue();

    qWarning() << "VoiceParam: type" << type << "-" << Attribute::getDescription(type, false) << "not found";
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
        qWarning() << "VoiceParam: type" << type << "is not readable with 'getPosition'";
    }

    return result;
}

void VoiceParam::computeModulations()
{
    // First clear all modulations
    for (int i = 0; i < 140; i++)
        if (_parameters[i])
            _parameters[i]->clearModulations();

    // Process modulators
    _modulatorGroupInst.process();
    _modulatorGroupPrst.process();
}
