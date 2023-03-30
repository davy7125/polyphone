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
#include "division.h"
#include "synth.h"
#include <QThread>

Voice * SoundEngine::s_voices[MAX_NUMBER_OF_VOICES];
int SoundEngine::s_numberOfVoices = 0;
QAtomicInt SoundEngine::s_indexVoice = 0;
QMutex SoundEngine::s_mutexVoices = QMutex();

int SoundEngine::s_gainSmpl = 0;
bool SoundEngine::s_isStereo = false;
bool SoundEngine::s_isLoopEnabled = true;

void SoundEngine::initialize(Synth * synth)
{
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
}

void SoundEngine::finalize()
{
    for (int i = 0; i < MAX_NUMBER_OF_VOICES; ++i)
        delete s_voices[i];
}

void SoundEngine::addVoices(VoiceInitializer * voiceInitializers, int numberOfVoicesToAdd)
{
    // Possibly stop voices having the same exclusive class
    for (int i = 0; i < numberOfVoicesToAdd; ++i)
    {
        if (voiceInitializers[i].key <= 0)
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
    VoiceInitializer * voiceInitializer;
    for (int i = 0; i < numberOfVoicesToAdd; i++)
    {
        if (s_numberOfVoices < MAX_NUMBER_OF_VOICES)
        {
            // Create a voice
            voiceInitializer = &voiceInitializers[i];
            s_voices[s_numberOfVoices]->initialize(voiceInitializer);

            if (voiceInitializer->key < 0)
            {
                s_voices[s_numberOfVoices]->setChorus(0, 0, 0);
                s_voices[s_numberOfVoices]->setLoopMode(s_isLoopEnabled);
                if (voiceInitializer->key == -1)
                    configureStereoVoice1(s_voices[s_numberOfVoices]);
                else
                    configureStereoVoice2(s_voices[s_numberOfVoices]);
            }
            else
            {
                s_voices[s_numberOfVoices]->setChorus(voiceInitializer->choLevel, voiceInitializer->choDepth, voiceInitializer->choFrequency);
                s_voices[s_numberOfVoices]->setGain(voiceInitializer->gain);
            }

            s_numberOfVoices++;
        }
        else
            break;
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
        if ((sf2Id == -1 || voice->getSf2Id() == sf2Id) &&
            (channel == -1 || presetId == -1 || voice->getPresetId() == -1 || voice->getPresetId() == presetId) &&
            (channel == -2 || voice->getChannel() == channel) &&
            (key == -2 || (key == -1 && voice->getKey() < 0) || voice->getKey() == key))
            voice->release();
    }

    s_mutexVoices.unlock();
}

void SoundEngine::setGain(double gain)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() >= 0)
            s_voices[i]->setGain(gain);
    s_mutexVoices.unlock();
}

void SoundEngine::setChorus(int level, int depth, int frequency)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() >= 0)
            s_voices[i]->setChorus(level, depth, frequency);
    s_mutexVoices.unlock();
}

void SoundEngine::setPitchCorrection(qint16 correction, bool repercute)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() == -1 ||
                (s_voices[i]->getKey() == -2 && repercute))
            s_voices[i]->setFineTune(correction);
    s_mutexVoices.unlock();
}

void SoundEngine::setStartLoop(quint32 startLoop, bool repercute)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() == -1 ||
                (s_voices[i]->getKey() == -2 && repercute))
            s_voices[i]->setLoopStart(startLoop);
    s_mutexVoices.unlock();
}

void SoundEngine::setEndLoop(quint32 endLoop, bool repercute)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() == -1 ||
                (s_voices[i]->getKey() == -2 && repercute))
            s_voices[i]->setLoopEnd(endLoop);
    s_mutexVoices.unlock();
}

void SoundEngine::setLoopEnabled(bool isEnabled)
{
    s_isLoopEnabled = isEnabled;

    // Update voices -1 and -2
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
        if (s_voices[i]->getKey() < 0)
            s_voices[i]->setLoopMode(isEnabled);
    s_mutexVoices.unlock();
}

