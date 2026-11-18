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

#include "modulatorgroup.h"
#include "modulatedparameter.h"
#include "parametermodulator.h"
#include "voicecontext.h"
#include "isynthsoundfont.h"

ModulatorGroup::ModulatorGroup(ModulatedParameter *parameters, bool isPrst, const VoiceContext * context) :
    _parameters(parameters),
    _isPrst(isPrst),
    _context(context)
{
}

void ModulatorGroup::clear()
{
    _numberOfParameterModulators = 0;
}

void ModulatorGroup::initialize(int channel, int initialKey, int keyForComputation, int velForComputation, ISynthZone * defaultModulators)
{
    _channel = channel;
    _initialKey = initialKey;
    _keyForComputation = keyForComputation;
    _velForComputation = velForComputation;

    // Load default modulators at the instrument level
    _numberOfParameterModulators = 0;
    if (_isPrst)
        return;

    if (defaultModulators == nullptr)
    {
        // Default modulators of the sf2 format
        int number;
        const ModulatorData * const defaultModData = _context->getDefaultModulators(number);
        loadModulators(defaultModData, number);
    }
    else
    {
        // The soundfont has its own list, replacing the other one
        // Loaded at once so that the modulators linked to another one find it
        ModulatorData modData[MAX_NUMBER_OF_PARAMETER_MODULATORS];
        int number = 0;
        ModulatorData * modulatorData;
        for (int i = 0; i < defaultModulators->getModulatorCount() && number < MAX_NUMBER_OF_PARAMETER_MODULATORS; i++)
            if ((modulatorData = defaultModulators->getModulatorData(i)) != nullptr)
                modData[number++] = *modulatorData;
        loadModulators(modData, number);
    }
}

void ModulatorGroup::loadModulators(const ModulatorData * const modData, int modulatorNumber)
{
    int existingModulatorNumber = _numberOfParameterModulators;

    // Create modulators
    for (int i = 0; i < modulatorNumber; ++i)
    {
        // Possibly overwrite an existing modulator
        bool overwritten = false;
        for (int j = 0; j < _numberOfParameterModulators; ++j)
        {
            if (_modulators[j].merge(modData[i]))
            {
                overwritten = true;
                break;
            }
        }

        // Or create another one
        if (!overwritten && _numberOfParameterModulators < MAX_NUMBER_OF_PARAMETER_MODULATORS)
            _modulators[_numberOfParameterModulators++].initialize(modData[i], _isPrst, _context, _channel, _initialKey, _keyForComputation, _velForComputation);
    }

    // Link the outputs of the newly created modulators
    for (int i = existingModulatorNumber; i < _numberOfParameterModulators; ++i)
    {
        ParameterModulator * modulator = &_modulators[i];
        uint16_t output = modulator->getOuputType();
        if (output < 32768)
        {
            // The target is a parameter
            modulator->setOutput(&_parameters[output]);
        }
        else
        {
            // The target is another modulator
            int indexToFind = output - 32768;
            for (int j = existingModulatorNumber; j < _numberOfParameterModulators; ++j)
            {
                ParameterModulator * otherMod = &_modulators[j];
                if (i != j && otherMod->getIndex() == indexToFind)
                {
                    modulator->setOutput(otherMod);
                    break;
                }
            }
        }
    }
}

void ModulatorGroup::initializeModulatorInputs()
{
    for (int i = 0; i < _numberOfParameterModulators; ++i)
        _modulators[i].initializeInputs();
}

void ModulatorGroup::process()
{
    // Initialize the modulator computation
    for (int i = 0; i < _numberOfParameterModulators; ++i)
        _modulators[i].initializeComputation();

    // Compute the output of the modulators, as long as everything has not been completed
    // or until a maximum is reached (in the case of a loop)
    bool ok;
    int count = 0;
    do {
        ok = true;
        for (int i = 0; i < _numberOfParameterModulators; ++i)
            ok &= _modulators[i].computeOutput();
    } while (!ok && count++ < _numberOfParameterModulators);
}

bool ModulatorGroup::processPolyPressureChanged(int pressure)
{
    bool change = false;
    for (int i = 0; i < _numberOfParameterModulators; i++)
        change |= _modulators[i].processPolyPressureChanged(pressure);
    return change;
}

bool ModulatorGroup::processMonoPressureChanged(int value)
{
    bool change = false;
    for (int i = 0; i < _numberOfParameterModulators; i++)
        change |= _modulators[i].processMonoPressureChanged(value);
    return change;
}

bool ModulatorGroup::processControllerChanged(int num, int value)
{
    bool change = false;
    for (int i = 0; i < _numberOfParameterModulators; i++)
        change |= _modulators[i].processControllerChanged(num, value);
    return change;
}

bool ModulatorGroup::processBendChanged(float value)
{
    bool change = false;
    for (int i = 0; i < _numberOfParameterModulators; i++)
        change |= _modulators[i].processBendChanged(value);
    return change;
}

bool ModulatorGroup::processBendSensitivityChanged(float semitones)
{
    bool change = false;
    for (int i = 0; i < _numberOfParameterModulators; i++)
        change |= _modulators[i].processBendSensitivityChanged(semitones);
    return change;
}
