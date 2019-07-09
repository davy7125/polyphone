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

#include "oscsinus.h"
#include "qmath.h"


OscSinus::OscSinus(quint32 sampleRate) :
    _sampleRate(sampleRate),
    _previousFreq(-1),
    _currentDelay(0),
    _delayEnded(false)
{
}

// Gordon-Smith Generator
void OscSinus::getSinus(float * data, quint32 len, float freq, double delay)
{
    quint32 total = 0;

    // Possible delay
    quint32 delayTime = static_cast<quint32>(delay * _sampleRate);
    if (!_delayEnded && _currentDelay < delayTime)
    {
        total = qMin(delayTime - _currentDelay, len);
        for (quint32 i = 0; i < total; i++)
            data[i] = 0;
        _currentDelay += total;
        _delayEnded = (_currentDelay >= delayTime);
    }

    // Sinus
    if (total != len)
    {
        if (_previousFreq < 0)
        {
            // Initialize the system
            _previousFreq = freq;
            computeEpsilon(freq, _theta, _epsilon);
            _posPrec = static_cast<float>(qSin(static_cast<double>(-_theta)));
            _posPrecQuad = static_cast<float>(qCos(static_cast<double>(-_theta)));
        }
        if (qAbs(_previousFreq - freq) > 0)
        {
            float theta2, epsilon2;
            computeEpsilon(freq, theta2, epsilon2);
            _previousFreq = freq;

            float progEpsilon;
            for (quint32 i = total; i < len; i++)
            {
                progEpsilon = static_cast<float>(len - i) / (len - total) * _epsilon
                        + static_cast<float>(i - total) / (len - total) * epsilon2;
                _posPrecQuad -= progEpsilon * _posPrec;
                _posPrec     += progEpsilon * _posPrecQuad;
                data[i] = _posPrec;
            }

            // Update values
            _theta = theta2;
            _epsilon = epsilon2;
            _previousFreq = freq;
        }
        else
        {
            for (quint32 i = total; i < len; i++)
            {
                _posPrecQuad -= _epsilon * _posPrec;
                _posPrec     += _epsilon * _posPrecQuad;
                data[i] = _posPrec;
            }
        }
    }
}

void OscSinus::computeEpsilon(float freq, float &theta, float &epsilon)
{
    theta = 2.f * static_cast<float>(M_PI) * freq / _sampleRate;
    epsilon = 2.f * static_cast<float>(qSin(static_cast<double>(theta) / 2.));
}
