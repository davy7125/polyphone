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

#include "mididevice.h"
#include <QApplication>
#include "rtmidi/RtMidi.h"
#include "confmanager.h"
#include "soundfontmanager.h"
#include "imidilistener.h"

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    Q_UNUSED(deltatime);
    MidiDevice * instance = static_cast<MidiDevice*>(userData);
    switch (message->at(0) & 0xF0)
    {
    case 0x80: // NOTE OFF
        // First message is the note
        instance->processKeyOff(message->at(0) & 0x0F, message->at(1));
        break;
    case 0x90: // NOTE ON
        // First message is the note, second is velocity
        if (message->at(2) == 0)
            instance->processKeyOff(message->at(0) & 0x0F, message->at(1));
        else
            instance->processKeyOn(message->at(0) & 0x0F, message->at(1), message->at(2));
        break;
    case 0xA0: // AFTERTOUCH
        // First message is the note, second is the pressure
        instance->processPolyPressureChanged(message->at(0) & 0x0F, message->at(1), message->at(2));
        break;
    case 0xB0: // CONTROLLER CHANGE
        // First message is the controller number, second is its value
        instance->processControllerChanged(true, message->at(0) & 0x0F, message->at(1), message->at(2));
        break;
    case 0xC0: // PROGRAM CHANGED
        // First message is the program number
        instance->processProgramChanged(message->at(0) & 0x0F, message->at(1));
        break;
    case 0xD0: // MONO PRESSURE
        // First message is the global pressure
        instance->processMonoPressureChanged(message->at(0) & 0x0F, message->at(1));
        break;
    case 0xE0: // BEND
        // First message is the value
        instance->processBendChanged(message->at(0) & 0x0F, static_cast<float>(((message->at(2) << 7) | message->at(1)) - 8192) / 8192.0f);
        break;
    default:
        // qDebug() << "unknown MIDI event" << (message->at(0) & 0xF0) << "for channel" << (message->at(0) & 0x0F);
        // for (unsigned int i = 0; i < message->size(); i++)
        //     qDebug() << "data #" << i << "=" << message->at(i);
        break;
    }
}

