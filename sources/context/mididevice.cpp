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

#include "mididevice.h"
#include "RtMidi.h"
#include "controllerevent.h"
#include "noteevent.h"
#include "bendevent.h"
#include "programevent.h"
#include "monopressureevent.h"
#include "polypressureevent.h"
#include "pianokeybdcustom.h"
#include "controllerarea.h"
#include "confmanager.h"
#include <QApplication>
#include "synth.h"

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    Q_UNUSED(deltatime);

    // Create an event
    QEvent* ev = nullptr;
    //unsigned char channel = message->at(0) & 0x0F;
    unsigned char status = message->at(0) & 0xF0;

    switch (status)
    {
    case 0x80: case 0x90: // NOTE ON or NOTE OFF
        // First message is the note, second is velocity
        if (status == 0x80 || message->at(2) == 0)
            ev = new NoteEvent(message->at(1), 0);
        else
            ev = new NoteEvent(message->at(1), message->at(2));
        break;
    case 0xA0: // AFTERTOUCH
        // First message is the note, second is the pressure
        ev = new PolyPressureEvent(message->at(1), message->at(2));
        break;
    case 0xB0: // CONTROLLER CHANGE
        // First message is the controller number, second is its value
        ev = new ControllerEvent(message->at(1), message->at(2));
        break;
    case 0xC0: // PROGRAM CHANGED
        // First message is the program number
        ev = new ProgramEvent(message->at(1));
        break;
    case 0xD0: // MONO PRESSURE
        // First message is the global pressure
        ev = new MonoPressureEvent(message->at(1));
        break;
    case 0xE0: // BEND
        // First message is the value
        ev = new BendEvent(message->at(1));
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
    _keyboard(nullptr),
    _controllerArea(nullptr),
    _configuration(configuration),
    _synth(synth),
    _bendValue(64),
    _isSustainOn(false),
    _isSostenutoOn(false)
{
    // Initialize MIDI values
    _bendSensitivityValue = _configuration->getValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", 2.0).toDouble();
    _monoPressureValue = 0; // Always starts with 0
    for (int i = 0; i < 128; i++)
    {
        // Default value, depending on the CC number
        int defaultValue = 64;
        bool forceDefault = false;
        switch (i)
        {
        case 0: // Bank select
        case 1: // Modulation wheel
        case 2: // Breath controller
        case 12: case 13: // Effect controllers
        case 80: case 81: case 82: case 83: // On/Off switch
        case 91: case 92: case 93: case 94: case 95: // Effect amount
            defaultValue = 0;
            break;
        case 4: case 64: case 65: case 66: case 67: case 68: case 69: // Pedals
            defaultValue = 0;
            forceDefault = true;
            break;
        case 7: case 11: // Main volume, expression
            defaultValue = 127;
            break;
        default:
            break;
        }

        _controllerValues[i] = forceDefault ?
                    defaultValue : _configuration->getValue(ConfManager::SECTION_MIDI, "CC_" + QString("%1").arg(i, 3, 10, QChar('0')), defaultValue).toInt();
    }

    // MIDI connection
    try
    {
#if defined(__LINUX_ALSASEQ__)
        _midiin = new RtMidiIn(RtMidi::LINUX_ALSA, "Polyphone");
#endif
#if defined(__WINDOWS_MM__)
        _midiin = new RtMidiIn(RtMidi::WINDOWS_MM, "Polyphone");
#endif
#if defined(__MACOSX_CORE__)
        _midiin = new RtMidiIn(RtMidi::MACOSX_CORE, "Polyphone");
#endif
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        _midiin = nullptr;
    }
    if (_midiin)
    {
        _midiin->ignoreTypes(false, false, false);
        _midiin->setCallback(&midiCallback, this);
    }

    this->openMidiPort(_configuration->getValue(ConfManager::SECTION_MIDI, "index_port", -1).toInt());
}

MidiDevice::~MidiDevice()
{
    // Store some MIDI values
    _configuration->setValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", _bendSensitivityValue);
    for (int i = 0; i < 128; i++)
        _configuration->setValue(ConfManager::SECTION_MIDI, "CC_" + QString("%1").arg(i, 3, 10, QChar('0')), _controllerValues[i]);

    delete _midiin;
}

QStringList MidiDevice::getMidiList()
{
    QStringList listRet;
    if (_midiin)
    {
        for (unsigned int i = 0; i < _midiin->getPortCount(); i++)
            listRet.append(QString(_midiin->getPortName(i).c_str()));
    }
    return listRet;
}

void MidiDevice::openMidiPort(int index)
{
    if (_midiin)
    {
        _midiin->closePort();
        if (index < static_cast<int>(_midiin->getPortCount()) && index != -1)
        {
            try
            {
                _midiin->openPort(static_cast<unsigned int>(index));
            }
            catch (std::exception &error)
            {
                Q_UNUSED(error)
            }
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
            this->processKeyOn(noteEvent->getNote(), noteEvent->getVelocity(), true);
        else
            this->processKeyOff(noteEvent->getNote(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 1)
    {
        // A controller value changed
        ControllerEvent *controllerEvent = dynamic_cast<ControllerEvent *>(event);
        processControllerChanged(controllerEvent->getNumController(), controllerEvent->getValue(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 2)
    {
        // The pressure of a note changed
        PolyPressureEvent *pressureEvent = dynamic_cast<PolyPressureEvent *>(event);
        processPolyPressureChanged(pressureEvent->getNote(), pressureEvent->getPressure(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 3)
    {
        // The global pressure changed
        MonoPressureEvent *pressureEvent = dynamic_cast<MonoPressureEvent *>(event);
        processMonoPressureChanged(pressureEvent->getPressure(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 4)
    {
        // The bend changed
        BendEvent *bendEvent = dynamic_cast<BendEvent *>(event);
        processBendChanged(bendEvent->getValue(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 5)
    {
        // The program changed (no need for now)
        //ProgramEvent *programEvent = dynamic_cast<ProgramEvent *>(event);
        event->accept();
    }
}

void MidiDevice::processControllerChanged(int numController, int value, bool syncControllerArea)
{
    _mutexValues.lock();
    _controllerValues[numController] = value;
    _mutexValues.unlock();

    if (numController == 64)
    {
        // Sustain pedal
        _isSustainOn = (value >= 64);
        if (!_isSustainOn)
        {
            // Release all keys that have been sustained by the sustained pedal
            while (_sustainedKeys.size())
            {
                int key = _sustainedKeys.takeFirst();
                if (!_isSostenutoOn || !_sostenutoMemoryKeys.contains(key))
                    processKeyOff(key, true);
            }
        }
    }
    else if (numController == 66)
    {
        // Sostenuto pedal
        if (_isSostenutoOn != (value >= 64))
        {
            _isSostenutoOn = (value >= 64);
            if (!_isSostenutoOn)
            {
                // Release all keys that have been sustained by the sostenuto pedal
                while (_sostenutoMemoryKeys.size())
                {
                    int key = _sostenutoMemoryKeys.takeFirst();
                    if (!_isSustainOn)
                        processKeyOff(key, true);
                    else if (!_sustainedKeys.contains(key))
                        _sustainedKeys << key; // Will be released later with the sustained pedal
                }
            }
        }
    }
    else if (numController == 101 || numController == 100 || numController == 6 || numController == 38)
    {
        // RPN reception, store the messages since they are grouped by 4
        // http://midi.teragonaudio.com/tech/midispec/rpn.htm
        _rpnHistory << QPair<int, int>(numController, value);
        if (_rpnHistory.size() > 4)
            _rpnHistory.removeFirst();
        if (numController == 38 && _rpnHistory.size() == 4)
        {
            // Bend sensitivity?
            if (_rpnHistory[0].first == 101 && _rpnHistory[0].second == 0 && // B0 65 00
                    _rpnHistory[1].first == 100 && _rpnHistory[1].second == 0 && // B0 64 00
                    _rpnHistory[2].first == 6 && // B0 06 XX => semitones
                    _rpnHistory[3].first == 38) // B0 38 YY => cents
            {
                double pitch = 0.01 * _rpnHistory[3].second + _rpnHistory[2].second;
                processBendSensitivityChanged(pitch, syncControllerArea);
            }
        }
    }

    if (syncControllerArea)
        _controllerArea->updateController(numController, value);
}

void MidiDevice::processKeyOn(int key, int vel, bool syncKeyboard)
{
    // Possibly initialize the poly pressure value
    _mutexValues.lock();
    if (!_polyPressureValues.contains(key) || _polyPressureValues[key] == -1)
        _polyPressureValues[key] = vel;
    _mutexValues.unlock();

    // Display the note on the keyboard
    if (_keyboard && syncKeyboard)
        _keyboard->inputNoteOn(key, vel);

    // Update the memory list for the sostenuto
    if (!_isSostenutoOn && !_sostenutoMemoryKeys.contains(key))
        _sostenutoMemoryKeys << key;

    // Notify about a key being played
    emit(keyPlayed(key, vel));
}

void MidiDevice::processKeyOff(int key, bool syncKeyboard)
{
    // Remove the note from the keyboard
    if (_keyboard && syncKeyboard)
    {
        _keyboard->inputNoteOff(key);
        _keyboard->removeCurrentRange(key);
    }

    // Stop a sample reading if key is -1
    if (key == -1)
        _synth->play(EltID(), -1, 0);
    else if (_isSustainOn)
    {
        // Add the key to the list of keys to release later when the pedal is released
        if (!_sustainedKeys.contains(key))
            _sustainedKeys << key;
    }
    else if (!_isSostenutoOn || !_sostenutoMemoryKeys.contains(key))
    {
        // Update the sostenuto memory
        if (!_isSostenutoOn)
            _sostenutoMemoryKeys.removeAll(key);

        // Notify about a key being not played anymore
        emit(keyPlayed(key, 0));
    }
}

void MidiDevice::processPolyPressureChanged(int key, int pressure, bool syncKeyboard)
{
    Q_UNUSED(syncKeyboard) // No synchronization with the keyboard

    _mutexValues.lock();
    _polyPressureValues[key] = pressure;
    _mutexValues.unlock();

    emit(polyPressureChanged(key, pressure));
}

void MidiDevice::processMonoPressureChanged(int value, bool syncControllerArea)
{
    _mutexValues.lock();
    _monoPressureValue = value;
    _mutexValues.unlock();

    emit(monoPressureChanged(value));
    if (syncControllerArea)
        _controllerArea->updateMonoPressure(value);
}

void MidiDevice::processBendChanged(int value, bool syncControllerArea)
{
    _mutexValues.lock();
    _bendValue = value;
    _mutexValues.unlock();

    emit(bendChanged(value));
    if (syncControllerArea)
        _controllerArea->updateBend(value);
}

void MidiDevice::processBendSensitivityChanged(double semitones, bool syncControllerArea)
{
    _mutexValues.lock();
    _bendSensitivityValue = semitones;
    _mutexValues.unlock();

    emit(bendSensitivityChanged(semitones));
    if (syncControllerArea)
        _controllerArea->updateBendSensitivity(semitones);
}

void MidiDevice::setKeyboard(PianoKeybdCustom * keyboard)
{
    connect(keyboard, SIGNAL(noteOn(int,int)), this, SLOT(processKeyOn(int,int)));
    connect(keyboard, SIGNAL(noteOff(int)), this, SLOT(processKeyOff(int)));
    connect(keyboard, SIGNAL(polyPressureChanged(int,int)), this, SLOT(processPolyPressureChanged(int, int)));
    _keyboard = keyboard;
}

void MidiDevice::setControllerArea(ControllerArea * controllerArea)
{
    connect(controllerArea, SIGNAL(monoPressureChanged(int)), this, SLOT(processMonoPressureChanged(int)));
    connect(controllerArea, SIGNAL(controllerChanged(int,int)), this, SLOT(processControllerChanged(int,int)));
    connect(controllerArea, SIGNAL(bendChanged(int)), this, SLOT(processBendChanged(int)));
    connect(controllerArea, SIGNAL(bendSensitivityChanged(double)), this, SLOT(processBendSensitivityChanged(double)));
    _controllerArea = controllerArea;
}

void MidiDevice::stopAll()
{
    // Release all keys sustained
    _isSustainOn = false;
    while (_sustainedKeys.size())
        processKeyOff(_sustainedKeys.takeFirst(), true);
    if (_isSostenutoOn)
    {
        _isSostenutoOn = false;
        while (_sostenutoMemoryKeys.size())
            processKeyOff(_sostenutoMemoryKeys.takeFirst(), true);
    }

    // Reset the keyboard
    _keyboard->clearCustomization();

    // Stop all voices
    _synth->stop();
}

int MidiDevice::getControllerValue(int controllerNumber)
{
    _mutexValues.lock();
    int result = _controllerValues.contains(controllerNumber) ? _controllerValues[controllerNumber] : -1;
    _mutexValues.unlock();
    return result;
}

int MidiDevice::getBendValue()
{
    _mutexValues.lock();
    int result = _bendValue;
    _mutexValues.unlock();
    return result;
}

double MidiDevice::getBendSensitivityValue()
{
    _mutexValues.lock();
    double result = _bendSensitivityValue;
    _mutexValues.unlock();
    return result;
}

int MidiDevice::getMonoPressure()
{
    _mutexValues.lock();
    int result = _monoPressureValue;
    _mutexValues.unlock();
    return result;
}

int MidiDevice::getPolyPressure(int key)
{
    _mutexValues.lock();
    int result = _polyPressureValues.contains(key) ? _polyPressureValues[key] : -1;
    _mutexValues.unlock();
    return result;
}
