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

#ifndef OSCTRIANGLE_H
#define OSCTRIANGLE_H

#include "sound.h"

class OscTriangle
{
public:
    OscTriangle(quint32 sampleRate);

    // Freq in Hz, delay in s
    void getData(float *data, quint32 len, float freq, double delay);

private:
    quint32 _sampleRate;
    float _previousFreq;
    quint32 _currentDelay;
    bool _delayEnded;
    float _delta, _previousPoint;

    void computeDelta(float freq, float &delta);
};

#endif // OSCTRIANGLE_H
