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

#ifndef IMIDIVALUES_H
#define IMIDIVALUES_H

class IMidiListener;

class IMidiValues
{
public:
    virtual int getControllerValue(int channel, int controllerNumber) = 0;
    virtual float getBendValue(int channel) = 0;
    virtual float getBendSensitivityValue(int channel) = 0;
    virtual int getMonoPressure(int channel) = 0;
    virtual int getPolyPressure(int channel, int key) = 0;
};

#endif // IMIDIVALUES_H
