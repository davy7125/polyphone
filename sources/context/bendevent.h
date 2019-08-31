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

#ifndef BENDEVENT_H
#define BENDEVENT_H

#include <QEvent>

class BendEvent : public QEvent
{
public:
    BendEvent(unsigned char val1, unsigned char val2) : QEvent((QEvent::Type)(QEvent::User+4)),
          _value1(val1),
          _value2(val2),
          _value((_value2 << 7) | _value1)
    {}

    double getValue() const
    {
        // Result between -1 and 1
        return static_cast<double>(_value - 8192) / 8192.0;
    }

protected:
    unsigned char _value1, _value2;
    qint32 _value;
};

#endif // BENDEVENT_H
