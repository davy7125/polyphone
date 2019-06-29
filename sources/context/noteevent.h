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

#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include <QEvent>

class NoteEvent : public QEvent
{
public:
    NoteEvent(unsigned char note, unsigned char val) : QEvent(QEvent::User),
          _note(note),
          _velocity(val) {}

    unsigned char getNote() const
    {
        return _note;
    }

    unsigned char getVelocity() const
    {
        return _velocity;
    }

protected:
    unsigned char _note;
    unsigned char _velocity;
};

#endif // NOTEEVENT_H
