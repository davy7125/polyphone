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
    // Default mapping
    _keyMap[0][PianoKeybd::KEY_C_LEFT]  = QKeySequence(QString::fromUtf8("Z"));
    _keyMap[0][PianoKeybd::KEY_C_SHARP] = QKeySequence(QString::fromUtf8("S"));
    _keyMap[0][PianoKeybd::KEY_D]       = QKeySequence(QString::fromUtf8("X"));
    _keyMap[0][PianoKeybd::KEY_D_SHARP] = QKeySequence(QString::fromUtf8("D"));
    _keyMap[0][PianoKeybd::KEY_E]       = QKeySequence(QString::fromUtf8("C"));
    _keyMap[0][PianoKeybd::KEY_F]       = QKeySequence(QString::fromUtf8("V"));
    _keyMap[0][PianoKeybd::KEY_F_SHARP] = QKeySequence(QString::fromUtf8("G"));
    _keyMap[0][PianoKeybd::KEY_G]       = QKeySequence(QString::fromUtf8("B"));
    _keyMap[0][PianoKeybd::KEY_G_SHARP] = QKeySequence(QString::fromUtf8("H"));
    _keyMap[0][PianoKeybd::KEY_A]       = QKeySequence(QString::fromUtf8("N"));
    _keyMap[0][PianoKeybd::KEY_A_SHARP] = QKeySequence(QString::fromUtf8("J"));
    _keyMap[0][PianoKeybd::KEY_B]       = QKeySequence(QString::fromUtf8("M"));
    _keyMap[0][PianoKeybd::KEY_C_RIGHT] = QKeySequence(QString::fromUtf8(","));

    _keyMap[1][PianoKeybd::KEY_C_LEFT]  = QKeySequence(QString::fromUtf8("Q"));
    _keyMap[1][PianoKeybd::KEY_C_SHARP] = QKeySequence(QString::fromUtf8("2"));
    _keyMap[1][PianoKeybd::KEY_D]       = QKeySequence(QString::fromUtf8("W"));
    _keyMap[1][PianoKeybd::KEY_D_SHARP] = QKeySequence(QString::fromUtf8("3"));
    _keyMap[1][PianoKeybd::KEY_E]       = QKeySequence(QString::fromUtf8("E"));
    _keyMap[1][PianoKeybd::KEY_F]       = QKeySequence(QString::fromUtf8("R"));
    _keyMap[1][PianoKeybd::KEY_F_SHARP] = QKeySequence(QString::fromUtf8("5"));
    _keyMap[1][PianoKeybd::KEY_G]       = QKeySequence(QString::fromUtf8("T"));
    _keyMap[1][PianoKeybd::KEY_G_SHARP] = QKeySequence(QString::fromUtf8("6"));
    _keyMap[1][PianoKeybd::KEY_A]       = QKeySequence(QString::fromUtf8("Y"));
    _keyMap[1][PianoKeybd::KEY_A_SHARP] = QKeySequence(QString::fromUtf8("7"));
    _keyMap[1][PianoKeybd::KEY_B]       = QKeySequence(QString::fromUtf8("U"));
    _keyMap[1][PianoKeybd::KEY_C_RIGHT] = QKeySequence(QString::fromUtf8("I"));

    _keyMap[2][PianoKeybd::KEY_C_LEFT]  = QKeySequence(QString::fromUtf8("Shift+Z"));
    _keyMap[2][PianoKeybd::KEY_C_SHARP] = QKeySequence(QString::fromUtf8("Shift+S"));
    _keyMap[2][PianoKeybd::KEY_D]       = QKeySequence(QString::fromUtf8("Shift+X"));
    _keyMap[2][PianoKeybd::KEY_D_SHARP] = QKeySequence(QString::fromUtf8("Shift+D"));
    _keyMap[2][PianoKeybd::KEY_E]       = QKeySequence(QString::fromUtf8("Shift+C"));
    _keyMap[2][PianoKeybd::KEY_F]       = QKeySequence(QString::fromUtf8("Shift+V"));
    _keyMap[2][PianoKeybd::KEY_F_SHARP] = QKeySequence(QString::fromUtf8("Shift+G"));
    _keyMap[2][PianoKeybd::KEY_G]       = QKeySequence(QString::fromUtf8("Shift+B"));
    _keyMap[2][PianoKeybd::KEY_G_SHARP] = QKeySequence(QString::fromUtf8("Shift+H"));
    _keyMap[2][PianoKeybd::KEY_A]       = QKeySequence(QString::fromUtf8("Shift+N"));
    _keyMap[2][PianoKeybd::KEY_A_SHARP] = QKeySequence(QString::fromUtf8("Shift+J"));
    _keyMap[2][PianoKeybd::KEY_B]       = QKeySequence(QString::fromUtf8("Shift+M"));
    _keyMap[2][PianoKeybd::KEY_C_RIGHT] = QKeySequence(QString::fromUtf8("Shift+<"));

    _keyMap[3][PianoKeybd::KEY_C_LEFT]  = QKeySequence(QString::fromUtf8("Shift+Q"));
    _keyMap[3][PianoKeybd::KEY_C_SHARP] = QKeySequence(QString::fromUtf8("Shift+@"));
    _keyMap[3][PianoKeybd::KEY_D]       = QKeySequence(QString::fromUtf8("Shift+W"));
    _keyMap[3][PianoKeybd::KEY_D_SHARP] = QKeySequence(QString::fromUtf8("Shift+#"));
    _keyMap[3][PianoKeybd::KEY_E]       = QKeySequence(QString::fromUtf8("Shift+E"));
    _keyMap[3][PianoKeybd::KEY_F]       = QKeySequence(QString::fromUtf8("Shift+R"));
    _keyMap[3][PianoKeybd::KEY_F_SHARP] = QKeySequence(QString::fromUtf8("Shift+%"));
    _keyMap[3][PianoKeybd::KEY_G]       = QKeySequence(QString::fromUtf8("Shift+T"));
    _keyMap[3][PianoKeybd::KEY_G_SHARP] = QKeySequence(QString::fromUtf8("Shift+^"));
    _keyMap[3][PianoKeybd::KEY_A]       = QKeySequence(QString::fromUtf8("Shift+Y"));
    _keyMap[3][PianoKeybd::KEY_A_SHARP] = QKeySequence(QString::fromUtf8("Shift+&"));
    _keyMap[3][PianoKeybd::KEY_B]       = QKeySequence(QString::fromUtf8("Shift+U"));
    _keyMap[3][PianoKeybd::KEY_C_RIGHT] = QKeySequence(QString::fromUtf8("Shift+I"));
}

