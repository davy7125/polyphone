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
    _sinus1(new OscSinus(48000)),
    _sinus2(new OscSinus(48000)),
    _sinus3(new OscSinus(48000)),
    _pitch(0),
    _currentPitch(-1.0f),
    _level(0.0f),
    _currentLevel(0.0f),
    _buf(nullptr)
{
    initBuffer(1024);
}

CalibrationSinus::~CalibrationSinus()
{
    delete _sinus1;
    delete _sinus2;
    delete _sinus3;
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
    delete _sinus1;
    delete _sinus2;
    delete _sinus3;
    _sinus1 = new OscSinus(sampleRate);
    _sinus2 = new OscSinus(sampleRate);
    _sinus3 = new OscSinus(sampleRate);
}

void CalibrationSinus::setPitch(int numNote)
{
    _pitch = numNote;
}

void CalibrationSinus::on()
{
    _level = 0.7f;
}

void CalibrationSinus::off()
{
    _level = 0.0f;
}

void CalibrationSinus::addData(float * dataL, float * dataR, quint32 len)
{
    // Copy data (they can be modified by another thread)
    float level = _level;

    // Possibly stop here
    if (level <= 0.0004f && _currentLevel <= 0.0004f)
        return;

    if (len > _bufSize)
        initBuffer(len);

    // Current frequency
    float pitch = static_cast<float>(_pitch); // Data copy
    if (_currentPitch < 0.0f)
        _currentPitch = pitch;
    if (_currentPitch - pitch < -1 || _currentPitch - pitch > 1.0f)
        _currentPitch += (pitch - _currentPitch) / 2.0f; // fast transition
    else
        _currentPitch = pitch; // smooth transition

    // Generate data and copy
    memset(_buf, 0, len * sizeof(float));
    if (_currentPitch > 84)
    {
        _sinus1->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch - 69.0) / 12.0)), 1.0f);
        _sinus2->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 12 - 69.0) / 12.0)), 0.0f);
        _sinus3->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 24 - 69.0) / 12.0)), 0.0f);
    }
    else if (_currentPitch > 60)
    {
        _sinus1->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch - 69.0) / 12.0)), 0.7f);
        _sinus2->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 12 - 69.0) / 12.0)), 0.7f);
        _sinus3->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 24 - 69.0) / 12.0)), 0.0f);
    }
    else
    {
        _sinus1->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch - 69.0) / 12.0)), 0.6f);
        _sinus2->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 12 - 69.0) / 12.0)), 0.6f);
        _sinus3->addData(_buf, len, 440.0f * static_cast<float>(qPow(2.0, (_currentPitch + 24 - 69.0) / 12.0)), 0.6f);
    }

    for (quint32 i = 0; i < len; i++)
    {
        // Smooth transition for the level
        if (_currentLevel > level)
            _currentLevel -= 0.0002f;
        else if (_currentLevel < level)
            _currentLevel += 0.0002f;

        dataL[i] += _currentLevel * _buf[i];
        dataR[i] += _currentLevel * _buf[i];
    }
}
