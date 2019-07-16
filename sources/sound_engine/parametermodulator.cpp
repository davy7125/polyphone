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
#include "utils.h"

ParameterModulator::ParameterModulator(ModulatorData &modData, bool isPrst, int initialKey, int keyForComputation, int velForComputation) :
    _data(modData),
    _inputNumber(0),
    _inputCount(0),
    _computed(false),
    _input1(0),
    _input2(0),
    _outputParameter(nullptr),
    _outputModulator(nullptr),
    _isPrst(isPrst),
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

void ParameterModulator::processInput()
{
    if (_data.amount != 0)
    {
        _input1 = getValue(_data.srcOper);
        _input2 = getValue(_data.amtSrcOper);
    }

    _inputCount = 0;
    _computed = false;
}

bool ParameterModulator::processOutput()
{
    if (_inputCount < _inputNumber)
        return false; // We need to wait
    if (_computed)
        return true; // Already computed

    // Compute data
    double result = _input1 * _input2 * _data.amount;
    if (_data.transOper == SFTransform::absolute_value && result < 0)
        result = -result;

    // Output
    if (_outputModulator != nullptr)
    {
        // Send the value to another modulator
        _outputModulator->setInput(result);
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

void ParameterModulator::setInput(double value)
{
    _input1 += value;
    _inputCount++;
}

double ParameterModulator::getValue(SFModulator sfMod)
{
    // Base value
    double value = -1;
    if (sfMod.CC)
    {
        // Midi controllers
        value = ContextManager::midi()->getControllerValue(sfMod.Index);
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
            value = ContextManager::midi()->getPolyPressure(_initialKey);
            break;
        case GC_channelPressure:
            // After touch for the whole keyboard
            value = ContextManager::midi()->getMonoPressure();
            break;
        case GC_pitchWheel:
            value = ContextManager::midi()->getBendValue();
            break;
        case GC_pitchWheelSensitivity:
            value = ContextManager::midi()->getBendSensitivityValue();
            break;
        case GC_link: default:
            // Link (the value will come later)
            return 0;
        }
    }
//qDebug() << "base value:" << value << "CC" << sfMod.CC << "index" << sfMod.Index;
    // The value might not be ready
    if (value < 0)
        return 0;

    switch (sfMod.Type)
    {
    case typeLinear:
        // Linearly increasing from 0 to 1
        value /= 127.0;

        // Possibly from 1 to 0 instead of 0 to 1
        if (sfMod.isDescending)
            value = 1.0 - value;

        // Possibly from -1 to 1 instead of 0 to 1
        if (sfMod.isBipolar)
            value = 2.0 * value - 1.0;

        break;
    case typeConcave:
        if (sfMod.isDescending)
        {
            if (sfMod.isBipolar)
                // Concave, bipolar, negative
                value = (value > 64) ? -Utils::concave(2 * (value - 64)) : Utils::concave(2 * (64 - value));
            else
                // Concave, unipolar, negative
                value = Utils::concave(127 - value);
        }
        else
        {
            if (sfMod.isBipolar)
                // Concave, bipolar, positive
                value = (value > 64) ? Utils::concave(2 * (value - 64)) : -Utils::concave(2 * (64 - value));
            else
                // Concave, unipolar, positive
                value = Utils::concave(value);
        }
        break;
    case typeConvex:
        if (sfMod.isDescending)
        {
            if (sfMod.isBipolar)
                // Convex, bipolar, negative
                value = (value > 64) ? -Utils::convex(2 * (value - 64)) : Utils::convex(2 * (64 - value));
            else
                // Convex, unipolar, negative
                value = Utils::convex(127 - value);
        }
        else
        {
            if (sfMod.isBipolar)
                // Convex, bipolar, positive
                value = (value > 64) ? Utils::convex(2 * (value - 64)) : -Utils::convex(2 * (64 - value));
            else
                // Convex, unipolar, positive
                value = Utils::convex(value);
        }
        break;
    case typeSwitch:
        // Switch
        value = (value >= 64 ? 1.0 : 0.0);

        // Possibly from 1 to 0 instead of 0 to 1
        if (sfMod.isDescending)
            value = 1.0 - value;

        // Possibly from -1 to 1 instead of 0 to 1
        if (sfMod.isBipolar)
            value = 2.0 * value - 1.0;

        break;
    }
//qDebug() << "after shape:" << value;
    return value;
}
