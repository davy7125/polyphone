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
#include "pianokeybdcustom.h"
#include "confmanager.h"
#include <QApplication>
#include "synth.h"

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
    Q_UNUSED(deltatime);

    // Constantes
    int STATUS_NOTEOFF    = 0x80;
    int STATUS_NOTEON     = 0x90;
    int STATUS_CONTROLLER = 0xB0;
    //int MASK_CHANNEL   = 0x0f;
    int MASK_STATUS    = 0xF0;

    // Récupération de l'instance de PianoKeybdCustom
    MidiDevice * instance = static_cast<MidiDevice*>(userData);

    // Create an event
    QEvent* ev = nullptr;
    //unsigned char channel = message->at(0) & MASK_CHANNEL;
    unsigned char status = message->at(0) & MASK_STATUS;
    if (status == STATUS_NOTEON || status == STATUS_NOTEOFF)
    {
        unsigned char midi_note = message->at(1);
        unsigned char vel = message->at(2);
        if ((status == STATUS_NOTEOFF) || (vel == 0))
            ev = new NoteEvent(midi_note, 0);
        else
            ev = new NoteEvent(midi_note, vel);
    }
    else if (status == STATUS_CONTROLLER)
    {
        unsigned char numController = message->at(1);
        unsigned char value = message->at(2);
        ev = new ControllerEvent(numController, value);
    }

    // Post the event
    if (ev)
        QApplication::postEvent(instance, ev);
}

MidiDevice::MidiDevice(ConfManager * configuration, Synth *synth) :
    _keyboard(nullptr),
    _configuration(configuration),
    _synth(synth),
    _isSustainOn(false)
{
    // Connexion midi
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
        if (index < (signed)_midiin->getPortCount() && index != -1)
        {
            try
            {
                _midiin->openPort(index);
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
        NoteEvent *noteEvent = dynamic_cast<NoteEvent *>(event);
        if (noteEvent->getVelocity() > 0)
            this->setKey(noteEvent->getNote(), noteEvent->getVelocity(), true);
        else
            this->setKeyOff(noteEvent->getNote(), true);
        event->accept();
    }
    else if (event->type() == QEvent::User + 1)
    {
        ControllerEvent *controllerEvent = dynamic_cast<ControllerEvent *>(event);
        this->setController(controllerEvent->getNumController(), controllerEvent->getValue());
        event->accept();
    }
}

void MidiDevice::setController(int numController, int value)
{
    if (numController == 64)
    {
        // Sustain pedal
        _isSustainOn = (value >= 64);
        if (!_isSustainOn)
        {
            // Release all keys that have been sustained
            while (_listKeysToRelease.size())
            {
                setKeyOff(_listKeysToRelease.takeFirst(), true);
            }
        }
    }
    else if (numController == 7)
    {
        // General volume
        double vol = (double)value / 127. * 101. - 50.5;
        _configuration->setValue(ConfManager::SECTION_SOUND_ENGINE, "gain", vol);
    }
}

void MidiDevice::setKey(int key, int vel, bool syncKeyboard)
{
    // Display the note on the keyboard
    if (_keyboard && syncKeyboard)
        _keyboard->inputNoteOn(key, vel);

    // Possibly restart a note if the same key is already played
    if (_listKeysToRelease.contains(key))
        setKeyOff(key);

    // Notify about a key being played
    emit(keyPlayed(key, vel));
}

void MidiDevice::setKeyOff(int key, bool syncKeyboard)
{
    // Remove the note from the keyboard
    if (_keyboard && syncKeyboard)
    {
        _keyboard->inputNoteOff(key);
        _keyboard->removeCurrentRange(key);
    }

    // Stop a sample reading if key is -1
    if (key == -1)
    {
        _synth->play(0, 0, 0, -1, 0);
    }
    else if (_isSustainOn)
    {
        // Add the key to the list of keys to release when the pedal is released
        if (!_listKeysToRelease.contains(key))
            _listKeysToRelease << key;
    }
    else
    {
        // Notify about a key being not played anymore
        emit(keyPlayed(key, 0));
    }
}

void MidiDevice::setKeyboard(PianoKeybdCustom * keyboard)
{
    connect(keyboard, SIGNAL(noteOn(int,int)), this, SLOT(setKey(int,int)));
    connect(keyboard, SIGNAL(noteOff(int)), this, SLOT(setKeyOff(int)));
    _keyboard = keyboard;
}

void MidiDevice::stopAll()
{
    // Release all keys sustained
    while (_listKeysToRelease.size())
    {
        setKeyOff(_listKeysToRelease.takeFirst(), true);
    }

    // Reset the keyboard
    _keyboard->clearCustomization();

    // Stop all voices
    _synth->stop();
}
