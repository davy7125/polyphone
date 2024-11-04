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
#include "division.h"
#include "synth.h"
#include <QThread>

Voice * SoundEngine::s_voices[MAX_NUMBER_OF_VOICES];
int SoundEngine::s_numberOfVoices = 0;
int SoundEngine::s_numberOfVoicesToCompute = 0;
QMutex SoundEngine::s_mutexVoices;
int SoundEngine::s_instanceCount = 0;
alignas(64) QAtomicInt SoundEngine::s_indexVoice = 0;

void SoundEngine::initialize(Synth * synth)
{
    s_mutexVoices.lock();

    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
    {
        s_voices[i] = new Voice();
        connect(s_voices[i], SIGNAL(currentPosChanged(quint32)), synth, SIGNAL(currentPosChanged(quint32)));
        connect(s_voices[i], SIGNAL(readFinished(int)), synth, SIGNAL(readFinished(int)));
    }
}

SoundEngine::SoundEngine(QSemaphore * semRunning, quint32 bufferSize) : QObject(),
    _interrupted(false),
    _semRunning(semRunning)
{
    _dataL = new float[4 * bufferSize];
    _dataR = new float[4 * bufferSize];
    _dataRevL = new float[4 * bufferSize];
    _dataRevR = new float[4 * bufferSize];
    _dataChoL = new float[4 * bufferSize];
    _dataChoR = new float[4 * bufferSize];
    _dataChoRevL = new float[4 * bufferSize];
    _dataChoRevR = new float[4 * bufferSize];
    _dataTmp = new float[4 * bufferSize];
    s_instanceCount++;
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
    s_instanceCount--;
}

void SoundEngine::finalize()
{
    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
        delete s_voices[i];
}

void SoundEngine::addVoices(VoiceInitializer * voiceInitializers, int numberOfVoicesToAdd, SynthConfig * config, SynthInternalConfig * internalConfig)
{
    // Possibly stop voices having the same exclusive class
    for (int i = 0; i < numberOfVoicesToAdd; ++i)
    {
        if (voiceInitializers[i].type != 0)
            continue;

        int presetNumber = (voiceInitializers[i].prst != nullptr ? voiceInitializers[i].prst->getExtraField(champ_wPreset) : -1);

        // Exclusive class: possibly stop voices
        int exclusiveClass = 0;
        if (voiceInitializers[i].inst != nullptr && voiceInitializers[i].inst->getGlobalDivision()->isSet(champ_exclusiveClass))
            exclusiveClass = voiceInitializers[i].inst->getGlobalDivision()->getGen(champ_exclusiveClass).wValue;
        if (voiceInitializers[i].instDiv != nullptr && voiceInitializers[i].instDiv->isSet(champ_exclusiveClass))
            exclusiveClass = voiceInitializers[i].instDiv->getGen(champ_exclusiveClass).wValue;
        if (exclusiveClass > 0)
            closeAll(voiceInitializers[i].channel, exclusiveClass, presetNumber);
    }

    // Add the voices
    s_mutexVoices.lock();
    for (int i = 0; i < numberOfVoicesToAdd; i++)
    {
        if (s_numberOfVoices >= MAX_NUMBER_OF_VOICES || s_voices[s_numberOfVoices] == nullptr)
            break;

        // Create a voice
        s_voices[s_numberOfVoices]->initialize(&voiceInitializers[i]);
        configureVoice(s_voices[s_numberOfVoices], config, internalConfig);
        s_numberOfVoices++;
    }
    s_mutexVoices.unlock();
}

void SoundEngine::stopAllVoices(bool allChannels)
{
    s_mutexVoices.lock();
    for (int i = s_numberOfVoices - 1; i >= 0; i--)
    {
        if (allChannels || s_voices[i]->getChannel() == -1)
        {
            // Signal emitted for the sample player (voice -1)
            if (s_voices[i]->getKey() == -1)
                s_voices[i]->triggerReadFinishedSignal();

            --s_numberOfVoices;
            Voice * voiceTmp = s_voices[s_numberOfVoices];
            s_voices[s_numberOfVoices] = s_voices[i];
            s_voices[i] = voiceTmp;
        }
    }
    s_mutexVoices.unlock();
}

