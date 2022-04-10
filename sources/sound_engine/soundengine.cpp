/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "soundengine.h"
#include <QThread>

SoundEngine ** SoundEngine::_listInstances = nullptr;
int SoundEngine::_instanceCount = 0;
int SoundEngine::_gainSmpl = 0;
bool SoundEngine::_isStereo = false;
bool SoundEngine::_isLoopEnabled = true;
const int SoundEngine::MAX_NUMBER_OF_VOICES = 256;

SoundEngine::SoundEngine(QSemaphore * semRunning, quint32 bufferSize) : QObject(),
    _interrupted(0),
    _semRunning(semRunning),
    _voices(new Voice * [MAX_NUMBER_OF_VOICES]),
    _numberOfVoices(0)
{
    _dataL = new float [4 * bufferSize];
    _dataR = new float [4 * bufferSize];
    _dataRevL = new float [4 * bufferSize];
    _dataRevR = new float [4 * bufferSize];
    _dataTmpL = new float [4 * bufferSize];
    _dataTmpR = new float [4 * bufferSize];
}

SoundEngine::~SoundEngine()
{
    delete [] _dataL;
    delete [] _dataR;
    delete [] _dataRevL;
    delete [] _dataRevR;
    delete [] _dataTmpL;
    delete [] _dataTmpR;
    delete [] _voices;
}

void SoundEngine::addVoice(Voice ** voicesToAdd, int numberOfVoicesToAdd)
{
    // Find the less busy SoundEngine
    int index = -1;
    int minVoiceNumber = -1;
    for (int i = 0; i < _instanceCount; i++)
    {
        if (minVoiceNumber == -1 || _listInstances[i]->_numberOfVoices < minVoiceNumber)
        {
            index = i;
            minVoiceNumber = _listInstances[i]->_numberOfVoices;
        }
    }

    bool sampleLevelIncluded = false;
    for (int i = 0; i < numberOfVoicesToAdd; ++i)
    {
        if (voicesToAdd[i]->getKey() > 0)
        {
            // Exclusive class: possibly stop voices
            if (voicesToAdd[i]->getExclusiveClass() > 0)
                closeAll(voicesToAdd[i]->getChannel(), voicesToAdd[i]->getExclusiveClass(), voicesToAdd[i]->getPresetNumber());
        }
        else
        {
            sampleLevelIncluded = true;
            voicesToAdd[i]->setLoopMode(_isLoopEnabled);
        }
    }

    // Add the voices
    _listInstances[index]->addVoiceInstance(voicesToAdd, numberOfVoicesToAdd);
    if (sampleLevelIncluded)
        _listInstances[index]->setStereoInstance(_isStereo);
}

void SoundEngine::addVoiceInstance(Voice ** voicesToAdd, int numberOfVoicesToAdd)
{
    _mutexVoices.lock();
    for (int i = 0; i < numberOfVoicesToAdd; i++)
    {
        if (_numberOfVoices < MAX_NUMBER_OF_VOICES)
            _voices[_numberOfVoices++] = voicesToAdd[i];
        else
            delete voicesToAdd[i];
    }
    _mutexVoices.unlock();
}

void SoundEngine::stopAllVoices(bool allChannels)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->stopAllVoicesInstance(allChannels);
}

void SoundEngine::stopAllVoicesInstance(bool allChannels)
{
    _mutexVoices.lock();
    for (int i = _numberOfVoices - 1; i >= 0; i--)
    {
        if (allChannels || _voices[i]->getChannel() == -1)
        {
            // Signal emitted for the sample player (voice -1)
            if (_voices[i]->getKey() == -1)
                emit(readFinished(_voices[i]->getToken()));

            delete _voices[i];
            _voices[i] = _voices[--_numberOfVoices];
        }
    }
    _mutexVoices.unlock();
}

void SoundEngine::releaseVoices(int sf2Id, int presetId, int channel, int key)
{
    //qWarning() << "RELEASE on channel" << channel << "key" << key << "sf2" << sf2Id << "preset" << presetId;
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->_mutexVoices.lock();

    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->releaseVoicesInstance(sf2Id, presetId, channel, key);

    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->_mutexVoices.unlock();
}

