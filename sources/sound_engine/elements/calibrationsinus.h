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

#ifndef CALIBRATIONSINUS_H
#define CALIBRATIONSINUS_H

#include "oscsinus.h"
#include <QMutex>

class CalibrationSinus
{
public:
    CalibrationSinus();
    ~CalibrationSinus();
    void setSampleRate(quint32 sampleRate);

    // Configuration du sinus
    void setPitch(int numNote);
    void on();
    void off();

    // Generate data
    void addData(float * dataR, float * dataL, quint32 len);

private:
    void initBuffer(quint32 size);

    OscSinus * _sinus;
    double _pitch, _currentPitch;
    float _level, _currentLevel;

    // Buffer de travail
    float * _buf;
    quint32 _bufSize;

    // Protection accès multiples
    QMutex _mutex;
};

#endif // CALIBRATIONSINUS_H