void SoundEngine::releaseVoices(int sf2Id, int presetId, int channel, int key)
{
    //qWarning() << "RELEASE on channel" << channel << "key" << key << "sf2" << sf2Id << "preset" << presetId;
    s_mutexVoices.lock();

    Voice * voice;
    for (int i = 0; i < s_numberOfVoices; i++)
    {
        voice = s_voices[i];

        // Channel filter
        if (channel != -2 && voice->getChannel() != channel)
            continue;

        // Sf2 filter
        if (sf2Id != -1 && voice->getSf2Id() != sf2Id)
            continue;

        // Preset filter
        if (presetId != -1 && voice->getPresetId() != -1 && voice->getPresetId() != presetId)
            continue;

        // Key filter
        if (key != -2 && (key != -1 || voice->getKey() >= 0) && voice->getKey() != key)
            continue;

        voice->release();
    }

    s_mutexVoices.unlock();
}

void SoundEngine::configureVoices(SynthConfig * config, SynthInternalConfig * internalConfig)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        configureVoice(s_voices[i], config, internalConfig);
    s_mutexVoices.unlock();
}

void SoundEngine::configureVoice(Voice * voice, SynthConfig * config, SynthInternalConfig * internalConfig)
{
    // Loop / gain
    if (voice->getType() == 0)
    {
        // Sample triggered from the instrument or preset level
        voice->setGain(config->masterGain);
    }
    else
    {
        // Sample triggered from the sample level
        voice->setLoopMode(internalConfig->smplIsLoopEnabled ? 1 : 0);

        double pan = voice->getDoubleAttribute(champ_pan);
        if (internalConfig->smplIsStereoEnabled)
        {
            if (pan < 0)
                voice->setPan(-50);
            else if (pan > 0)
                voice->setPan(50);

            voice->setGain(config->masterGain + internalConfig->smplGain);
        }
        else
        {
            if (pan < 0)
                voice->setPan(-1);
            else if (pan > 0)
                voice->setPan(1);

            voice->setGain(voice->getType() == 1 ? config->masterGain + internalConfig->smplGain : -1000);
        }
    }
}

void SoundEngine::setPitchCorrection(qint16 correction, bool withLinkedSample)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getType() == 1 || (s_voices[i]->getType() == 2 && withLinkedSample))
            s_voices[i]->setFineTune(correction);
    s_mutexVoices.unlock();
}

void SoundEngine::setStartLoop(quint32 startLoop, bool withLinkedSample)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getType() == 1 || (s_voices[i]->getType() == 2 && withLinkedSample))
            s_voices[i]->setLoopStart(startLoop);
    s_mutexVoices.unlock();
}

void SoundEngine::setEndLoop(quint32 endLoop, bool withLinkedSample)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getType() == 1 || (s_voices[i]->getType() == 2 && withLinkedSample))
            s_voices[i]->setLoopEnd(endLoop);
    s_mutexVoices.unlock();
}

void SoundEngine::closeAll(int channel, int exclusiveClass, int numPreset)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
    {
        if (s_voices[i]->getIntAttribute(champ_exclusiveClass) == exclusiveClass &&
                s_voices[i]->getIntAttribute(champ_wPreset) == numPreset &&
                s_voices[i]->getChannel() == channel)
            s_voices[i]->release(true);
    }
    s_mutexVoices.unlock();
}

// DATA GENERATION //

