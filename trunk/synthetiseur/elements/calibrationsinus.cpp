/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "calibrationsinus.h"
#include "qmath.h"

CalibrationSinus::CalibrationSinus() :
    _sinus(NULL),
    _freq(440),
    _level(0),
    _currentLevel(0),
    _buf(NULL)
{
    initBuffer(1024);
}

CalibrationSinus::~CalibrationSinus()
{
    delete _sinus;
    delete [] _buf;
}

void CalibrationSinus::initBuffer(int size)
{
    delete [] _buf;
    _bufSize = size;
    _buf = new float[_bufSize];
}

void CalibrationSinus::setSampleRate(int sampleRate)
{
    delete _sinus;
    _sinus = new OscSinus(sampleRate);
}

void CalibrationSinus::setPitch(int numNote)
{
    _mutex.lock();
    _freq = 440.0 * qPow(2., (double)(numNote - 69) / 12.);
    _mutex.unlock();
}

void CalibrationSinus::on()
{
    _mutex.lock();
    _level = 0.7;
    _mutex.unlock();
}

void CalibrationSinus::off()
{
    _mutex.lock();
    _level = 0;
    _mutex.unlock();
}

void CalibrationSinus::addData(float * dataR, float * dataL, int len)
{
    if (!_sinus)
        return;

    if (len > _bufSize)
        initBuffer(len);

    if (!_mutex.tryLock(1)) // Impossible ici d'attendre
        return;
    double freq = _freq;
    double level = _level;
    _mutex.unlock();

    if (level == 0 && _currentLevel <= 0.0004)
        return;

    // Génération et copie
    _sinus->getSinus(_buf, len, freq);

    for (int i = 0; i < len; i++)
    {
        (level > _currentLevel) ? _currentLevel += 0.0004 : _currentLevel -= 0.0004;
        dataR[i] += _currentLevel * _buf[i];
        dataL[i] += _currentLevel * _buf[i];
    }
}
