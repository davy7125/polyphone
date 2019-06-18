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

#include "pianokeybdcustom.h"
#include "basetypes.h"
#include <QApplication>
#include "contextmanager.h"

// Keyboard colors
QColor PianoKeybdCustom::COLOR_PRESSED;
QColor PianoKeybdCustom::COLOR_BLACK_ENABLED;
QColor PianoKeybdCustom::COLOR_WHITE_ENABLED;
QColor PianoKeybdCustom::COLOR_BLACK_DISABLED;
QColor PianoKeybdCustom::COLOR_WHITE_DISABLED;
QColor PianoKeybdCustom::COLOR_BLACK_RANGE;
QColor PianoKeybdCustom::COLOR_WHITE_RANGE;

PianoKeybdCustom::PianoKeybdCustom(QWidget *parent) : PianoKeybd(parent)
{
    /// DEFINITION OF THE COLORS

    // Key pressed
    COLOR_PRESSED = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    if (ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        // Normal color of keys
        COLOR_BLACK_ENABLED = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
        COLOR_WHITE_ENABLED = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
    }
    else
    {
        // Normal color of keys
        COLOR_BLACK_ENABLED = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);
        COLOR_WHITE_ENABLED = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    }

    // Color when keys are disabled
    COLOR_BLACK_DISABLED = COLOR_WHITE_DISABLED = ThemeManager::mix(COLOR_BLACK_ENABLED, COLOR_WHITE_ENABLED, 0.5);

    // Color of the range around a key
    COLOR_BLACK_RANGE = ThemeManager::mix(COLOR_BLACK_ENABLED, COLOR_PRESSED, 0.5);
    COLOR_WHITE_RANGE = ThemeManager::mix(COLOR_WHITE_ENABLED, COLOR_PRESSED, 0.3);

    /// APPLY THEME

    set(PROPERTY_COLOR_BLACK_KEYS, COLOR_BLACK_DISABLED);
    set(PROPERTY_COLOR_WHITE_KEYS, COLOR_WHITE_DISABLED);
    set(PROPERTY_COLOR_1, COLOR_PRESSED);
    setFrameStyle(0);

    /// CONNECTIONS

    connect(this, SIGNAL(noteOn(int,int)), this, SLOT(setKey(int,int)));
    connect(this, SIGNAL(noteOff(int)), this, SLOT(removeCurrentRange(int)));
}

void PianoKeybdCustom::addRangeAndRootKey(int rootKey, int noteMin, int noteMax)
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
    _mapPressed.clear();
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

void PianoKeybdCustom::removeCurrentRange(int num)
{
    _mapPressed.remove(num);
    updateRanges();
}

void PianoKeybdCustom::addCurrentRange(int note, int noteMin, int noteMax)
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
