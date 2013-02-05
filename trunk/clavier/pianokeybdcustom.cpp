/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
    int STATUS_NOTEOFF = 0x80;
    int STATUS_NOTEON  = 0x90;
    //int MASK_CHANNEL   = 0x0f;
    int MASK_STATUS    = 0xf0;
    // Récupération de l'instance de PianoKeybdCustom
    PianoKeybdCustom * instance = static_cast<PianoKeybdCustom*>(userData);
    NoteEvent* ev = 0;
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
    if (ev)
        QApplication::postEvent(instance, ev);
}

// Constructeur, destructeur
PianoKeybdCustom::PianoKeybdCustom(QWidget *parent) : PianoKeybd(parent),
    midiin(NULL)
{
    // Connexion midi
    try
    {
#if defined(__LINUX_ALSASEQ__)
        this->midiin = new RtMidiInAlsa("Polyphone");
#endif
#if defined(__WINDOWS_MM__)
        this->midiin = new RtMidiInWinMM("Polyphone");
#endif
    }
    catch (RtError &error)
    {
        // Handle the exception here
        error.printMessage();
        this->midiin = NULL;
    }
    if (this->midiin)
    {
        midiin->ignoreTypes(false, false, false);
        this->midiin->setCallback(&midiCallback, this);
    }
    this->setKeyboardMap(NULL);
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
    if (!this->midiin)
        return;
    if ((signed)this->midiin->getPortCount() <= val)
    {
        this->midiin = NULL;
        return;
    }
    this->midiin->closePort();
    try
    {
        this->midiin->openPort(val);
    }
    catch (RtError &error)
    {
        this->midiin = NULL;
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