void SoundEngine::setStereo(bool isStereo)
{
    s_isStereo = isStereo;

    // Update voices -1 and -2
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
    {
        if (s_voices[i]->getKey() == -1)
            configureStereoVoice1(s_voices[i]);
        else if (s_voices[i]->getKey() == -2)
            configureStereoVoice2(s_voices[i]);
    }
    s_mutexVoices.unlock();
}

void SoundEngine::configureStereoVoice1(Voice * voice1)
{
    double pan = voice1->getPan();
    if (s_isStereo)
    {
        if (pan < 0)
            voice1->setPan(-50);
        else if (pan > 0)
            voice1->setPan(50);
        voice1->setGain(s_gainSmpl - 3);
    }
    else
    {
        if (pan < 0)
            voice1->setPan(-1);
        else if (pan > 0)
            voice1->setPan(1);
        voice1->setGain(s_gainSmpl);
    }
}

void SoundEngine::configureStereoVoice2(Voice * voice2)
{
    voice2->setGain(s_isStereo ? s_gainSmpl - 3 : -1000);
}

void SoundEngine::setGainSample(int gain)
{
    s_gainSmpl = gain;

    // Update voices -1 and -2
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
    {
        if (s_voices[i]->getKey() == -1)
        {
            if (s_isStereo)
                s_voices[i]->setGain(gain - 3);
            else
                s_voices[i]->setGain(gain);
        }
        else if (s_voices[i]->getKey() == -2 && s_isStereo)
            s_voices[i]->setGain(gain - 3);
    }
    s_mutexVoices.unlock();
}

void SoundEngine::closeAll(int channel, int exclusiveClass, int numPreset)
{
    s_mutexVoices.lock();
    for (int i = 0; i < s_numberOfVoices; i++)
    {
        if (s_voices[i]->getExclusiveClass() == exclusiveClass &&
                s_voices[i]->getPresetNumber() == numPreset &&
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

void SoundEngine::prepareComputation()
{
    s_indexVoice.storeRelaxed(0);
    s_mutexVoices.lock();
}

Voice * SoundEngine::getNextVoiceToCompute()
{
    int voiceIndex = s_indexVoice.fetchAndAddRelaxed(1);
    return voiceIndex < s_numberOfVoices ? s_voices[voiceIndex] : nullptr;
}

void SoundEngine::endComputation()
{
    // Voices ended?
    Voice * voice;
    for (int i = 0; i < s_numberOfVoices; ++i)
    {
        voice = s_voices[i];
        if (voice->isFinished())
        {
            // Signal emitted for the sample player (voice -1)
            if (voice->getKey() == -1)
                voice->triggerReadFinishedSignal();

            --s_numberOfVoices;
            Voice * voiceTmp = s_voices[s_numberOfVoices];
            s_voices[s_numberOfVoices] = s_voices[i];
            s_voices[i] = voiceTmp;
            --i;
        }
    }

    s_mutexVoices.unlock();
}

void SoundEngine::prepareData(quint32 len)
{
    _lenToPrepare = len;
    _mutexSynchro.unlock();
}

void SoundEngine::generateData(quint32 len)
{
    // Initialize data
    memset(_dataL, 0, len * sizeof(float));
    memset(_dataR, 0, len * sizeof(float));
    memset(_dataRevL, 0, len * sizeof(float));
    memset(_dataRevR, 0, len * sizeof(float));

    Voice * voice;
    while ((voice = getNextVoiceToCompute()) != nullptr)
    {
        // Get data
        voice->generateData(_dataTmpL, _dataTmpR, len);
        float coef1 = voice->getReverb() / 100.0f;
        float coef2 = 1.f - coef1;

        // Merge data
        for (quint32 j = 0; j < len; j++)
        {
            _dataL   [j] += coef2 * _dataTmpL[j];
            _dataR   [j] += coef2 * _dataTmpR[j];
            _dataRevL[j] += coef1 * _dataTmpL[j];
            _dataRevR[j] += coef1 * _dataTmpR[j];
        }
    }
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
