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

const int ModulatorGroup::MAX_NUMBER_OF_PARAMETER_MODULATORS = 64;

ModulatorGroup::ModulatorGroup(ModulatedParameter *parameters, bool isPrst) :
    _parameters(parameters),
    _isPrst(isPrst),
    _modulators(new ParameterModulator * [MAX_NUMBER_OF_PARAMETER_MODULATORS]),
    _numberOfParameterModulators(0)
{
}

ModulatorGroup::~ModulatorGroup()
{
    for (int i = 0; i < _numberOfParameterModulators; ++i)
        delete _modulators[i];
    delete [] _modulators;
}

void ModulatorGroup::initialize(int channel, int initialKey, int keyForComputation, int velForComputation)
{
    _channel = channel;
    _initialKey = initialKey;
    _keyForComputation = keyForComputation;
    _velForComputation = velForComputation;

    // Load default modulators at the instrument level
    if (!_isPrst)
        loadDefaultModulators();
}

void ModulatorGroup::loadDefaultModulators()
{
    QList<ModulatorData> modDataList;
    int count = ModulatorData::defaultModulatorNumber();
    for (quint16 i = 0; i < count; i++)
    {
        ModulatorData modData;
        modData.loadDefaultModulator(i);
        modDataList << modData;
    }
    loadModulators(modDataList);
}

void ModulatorGroup::loadModulators(QList<ModulatorData> &modulators)
{
    int existingModulatorNumber = _numberOfParameterModulators;

    // Create modulators
    foreach (ModulatorData modData, modulators)
    {
        // Possibly overwrite an existing modulator
        bool overwritten = false;
        for (int i = 0; i < _numberOfParameterModulators; ++i)
        {
            if (_modulators[i]->merge(modData))
            {
                overwritten = true;
                break;
            }
        }

        // Or create another one
        if (!overwritten && _numberOfParameterModulators < MAX_NUMBER_OF_PARAMETER_MODULATORS)
            _modulators[_numberOfParameterModulators++] = new ParameterModulator(modData, _isPrst, _channel, _initialKey, _keyForComputation, _velForComputation);
    }

    // Link the outputs of the newly created modulators
    for (int i = existingModulatorNumber; i < _numberOfParameterModulators; ++i)
    {
        ParameterModulator * modulator = _modulators[i];
        quint16 output = modulator->getOuputType();
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
                ParameterModulator * otherMod = _modulators[j];
                if (i != j && otherMod->getIndex() == indexToFind)
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
    // Initialize the modulator computation
    for (int i = 0; i < _numberOfParameterModulators; ++i)
        _modulators[i]->initialize();

    // Compute the output of the modulators, as long as everything has not been completed
    // or until a maximum is reached (in the case of a loop)
    bool ok;
    int count = 0;
    do {
        ok = true;
        for (int i = 0; i < _numberOfParameterModulators; ++i)
            ok &= _modulators[i]->computeOutput();
    } while (!ok && count++ < _numberOfParameterModulators);
}