void KeyboardMap::setMapping(PianoKeybd::Key key, int numOctave, QKeySequence sequence)
{
    _keyMap[numOctave][key] = sequence;
}

QKeySequence KeyboardMap::getMapping(PianoKeybd::Key key, int numOctave)
{
    return _keyMap.value(numOctave).value(key);
}

int KeyboardMap::getKey(QKeySequence sequence)
{
    int iRet = -1;
    QList<int> listOctaves = _keyMap.keys();
    for (int i = 0; i < listOctaves.size(); i++)
    {
        QMap<PianoKeybd::Key, QKeySequence> mapTmp = _keyMap.value(listOctaves.at(i));
        QList<PianoKeybd::Key> listKeys = mapTmp.keys();
        for (int j = 0; j < listKeys.size(); j++)
        {
            if (mapTmp.value(listKeys.at(j)) == sequence)
                iRet = _firstNote + 12 * listOctaves.at(i) + (int)listKeys.at(j);
        }
    }
    return iRet;
}

QList<QKeySequence> KeyboardMap::getSequences(int note)
{
    note -= _firstNote;
    QKeySequence seq1 = _keyMap.value(note / 12).value((PianoKeybd::Key)(note % 12));
    QKeySequence seq2;
    if (note % 12 == 0)
        seq2 = _keyMap.value(note / 12 - 1).value(PianoKeybd::KEY_C_RIGHT).toString();

    QList<QKeySequence> listRet;
    if (!seq1.isEmpty())
        listRet << seq1;
    if (!seq2.isEmpty())
        listRet << seq2;
    return listRet;
}
