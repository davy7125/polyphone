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

#ifndef BALANCEPARAMETERS_H
#define BALANCEPARAMETERS_H

#include "basetypes.h"
class SoundfontManager;

class BalanceParameters
{
public:
    BalanceParameters(bool isMono);
    void getAttenuationAndPan(SoundfontManager * sf2, EltID  idInstSmpl, int channel);
    void computeSfzParameters();

    bool isMono() { return _isMono; }

    // SFZ computed parameters
    int getPan() { return _pan; }
    int getWidth() { return _width; }
    int getPosition() { return _position; }
    double getVolume() { return _volume; }

private:
    bool _isMono;
    bool _leftUsed;
    double _leftPan;
    double _leftAttenuation;
    bool _rightUsed;
    double _rightPan;
    double _rightAttenuation;

    int _pan, _width, _position;
    double _volume;

    static int toInt(double value);
    static double dBtoGain(double dB) { return pow(10., 0.05 * dB); }
};

#endif // BALANCEPARAMETERS_H
