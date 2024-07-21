/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include <QEvent>

class NoteEvent : public QEvent
{
public:
    NoteEvent(qint8 channel, qint8 note, qint8 val) : QEvent(QEvent::User),
        _channel(channel),
        _note(note),
        _velocity(val) {}

    qint8 getChannel() const
    {
        return _channel;
    }

    qint8 getNote() const
    {
        return _note;
    }

    qint8 getVelocity() const
    {
        return _velocity;
    }

protected:
    qint8 _channel;
    qint8 _note;
    qint8 _velocity;
};

#endif // NOTEEVENT_H
