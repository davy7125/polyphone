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

#include "parametermodulator.h"
#include "modulatedparameter.h"
#include "contextmanager.h"

ParameterModulator::ParameterModulator(const ModulatorData &modData, bool isPrst, int channel, int initialKey, int keyForComputation, int velForComputation) :
    _data(modData),
    _inputNumber(0),
    _inputCount(0),
    _minSum(0),
    _maxSum(0),
    _computed(false),
    _inputSum(0),
    _outputParameter(nullptr),
    _outputModulator(nullptr),
    _isPrst(isPrst),
    _channel(channel),
    _initialKey(initialKey),
    _keyForComputation(keyForComputation),
    _velForComputation(velForComputation)
{}

bool ParameterModulator::merge(ModulatorData &modData)
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
    _outputParameter = parameter;
}

void ParameterModulator::setOutput(ParameterModulator * modulator)
{
    _outputModulator = modulator;
    _outputModulator->waitForAnInput();
}

void ParameterModulator::initialize()
{
    _inputCount = 0;
    _inputSum = 0.0;
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
        input1 = _minSum != _maxSum ? 127.0 * (_inputSum - _minSum) / (_maxSum - _minSum) : 0;
    }
    else
        input1 = getValue(_data.srcOper);

    // Compute input2
    double input2 = getValue(_data.amtSrcOper);

    // Apply shapes
    input1 = input1 < 0 ? 0 : _data.srcOper.applyShape(input1);
    input2 = input2 < 0 ? 0 : _data.amtSrcOper.applyShape(input2);

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
    _inputSum += value;
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
        value = ContextManager::midi()->getControllerValue(_channel, sfMod.Index);
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
            value = ContextManager::midi()->getPolyPressure(_channel, _initialKey);
            break;
        case GC_channelPressure:
            // After touch for the whole keyboard
            value = ContextManager::midi()->getMonoPressure(_channel);
            break;
        case GC_pitchWheel:
            value = 64.0 * (ContextManager::midi()->getBendValue(_channel) + 1.0);
            break;
        case GC_pitchWheelSensitivity:
            value = ContextManager::midi()->getBendSensitivityValue(_channel);
            break;
        case GC_link: default:
            // Link (the value will come later)
            break;
        }
    }

    return value;
}
