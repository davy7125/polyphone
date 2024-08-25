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
#include "controllerevent.h"
#include "noteevent.h"
#include "bendevent.h"
#include "programevent.h"
#include "monopressureevent.h"
#include "polypressureevent.h"
#include "confmanager.h"
#include "soundfontmanager.h"
#include "imidilistener.h"

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    Q_UNUSED(deltatime);

    // Create an event
    QEvent* ev = nullptr;
    quint8 channel = message->at(0) & 0x0F;
    quint8 status = message->at(0) & 0xF0;

    switch (status)
    {
    case 0x80: case 0x90: // NOTE ON or NOTE OFF
        // First message is the note, second is velocity
        if (status == 0x80 || message->at(2) == 0)
            ev = new NoteEvent(channel, message->at(1), 0);
        else
            ev = new NoteEvent(channel, message->at(1), message->at(2));
        break;
    case 0xA0: // AFTERTOUCH
        // First message is the note, second is the pressure
        ev = new PolyPressureEvent(channel, message->at(1), message->at(2));
        break;
    case 0xB0: // CONTROLLER CHANGE
        // First message is the controller number, second is its value
        ev = new ControllerEvent(channel, message->at(1), message->at(2));
        break;
    case 0xC0: // PROGRAM CHANGED
        // First message is the program number
        ev = new ProgramEvent(channel, message->at(1));
        break;
    case 0xD0: // MONO PRESSURE
        // First message is the global pressure
        ev = new MonoPressureEvent(channel, message->at(1));
        break;
    case 0xE0: // BEND
        // First message is the value
        ev = new BendEvent(channel, message->at(1), message->at(2));
        break;
    default:
        // Nothing
        break;
    }

    if (ev)
    {
        // Get the midi device instance and post the event
        MidiDevice * instance = static_cast<MidiDevice*>(userData);
        QApplication::postEvent(instance, ev);
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
        _midiStates[channel]._bendSensitivityValue = (channel == 0) ?
                    _configuration->getValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", 2.0f).toFloat() : 2.0f;
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
        }
    }

    // Initialize the connection
    this->openMidiPort(_configuration->getValue(ConfManager::SECTION_MIDI, "index_port", "-1#-1").toString());

    // Link to the soundfont manager to initialize the CC values
    connect(SoundfontManager::getInstance(), SIGNAL(inputModulatorChanged(int,bool,bool)), this, SLOT(onInputModulatorChanged(int,bool,bool)));
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

    // Get the api and the port number
    QStringList split = source.split('#');
    if (split.count() != 2)
        return;
    bool ok;
    int api = split[0].toInt(&ok);
    if (!ok)
        return;
    int portNumber = split[1].toInt(&ok);
    if (!ok)
        return;
    if (portNumber == -1 || api == -1)
        return;

    // Create a MIDI input based on the selected API
    try
    {
        _midiIn = new RtMidiIn(static_cast<RtMidi::Api>(api), "Polyphone");
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
    if (portNumber < static_cast<int>(_midiIn->getPortCount()))
    {
        try
        {
            _midiIn->openPort(static_cast<unsigned int>(portNumber));
        }
        catch (std::exception &error)
        {
            Q_UNUSED(error)
            delete _midiIn;
            _midiIn = nullptr;
        }
    }
}

void MidiDevice::customEvent(QEvent * event)
{
    if (event->type() == QEvent::User)
    {
        // Note on or off
        NoteEvent *noteEvent = dynamic_cast<NoteEvent *>(event);
        if (noteEvent->getVelocity() > 0)
            this->processKeyOn(noteEvent->getChannel(), noteEvent->getNote(), noteEvent->getVelocity());
        else
            this->processKeyOff(noteEvent->getChannel(), noteEvent->getNote());
        event->accept();
    }
    else if (event->type() == QEvent::User + 1)
    {
        // A controller value changed
        ControllerEvent * controllerEvent = dynamic_cast<ControllerEvent *>(event);
        processControllerChanged(controllerEvent->getChannel(), controllerEvent->getNumController(), controllerEvent->getValue());
        event->accept();
    }
    else if (event->type() == QEvent::User + 2)
    {
        // The pressure of a note changed
        PolyPressureEvent *pressureEvent = dynamic_cast<PolyPressureEvent *>(event);
        processPolyPressureChanged(pressureEvent->getChannel(), pressureEvent->getNote(), pressureEvent->getPressure());
        event->accept();
    }
    else if (event->type() == QEvent::User + 3)
    {
        // The global pressure changed
        MonoPressureEvent *pressureEvent = dynamic_cast<MonoPressureEvent *>(event);
        processMonoPressureChanged(pressureEvent->getChannel(), pressureEvent->getPressure());
        event->accept();
    }
    else if (event->type() == QEvent::User + 4)
    {
        // The bend changed
        BendEvent *bendEvent = dynamic_cast<BendEvent *>(event);
        processBendChanged(bendEvent->getChannel(), bendEvent->getValue());
        event->accept();
    }
    else if (event->type() == QEvent::User + 5)
    {
        // The program changed (no need for now)
        //ProgramEvent *programEvent = dynamic_cast<ProgramEvent *>(event);
        event->accept();
    }
}

void MidiDevice::processControllerChanged(int channel, int numController, int value)
{
    MIDI_State * midiState = &_midiStates[channel + 1];
    Sustain_State * sustainState = &_sustainStates[channel + 1];

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

    // Process the controller change for the current channel
    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (_listeners[i]->processControllerChanged(channel, numController, value))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processControllerChanged(-1, numController, value);
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
    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (stopFirst)
            _listeners[i]->processKey(channel, key, 0);
        if (_listeners[i]->processKey(channel, key, vel))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processKeyOn(-1, key, vel);
}

void MidiDevice::processKeyOff(int channel, int key)
{
    Sustain_State * sustainState = &_sustainStates[channel + 1];

    // Key currently deactivated
    sustainState->_currentKeys[key] = false;

    // Release the key if it is not currently sustained or held by the sostenuto
    if (!sustainState->_sostenutoMemoryKeys[key] && !sustainState->_sustainedKeys[key])
    {
        for (int i = 0; i < _listeners.size(); ++i)
        {
            if (_listeners[i]->processKey(channel, key, 0))
                return;
        }
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processKeyOff(-1, key);
}

void MidiDevice::processPolyPressureChanged(int channel, int key, int pressure)
{
    // Update the current channel state
    _midiStates[channel + 1]._polyPressureValues[key] = pressure;

    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (_listeners[i]->processPolyPressureChanged(channel, key, pressure))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processPolyPressureChanged(-1, key, pressure);
}

void MidiDevice::processMonoPressureChanged(int channel, int value)
{
    // Update the current channel state
    _midiStates[channel + 1]._monoPressureValue = value;

    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (_listeners[i]->processMonoPressureChanged(channel, value))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processMonoPressureChanged(-1, value);
}

void MidiDevice::processBendChanged(int channel, float value)
{
    // Update the current channel state
    _midiStates[channel + 1]._bendValue = value;

    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (_listeners[i]->processBendChanged(channel, value))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processBendChanged(-1, value);
}

void MidiDevice::processBendSensitivityChanged(int channel, float semitones)
{
    // Update the current channel state
    _midiStates[channel + 1]._bendSensitivityValue = semitones;

    for (int i = 0; i < _listeners.size(); ++i)
    {
        if (_listeners[i]->processBendSensitivityChanged(channel, semitones))
            return;
    }

    // And possibly update channel -1 if the change has not been consumed
    if (channel != -1)
        processBendSensitivityChanged(-1, semitones);
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
