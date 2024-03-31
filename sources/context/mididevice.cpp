/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "mididevice.h"
#include "controllerevent.h"
#include "noteevent.h"
#include "bendevent.h"
#include "programevent.h"
#include "monopressureevent.h"
#include "polypressureevent.h"
#include "dialogkeyboard.h"
#include "pianokeybdcustom.h"
#include "controllerarea.h"
#include "confmanager.h"
#include <QApplication>
#include "synth.h"
#include "extensionmanager.h"
#include "soundfontmanager.h"

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

MidiDevice::MidiDevice(ConfManager * configuration, Synth *synth) :
    _dialogKeyboard(nullptr),
    _configuration(configuration),
    _midiin(nullptr),
    _synth(synth)
{
    memset((MIDI_State*)_midiStates, 0, 17 * sizeof(MIDI_State));

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
    connect(SoundfontManager::getInstance(), SIGNAL(inputModulatorChanged(int, bool, bool)), this, SLOT(onInputModulatorChanged(int, bool, bool)));
}

MidiDevice::~MidiDevice()
{
    // Store the wheel sensitivity from channel -1
    _configuration->setValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", _midiStates[0]._bendSensitivityValue);

    if (_midiin != nullptr)
    {
        _midiin->closePort();
        delete _midiin;
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

void MidiDevice::getMidiList(RtMidi::Api api, QMap<QString, QString> * map)
{
    try
    {
        RtMidiIn * midiInTmp = new RtMidiIn(api, "Polyphone");
        for (unsigned int i = 0; i < midiInTmp->getPortCount(); i++)
        {
            map->insert(QString::number(static_cast<int>(api)) + "#" + QString::number(i),
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
    if (_midiin)
    {
        _midiin->closePort();
        delete _midiin;
        _midiin = nullptr;
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
        _midiin = new RtMidiIn(static_cast<RtMidi::Api>(api), "Polyphone");
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        delete _midiin;
        _midiin = nullptr;
        return;
    }

    // Associate a callback
    _midiin->setCallback(&midiCallback, this);

    // Initialize the midi connection
    if (portNumber < static_cast<int>(_midiin->getPortCount()))
    {
        try
        {
            _midiin->openPort(static_cast<unsigned int>(portNumber));
        }
        catch (std::exception &error)
        {
            Q_UNUSED(error)
            delete _midiin;
            _midiin = nullptr;
        }
    }
}

PianoKeybdCustom * MidiDevice::keyboard()
{
    return _dialogKeyboard != nullptr ? _dialogKeyboard->getKeyboard() : nullptr;
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
        ControllerEvent *controllerEvent = dynamic_cast<ControllerEvent *>(event);
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
    // Update the current channel
    _midiStates[channel + 1]._controllerValues[numController] = value;
    _midiStates[channel + 1]._controllerValueSpecified[numController] = true;
    if (numController == 101 || numController == 100 || numController == 6 || numController == 38)
    {
        // RPN reception, store the messages since they are grouped by 4
        // http://midi.teragonaudio.com/tech/midispec/rpn.htm
        _midiStates[channel + 1]._rpnHistoryControllers[_midiStates[channel + 1]._rpnHistoryPosition] = numController;
        _midiStates[channel + 1]._rpnHistoryValues[_midiStates[channel + 1]._rpnHistoryPosition] = value;
        _midiStates[channel + 1]._rpnHistoryPosition = (_midiStates[channel + 1]._rpnHistoryPosition + 1) % 4;

        // Try to recognize an update in the bend sensitivity
        if (numController == 38)
        {
            int pos = _midiStates[channel + 1]._rpnHistoryPosition;
            if (_midiStates[channel + 1]._rpnHistoryControllers[pos] == 101 && _midiStates[channel + 1]._rpnHistoryValues[pos] == 0 && // B0 65 00
                    _midiStates[channel + 1]._rpnHistoryControllers[(pos + 1) % 4] == 100 && _midiStates[channel + 1]._rpnHistoryValues[(pos + 1) % 4] == 0 && // B0 64 00
                    _midiStates[channel + 1]._rpnHistoryControllers[(pos + 2) % 4] == 6 && // B0 06 XX => semitones
                    _midiStates[channel + 1]._rpnHistoryControllers[(pos + 3) % 4] == 38) // B0 38 YY => cents
            {
                float pitch = 0.01f * _midiStates[channel + 1]._rpnHistoryValues[(pos + 3) % 4] + _midiStates[channel + 1]._rpnHistoryValues[(pos + 2) % 4];
                processBendSensitivityChanged(channel, pitch);
            }
        }
    }

    if (channel != -1)
    {
        // Trigger a possible extension
        if (ExtensionManager::midi()->processControllerChanged(channel, numController, value))
            return;

        // The change has not been consumed, update channel -1 and the keyboard
        _midiStates[0]._controllerValues[numController] = value;
        _midiStates[0]._controllerValueSpecified[numController] = true;
        _dialogKeyboard->getControllerArea()->updateController(numController, value);
    }

    if (numController == 64)
    {
        // Sustain pedal
        if (value >= 64 && !_isSustainOn)
        {
            _isSustainOn = true;
            
            // All current keys are now sustained
            for (int key = 0; key < 128; key++)
                _sustainedKeys[key] = _currentKeys[key] || _sostenutoMemoryKeys[key];
        }
        else if (value < 64 && _isSustainOn)
        {
            _isSustainOn = false;
            
            // Remove all keys that have been previously sustained
            for (int key = 0; key < 128; key++)
            {
                if (_sustainedKeys[key])
                {
                    _sustainedKeys[key] = false;

                    // And release them if they are not currently triggered or held by the sostenuto
                    if (!_currentKeys[key] && !_sostenutoMemoryKeys[key])
                        processKeyOff(-1, key);
                }
            }
        }
    }
    else if (numController == 66)
    {
        // Sostenuto pedal
        if (value < 64 && _isSostenutoOn)
        {
            _isSostenutoOn = false;
            
            // Remove all keys that have been held by the sostenuto pedal
            for (int key = 0; key < 128; key++)
            {
                if (_sostenutoMemoryKeys[key])
                {
                    _sostenutoMemoryKeys[key] = false;

                    // And release them if they are not currently triggered or activated by the sustain
                    if (!_currentKeys[key] && !_sustainedKeys[key])
                        processKeyOff(-1, key);
                }
            }
        }
        else if (value >= 64 && !_isSostenutoOn)
        {
            _isSostenutoOn = true;
            
            // All current keys are now held by the sostenuto
            for (int key = 0; key < 128; key++)
                _sostenutoMemoryKeys[key] = _currentKeys[key] || _sustainedKeys[key];
        }
    }
}

void MidiDevice::processKeyOn(int channel, int key, int vel)
{
    // Initialize the poly pressure value
    _midiStates[channel + 1]._polyPressureValues[key] = vel;

    if (channel != -1)
    {
        // Trigger a possible extension
        if (ExtensionManager::midi()->processKeyOn(channel, key, vel))
            return;

        // The change has not been consumed, update channel -1 and the keyboard
        _midiStates[0]._polyPressureValues[key] = vel;

        if (_dialogKeyboard)
        {
            _dialogKeyboard->updateKeyPlayed(key, vel);
            _dialogKeyboard->getKeyboard()->inputNoteOn(key, vel);
        }
    }

    if (key != -1)
    {
        // Key currently activated
        _currentKeys[key] = true;

        // Possibly add it to the sustain
        if (_isSustainOn)
            _sustainedKeys[key] = true;
    }

    // Notify about a key being played
    emit(keyPlayed(key, vel));
}

void MidiDevice::processKeyOff(int channel, int key)
{
    if (channel != -1)
    {
        // Trigger a possible extension
        if (ExtensionManager::midi()->processKeyOff(channel, key))
            return;

        // The change has not been consumed, update the virtual keyboard
        if (_dialogKeyboard)
        {
            _dialogKeyboard->updateKeyPlayed(key, 0);
            _dialogKeyboard->getKeyboard()->inputNoteOff(key);
            _dialogKeyboard->getKeyboard()->removeCurrentRange(key);
        }
    }

    // Stop a sample playback if key is -1
    if (key == -1)
        _synth->play(EltID(), -1, -1, 0);
    else
    {
        // Key currently deactivated
        _currentKeys[key] = false;

        // Release the key if it is not currently sustained or held by the sostenuto
        if (!_sostenutoMemoryKeys[key] && !_sustainedKeys[key])
            emit(keyPlayed(key, 0));
    }
}

void MidiDevice::processPolyPressureChanged(int channel, int key, int pressure)
{
    // Update the channel state
    _midiStates[channel + 1]._polyPressureValues[key] = pressure;

    if (channel != -1)
    {
        // Trigger a possible extension
        if (!ExtensionManager::midi()->processPolyPressureChanged(channel, key, pressure))
        {
            // The change has not been consumed, update channel -1 and the virtual keyboard
            _midiStates[0]._polyPressureValues[key] = pressure;
            _dialogKeyboard->updatePolyPressure(key, pressure);
        }
    }
}

void MidiDevice::processMonoPressureChanged(int channel, int value)
{
    // Update the channel state
    _midiStates[channel + 1]._monoPressureValue = value;

    if (channel != -1)
    {
        // Trigger a possible extension
        if (!ExtensionManager::midi()->processMonoPressureChanged(channel, value))
        {
            // The change has not been consumed, update channel -1 and the virtual keyboard
            _midiStates[0]._monoPressureValue = value;
            _dialogKeyboard->getControllerArea()->updateMonoPressure(value);
        }
    }
}

void MidiDevice::processBendChanged(int channel, float value)
{
    // Update the channel state
    _midiStates[channel + 1]._bendValue = value;

    if (channel != -1)
    {
        // Trigger a possible extension
        if (!ExtensionManager::midi()->processBendChanged(channel, value))
        {
            // The change has not been consumed, update channel -1 and the virtual keyboard
            _midiStates[0]._bendValue = value;
            _dialogKeyboard->getControllerArea()->updateBend(value);
        }
    }
}

void MidiDevice::processBendSensitivityChanged(int channel, float semitones)
{
    // Update the channel state
    _midiStates[channel + 1]._bendSensitivityValue = semitones;

    if (channel != -1)
    {
        // Trigger a possible extension
        if (!ExtensionManager::midi()->processBendSensitivityChanged(channel, semitones))
        {
            // The change has not been consumed, update channel -1 and the virtual keyboard
            _midiStates[0]._bendSensitivityValue = semitones;
            _dialogKeyboard->getControllerArea()->updateBendSensitivity(semitones);
        }
    }
}

void MidiDevice::stopAll()
{
    // Release all keys sustained or held by the sostenuto
    _isSustainOn = false;
    for (int key = 0; key < 128; key++)
    {
        _currentKeys[key] = false;
        _sustainedKeys[key] = false;
        _sostenutoMemoryKeys[key] = false;

        processKeyOff(-1, key);
    }

    // Reset the keyboard
    _dialogKeyboard->getKeyboard()->clearCustomization();

    // Stop all voices
    _synth->stop(false);
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
