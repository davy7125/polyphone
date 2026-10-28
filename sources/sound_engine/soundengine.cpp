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
#include "fastmaths.h"
#include <QThread>

VoiceList * SoundEngine::s_voices = nullptr;

SoundEngine::SoundEngine(quint32 bufferSize) : QObject(),
    _interrupted(false),
    _runningState(0)
{
    _buses.reserve(bufferSize);
    _mutexSynchro.lock();
}

SoundEngine::~SoundEngine()
{

}

// DATA GENERATION //

void SoundEngine::start()
{
    // Thread dedicated to the computation of the voices
    DenormalsDisabled denormalsDisabled;

    // Generate and copy data into the buffer after each reading
    while (true)
    {
        // Lock not needed if the running state if 3
        if (!_runningState.testAndSetRelaxed(3, 1))
            _mutexSynchro.lock();

        if (_interrupted)
        {
            _mutexSynchro.unlock();
            return;
        }

        // Generate data
        generateData(_lenToPrepare);
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
    _buses.clear(len);
    if (_runningState.testAndSetRelaxed(2, 3))
    {
        // Nothing else to do, unlock not needed since the mutex will not be locked
    }
    else if (_runningState.testAndSetRelaxed(0, 1))
        _mutexSynchro.unlock();
}

void SoundEngine::endCurrentProcessing()
{
    // Can be paused only if running
    _runningState.testAndSetRelaxed(1, 2);
}

bool SoundEngine::isIdle()
{
    // The release store in generateData makes the computed data visible
    return _runningState.loadAcquire() == 0;
}

void SoundEngine::generateData(quint32 len)
{
    bool leftInitialized = false;
    bool rightInitialized = false;

    Voice * voice;
    while (true)
    {
        voice = s_voices->getNextVoiceToCompute();
        if (voice == nullptr)
        {
            _runningState.storeRelease(0);
            break;
        }
        else
        {
            // If paused, go to the ready state et quit the loop
            int currentValue;
            if (_runningState.testAndSetRelaxed(2, 0, currentValue))
                return;
            if (currentValue == 3)
                return;
        }

        // Compute the voice and mix it
        _buses.addVoice(voice, len, leftInitialized, rightInitialized);
    }
}

void SoundEngine::copyData(SynthBuses &buses, quint32 len)
{
    _buses.copyTo(buses, len);
}

void SoundEngine::addData(SynthBuses &buses, quint32 len)
{
    _buses.addTo(buses, len);
}
