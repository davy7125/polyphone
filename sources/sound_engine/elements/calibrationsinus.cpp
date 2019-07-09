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

#include "calibrationsinus.h"
#include "qmath.h"

CalibrationSinus::CalibrationSinus() :
    _sinus(nullptr),
    _pitch(0),
    _currentPitch(-1),
    _level(0),
    _currentLevel(0),
    _buf(nullptr)
{
    initBuffer(1024);
}

CalibrationSinus::~CalibrationSinus()
{
    delete _sinus;
    delete [] _buf;
}

void CalibrationSinus::initBuffer(quint32 size)
{
    delete [] _buf;
    _bufSize = size;
    _buf = new float[_bufSize];
}

void CalibrationSinus::setSampleRate(quint32 sampleRate)
{
    delete _sinus;
    _sinus = new OscSinus(sampleRate);
}

void CalibrationSinus::setPitch(int numNote)
{
    _mutex.lock();
    _pitch = numNote;
    _mutex.unlock();
}

void CalibrationSinus::on()
{
    _mutex.lock();
    _level = 0.7f;
    _mutex.unlock();
}

void CalibrationSinus::off()
{
    _mutex.lock();
    _level = 0;
    _mutex.unlock();
}

void CalibrationSinus::addData(float * dataR, float * dataL, quint32 len)
{
    if (!_sinus)
        return;

    if (len > _bufSize)
        initBuffer(len);

    if (!_mutex.tryLock(1)) // Impossible ici d'attendre
        return;
    double pitch = _pitch;
    float level = _level;
    _mutex.unlock();

    // Possibly stop here
    if (level <= 0.0004f && _currentLevel <= 0.0004f)
        return;

    // Current frequency (smooth transitions)
    if (_currentPitch < 0)
        _currentPitch = pitch;
    if (_currentPitch > pitch)
        _currentPitch -= 0.5;
    else if (_currentPitch < pitch)
        _currentPitch += 0.5;

    // Génération et copie
    _sinus->getSinus(_buf, len, 440.0f * static_cast<float>(qPow(2., (_currentPitch - 69.) / 12.)), 0);

    for (quint32 i = 0; i < len; i++)
    {
        // Smooth transition for the level
        if (_currentLevel > level)
            _currentLevel -= 0.0002f;
        else if (_currentLevel < level)
            _currentLevel += 0.0002f;

        dataR[i] += _currentLevel * _buf[i];
        dataL[i] += _currentLevel * _buf[i];
    }
}
