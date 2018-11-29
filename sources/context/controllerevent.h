/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#ifndef CONTROLLEREVENT_H
#define CONTROLLEREVENT_H

#include <QEvent>

class ControllerEvent : public QEvent
{
public:
    ControllerEvent(unsigned char numController, unsigned char value) : QEvent((QEvent::Type)(QEvent::User+1)),
          m_numController(numController),
          m_value(value) {}

    unsigned char getNumController() const
    {
        return m_numController;
    }

    unsigned char getValue() const
    {
        return m_value;
    }

protected:
    unsigned char m_numController;
    unsigned char m_value;
};

#endif // CONTROLLEREVENT_H
