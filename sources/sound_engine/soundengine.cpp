/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "soundengine.h"
#include "voicelist.h"
#include "voice.h"
#include <QThread>

VoiceList * SoundEngine::s_voices = nullptr;

SoundEngine::SoundEngine(QSemaphore * semRunning, quint32 bufferSize) : QObject(),
    _interrupted(false),
    _semRunning(semRunning)
{
    _dataL = new float[bufferSize];
    _dataR = new float[bufferSize];
    _dataRevL = new float[bufferSize];
    _dataRevR = new float[bufferSize];
    _dataChoL = new float[bufferSize];
    _dataChoR = new float[bufferSize];
    _dataChoRevL = new float[bufferSize];
    _dataChoRevR = new float[bufferSize];
    _dataTmp = new float[bufferSize];
    _mutexSynchro.lock();
}

SoundEngine::~SoundEngine()
{
    delete [] _dataL;
    delete [] _dataR;
    delete [] _dataRevL;
    delete [] _dataRevR;
    delete [] _dataChoL;
    delete [] _dataChoR;
    delete [] _dataChoRevL;
    delete [] _dataChoRevR;
    delete [] _dataTmp;
}

// DATA GENERATION //

void SoundEngine::start()
{
    // Generate and copy data into the buffer after each reading
    while (true)
    {
        _mutexSynchro.lock();
        if (_interrupted)
        {
            _mutexSynchro.unlock();
            return;
        }

        // Generate data
        generateData(_lenToPrepare);
        _semRunning->release();
    }
}

void SoundEngine::stop()
{
    _interrupted = true;
    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

void SoundEngine::prepareData(quint32 len)
{
    _lenToPrepare = len;
    _mutexSynchro.unlock();
}

void SoundEngine::generateData(quint32 len)
{
    bool initialized = false;

    Voice * voice;
    float tmp, coefR, coefL, coefRev, coefNonRev, coefCho, coefNonCho;
    quint32 i;
    while ((voice = s_voices->getNextVoiceToCompute()) != nullptr)
    {
        // Get data
        voice->generateData(_dataTmp, len);

        tmp = 0.005f * (voice->getParam()->getFloat(champ_pan) + 50.f); // Between 0 and 1/2 for [0; PI/2]
        coefL = Voice::fastCos(tmp);
        coefR = Voice::fastSin(tmp);

        coefRev = 0.01f * voice->getParam()->getFloat(champ_reverbEffectsSend);
        coefNonRev = 1.f - coefRev;

        coefCho = 0.01f * voice->getParam()->getFloat(champ_chorusEffectsSend);
        coefNonCho = 1.f - coefCho;

        // Merge or initialize data
        if (initialized)
        {
            // Left, no reverb, no chorus
            tmp = coefL * coefNonRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataL[i] += tmp * _dataTmp[i];

            // Right, no reverb, no chorus
            tmp = coefR * coefNonRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataR[i] += tmp * _dataTmp[i];

            // Left, reverb, no chorus
            tmp = coefL * coefRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataRevL[i] += tmp * _dataTmp[i];

            // Right, reverb, no chorus
            tmp = coefR * coefRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataRevR[i] += tmp * _dataTmp[i];

            // Left, no reverb, chorus
            tmp = coefL * coefNonRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoL[i] += tmp * _dataTmp[i];

            // Right, no reverb, chorus
            tmp = coefR * coefNonRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoR[i] += tmp * _dataTmp[i];

            // Left, reverb, chorus
            tmp = coefL * coefRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoRevL[i] += tmp * _dataTmp[i];

            // Right, reverb, chorus
            tmp = coefR * coefRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoRevR[i] += tmp * _dataTmp[i];
        }
        else
        {
            initialized = true;

            // Left, no reverb, no chorus
            tmp = coefL * coefNonRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataL[i] = tmp * _dataTmp[i];

            // Right, no reverb, no chorus
            tmp = coefR * coefNonRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataR[i] = tmp * _dataTmp[i];

            // Left, reverb, no chorus
            tmp = coefL * coefRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataRevL[i] = tmp * _dataTmp[i];

            // Right, reverb, no chorus
            tmp = coefR * coefRev * coefNonCho;
            for (i = 0; i < len; ++i)
                _dataRevR[i] = tmp * _dataTmp[i];

            // Left, no reverb, chorus
            tmp = coefL * coefNonRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoL[i] = tmp * _dataTmp[i];

            // Right, no reverb, chorus
            tmp = coefR * coefNonRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoR[i] = tmp * _dataTmp[i];

            // Left, reverb, chorus
            tmp = coefL * coefRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoRevL[i] = tmp * _dataTmp[i];

            // Right, reverb, chorus
            tmp = coefR * coefRev * coefCho;
            for (i = 0; i < len; ++i)
                _dataChoRevR[i] = tmp * _dataTmp[i];
        }
    }

    if (!initialized)
    {
        memset(_dataL, 0, len * sizeof(float));
        memset(_dataR, 0, len * sizeof(float));
        memset(_dataRevL, 0, len * sizeof(float));
        memset(_dataRevR, 0, len * sizeof(float));
        memset(_dataChoL, 0, len * sizeof(float));
        memset(_dataChoR, 0, len * sizeof(float));
        memset(_dataChoRevL, 0, len * sizeof(float));
        memset(_dataChoRevR, 0, len * sizeof(float));
    }
}

void SoundEngine::setData(float * dataL, float * dataR, float * dataChoL, float * dataChoR,
                          float * dataRevL, float * dataRevR, float * dataChoRevL, float * dataChoRevR, quint32 len)
{
    memcpy(dataL, _dataL, len * sizeof(float));
    memcpy(dataR, _dataR, len * sizeof(float));
    memcpy(dataChoL, _dataChoL, len * sizeof(float));
    memcpy(dataChoR, _dataChoR, len * sizeof(float));
    memcpy(dataRevL, _dataRevL, len * sizeof(float));
    memcpy(dataRevR, _dataRevR, len * sizeof(float));
    memcpy(dataChoRevL, _dataChoRevL, len * sizeof(float));
    memcpy(dataChoRevR, _dataChoRevR, len * sizeof(float));
}

void SoundEngine::addData(float * dataL, float * dataR, float * dataChoL, float * dataChoR,
                          float * dataRevL, float * dataRevR, float * dataChoRevL, float * dataChoRevR, quint32 len)
{
    quint32 i;
    for (i = 0; i < len; ++i)
        dataL[i] += _dataL[i];
    for (i = 0; i < len; ++i)
        dataR[i] += _dataR[i];

    for (i = 0; i < len; ++i)
        dataChoL[i] += _dataChoL[i];
    for (i = 0; i < len; ++i)
        dataChoR[i] += _dataChoR[i];

    for (i = 0; i < len; ++i)
        dataRevL[i] += _dataRevL[i];
    for (i = 0; i < len; ++i)
        dataRevR[i] += _dataRevR[i];

    for (i = 0; i < len; ++i)
        dataChoRevL[i] += _dataChoRevL[i];
    for (i = 0; i < len; ++i)
        dataChoRevR[i] += _dataChoRevR[i];
}
