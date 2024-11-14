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

#ifndef PARAMETERMODULATOR_H
#define PARAMETERMODULATOR_H

#include "basetypes.h"
class ModulatedParameter;
class IMidiValues;

class ParameterModulator
{
public:
    static void setIMidiValues(IMidiValues * midiValues);

    ParameterModulator() {}

    // Initialize a modulator
    void initialize(const ModulatorData &modData, bool isPrst, int channel, int initialKey, int keyForComputation, int velForComputation);

    // Try to merge an existing modulator with another one, return true if success
    bool merge(const ModulatorData &modData);

    // Get info about the modulator
    quint16 getOuputType() { return _data.destOper; }
    quint16 getIndex() { return _data.index; }

    // Set the output
    void setOutput(ModulatedParameter * parameter);
    void setOutput(ParameterModulator * modulator);

    // Read all inputs (before the first computation)
    void initializeInputs();

    // Update from MIDI signals
    bool processPolyPressureChanged(int pressure);
    bool processMonoPressureChanged(int value);
    bool processControllerChanged(int num, int value);
    bool processBendChanged(float value);
    bool processBendSensitivityChanged(float semitones);

    // Initialize a computation
    void initializeComputation();

    // Compute the output
    // Return true if OK
    // Return false if at least one input is not known yet (linked modulators)
    bool computeOutput();

private:
    // Input coming from another modulator
    void setInput(double value, qint16 min, qint16 max);

    // Add 1 to the number of input to wait before computing the result
    void waitForAnInput() { _inputNumber++; }

    // Get a current input value
    double getValue(SFModulator sfMod);

    ModulatorData _data;

    int _inputNumber, _inputCount, _minSum, _maxSum;
    bool _computed;
    double _input1, _input2;
    ModulatedParameter * _outputParameter;
    ParameterModulator * _outputModulator;
    bool _isPrst;

    // Distinction between the initial key that triggered the sound (still useful for the aftertouch)
    // and the value used for the modulators (keynum and velocity might be overridden)
    int _channel, _initialKey, _keyForComputation, _velForComputation;

    static IMidiValues * s_midiValues;
};

#endif // PARAMETERMODULATOR_H
