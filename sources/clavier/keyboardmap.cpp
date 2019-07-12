/*
    MIDI Virtual Piano Keyboard
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>
    Copyright (C) 2014,      Davy Triponney         <davy.triponney@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "keyboardmap.h"

KeyboardMap::KeyboardMap() :
    _firstNote(36)
{
    initializeMapping();
}

void KeyboardMap::initializeMapping()
{
    _keyMap.clear();
    for (int octave = 0; octave < 4; octave++)
    {
        for (int keyNum = (int)ConfManager::KEY_C_LEFT; keyNum <= (int)ConfManager::KEY_C_RIGHT; keyNum++)
        {
            ConfManager::Key key = (ConfManager::Key)keyNum;
            _keyMap[octave][key] = QKeySequence(ContextManager::configuration()->getMapping(octave, key));
        }
    }
    _firstNote = 12 * ContextManager::configuration()->getValue(ConfManager::SECTION_KEYBOARD, "octave_offset", 3).toInt();
}

int KeyboardMap::getKey(QKeySequence sequence)
{
    int iRet = -1;
    QList<int> listOctaves = _keyMap.keys();
    for (int i = 0; i < listOctaves.size(); i++)
    {
        QMap<ConfManager::Key, QKeySequence> mapTmp = _keyMap.value(listOctaves.at(i));
        QList<ConfManager::Key> listKeys = mapTmp.keys();
        for (int j = 0; j < listKeys.size(); j++)
        {
            if (mapTmp.value(listKeys.at(j)) == sequence)
                iRet = _firstNote + 12 * listOctaves.at(i) + (int)listKeys.at(j);
        }
    }
    return iRet;
}
