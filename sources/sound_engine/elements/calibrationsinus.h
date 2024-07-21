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

#ifndef CALIBRATIONSINUS_H
#define CALIBRATIONSINUS_H

#include "oscsinus.h"

class CalibrationSinus
{
public:
    CalibrationSinus();
    ~CalibrationSinus();
    void setSampleRate(quint32 sampleRate);

    // Calibration sine configuration
    void setPitch(int numNote);
    void on();
    void off();

    // Generate data
    void addData(float * dataL, float * dataR, quint32 len);

private:
    void initBuffer(quint32 size);

    // Oscillators
    OscSinus * _sine1, * _sine2, * _sine4;

    // Configuration
    volatile int _pitch;
    volatile float _level;

    // Evolving values for smooth transitions
    float _currentPitch;
    float _currentLevel, _currentLevelSine1, _currentLevelSine2, _currentLevelSine4;

    // Buffers
    float * _buf1, * _buf2, * _buf4;
    quint32 _bufSize;
};

#endif // CALIBRATIONSINUS_H
