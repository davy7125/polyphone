/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
#include "config.h"
#include <QApplication>

// Définition des couleurs du clavier
QColor PianoKeybdCustom::COLOR_PRESSED          = QColor(  0,   0, 255);    // Bleu
QColor PianoKeybdCustom::COLOR_BLACK_ENABLED    = QColor(  0,   0,   0);    // Noir
QColor PianoKeybdCustom::COLOR_WHITE_ENABLED    = QColor(255, 255, 255);    // Blanc
QColor PianoKeybdCustom::COLOR_BLACK_DISABLED   = QColor(200, 200, 200);    // Gris moyen
QColor PianoKeybdCustom::COLOR_WHITE_DISABLED   = QColor(200, 200, 200);    // Gris moyen
QColor PianoKeybdCustom::COLOR_BLACK_RANGE      = QColor(200, 200,   0);    // Jaune foncé
QColor PianoKeybdCustom::COLOR_WHITE_RANGE      = QColor(255, 255,  50);    // Jaune clair

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

// Constructeur, destructeur_noteOnRange
PianoKeybdCustom::PianoKeybdCustom(QWidget *parent) : PianoKeybd(parent),
    midiin(NULL)
{
    // Couleurs et style
    set(PROPERTY_COLOR_BLACK_KEYS, COLOR_BLACK_DISABLED);
    set(PROPERTY_COLOR_WHITE_KEYS, COLOR_WHITE_DISABLED);
    set(PROPERTY_COLOR_1, COLOR_PRESSED);
    setFrameStyle(0);

    // Connexion midi
    try
    {
#if defined(__LINUX_ALSASEQ__)
        this->midiin = new RtMidiIn(RtMidi::LINUX_ALSA, "Polyphone");
#endif
#if defined(__WINDOWS_MM__)
        this->midiin = new RtMidiIn(RtMidi::WINDOWS_MM, "Polyphone");
#endif
#if defined(__MACOSX_CORE__)
        this->midiin = new RtMidiIn(RtMidi::MACOSX_CORE, "Polyphone");
#endif
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        this->midiin = NULL;
    }
    if (this->midiin)
    {
        midiin->ignoreTypes(false, false, false);
        this->midiin->setCallback(&midiCallback, this);
    }

    connect(this, SIGNAL(noteOn(int,int)), this, SLOT(setKey(int,int)));
    connect(this, SIGNAL(noteOff(int)), this, SLOT(setKeyOff(int)));
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
            catch (std::exception &error)
            {
                Q_UNUSED(error)
            }
        }
    }
}
void PianoKeybdCustom::changeKey(int key, int vel)
{
    // Action sur les touches
    if (vel > 0)
        this->inputNoteOn(key, vel);
    else
        this->inputNoteOff(key);

    // Envoi signal
    this->noteOn(key, vel);
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

void PianoKeybdCustom::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        int key = event->key();
        if (key >= Qt::Key_1 && key <= Qt::Key_8)
        {
            int octave = key - Qt::Key_1;
            set(PROPERTY_MAPPING_FIRST_NOTE, 12 * octave);
            Config::getInstance()->setOctaveMap(octave);
        }
    }
    PianoKeybd::keyPressEvent(event);
}

void PianoKeybdCustom::setRangeAndRootKey(int rootKey, int noteMin, int noteMax)
{
    // Mémorisation de l'étendue
    for (int i = noteMin; i <= noteMax; i++)
        if (!_currentRange.contains(i))
            _currentRange << i;

    // Mémorisation de la note de base
    if (!_rootKeys.contains(rootKey) && rootKey != -1)
        _rootKeys << rootKey;

    // Mise à jour du clavier
    updateRanges();
}

void PianoKeybdCustom::clearCustomization()
{
    _rootKeys.clear();
    _currentRange.clear();
    updateRanges();
}

void PianoKeybdCustom::setKey(int num, int vel)
{
    if (vel > 0)
        _mapPressed.insert(num, QList<int>());
    else
        _mapPressed.remove(num);
    updateRanges();
}

void PianoKeybdCustom::setKeyOff(int num)
{
    _mapPressed.remove(num);
    updateRanges();
}

void PianoKeybdCustom::setCurrentRange(int note, int noteMin, int noteMax)
{
    for (int i = noteMin; i <= noteMax; i++)
        if (!_mapPressed[note].contains(i))
            _mapPressed[note] << i;
    updateRanges();
}

void PianoKeybdCustom::updateRanges()
{
    // Réinitialisation des décorations
    PianoKeybd::clearCustomization();

    // Liste des notes appuyées
    QList<int> notePressed = _mapPressed.keys();

    // Etendues des rangées
    QList<int> noteRanges;
    foreach (QList<int> values, _mapPressed.values())
    {
        foreach (int key, values)
            if (!noteRanges.contains(key))
                noteRanges << key;
    }

    // Etendue de la section courante
    QList<int> noteCurrentRange = _currentRange;

    // Les notes appuyées ne doivent pas être modifiées
    foreach (int key, notePressed)
    {
        noteRanges.removeAll(key);
        noteCurrentRange.removeAll(key);
    }

    // Priorité sur les étendues des rangées plutôt que la section courante
    foreach (int key, noteRanges)
        noteCurrentRange.removeAll(key);

    // Dessin de l'étendue courante
    foreach (int key, noteCurrentRange)
    {
        int note = key % 12;
        if (note == 1 || note == 3 || note == 6 || note == 8 || note == 10)
            customize(key, CUSTOMIZATION_TYPE_COLOR, COLOR_BLACK_ENABLED);
        else
            customize(key, CUSTOMIZATION_TYPE_COLOR, COLOR_WHITE_ENABLED);
    }

    // Dessin des notes de base
    customize(60, CUSTOMIZATION_TYPE_MARKER, MARKER_TYPE_DOT_BLACK);
    foreach (int rootKey, _rootKeys)
        customize(rootKey, CUSTOMIZATION_TYPE_MARKER, MARKER_TYPE_DOT_RED);

    // Dessin des étendues
    foreach (int key, noteRanges)
    {
        int note = key % 12;
        if (note == 1 || note == 3 || note == 6 || note == 8 || note == 10)
            customize(key, CUSTOMIZATION_TYPE_COLOR, COLOR_BLACK_RANGE);
        else
            customize(key, CUSTOMIZATION_TYPE_COLOR, COLOR_WHITE_RANGE);
    }
}
