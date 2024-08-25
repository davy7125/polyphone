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
    virtual bool processKey(int channel, int key, int vel) = 0;
    virtual bool processPolyPressureChanged(int channel, int key, int pressure) = 0;
    virtual bool processMonoPressureChanged(int channel, int value) = 0;
    virtual bool processControllerChanged(int channel, int num, int value) = 0;
    virtual bool processBendChanged(int channel, float value) = 0;
    virtual bool processBendSensitivityChanged(int channel, float semitones) = 0;
};

#endif // IMIDILISTENER_H
