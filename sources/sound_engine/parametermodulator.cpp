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

#include "parametermodulator.h"
#include "modulatedparameter.h"
#include "imidivalues.h"

IMidiValues * ParameterModulator::s_midiValues = nullptr;
void ParameterModulator::setIMidiValues(IMidiValues * midiValues) { s_midiValues = midiValues; }

void ParameterModulator::initialize(const ModulatorData &modData, bool isPrst, int channel, int initialKey, int keyForComputation, int velForComputation)
{
    _data.srcOper = modData.srcOper;
    _data.destOper = modData.destOper;
    _data.amtSrcOper = modData.amtSrcOper;
    _data.transOper = modData.transOper;
    _data.amount = modData.amount;
    _data.index = modData.index;

    _inputNumber = 0;
    _isPrst = isPrst;
    _channel = channel;
    _initialKey = initialKey;
    _input1 = 0.0;
    _input2 = 0.0;
    _keyForComputation = keyForComputation;
    _velForComputation = velForComputation;
}

bool ParameterModulator::merge(const ModulatorData &modData)
{
    if (modData.srcOper == _data.srcOper && modData.destOper == _data.destOper &&
            modData.amtSrcOper == _data.amtSrcOper && modData.transOper == _data.transOper)
    {
        // Updade the current modulator
        _data.index = modData.index;
        _data.amount = modData.amount;
        return true;
    }

    // Merge not possible
    return false;
}

void ParameterModulator::setOutput(ModulatedParameter * parameter)
{
    _outputModulator = nullptr;
    _outputParameter = parameter;
}

void ParameterModulator::setOutput(ParameterModulator * modulator)
{
    _outputParameter = nullptr;
    _outputModulator = modulator;
    _outputModulator->waitForAnInput();
}

void ParameterModulator::initializeInputs()
{
    if (_inputNumber == 0)
        _input1 = getValue(_data.srcOper);
    _input2 = getValue(_data.amtSrcOper);
}

bool ParameterModulator::processPolyPressureChanged(int pressure)
{
    bool change = false;
    if (!_data.srcOper.CC && _data.srcOper.Index == GC_polypressure)
    {
        _input1 = pressure;
        change = true;
    }
    if (!_data.amtSrcOper.CC && _data.amtSrcOper.Index == GC_polypressure)
    {
        _input2 = pressure;
        change = true;
    }
    return change;
}

bool ParameterModulator::processMonoPressureChanged(int value)
{
    bool change = false;
    if (!_data.srcOper.CC && _data.srcOper.Index == GC_channelPressure)
    {
        _input1 = value;
        change = true;
    }
    if (!_data.amtSrcOper.CC && _data.amtSrcOper.Index == GC_channelPressure)
    {
        _input2 = value;
        change = true;
    }
    return change;
}

bool ParameterModulator::processControllerChanged(int num, int value)
{
    bool change = false;
    if (_data.srcOper.CC && _data.srcOper.Index == num)
    {
        _input1 = value;
        change = true;
    }
    if (_data.amtSrcOper.CC && _data.amtSrcOper.Index == num)
    {
        _input2 = value;
        change = true;
    }
    return change;
}

bool ParameterModulator::processBendChanged(float value)
{
    bool change = false;
    if (!_data.srcOper.CC && _data.srcOper.Index == GC_pitchWheel)
    {
        _input1 = 64.0f * (value + 1.0f);
        change = true;
    }
    if (!_data.amtSrcOper.CC && _data.amtSrcOper.Index == GC_pitchWheel)
    {
        _input2 = 64.0f * (value + 1.0f);
        change = true;
    }
    return change;
}

bool ParameterModulator::processBendSensitivityChanged(float semitones)
{
    bool change = false;
    if (!_data.srcOper.CC && _data.srcOper.Index == GC_pitchWheelSensitivity)
    {
        _input1 = semitones;
        change = true;
    }
    if (!_data.amtSrcOper.CC && _data.amtSrcOper.Index == GC_pitchWheelSensitivity)
    {
        _input2 = semitones;
        change = true;
    }
    return change;
}

void ParameterModulator::initializeComputation()
{
    if (_inputNumber > 0)
        _input1 = 0;
    _inputCount = 0;
    _minSum = 0;
    _maxSum = 0;
    _computed = false;
}

bool ParameterModulator::computeOutput()
{
    if (_inputCount < _inputNumber)
        return false; // At least one input is not known yet, we need to wait

    if (_computed)
        return true; // Already computed

    // Compute input1
    double input1;
    if (_inputNumber > 0)
    {
        // Normalize the external inputs between 0 and 127
        input1 = _minSum != _maxSum ? 127.0 * (_input1 - _minSum) / (_maxSum - _minSum) : 0;
    }
    else
        input1 = _input1;

    // Apply shapes
    input1 = input1 < 0 ? 0 : _data.srcOper.applyShape(_input1);
    double input2 = _input2 < 0 ? 0 : _data.amtSrcOper.applyShape(_input2);

    // Compute data
    double result = input1 * input2 * _data.amount;
    if (_data.transOper == SFTransform::absolute_value && result < 0)
        result = -result;

    // Apply the output
    if (_outputModulator != nullptr)
    {
        // Compute min / max
        qint16 min, max;
        _data.getRange(min, max);

        // Send the value to another modulator
        _outputModulator->setInput(result, min, max);
    }
    else if (_outputParameter != nullptr)
    {
        // Send the value to a parameter
        if (_isPrst)
            _outputParameter->addPrstModulation(result);
        else
            _outputParameter->addInstModulation(result);
    }

    // Computed!
    _computed = true;
    return true;
}

void ParameterModulator::setInput(double value, qint16 min, qint16 max)
{
    _input1 += value;
    _minSum += min;
    _maxSum += max;
    _inputCount++;
}

double ParameterModulator::getValue(SFModulator sfMod)
{
    // Default value
    double value = -1;

    if (sfMod.CC)
    {
        // Midi controllers
        value = s_midiValues->getControllerValue(_channel, sfMod.Index);
    }
    else
    {
        // General controllers
        switch (sfMod.Index)
        {
        case GC_noController:
            // Will be "1"
            value = 127;
            break;
        case GC_noteOnVelocity:
            value = _velForComputation;
            break;
        case GC_noteOnKeyNumber:
            value = _keyForComputation;
            break;
        case GC_polypressure:
            // After touch by key
            value = (_initialKey >= 0 ? s_midiValues->getPolyPressure(_channel, _initialKey) : 0);
            break;
        case GC_channelPressure:
            // After touch for the whole keyboard
            value = s_midiValues->getMonoPressure(_channel);
            break;
        case GC_pitchWheel:
            value = 64.0 * (static_cast<double>(s_midiValues->getBendValue(_channel)) + 1.0);
            break;
        case GC_pitchWheelSensitivity:
            value = static_cast<double>(s_midiValues->getBendSensitivityValue(_channel));
            break;
        case GC_link: default:
            // Link (the value will come later)
            break;
        }
    }

    return value;
}