void SoundEngine::start()
{
    _mutexSynchro.lock();

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

void SoundEngine::prepareComputation(int uncomputedVoiceNumber, bool voicesUnlocked)
{
    s_indexVoice.storeRelaxed(0);
    if (voicesUnlocked)
        s_mutexVoices.lock();

    // Minimum number of voices to close
    int numberOfVoicesToClose = uncomputedVoiceNumber > 0 ? uncomputedVoiceNumber + s_instanceCount : 0;

    // Voices ended?
    Voice * voice;
    for (int i = 0; i < s_numberOfVoices; ++i)
    {
        voice = s_voices[i];
        if (voice->isFinished() || (numberOfVoicesToClose > 0 && voice->isInRelease()))
        {
            // Signal emitted for the sample player (voice -1)
            if (voice->getKey() == -1)
                voice->triggerReadFinishedSignal();

            --s_numberOfVoices;
            Voice * voiceTmp = s_voices[s_numberOfVoices];
            s_voices[s_numberOfVoices] = s_voices[i];
            s_voices[i] = voiceTmp;
            --i;

            --numberOfVoicesToClose;
        }
    }

    if (numberOfVoicesToClose > 0)
    {
        // Remove the last voices
        s_numberOfVoices = s_numberOfVoices > numberOfVoicesToClose ? s_numberOfVoices - numberOfVoicesToClose : 0;
    }

    s_numberOfVoicesToCompute = s_numberOfVoices;
}

Voice * SoundEngine::getNextVoiceToCompute()
{
    int voiceIndex = s_indexVoice.fetchAndAddRelaxed(1);

    // If this is the last sound engine to finish the computation series, release the lock on the voices
    if (voiceIndex == s_numberOfVoices + s_instanceCount - 1)
        s_mutexVoices.unlock();

    return voiceIndex < s_numberOfVoices ? s_voices[voiceIndex] : nullptr;
}

void SoundEngine::endComputation(int &uncomputedVoiceCount, bool &voicesUnLocked)
{
    int currentIndex = s_indexVoice.fetchAndAddRelaxed(MAX_NUMBER_OF_VOICES);
    uncomputedVoiceCount = currentIndex < s_numberOfVoicesToCompute ? s_numberOfVoicesToCompute - currentIndex : 0;
    voicesUnLocked = (currentIndex >= s_numberOfVoicesToCompute + s_instanceCount);
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
    while ((voice = getNextVoiceToCompute()) != nullptr)
    {
        // Get data
        voice->generateData(_dataTmp, len);

        tmp = 0.005f * (static_cast<float>(voice->getDoubleAttribute(champ_pan)) + 50.f); // Between 0 and 1/2 for [0; PI/2]
        coefL = Voice::fastCos(tmp);
        coefR = Voice::fastSin(tmp);

        coefRev = 0.01f * static_cast<float>(voice->getDoubleAttribute(champ_reverbEffectsSend));
        coefNonRev = 1.f - coefRev;

        coefCho = 0.01f * static_cast<float>(voice->getDoubleAttribute(champ_chorusEffectsSend));
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

void SoundEngine::setData(float * dataL, float * dataR, quint32 len)
{
    memcpy(dataL, _dataL, len * sizeof(float));
    memcpy(dataR, _dataR, len * sizeof(float));
}

void SoundEngine::addData(float * dataL, float * dataR, quint32 len)
{
    quint32 i;
    for (i = 0; i < len; ++i)
        dataL[i] += _dataL[i];
    for (i = 0; i < len; ++i)
        dataR[i] += _dataR[i];
}

void SoundEngine::setChoData(float * dataL, float * dataR, quint32 len)
{
    memcpy(dataL, _dataChoL, len * sizeof(float));
    memcpy(dataR, _dataChoR, len * sizeof(float));
}

void SoundEngine::addChoData(float * dataL, float * dataR, quint32 len)
{
    quint32 i;
    for (i = 0; i < len; ++i)
        dataL[i] += _dataChoL[i];
    for (i = 0; i < len; ++i)
        dataR[i] += _dataChoR[i];
}

void SoundEngine::setRevData(float * dataL, float * dataR, quint32 len)
{
    memcpy(dataL, _dataRevL, len * sizeof(float));
    memcpy(dataR, _dataRevR, len * sizeof(float));
}

void SoundEngine::addRevData(float * dataL, float * dataR, quint32 len)
{
    quint32 i;
    for (i = 0; i < len; ++i)
        dataL[i] += _dataRevL[i];
    for (i = 0; i < len; ++i)
        dataR[i] += _dataRevR[i];
}

void SoundEngine::setChoRevData(float * dataL, float * dataR, quint32 len)
{
    memcpy(dataL, _dataChoRevL, len * sizeof(float));
    memcpy(dataR, _dataChoRevR, len * sizeof(float));
}

void SoundEngine::addChoRevData(float * dataL, float * dataR, quint32 len)
{
    quint32 i;
    for (i = 0; i < len; ++i)
        dataL[i] += _dataChoRevL[i];
    for (i = 0; i < len; ++i)
        dataR[i] += _dataChoRevR[i];
}
