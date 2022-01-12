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

#include "osctriangle.h"
#include "qmath.h"


OscTriangle::OscTriangle(quint32 sampleRate) :
    _sampleRate(sampleRate),
    _previousFreq(-1),
    _currentDelay(0),
    _delayEnded(false)
{
}

void OscTriangle::getData(float * data, quint32 len, float freq, double delay)
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

    // Triangle
    if (total != len)
    {
        if (_previousFreq < 0)
        {
            // Initialize the system
            _previousFreq = freq;
            computeDelta(freq, _delta);
            _previousPoint = 0;
        }
        if (qAbs(_previousFreq - freq) > 0)
        {
            float delta2;
            computeDelta(freq, delta2);
            if (_delta < 0)
                delta2 = -delta2;

            float progDelta;
            for (quint32 i = total; i < len; i++)
            {
                progDelta = static_cast<float>(len - i) / (len - total) * _delta
                        + static_cast<float>(i - total) / (len - total) * delta2;

                data[i] = _previousPoint + progDelta;
                if (data[i] > 1.0f)
                {
                    data[i] = 2.0f - data[i];
                    _delta = -_delta;
                    delta2 = -delta2;
                }
                else if (data[i] < -1.0f)
                {
                    data[i] = -2.0f - data[i];
                    _delta = -_delta;
                    delta2 = -delta2;
                }
                _previousPoint = data[i];
            }

            // Update values
            _delta = delta2;
            _previousFreq = freq;
        }
        else
        {
            for (quint32 i = total; i < len; i++)
            {
                data[i] = _previousPoint + _delta;
                if (data[i] > 1.0f)
                {
                    data[i] = 2.0f - data[i];
                    _delta = -_delta;
                }
                else if (data[i] < -1.0f)
                {
                    data[i] = -2.0f - data[i];
                    _delta = -_delta;
                }
                _previousPoint = data[i];
            }
        }
    }
}

void OscTriangle::computeDelta(float freq, float &delta)
{
    delta = 4.0f * freq / _sampleRate;
}
