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

#include "modulatorgroup.h"
#include "modulator.h"
#include "modulatedparameter.h"
#include "parametermodulator.h"

ModulatorGroup::ModulatorGroup(QMap<AttributeType, ModulatedParameter *> * parameters, bool isPrst, int key, int vel) :
    _parameters(parameters),
    _isPrst(isPrst),
    _key(key),
    _vel(vel)
{
    // Load default modulators at the instrument level
    if (!_isPrst)
        loadDefaultModulators();
}

ModulatorGroup::~ModulatorGroup()
{
    while (!_modulators.empty())
        delete _modulators.takeFirst();
}

void ModulatorGroup::loadDefaultModulators()
{
    ModulatorData modData;
    modData.transOper = SFTransform::linear;
    modData.index = 65535; // Max

    // 1. MIDI Note-On Velocity to Initial Attenuation
    modData.srcOper = SFModulator(GC_noteOnVelocity, typeConcave, directionDescending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 960;
    modData.destOper = champ_initialAttenuation;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 2. MIDI Note-On Velocity to Filter Cutoff
    modData.srcOper = SFModulator(GC_noteOnVelocity, typeLinear, directionDescending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = -2400;
    modData.destOper = champ_initialFilterFc;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 3. MIDI Channel Pressure to Vibrato LFO Pitch Depth
    modData.srcOper = SFModulator(GC_channelPressure, typeLinear, directionAscending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 50;
    modData.destOper = champ_vibLfoToPitch;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 4. MIDI Continuous Controller 1 to Vibrato LFO Pitch Depth
    modData.srcOper = SFModulator(1, typeLinear, directionAscending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 50;
    modData.destOper = champ_vibLfoToPitch;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 5. MIDI Continuous Controller 7 to Initial Attenuation
    modData.srcOper = SFModulator(7, typeConcave, directionDescending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 960;
    modData.destOper = champ_initialAttenuation;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 6. MIDI Continuous Controller 10 to Pan Position
    modData.srcOper = SFModulator(10, typeLinear, directionAscending, polarityBipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 1000;
    modData.destOper = champ_pan;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 7. MIDI Continuous Controller 11 to Initial Attenuation
    modData.srcOper = SFModulator(11, typeConcave, directionDescending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 960;
    modData.destOper = champ_initialAttenuation;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 8. MIDI Continuous Controller 91 to Reverb Effects Send
    modData.srcOper = SFModulator(91, typeLinear, directionAscending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 200; // Yes, only 20% added...
    modData.destOper = champ_reverbEffectsSend;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 9. MIDI Continuous Controller 93 to Chorus Effects Send
    modData.srcOper = SFModulator(93, typeLinear, directionAscending, polarityUnipolar);
    modData.amtSrcOper = SFModulator();
    modData.amount = 200;
    modData.destOper = champ_chorusEffectsSend;
    _modulators << new ParameterModulator(modData, false, _key, _vel);

    // 10. MIDI Pitch Wheel to Initial Pitch Controlled by MIDI Pitch Wheel Sensitivity
    modData.srcOper = SFModulator(GC_pitchWheel, typeLinear, directionAscending, polarityBipolar);
    modData.amtSrcOper = SFModulator(GC_pitchWheelSensitivity, typeLinear, directionAscending, polarityUnipolar);
    modData.amount = 12700;
    modData.destOper = champ_fineTune;
    _modulators << new ParameterModulator(modData, false, _key, _vel);
}

void ModulatorGroup::loadModulators(QList<ModulatorData> &modulators)
{
    // Create modulators
    foreach (ModulatorData modData, modulators)
    {
        // Possibly overwrite an existing modulator
        bool overwritten = false;
        foreach (ParameterModulator * modulator, _modulators)
        {
            if (modulator->merge(modData))
            {
                overwritten = true;
                break;
            }
        }

        // Or create another one
        if (!overwritten)
            _modulators << new ParameterModulator(modData, _isPrst, _key, _vel);
    }

    // Link outputs
    foreach (ParameterModulator * modulator, _modulators)
    {
        quint16 output = modulator->getOuputType();
        if (output < 32768)
        {
            // The target is a parameter
            if (_parameters->contains(static_cast<AttributeType>(output)))
                modulator->setOutput(_parameters->value(static_cast<AttributeType>(output)));
        }
        else
        {
            // The target is another modulator
            int indexToFind = output - 32768;
            foreach (ParameterModulator * otherMod, _modulators)
            {
                if (otherMod != modulator && otherMod->getIndex() == indexToFind)
                {
                    modulator->setOutput(otherMod);
                    break;
                }
            }
        }
    }
}

void ModulatorGroup::process()
{
    // Process the input of the modulators
    foreach (ParameterModulator * modulator, _modulators)
        modulator->processInput();

    // Compute the output of the modulators, as long as everything has not been completed
    // or until a maximum (reached in the case where we have a loop)
    bool ok;
    int count = 0;
    do {
        ok = true;
        foreach (ParameterModulator * modulator, _modulators)
            ok &= modulator->processOutput();
    } while (!ok && count++ < _modulators.count());
}
