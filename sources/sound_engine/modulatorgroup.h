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

#ifndef MODULATORGROUP_H
#define MODULATORGROUP_H

#include "basetypes.h"
#include "parametermodulator.h"
class ModulatedParameter;

class ModulatorGroup
{
public:
    ModulatorGroup(ModulatedParameter * parameters, bool isPrst);

    // Remove all modulators (sample level)
    void clear();

    // Initialize with keys and vel
    void initialize(int channel, int initialKey, int keyForComputation, int velForComputation);

    // Load modulators from the instrument or preset level
    void loadModulators(const ModulatorData * const modData, int modulatorNumber);

    // Before the first computation, load all the input values
    void initializeModulatorInputs();

    // Possible updates from the MIDI input
    bool processPolyPressureChanged(int pressure);
    bool processMonoPressureChanged(int value);
    bool processControllerChanged(int num, int value);
    bool processBendChanged(float value);
    bool processBendSensitivityChanged(float semitones);

    // Compute the modulations and apply them on the parameters
    void process();

private:
    void loadDefaultModulators();

    ModulatedParameter * _parameters;
    bool _isPrst;
    int _channel, _initialKey, _keyForComputation, _velForComputation;
    ParameterModulator _modulators[MAX_NUMBER_OF_PARAMETER_MODULATORS];
    int _numberOfParameterModulators;
};

#endif // MODULATORGROUP_H