void SoundEngine::releaseVoicesInstance(int sf2Id, int presetId, int channel, int key)
{
    Voice * voice;
    for (int i = 0; i < _numberOfVoices; i++)
    {
        voice = _voices[i];
        if ((sf2Id == -1 || voice->getSf2Id() == sf2Id) &&
                (channel == -1 || presetId == -1 || voice->getPresetId() == -1 || voice->getPresetId() == presetId) &&
                (channel == -2 || voice->getChannel() == channel) &&
                (key == -2 || (key == -1 && voice->getKey() < 0) || voice->getKey() == key))
            voice->release();
    }
}

void SoundEngine::setGain(double gain)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setGainInstance(gain);
}

void SoundEngine::setGainInstance(double gain)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() >= 0)
            _voices[i]->setGain(gain);
    _mutexVoices.unlock();
}

void SoundEngine::setChorus(int level, int depth, int frequency)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setChorusInstance(level, depth, frequency);
}

void SoundEngine::setChorusInstance(int level, int depth, int frequency)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() >= 0)
            _voices[i]->setChorus(level, depth, frequency);
    _mutexVoices.unlock();
}

void SoundEngine::setPitchCorrection(qint16 correction, bool repercute)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setPitchCorrectionInstance(correction, repercute);
}

void SoundEngine::setPitchCorrectionInstance(qint16 correction, bool repercute)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() == -1 ||
                (_voices[i]->getKey() == -2 && repercute))
            _voices[i]->setFineTune(correction);
    _mutexVoices.unlock();
}

void SoundEngine::setStartLoop(quint32 startLoop, bool repercute)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setStartLoopInstance(startLoop, repercute);
}

void SoundEngine::setStartLoopInstance(quint32 startLoop, bool repercute)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() == -1 ||
                (_voices[i]->getKey() == -2 && repercute))
            _voices[i]->setLoopStart(startLoop);
    _mutexVoices.unlock();
}

void SoundEngine::setEndLoop(quint32 endLoop, bool repercute)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setEndLoopInstance(endLoop, repercute);
}

void SoundEngine::setEndLoopInstance(quint32 endLoop, bool repercute)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() == -1 ||
                (_voices[i]->getKey() == -2 && repercute))
            _voices[i]->setLoopEnd(endLoop);
    _mutexVoices.unlock();
}

void SoundEngine::setLoopEnabled(bool isEnabled)
{
    _isLoopEnabled = isEnabled;
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setLoopEnabledInstance(isEnabled);
}

void SoundEngine::setLoopEnabledInstance(bool isEnabled)
{
    // Update voices -1 and -2
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
        if (_voices[i]->getKey() < 0)
            _voices[i]->setLoopMode(isEnabled);
    _mutexVoices.unlock();
}

void SoundEngine::setStereo(bool isStereo)
{
    _isStereo = isStereo;
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setStereoInstance(isStereo);
}

void SoundEngine::setStereoInstance(bool isStereo)
{
    // Update voices -1 and -2
    _mutexVoices.lock();
    Voice * voice1 = nullptr;
    Voice * voice2 = nullptr;
    for (int i = 0; i < _numberOfVoices; i++)
    {
        if (_voices[i]->getKey() == -1)
            voice1 = _voices[i];
        else if (_voices[i]->getKey() == -2)
            voice2 = _voices[i];
    }
    if (isStereo)
    {
        if (voice1)
        {
            double pan = voice1->getPan();
            if (pan < 0)
                voice1->setPan(-50);
            else if (pan > 0)
                voice1->setPan(50);
            voice1->setGain(_gainSmpl - 3);
        }
        if (voice2)
            voice2->setGain(_gainSmpl - 3);
    }
    else
    {
        if (voice1)
        {
            double pan = voice1->getPan();
            if (pan < 0)
                voice1->setPan(-1);
            else if (pan > 0)
                voice1->setPan(1);
            voice1->setGain(_gainSmpl);
        }
        if (voice2)
            voice2->setGain(-1000);
    }
    _mutexVoices.unlock();
}

