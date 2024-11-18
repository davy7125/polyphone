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

#include "oscsinus.h"
#include "qmath.h"


OscSinus::OscSinus(quint32 sampleRate) :
    _invSampleRate(1.0f / static_cast<float>(sampleRate)),
    _previousFreq(-1)
{
}

// Gordon-Smith Generator
void OscSinus::getData(float * data, quint32 len, float freq)
{
    quint32 total = 0;

    // Initialize the system
    if (_previousFreq < 0.0f)
    {
        if (freq < 0.0f)
            return;

        _previousFreq = freq;
        computeEpsilon(freq, _theta, _epsilon);
        _posPrec = static_cast<float>(qSin(static_cast<double>(-_theta)));
        _posPrecQuad = static_cast<float>(qCos(static_cast<double>(-_theta)));
    }

    if (freq < 0.0f || _previousFreq == freq)
    {
        // Sinus with a fixed frequency
        for (quint32 i = total; i < len; i++)
        {
            _posPrecQuad -= _epsilon * _posPrec;
            _posPrec     += _epsilon * _posPrecQuad;
            data[i] = _posPrec;
        }
    }
    else
    {
        // Sinus with a changing frequency (transition)
        float theta2, epsilon2;
        computeEpsilon(freq, theta2, epsilon2);

        float progEpsilon;
        float coef1 = _epsilon / (len - total);
        float coef2 = epsilon2 / (len - total);
        for (quint32 i = total; i < len; i++)
        {
            progEpsilon = static_cast<float>(len - i) * coef1 + static_cast<float>(i - total) * coef2;
            _posPrecQuad -= progEpsilon * _posPrec;
            _posPrec     += progEpsilon * _posPrecQuad;
            data[i] = _posPrec;
        }

        // Update values
        _theta = theta2;
        _epsilon = epsilon2;
        _previousFreq = freq;
    }
}

void OscSinus::computeEpsilon(float freq, float &theta, float &epsilon)
{
    theta = 2.f * static_cast<float>(M_PI) * freq * _invSampleRate;
    epsilon = 2.f * static_cast<float>(qSin(static_cast<double>(theta) / 2.));
}
