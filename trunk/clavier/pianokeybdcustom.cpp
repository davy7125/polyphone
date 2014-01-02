/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pianokeybdcustom.h"
#include "sf2_types.h"


// Callback des signaux midi
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
    PianoKeybdCustom * instance = static_cast<PianoKeybdCustom*>(userData);
    QEvent* ev = 0;
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
    if (ev)
        QApplication::postEvent(instance, ev);
}

// Constructeur, destructeur
PianoKeybdCustom::PianoKeybdCustom(QWidget *parent) : PianoKeybd(parent),
    midiin(NULL),
    _mapper(NULL)
{
    this->setFrameStyle(0);
    // Connexion midi
    try
    {
#if defined(__LINUX_ALSASEQ__)
        this->midiin = new RtMidiInAlsa("Polyphone");
#endif
#if defined(__WINDOWS_MM__)
        this->midiin = new RtMidiInWinMM("Polyphone");
#endif
#if defined(__MACOSX_CORE__)
        this->midiin = new RtMidiInCoreMidi("Polyphone");
#endif
    }
    catch (RtError &error)
    {
        this->midiin = NULL;
    }
    if (this->midiin)
    {
        midiin->ignoreTypes(false, false, false);
        this->midiin->setCallback(&midiCallback, this);
    }
}
PianoKeybdCustom::~PianoKeybdCustom()
{
    delete this->midiin;
}

QStringList PianoKeybdCustom::getPortNames()
{
    QStringList listRet;
    if (this->midiin)
    {
        for (unsigned int i = 0; i < this->midiin->getPortCount(); i++)
            listRet.append(QString(this->midiin->getPortName(i).c_str()));
    }
    return listRet;
}
void PianoKeybdCustom::openMidiPort(int val)
{
    if (this->midiin)
    {
        this->midiin->closePort();
        if (val < (signed)this->midiin->getPortCount() && val != -1)
        {
            try
            {
                this->midiin->openPort(val);
            }
            catch (RtError &error)
            {
            }
        }
    }
}
void PianoKeybdCustom::changeKey(int key, int vel)
{
    // Action sur les touches
    if (vel > 0)
        this->showNoteOn(key, vel);
    else
        this->showNoteOff(key, 0);

    // Envoi signal
    this->keyChanged(key, vel);
}

void PianoKeybdCustom::changeController(int numController, int value)
{
    if (numController == 64)
    {
        // Pédale sustain
        this->sustainChanged(value >= 64);
    }
    else if (numController == 7)
    {
        // Volume général
        this->volumeChanged(value);
    }
}

void PianoKeybdCustom::setKeyboardType(KeyboardType type)
{
    switch(type)
    {
    case KEYBOARD_5_OCTAVES:
        this->setMaximumWidth(330);
        this->setMinimumWidth(330);
        this->setNumOctaves(6);
        this->setBaseOctave(3);
        this->setMaxNote(96);
        break;
    case KEYBOARD_6_OCTAVES:
        this->setMaximumWidth(380);
        this->setMinimumWidth(380);
        this->setNumOctaves(7);
        this->setBaseOctave(3);
        this->setMaxNote(108);
        break;
    case KEYBOARD_128_NOTES:
        this->setMaximumWidth(610);
        this->setMinimumWidth(610);
        this->setNumOctaves(11);
        this->setBaseOctave(0);
        this->setMaxNote(127);
        break;
    default: break;
    }
}
