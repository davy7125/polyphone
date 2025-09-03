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

#ifndef IMIDILISTENER_H
#define IMIDILISTENER_H

class IMidiListener
{
public:
    virtual bool processKey(int channel, int key, int vel)
    {
        Q_UNUSED(channel)
        Q_UNUSED(key)
        Q_UNUSED(vel)
        return false;
    }

    virtual bool processPolyPressureChanged(int channel, int key, int pressure)
    {
        Q_UNUSED(channel)
        Q_UNUSED(key)
        Q_UNUSED(pressure)
        return false;
    }

    virtual bool processMonoPressureChanged(int channel, int value)
    {
        Q_UNUSED(channel)
        Q_UNUSED(value)
        return false;
    }

    virtual bool processControllerChanged(int channel, int num, int value)
    {
        Q_UNUSED(channel)
        Q_UNUSED(num)
        Q_UNUSED(value)
        return false;
    }

    virtual bool processBendChanged(int channel, float value)
    {
        Q_UNUSED(channel)
        Q_UNUSED(value)
        return false;
    }

    virtual bool processProgramChanged(int channel, quint16 bank, quint8 preset)
    {
        Q_UNUSED(channel)
        Q_UNUSED(bank)
        Q_UNUSED(preset)
        return false;
    }

    // (N)RPN changes are the combination of 4 messages:
    // 1 - parameter MSB (100 for RPN or 98 for NRPN)
    // 2 - parameter LSB (101 for RPN or 99 for NRPN)
    // 3 - value MSB (6)
    // 4 - value LSB (38)
    // The trigger in the function is value 1 to 4 above or:
    // 5 - increment (96)
    // 6 - decrement (97)
    // Depending on the parameter type, we may have to wait for a specific trigger for having a complete command
    virtual bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
    {
        Q_UNUSED(channel)
        Q_UNUSED(parameter)
        Q_UNUSED(value)
        Q_UNUSED(isRegistered)
        Q_UNUSED(trigger)
        return false;
    }
};

#endif // IMIDILISTENER_H
