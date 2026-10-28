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

#include <cstdint>

// Receiver of MIDI events, the channel -1 being the one of the editor
// A function returns true if the event is consumed (MidiProcessor does not repeat it on the channel -1)
class IMidiListener
{
public:
    virtual ~IMidiListener() {}

    virtual bool processKey(int channel, int key, int vel)
    {
        (void)channel;
        (void)key;
        (void)vel;
        return false;
    }

    // Key pressed again while its note is held by the sustain or the sostenuto pedal: stopped, then played again
    virtual bool processKeyRestarted(int channel, int key, int vel)
    {
        processKey(channel, key, 0);
        return processKey(channel, key, vel);
    }

    virtual bool processPolyPressureChanged(int channel, int key, int pressure)
    {
        (void)channel;
        (void)key;
        (void)pressure;
        return false;
    }

    virtual bool processMonoPressureChanged(int channel, int value)
    {
        (void)channel;
        (void)value;
        return false;
    }

    virtual bool processControllerChanged(int channel, int num, int value)
    {
        (void)channel;
        (void)num;
        (void)value;
        return false;
    }

    virtual bool processBendChanged(int channel, float value)
    {
        (void)channel;
        (void)value;
        return false;
    }

    virtual bool processProgramChanged(int channel, uint16_t bank, uint8_t preset)
    {
        (void)channel;
        (void)bank;
        (void)preset;
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
        (void)channel;
        (void)parameter;
        (void)value;
        (void)isRegistered;
        (void)trigger;
        return false;
    }
};

#endif // IMIDILISTENER_H
