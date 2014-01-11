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

#ifndef KEYBOARDMAP_H
#define KEYBOARDMAP_H

#include "pianokeybd.h"
#include <QMap>

class KeyboardMap
{
public:
    KeyboardMap();
    void setMapping(PianoKeybd::Key key, int numOctave, QKeySequence sequence);
    QKeySequence getMapping(PianoKeybd::Key key, int numOctave);
    int getKey(QKeySequence sequence);
    QList<QKeySequence> getSequences(int note);

    void setFirstNote(int note) { _firstNote = note; }
    int getFirstNote()          { return _firstNote; }

private:
    int _firstNote;
    QMap<int, QMap<PianoKeybd::Key, QKeySequence> > _keyMap;
};

#endif /* KEYBOARDMAP_H */
