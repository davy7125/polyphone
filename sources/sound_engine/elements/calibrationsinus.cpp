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
    _sine1(new OscSinus(48000)),
    _sine2(new OscSinus(48000)),
    _sine4(new OscSinus(48000)),
    _pitch(0),
    _level(0.0f),
    _currentPitch(-1.0f),
    _currentLevel(0.0f),
    _currentLevelSine1(0.0f),
    _currentLevelSine2(0.0f),
    _currentLevelSine4(0.0f),
    _buf1(nullptr),
    _buf2(nullptr),
    _buf4(nullptr)
{
    initBuffer(1024);
}

CalibrationSinus::~CalibrationSinus()
{
    delete _sine1;
    delete _sine2;
    delete _sine4;
    delete [] _buf1;
    delete [] _buf2;
    delete [] _buf4;
}

void CalibrationSinus::initBuffer(quint32 size)
{
    delete [] _buf1;
    delete [] _buf2;
    delete [] _buf4;
    _bufSize = size;
    _buf1 = new float[_bufSize];
    _buf2 = new float[_bufSize];
    _buf4 = new float[_bufSize];
}

void CalibrationSinus::setSampleRate(quint32 sampleRate)
{
    delete _sine1;
    delete _sine2;
    delete _sine4;
    _sine1 = new OscSinus(sampleRate);
    _sine2 = new OscSinus(sampleRate);
    _sine4 = new OscSinus(sampleRate);
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

    // Current pitch
    float tmp = static_cast<float>(_pitch); // Data copy
    if (_currentPitch < 0.0f)
        _currentPitch = tmp;
    if (_currentPitch - tmp < -1.0f || _currentPitch - tmp > 1.0f)
        _currentPitch += (tmp - _currentPitch) / 1.1f; // fast transition
    else
        _currentPitch = tmp; // smooth transition

    float intensity1, intensity2, intensity4;
    if (tmp < _currentPitch)
    {
        // A lower sine is required: first change the frequencies and then adapt the levels
        tmp = 440.0f * static_cast<float>(qPow(2.0, (static_cast<double>(_currentPitch) - 69.0) / 12.0)); // Fondamental frequency

        intensity1 = _currentLevelSine1;
        intensity2 = _currentLevelSine2;
        intensity4 = _currentLevelSine4;
    }
    else
    {
        // A higher sine is required: first change the levels and then adapt the frequencies
        tmp = 440.0f * static_cast<float>(qPow(2.0, (static_cast<double>(tmp) - 69.0) / 12.0)); // Fondamental frequency

        // Level to reach for each sine
        if (_currentPitch <= 48)
        {
            intensity1 = 0.33f;
            intensity4 = 0.33f;
        }
        else if (_currentPitch <= 66)
        {
            intensity1 = 0.33f + (_currentPitch - 48.0f) * (0.17f / 18.0f);
            intensity4 = 0.33f - (_currentPitch - 48.0f) * (0.33f / 18.0f);
        }
        else if (_currentPitch <= 84)
        {
            intensity1 = 0.5f + (_currentPitch - 66.0f) * (0.5f / 18.0f);
            intensity4 = 0.0f;
        }
        else
        {
            intensity1 = 1.0f;
            intensity4 = 0.0f;
        }

        intensity2 = 1.0f - intensity1 - intensity4;
    }

    // Generate data for the fondamental, the first octave (frequency * 2) and the second octave (frequency * 4)
    _sine1->getData(_buf1, len, tmp);
    _sine2->getData(_buf2, len, (intensity2 > 0.0f || _currentLevelSine2 <= 0.0004f) ? 2.0f * tmp : -1.0f);
    _sine4->getData(_buf4, len, (intensity4 > 0.0f || _currentLevelSine4 <= 0.0004f) ? 4.0f * tmp : -1.0f);

    // Add the sines to the audio signal
    for (quint32 i = 0; i < len; i++)
    {
        // Smooth transition for the levels
        if (_currentLevel > level + 0.0002f)
            _currentLevel -= 0.0002f;
        else if (_currentLevel < level - 0.0002f)
            _currentLevel += 0.0002f;
        else
            _currentLevel = level;

        if (_currentLevelSine1 > intensity1 + 0.0002f)
            _currentLevelSine1 -= 0.0002f;
        else if (_currentLevelSine1 < intensity1 - 0.0002f)
            _currentLevelSine1 += 0.0002f;
        else
            _currentLevelSine1 = intensity1;

        if (_currentLevelSine2 > intensity2 + 0.0002f)
            _currentLevelSine2 -= 0.0002f;
        else if (_currentLevelSine2 < intensity2 - 0.0002f)
            _currentLevelSine2 += 0.0002f;
        else
            _currentLevelSine2 = intensity2;

        if (_currentLevelSine4 > intensity4 + 0.0002f)
            _currentLevelSine4 -= 0.0002f;
        else if (_currentLevelSine4 < intensity4 - 0.0002f)
            _currentLevelSine4 += 0.0002f;
        else
            _currentLevelSine4 = intensity4;

        tmp = _currentLevel * (_currentLevelSine1 * _buf1[i] + _currentLevelSine2 * _buf2[i] + _currentLevelSine4 * _buf4[i]);
        dataL[i] += tmp;
        dataR[i] += tmp;
    }
}
