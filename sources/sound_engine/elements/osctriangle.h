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

#ifndef OSCTRIANGLE_H
#define OSCTRIANGLE_H

#include <QtGlobal>

class OscTriangle
{
public:
    OscTriangle() {}

    void initialize(quint32 sampleRate);

    // Freq in Hz, delay in s
    void getData(float * data, quint32 chunkCount, float freq, float delay);

private:
    quint32 _sampleRate;
    float _fourInvSampleRate;
    float _previousFreq;
    quint32 _currentDelay;
    bool _delayEnded;
    float _delta, _previousPoint;
};

#endif // OSCTRIANGLE_H
