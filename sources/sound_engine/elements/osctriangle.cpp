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

#include "osctriangle.h"
#include <string.h>

void OscTriangle::initialize(quint32 sampleRate)
{
    _sampleRate = sampleRate;
    _fourInvSampleRate = _sampleRate > 0 ? 4.0f / static_cast<float>(_sampleRate) : 0.0f;
    _previousFreq = -1.0f;
    _currentDelay = 0;
    _delayEnded = false;
    _delta = 0.0f;
    _previousPoint = 0.0f;
}

void OscTriangle::getData(float * data, quint32 chunkCount, float freq, float delay)
{
    quint32 total = 0;

    // Possible delay
    quint32 delayTime = static_cast<quint32>(delay * _sampleRate);
    if (!_delayEnded && _currentDelay < delayTime)
    {
        total = qMin(delayTime - _currentDelay, chunkCount);
        memset(data, 0, total * sizeof(float));
        _currentDelay += total;
        _delayEnded = (_currentDelay >= delayTime);
    }

    // Triangle
    if (total != chunkCount)
    {
        if (_previousFreq < 0)
        {
            // Initialize the system
            _previousFreq = freq;
            _delta = _fourInvSampleRate * freq;
            _previousPoint = 0;
        }

        if (qAbs(_previousFreq - freq) > 0)
        {
            float progDelta;
            float invTmp = 1.0f / static_cast<float>(chunkCount - total);
            float delta2;
            delta2 = _fourInvSampleRate * freq;
            int totalInit = total;
            if (_delta < 0)
            {
                delta2 = -delta2;
                goto down1;
            }

        up1:
            while (total < chunkCount)
            {
                progDelta = (static_cast<float>(chunkCount - total) * _delta + static_cast<float>(total - totalInit) * delta2) * invTmp;
                data[total] = _previousPoint + progDelta;

                if (data[total] > 1.0f)
                {
                    data[total] = 2.0f - data[total];
                    _delta = -_delta;
                    delta2 = -delta2;
                    _previousPoint = data[total++];
                    goto down1;
                }

                _previousPoint = data[total++];
            }
            goto end1;

        down1:
            while (total < chunkCount)
            {
                progDelta = (static_cast<float>(chunkCount - total) * _delta + static_cast<float>(total - totalInit) * delta2) * invTmp;
                data[total] = _previousPoint + progDelta;

                if (data[total] < -1.0f)
                {
                    data[total] = -2.0f - data[total];
                    _delta = -_delta;
                    delta2 = -delta2;
                    _previousPoint = data[total++];
                    goto up1;
                }

                _previousPoint = data[total++];
            }
            goto end1;

        end1:
            // Update values
            _delta = delta2;
            _previousFreq = freq;
        }
        else
        {
            if (_delta < 0)
                goto down2;

        up2:
            while (total < chunkCount)
            {
                data[total] = _previousPoint + _delta;

                if (data[total] > 1.0f)
                {
                    data[total] = 2.0f - data[total];
                    _delta = -_delta;
                    _previousPoint = data[total++];
                    goto down2;
                }

                _previousPoint = data[total++];
            }
            return;

        down2:
            while (total < chunkCount)
            {
                data[total] = _previousPoint + _delta;

                if (data[total] < -1.0f)
                {
                    data[total] = -2.0f - data[total];
                    _delta = -_delta;
                    _previousPoint = data[total++];
                    goto up2;
                }

                _previousPoint = data[total++];
            }
            return;
        }
    }
}