void SoundEngine::setGainSample(int gain)
{
    _gainSmpl = gain;
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->setGainSampleInstance(gain);
}

void SoundEngine::setGainSampleInstance(int gain)
{
    // Update voices -1 and -2
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
    {
        if (_voices[i]->getKey() == -1)
        {
            if (_isStereo)
                _voices[i]->setGain(gain - 3);
            else
                _voices[i]->setGain(gain);
        }
        else if (_voices[i]->getKey() == -2 && _isStereo)
            _voices[i]->setGain(gain - 3);
    }
    _mutexVoices.unlock();
}

void SoundEngine::closeAll(int channel, int exclusiveClass, int numPreset)
{
    for (int i = 0; i < _instanceCount; i++)
        _listInstances[i]->closeAllInstance(channel, exclusiveClass, numPreset);
}

void SoundEngine::closeAllInstance(int channel, int exclusiveClass, int numPreset)
{
    _mutexVoices.lock();
    for (int i = 0; i < _numberOfVoices; i++)
    {
        if (_voices[i]->getExclusiveClass() == exclusiveClass &&
                _voices[i]->getPresetNumber() == numPreset &&
                _voices[i]->getChannel() == channel)
            _voices[i]->release(true);
    }
    _mutexVoices.unlock();
}

// DATA GENERATION //

void SoundEngine::start()
{
    _mutexSynchro.lock();

    // Generate and copy data into the buffer after each reading
    while (true)
    {
        _mutexSynchro.lock();
        if (_interrupted.loadRelaxed() == 1)
        {
            _mutexSynchro.unlock();
            return;
        }

        // Generate data
        generateData();
        _semRunning->release();
    }

    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

void SoundEngine::stop()
{
    _interrupted.storeRelaxed(1);
    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

void SoundEngine::prepareData(quint32 len)
{
    _lenToPrepare = len;
    _mutexSynchro.unlock();
}

void SoundEngine::generateData()
{
    // Initialize data
    memset(_dataL, 0, _lenToPrepare * sizeof(float));
    memset(_dataR, 0, _lenToPrepare * sizeof(float));
    memset(_dataRevL, 0, _lenToPrepare * sizeof(float));
    memset(_dataRevR, 0, _lenToPrepare * sizeof(float));

    _mutexVoices.lock();

    for (int i = _numberOfVoices - 1; i >= 0; i--)
    {
        // Get data
        _voices[i]->generateData(_dataTmpL, _dataTmpR, _lenToPrepare);
        float coef1 = _voices[i]->getReverb() / 100.0f;
        float coef2 = 1.f - coef1;

        // Merge data
        for (quint32 j = 0; j < _lenToPrepare; j++)
        {
            _dataL   [j] += coef2 * _dataTmpL[j];
            _dataR   [j] += coef2 * _dataTmpR[j];
            _dataRevL[j] += coef1 * _dataTmpL[j];
            _dataRevR[j] += coef1 * _dataTmpR[j];
        }

        // Voice ended?
        if (_voices[i]->isFinished())
        {
            // Signal emitted for the sample player (voice -1)
            if (_voices[i]->getKey() == -1)
                emit(readFinished(_voices[i]->getToken()));

            delete _voices[i];
            _voices[i] = _voices[--_numberOfVoices];
        }
    }
    _mutexVoices.unlock();
}

void SoundEngine::addRevData(float * dataL, float * dataR, quint32 len)
{
    for (quint32 i = 0; i < len; i++)
    {
        dataL[i] += _dataRevL[i];
        dataR[i] += _dataRevR[i];
    }
}

void SoundEngine::addNonRevData(float * dataL, float * dataR, quint32 len)
{
    for (quint32 i = 0; i < len; i++)
    {
        dataL[i] += _dataL[i];
        dataR[i] += _dataR[i];
    }
}
