/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "midiprocessor.h"
#include <cstring>

MidiProcessor::MidiProcessor() :
    _listener(nullptr)
{
    memset((MIDI_State *)_midiStates, 0, 17 * sizeof(MIDI_State));

    // Default values of the controllers, then the other values
    for (int channel = 0; channel <= 16; channel++)
    {
        for (int i = 0; i < 128; i++)
        {
            // Default value, depending on the CC number
            int defaultValue = 0;
            switch (i)
            {
            case 0: // Bank number
                // Default bank is 128 for channel 10 (percussions)
                if (channel == 10)
                    defaultValue = 128;
                break;
            case 8: // Balance
            case 10: // Pan position
                defaultValue = 64;
                break;
            case 4: case 64: case 65: case 66: case 67: case 68: case 69: // Pedals
                defaultValue = 0;
                break;
            case 7: case 11: // Main volume, expression
                defaultValue = 127;
                break;
            default:
                break;
            }

            _midiStates[channel]._controllerDefaultValues[i] = defaultValue;
        }
    }
    resetChannels();
}

void MidiProcessor::resetChannels()
{
    memset((Sustain_State *)_sustainStates, 0, 17 * sizeof(Sustain_State));
    for (int channel = 0; channel <= 16; channel++)
    {
        MIDI_State * midiState = &_midiStates[channel];
        for (int i = 0; i < 128; i++)
        {
            midiState->_controllerValues[i] = midiState->_controllerDefaultValues[i];
            midiState->_controllerValueSpecified[i] = false;
            midiState->_controllerValueIsRelative[i] = true; // Default is true, until an "absolute" value is encountered
            midiState->_controllerValueIsRelativeCounter[i] = 0;
            midiState->_polyPressureValues[i] = 0;
        }
        midiState->_bendValue = 0.0f;
        midiState->_bendSensitivityValue = 2.0f;
        midiState->_monoPressureValue = 0;
        midiState->_rpnMSBparameter = 0;
        midiState->_rpnLSBparameter = 0;
        midiState->_rpnMSBvalue = 0;
        midiState->_rpnLSBvalue = 0;
        midiState->_rpnIsRegistered = false;
    }
}

void MidiProcessor::processMidiMessage(const unsigned char * message, size_t length)
{
    if (length < 2)
        return;

    int channel = message[0] & 0x0F;
    switch (message[0] & 0xF0)
    {
    case 0x80: // NOTE OFF
        // First message is the note
        processKeyOff(channel, message[1]);
        break;
    case 0x90: // NOTE ON
        // First message is the note, second is velocity
        if (length < 3)
            return;
        if (message[2] == 0)
            processKeyOff(channel, message[1]);
        else
            processKeyOn(channel, message[1], message[2]);
        break;
    case 0xA0: // AFTERTOUCH
        // First message is the note, second is the pressure
        if (length >= 3)
            processPolyPressureChanged(channel, message[1], message[2]);
        break;
    case 0xB0: // CONTROLLER CHANGE
        // First message is the controller number, second is its value
        if (length >= 3)
            processControllerChanged(true, channel, message[1], message[2]);
        break;
    case 0xC0: // PROGRAM CHANGED
        // First message is the program number
        processProgramChanged(channel, message[1]);
        break;
    case 0xD0: // MONO PRESSURE
        // First message is the global pressure
        processMonoPressureChanged(channel, message[1]);
        break;
    case 0xE0: // BEND
        // First message is the value
        if (length >= 3)
            processBendChanged(channel, (static_cast<float>((message[2] << 7) | message[1]) - 8192.0f) / 8192.0f);
        break;
    default:
        break;
    }
}