MidiDevice::MidiDevice(ConfManager * configuration) :
    _configuration(configuration),
    _midiIn(nullptr)
{
    memset((MIDI_State *)_midiStates, 0, 17 * sizeof(MIDI_State));
    memset((Sustain_State *)_sustainStates, 0, 17 * sizeof(Sustain_State));

    // Initialize MIDI values
    for (int channel = 0; channel <= 16; channel++)
    {
        _midiStates[channel]._bendSensitivityValue =
            (channel == 0) ? _configuration->getValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", 2.0f).toFloat() : 2.0f;
        for (int i = 0; i < 128; i++)
        {
            // Default value, depending on the CC number
            int defaultValue = 0;
            switch (i)
            {
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

            _midiStates[channel]._controllerValues[i] = defaultValue;
            _midiStates[channel]._controllerValueSpecified[i] = false;
            _midiStates[channel]._controllerValueIsRelative[i] = true; // Default is true, until an "absolute" value is encountered
            _midiStates[channel]._controllerValueIsRelativeCounter[i] = 0;
        }
    }

    // Initialize the connection
    this->openMidiPort(_configuration->getValue(ConfManager::SECTION_MIDI, "index_port", "-1#-1").toString());

    // Link to the soundfont manager to initialize the CC values
    connect(SoundfontManager::getInstance(), SIGNAL(inputModulatorChanged(int,bool,bool)), this, SLOT(onInputModulatorChanged(int,bool,bool)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(check()));
    timer->start(1000);
}

MidiDevice::~MidiDevice()
{
    // Store the wheel sensitivity from channel -1
    _configuration->setValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", _midiStates[0]._bendSensitivityValue);

    if (_midiIn != nullptr)
    {
        _midiIn->closePort();
        delete _midiIn;
    }
}

bool MidiDevice::readConfiguration(QString config)
{
    _api = -1;
    _portNumber = -1;
    QStringList split = config.split('#');
    if (split.count() != 2)
        return false;

    // Api number
    bool ok;
    _api = split[0].toInt(&ok);
    if (!ok)
    {
        _api = -1;
        return false;
    }

    // Port number
    _portNumber = split[1].toInt(&ok);
    if (!ok)
    {
        _api = -1;
        _portNumber = -1;
        return false;
    }

    if (_portNumber == -1 || _api == -1)
    {
        _api = -1;
        _portNumber = -1;
        return false;
    }

    return true;
}

void MidiDevice::onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending)
{
    int defaultValue = isBipolar ? 64 : (isDescending ? 127 : 0);
    for (int channel = 0; channel <= 16; channel++)
        if (!_midiStates[channel]._controllerValueSpecified[controllerNumber])
            _midiStates[channel]._controllerValues[controllerNumber] = defaultValue;
}

QMap<QString, QString> MidiDevice::getMidiList()
{
    QMap<QString, QString> mapRet;

#if defined(__LINUX_ALSASEQ__)
    getMidiList(RtMidi::LINUX_ALSA, &mapRet);
#endif
#if defined(__UNIX_JACK__)
    getMidiList(RtMidi::UNIX_JACK, &mapRet);
#endif
#if defined(__WINDOWS_MM__)
    getMidiList(RtMidi::WINDOWS_MM, &mapRet);
#endif
#if defined(__MACOSX_CORE__)
    getMidiList(RtMidi::MACOSX_CORE, &mapRet);
#endif

    return mapRet;
}

void MidiDevice::getMidiList(int api, QMap<QString, QString> * map)
{
    try
    {
        RtMidiIn * midiInTmp = new RtMidiIn((RtMidi::Api)api, "Polyphone");
        for (unsigned int i = 0; i < midiInTmp->getPortCount(); i++)
        {
            map->insert(QString::number(api) + "#" + QString::number(i),
                        QString(midiInTmp->getPortName(i).c_str()));
        }
        delete midiInTmp;
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
    }
}

void MidiDevice::openMidiPort(QString source)
{
    // Possibly close an existing midi input
    if (_midiIn)
    {
        _midiIn->closePort();
        delete _midiIn;
        _midiIn = nullptr;
    }

    if (readConfiguration(source))
        openMidiPort();
}

void MidiDevice::openMidiPort()
{
    // Create a MIDI input based on the selected API
    try
    {
        _midiIn = new RtMidiIn(static_cast<RtMidi::Api>(_api), "Polyphone");
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        delete _midiIn;
        _midiIn = nullptr;
        return;
    }

    // Associate a callback
    _midiIn->setCallback(&midiCallback, this);

    // Initialize the midi connection
    if (_portNumber < static_cast<int>(_midiIn->getPortCount()))
    {
        try
        {
            _midiIn->openPort(static_cast<unsigned int>(_portNumber));
        }
        catch (std::exception &error)
        {
            Q_UNUSED(error)
            delete _midiIn;
            _midiIn = nullptr;
        }
    }
}

void MidiDevice::check()
{
    // Nothing to do if there is no configured connection
    if (_api == -1 || _portNumber == -1)
        return;

    // Possibly create a MIDI input
    if (_midiIn == nullptr)
    {
        openMidiPort();
        return;
    }

    if (_midiIn->isPortOpen())
    {
        // Check that the port number is still valid
        if ((unsigned int)_portNumber >= _midiIn->getPortCount())
            _midiIn->closePort();
    }
    else
    {
        // Maybe we can open the port
        if ((unsigned int)_portNumber < _midiIn->getPortCount())
            openMidiPort();
    }
}

void MidiDevice::processControllerChanged(bool external, int channel, int numController, int value)
{
    int initialValue = value;
    MIDI_State * midiState = &_midiStates[channel + 1];
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    if (false && external)
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
    if (numController == 101 || numController == 100 || numController == 6 || numController == 38)
    {
        // RPN reception, store the messages since they are grouped by 4
        // http://midi.teragonaudio.com/tech/midispec/rpn.htm
        midiState->_rpnHistoryControllers[midiState->_rpnHistoryPosition] = numController;
        midiState->_rpnHistoryValues[midiState->_rpnHistoryPosition] = value;
        midiState->_rpnHistoryPosition = (midiState->_rpnHistoryPosition + 1) % 4;

        // Try to recognize an update in the bend sensitivity
        if (numController == 38)
        {
            int pos = midiState->_rpnHistoryPosition;
            if (midiState->_rpnHistoryControllers[pos] == 101 && midiState->_rpnHistoryValues[pos] == 0 && // B0 65 00
                midiState->_rpnHistoryControllers[(pos + 1) % 4] == 100 && midiState->_rpnHistoryValues[(pos + 1) % 4] == 0 && // B0 64 00
                midiState->_rpnHistoryControllers[(pos + 2) % 4] == 6 && // B0 06 XX => semitones
                midiState->_rpnHistoryControllers[(pos + 3) % 4] == 38) // B0 38 YY => cents
            {
                float pitch = 0.01f * midiState->_rpnHistoryValues[(pos + 3) % 4] + midiState->_rpnHistoryValues[(pos + 2) % 4];
                processBendSensitivityChanged(channel, pitch);
            }
        }
    }

    if (numController == 64)
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
        // All sound off: clear the sustain state and stop all notes
        memset(sustainState, 0, sizeof(Sustain_State));
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

    // Process the controller change for the current channel
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processControllerChanged(channel, numController, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processControllerChanged(external, -1, numController, initialValue);
}

void MidiDevice::processKeyOn(int channel, int key, int vel)
{
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    // Initialize the poly pressure value
    _midiStates[channel + 1]._polyPressureValues[key] = vel;

    // Key currently activated
    sustainState->_currentKeys[key] = true;

    // Possibly stop the sound if the key is already on
    bool stopFirst = sustainState->_sustainedKeys[key] || sustainState->_sostenutoMemoryKeys[key];

    // Possibly add it to the sustain
    if (sustainState->_isSustainOn)
        sustainState->_sustainedKeys[key] = true;

    // Process the change for the current channel
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (stopFirst)
            _listeners[i]->processKey(channel, key, 0);
        consumed |= _listeners[i]->processKey(channel, key, vel);
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processKeyOn(-1, key, vel);
}

void MidiDevice::processKeyOff(int channel, int key)
{
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    // Key currently deactivated
    sustainState->_currentKeys[key] = false;

    // Release the key if it is not currently sustained or held by the sostenuto
    bool consumed = false;
    if (!sustainState->_sostenutoMemoryKeys[key] && !sustainState->_sustainedKeys[key])
    {
        for (int i = 0; i < _listeners.size(); ++i)
            consumed |= _listeners[i]->processKey(channel, key, 0);
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processKeyOff(-1, key);
}

void MidiDevice::processPolyPressureChanged(int channel, int key, int pressure)
{
    // Update the current channel state
    _midiStates[channel + 1]._polyPressureValues[key] = pressure;

    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processPolyPressureChanged(channel, key, pressure);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processPolyPressureChanged(-1, key, pressure);
}

void MidiDevice::processMonoPressureChanged(int channel, int value)
{
    // Update the current channel state
    _midiStates[channel + 1]._monoPressureValue = value;

    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processMonoPressureChanged(channel, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processMonoPressureChanged(-1, value);
}

void MidiDevice::processBendChanged(int channel, float value)
{
    // Update the current channel state
    _midiStates[channel + 1]._bendValue = value;

    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processBendChanged(channel, value);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processBendChanged(-1, value);
}

void MidiDevice::processBendSensitivityChanged(int channel, float semitones)
{
    // Update the current channel state
    _midiStates[channel + 1]._bendSensitivityValue = semitones;

    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processBendSensitivityChanged(channel, semitones);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processBendSensitivityChanged(-1, semitones);
}

void MidiDevice::processProgramChanged(int channel, quint8 preset)
{
    // Compute the corresponding bank for the current channel
    quint16 bankNumber = 256 * _midiStates[channel + 1]._controllerValues[32] +
                         _midiStates[channel + 1]._controllerValues[0];

    // Update the current channel state
    _midiStates[channel + 1]._preset = preset;

    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processProgramChanged(channel, bankNumber, preset);

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1 && !consumed)
        processProgramChanged(-1, preset);
}

void MidiDevice::stopAll()
{
    // Release all keys sustained or held by the sostenuto
    memset((Sustain_State *)_sustainStates, 0, 17 * sizeof(Sustain_State));
    for (int key = 0; key < 128; key++)
        processKeyOff(-1, key);
}

int MidiDevice::getControllerValue(int channel, int controllerNumber)
{
    return _midiStates[channel + 1]._controllerValues[controllerNumber];
}

float MidiDevice::getBendValue(int channel)
{
    return _midiStates[channel + 1]._bendValue;
}

float MidiDevice::getBendSensitivityValue(int channel)
{
    return _midiStates[channel + 1]._bendSensitivityValue;
}

int MidiDevice::getMonoPressure(int channel)
{
    return _midiStates[channel + 1]._monoPressureValue;
}

int MidiDevice::getPolyPressure(int channel, int key)
{
    return _midiStates[channel + 1]._polyPressureValues[key];
}

void MidiDevice::addListener(IMidiListener * listener, int priority)
{
    // Possibly insert a listener before another one
    for (int i = 0; i < _listenerPriorities.size(); i++)
    {
        if (_listenerPriorities[i] < priority)
        {
            _listenerPriorities.insert(i, priority);
            _listeners.insert(i, listener);
            return;
        }
    }

    // Or add it at the end of the list
    _listenerPriorities.append(priority);
    _listeners.append(listener);
}

void MidiDevice::removeListener(IMidiListener * listener)
{
    int pos = _listeners.indexOf(listener);
    if (pos != -1)
    {
        _listenerPriorities.removeAt(pos);
        _listeners.removeAt(pos);
    }
}
