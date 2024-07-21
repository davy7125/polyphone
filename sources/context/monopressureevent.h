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

#ifndef MONOPRESSUREEVENT_H
#define MONOPRESSUREEVENT_H

#include <QEvent>

class MonoPressureEvent : public QEvent
{
public:
    MonoPressureEvent(qint8 channel, quint8 val) : QEvent((QEvent::Type)(QEvent::User+3)),
        _channel(channel),
        _pressure(val) {}

    qint8 getChannel() const
    {
        return _channel;
    }

    quint8 getPressure() const
    {
        return _pressure;
    }

protected:
    qint8 _channel;
    quint8 _pressure;
};

#endif // MONOPRESSUREEVENT_H