void MidiProcessor::processControllerChanged(bool external, int channel, int numController, int value)
{
    int initialValue = value;
    MIDI_State * midiState = &_midiStates[channel + 1];
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    if (numController == 0 && channel == 9)
        return; // Ignored: keep bank 128 on channel 10 (percussions)

    if (external)
    {
        if (midiState->_controllerValueIsRelative[numController])
        {
            // Convert a possible relative value
            if (value == 126 || value == 62) // -2
                value = midiState->_controllerValues[numController] - 2;
            else if (value == 127 || value == 63) // -1
                value = midiState->_controllerValues[numController] - 1;
            else if (value == 1 || value == 65) // +1
                value = midiState->_controllerValues[numController] + 1;
            else if (value == 2 || value == 66) // +2
                value = midiState->_controllerValues[numController] + 2;
            else
            {
                // Not a relative value
                midiState->_controllerValueIsRelative[numController] = false;
                midiState->_controllerValueIsRelativeCounter[numController] = 0;
            }
        }
        else
        {
            // Possibly switch to the relative mode
            if (value == 127 || value == 126 || value == 63 || value == 62 ||
                value == 1 || value == 2 || value == 65 || value == 66)
            {
                midiState->_controllerValueIsRelativeCounter[numController]++;
                if (midiState->_controllerValueIsRelativeCounter[numController] > 3)
                {
                    // Following values will be considered relative
                    midiState->_controllerValueIsRelative[numController] = true;
                }
            }
            else
                midiState->_controllerValueIsRelativeCounter[numController] = 0;
        }
    }

    // Limits
    if (value < 0)
        value = 0;
    if (value > 127)
        value = 127;

    // Update the current channel
    midiState->_controllerValues[numController] = value;
    midiState->_controllerValueSpecified[numController] = true;

    if ((numController >= 96 && numController <= 101) || numController == 6 || numController == 38)
    {
        // (N)RPN reception
        if (numController == 101 || numController == 99)
        {
            // Full parameter not known yet
            midiState->_rpnIsRegistered = (numController == 101);
            midiState->_rpnMSBparameter = value;
        }
        else if (numController == 100 || numController == 98)
        {
            // No value yet
            midiState->_rpnIsRegistered = (numController == 100);
            midiState->_rpnLSBparameter = value;
            processRPNChanged(channel,
                              midiState->_rpnMSBparameter << 7 | midiState->_rpnLSBparameter,
                              0, midiState->_rpnIsRegistered, 2);
        }
        else if (numController == 6)
        {
            // Coarse value
            midiState->_rpnMSBvalue = value;
            processRPNChanged(channel,
                              midiState->_rpnMSBparameter << 7 | midiState->_rpnLSBparameter,
                              value, midiState->_rpnIsRegistered, 3);
        }
        else if (numController == 38)
        {
            // Fine value
            processRPNChanged(channel,
                              midiState->_rpnMSBparameter << 7 | midiState->_rpnLSBparameter,
                              midiState->_rpnMSBvalue << 7 | value,
                              midiState->_rpnIsRegistered, 4);
        }
        else if (numController == 96)
        {
            // Increment
            processRPNChanged(channel,
                              midiState->_rpnMSBparameter << 7 | midiState->_rpnLSBparameter,
                              value, midiState->_rpnIsRegistered, 5);
        }
        else if (numController == 97)
        {
            // Decrement
            processRPNChanged(channel,
                              midiState->_rpnMSBparameter << 7 | midiState->_rpnLSBparameter,
                              value, midiState->_rpnIsRegistered, 6);
        }
        return;
    }
    else if (numController == 64)
    {
        // Sustain pedal
        if (value >= 64 && !sustainState->_isSustainOn)
        {
            sustainState->_isSustainOn = true;

            // All current keys are now sustained
            for (int key = 0; key < 128; key++)
                sustainState->_sustainedKeys[key] = sustainState->_currentKeys[key] || sustainState->_sostenutoMemoryKeys[key];
        }
        else if (value < 64 && sustainState->_isSustainOn)
        {
            sustainState->_isSustainOn = false;

            // Remove all keys that have been previously sustained
            for (int key = 0; key < 128; key++)
            {
                if (sustainState->_sustainedKeys[key])
                {
                    sustainState->_sustainedKeys[key] = false;

                    // And release them if they are not currently triggered or held by the sostenuto
                    if (!sustainState->_currentKeys[key] && !sustainState->_sostenutoMemoryKeys[key])
                        processKeyOff(channel, key);
                }
            }
        }
    }
    else if (numController == 66)
    {
        // Sostenuto pedal
        if (value >= 64 && !sustainState->_isSostenutoOn)
        {
            sustainState->_isSostenutoOn = true;

            // All current keys are now held by the sostenuto
            for (int key = 0; key < 128; key++)
                sustainState->_sostenutoMemoryKeys[key] = sustainState->_currentKeys[key] || sustainState->_sustainedKeys[key];
        }
        else if (value < 64 && sustainState->_isSostenutoOn)
        {
            sustainState->_isSostenutoOn = false;

            // Remove all keys that have been held by the sostenuto pedal
            for (int key = 0; key < 128; key++)
            {
                if (sustainState->_sostenutoMemoryKeys[key])
                {
                    sustainState->_sostenutoMemoryKeys[key] = false;

                    // And release them if they are not currently triggered or activated by the sustain
                    if (!sustainState->_currentKeys[key] && !sustainState->_sustainedKeys[key])
                        processKeyOff(channel, key);
                }
            }
        }
    }
    else if (numController == 120)
    {
        // All sound off: clear the sustain state
        memset(sustainState, 0, sizeof(Sustain_State));
    }

    // Process the controller change for the current channel
    bool consumed = _listener != nullptr && _listener->processControllerChanged(channel, numController, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processControllerChanged(external, -1, numController, initialValue);

    // In case the listener does not support CC120 / CC123
    if (numController == 120)
    {
        // All sound off: clear the sustain state and stop all notes
        for (int key = 0; key < 128; key++)
            processKeyOff(channel, key);
    }
    else if (numController == 123)
    {
        // All notes off: stop all keys that are not held by the sustain or sostenuto pedal
        for (int key = 0; key < 128; key++)
        {
            if (!sustainState->_sostenutoMemoryKeys[key] && !sustainState->_sustainedKeys[key])
                processKeyOff(channel, key);
        }
    }
}

void MidiProcessor::processKeyOn(int channel, int key, int vel)
{
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    // Initialize the poly pressure value
    _midiStates[channel + 1]._polyPressureValues[key] = vel;

    // Possibly stop the sound if the key is already on: still pressed (as FluidSynth, otherwise the notes would pile
    // up), or held by the sustain or the sostenuto pedal
    bool stopFirst = sustainState->_currentKeys[key] || sustainState->_sustainedKeys[key] ||
                     sustainState->_sostenutoMemoryKeys[key];

    // Key currently activated
    sustainState->_currentKeys[key] = true;

    // Possibly add it to the sustain
    if (sustainState->_isSustainOn)
        sustainState->_sustainedKeys[key] = true;

    // Process the change for the current channel
    bool consumed = false;
    if (_listener != nullptr)
        consumed = stopFirst ? _listener->processKeyRestarted(channel, key, vel) : _listener->processKey(channel, key, vel);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processKeyOn(-1, key, vel);
}

void MidiProcessor::processKeyOff(int channel, int key)
{
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    // Key currently deactivated
    sustainState->_currentKeys[key] = false;

    // Release the key if it is not currently sustained or held by the sostenuto
    bool consumed = false;
    if (!sustainState->_sostenutoMemoryKeys[key] && !sustainState->_sustainedKeys[key])
        consumed = _listener != nullptr && _listener->processKey(channel, key, 0);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processKeyOff(-1, key);
}

void MidiProcessor::processPolyPressureChanged(int channel, int key, int pressure)
{
    // Update the current channel state
    _midiStates[channel + 1]._polyPressureValues[key] = pressure;

    bool consumed = _listener != nullptr && _listener->processPolyPressureChanged(channel, key, pressure);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processPolyPressureChanged(-1, key, pressure);
}

void MidiProcessor::processMonoPressureChanged(int channel, int value)
{
    // Update the current channel state
    _midiStates[channel + 1]._monoPressureValue = value;

    bool consumed = _listener != nullptr && _listener->processMonoPressureChanged(channel, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processMonoPressureChanged(-1, value);
}

void MidiProcessor::processBendChanged(int channel, float value)
{
    // Update the current channel state
    _midiStates[channel + 1]._bendValue = value;

    bool consumed = _listener != nullptr && _listener->processBendChanged(channel, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processBendChanged(-1, value);
}

void MidiProcessor::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    // Possibly update the bend sensitivity value
    if (isRegistered && parameter == 0 && trigger == 4)
        _midiStates[channel + 1]._bendSensitivityValue = static_cast<float>(value >> 7) + 0.01f * static_cast<float>(value & 0x7F);

    bool consumed = _listener != nullptr && _listener->processRPNChanged(channel, parameter, value, isRegistered, trigger);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processRPNChanged(-1, parameter, value, isRegistered, trigger);
}

void MidiProcessor::processProgramChanged(int channel, uint8_t preset)
{
    // Compute the corresponding bank for the current channel
    uint16_t bankNumber = 256 * _midiStates[channel + 1]._controllerValues[32] +
                          _midiStates[channel + 1]._controllerValues[0];

    bool consumed = _listener != nullptr && _listener->processProgramChanged(channel, bankNumber, preset);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processProgramChanged(-1, preset);
}

void MidiProcessor::stopAll()
{
    // Release all keys sustained or held by the sostenuto
    memset((Sustain_State *)_sustainStates, 0, 17 * sizeof(Sustain_State));
    for (int key = 0; key < 128; key++)
        processKeyOff(-1, key);
}

void MidiProcessor::setDefaultControllerValue(int controllerNumber, bool isBipolar, bool isDescending)
{
    int defaultValue = isBipolar ? 64 : (isDescending ? 127 : 0);
    for (int channel = 0; channel <= 16; channel++)
    {
        _midiStates[channel]._controllerDefaultValues[controllerNumber] = defaultValue;
        if (!_midiStates[channel]._controllerValueSpecified[controllerNumber])
            _midiStates[channel]._controllerValues[controllerNumber] = defaultValue;
    }
}

int MidiProcessor::getDefaultControllerValue(int channel, int controllerNumber)
{
    return _midiStates[channel + 1]._controllerDefaultValues[controllerNumber];
}

void MidiProcessor::setBendSensitivityValue(int channel, float value)
{
    _midiStates[channel + 1]._bendSensitivityValue = value;
}

int MidiProcessor::getControllerValue(int channel, int controllerNumber)
{
    return _midiStates[channel + 1]._controllerValues[controllerNumber];
}

float MidiProcessor::getBendValue(int channel)
{
    return _midiStates[channel + 1]._bendValue;
}

float MidiProcessor::getBendSensitivityValue(int channel)
{
    return _midiStates[channel + 1]._bendSensitivityValue;
}

int MidiProcessor::getMonoPressure(int channel)
{
    return _midiStates[channel + 1]._monoPressureValue;
}

int MidiProcessor::getPolyPressure(int channel, int key)
{
    return _midiStates[channel + 1]._polyPressureValues[key];
}
