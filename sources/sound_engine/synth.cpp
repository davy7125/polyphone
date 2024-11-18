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

#include "synth.h"
#include <QThread>
#include <QFile>
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "division.h"
#include "smpl.h"
#include "parametermodulator.h"
#include "voice.h"

int Synth::s_sampleVoiceTokenCounter = 0;

Synth::Synth(Soundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts) : QObject(nullptr),
    _soundfonts(soundfonts),
    _mutexSoundfonts(mutexSoundfonts),
    _soundEngines(nullptr),
    _soundEngineCount(0),
    _effectsInUse(0),
    _bufferSize(0),
    _dataL(nullptr),
    _dataR(nullptr),
    _dataChoL(nullptr),
    _dataChoR(nullptr),
    _dataRevL(nullptr),
    _dataRevR(nullptr),
    _dataChoRevL(nullptr),
    _dataChoRevR(nullptr)
{
    connect(&_recorder, SIGNAL(dataWritten(quint32,quint32)), this, SIGNAL(dataWritten(quint32,quint32)));
}

Synth::~Synth()
{
    destroySoundEnginesAndBuffers();
}

void Synth::destroySoundEnginesAndBuffers()
{
    if (_soundEngineCount == 0)
        return;

    // Stop sound engines
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->stop();

    // Stop threads
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->thread()->quit();

    // Delete sound engines and threads
    for (int i = 0; i < _soundEngineCount; i++)
    {
        QThread * thread = _soundEngines[i]->thread();
        thread->wait(50);
        delete _soundEngines[i];
        delete thread;
    }
    delete [] _soundEngines;
    delete [] _dataL;
    delete [] _dataR;
    delete [] _dataChoL;
    delete [] _dataChoR;
    delete [] _dataRevL;
    delete [] _dataRevR;
    delete [] _dataChoRevL;
    delete [] _dataChoRevR;
}

void Synth::createSoundEnginesAndBuffers()
{
    _recorder.setBufferSize(_bufferSize);
    _dataL = new float[_bufferSize];
    _dataR = new float[_bufferSize];
    _dataChoL = new float[_bufferSize];
    _dataChoR = new float[_bufferSize];
    _dataRevL = new float[_bufferSize];
    _dataRevR = new float[_bufferSize];
    _dataChoRevL = new float[_bufferSize];
    _dataChoRevR = new float[_bufferSize];

    _soundEngineCount = qMax(QThread::idealThreadCount(), 1);
    _soundEngines = new SoundEngine * [_soundEngineCount];
    //qWarning() << _soundEngineCount << "sound engines created";

    for (int i = 0; i < _soundEngineCount; i++)
    {
        SoundEngine * soundEngine = new SoundEngine(&_semRunningSoundEngines, _bufferSize);
        soundEngine->moveToThread(new QThread());
        soundEngine->thread()->start(QThread::TimeCriticalPriority);
        QMetaObject::invokeMethod(soundEngine, "start");
        _soundEngines[i] = soundEngine;
    }

    _voices.initialize(this, _soundEngineCount);
    SoundEngine::initialize(&this->_voices);

    // Start producing data
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->prepareData(_bufferSize);
}

int Synth::play(EltID id, int channel, int key, int velocity)
{
    //qWarning() << "PLAY on channel" << channel << "key" << key << "vel" << velocity << id.toString();

    // Possibly release voices
    if (velocity == 0)
    {
        releaseVoices(id.indexSf2, id.indexElt, channel, key);
        return -1;
    }

    // Corresponding soundfont
    if (_mutexSoundfonts)
        QMutexLocker mutexLocker(_mutexSoundfonts);
    Soundfont * soundfont = _soundfonts->getSoundfont(id.indexSf2);
    if (soundfont == nullptr)
        return -1;

    // A key is pressed
    int playingToken = -1;
    switch (id.typeElement)
    {
    case elementSmpl:
    {
        Smpl * smpl = soundfont->getSample(id.indexElt);
        if (smpl == nullptr)
            return -1;
        playingToken = playSmpl(smpl, channel, key, velocity, false);

        // Stereo link?
        if (smpl->_sfSampleType != monoSample && smpl->_sfSampleType != RomMonoSample)
        {
            Smpl * otherSmpl = soundfont->getSample(smpl->_wSampleLink);
            if (otherSmpl != nullptr)
                playSmpl(otherSmpl, channel, key, velocity, true);
        }
    } break;
    case elementInst: case elementInstSmpl:
    {
        // Launch the instrument and all other instruments that are "always played"
        QVector<InstPrst *> instruments = soundfont->getInstruments().values();
        foreach (InstPrst * inst, instruments)
            if (inst->isAlwaysPlayed() || inst->getId().indexElt == id.indexElt)
                playInst(soundfont, inst, channel, key, velocity);
    } break;
    case elementPrst: case elementPrstInst:
    {
        // Launch the preset and all other presets that are "always played"
        QVector<InstPrst *> presets = soundfont->getPresets().values();
        foreach (InstPrst * prst, presets)
            if (prst->isAlwaysPlayed() || prst->getId().indexElt == id.indexElt)
                playPrst(soundfont, prst, channel, key, velocity);
    } break;
    default:
        return -1;
    }

    // Prepared voices are now ready to be computed
    _voices.addPreparedVoices();

    return playingToken;
}

void Synth::playPrst(Soundfont * soundfont, InstPrst * prst, int channel, int key, int velocity)
{
    // Default preset range
    RangesType defaultKeyRange, defaultVelRange;
    if (prst->getGlobalDivision()->isSet(champ_keyRange))
        defaultKeyRange = prst->getGlobalDivision()->getGen(champ_keyRange).rValue;
    else
    {
        defaultKeyRange.byLo = 0;
        defaultKeyRange.byHi = 127;
    }
    if (prst->getGlobalDivision()->isSet(champ_velRange))
        defaultVelRange = prst->getGlobalDivision()->getGen(champ_velRange).rValue;
    else
    {
        defaultVelRange.byLo = 0;
        defaultVelRange.byHi = 127;
    }

    // Browse the ranges of all linked instruments
    RangesType currentKeyRange, currentVelRange;
    QVector<Division *> divisions = prst->getDivisions().values();
    for (int i = 0; i < divisions.count(); ++i)
    {
        // Skip hidden or muted divisions
        Division * prstDiv = divisions[i];
        if (prstDiv->isHidden() || prstDiv->isMute())
            continue;

        // Check that {key, vel} is in the division and go inside the instruments
        currentKeyRange = prstDiv->isSet(champ_keyRange) ? prstDiv->getGen(champ_keyRange).rValue : defaultKeyRange;
        currentVelRange = prstDiv->isSet(champ_velRange) ? prstDiv->getGen(champ_velRange).rValue : defaultVelRange;
        if (currentKeyRange.byLo <= key && key <= currentKeyRange.byHi && currentVelRange.byLo <= velocity && velocity <= currentVelRange.byHi)
        {
            InstPrst * inst = soundfont->getInstrument(prstDiv->getGen(champ_instrument).wValue);
            if (inst != nullptr)
                this->playInst(soundfont, inst, channel, key, velocity, prst, prstDiv);
        }
    }
}

void Synth::playInst(Soundfont * soundfont, InstPrst * inst, int channel, int key, int velocity,
                     InstPrst * prst, Division * prstDiv)
{
    // Default instrument range
    RangesType defaultKeyRange, defaultVelRange;
    if (inst->getGlobalDivision()->isSet(champ_keyRange))
        defaultKeyRange = inst->getGlobalDivision()->getGen(champ_keyRange).rValue;
    else
    {
        defaultKeyRange.byLo = 0;
        defaultKeyRange.byHi = 127;
    }
    if (inst->getGlobalDivision()->isSet(champ_velRange))
        defaultVelRange = inst->getGlobalDivision()->getGen(champ_velRange).rValue;
    else
    {
        defaultVelRange.byLo = 0;
        defaultVelRange.byHi = 127;
    }

    // Browse the range of all linked samples
    RangesType currentKeyRange, currentVelRange;
    QVector<Division *> divisions = inst->getDivisions().values();
    for (int i = 0; i < divisions.count(); ++i)
    {
        // Skip hidden or muted divisions
        Division * instDiv = divisions[i];
        if (instDiv->isHidden() || instDiv->isMute())
            continue;

        // Check that {key, vel} is in the division and go inside the samples
        currentKeyRange = instDiv->isSet(champ_keyRange) ? instDiv->getGen(champ_keyRange).rValue : defaultKeyRange;
        currentVelRange = instDiv->isSet(champ_velRange) ? instDiv->getGen(champ_velRange).rValue : defaultVelRange;
        if (currentKeyRange.byLo <= key && key <= currentKeyRange.byHi && currentVelRange.byLo <= velocity && velocity <= currentVelRange.byHi)
        {
            Smpl * smpl = soundfont->getSample(instDiv->getGen(champ_sampleID).wValue);
            if (smpl != nullptr)
                this->playSmpl(smpl, channel, key, velocity, false,
                               inst, instDiv, prst, prstDiv);
        }
    }
}

int Synth::playSmpl(Smpl * smpl, int channel, int key, int velocity, bool other,
                    InstPrst * inst, Division * instDiv,
                    InstPrst * prst, Division * prstDiv)
{
    // Load the sound
    smpl->_sound.loadInRam();

    int currentToken = s_sampleVoiceTokenCounter++;

    // Get a new voice to prepare
    Voice * voice = _voices.getVoiceToPrepare();
    if (voice == nullptr) // Maybe nothing else can be added
        return currentToken;

    // Configure it
    voice->initialize(prst, prstDiv, inst, instDiv, smpl,
                      channel, key, velocity, inst == nullptr ? (other ? 2 : 1) : 0,
                      _internalConfiguration.sampleRate, currentToken);
    configureVoice(voice);

    return currentToken;
}

void Synth::configureVoice(Voice * voice)
{
    // Loop / gain
    VoiceParam * voiceParam = voice->getParam();
    if (voiceParam->getType() == 0)
    {
        // Sample triggered from the instrument or preset level
        voice->setGain(_configuration.masterGain);
    }
    else
    {
        // Sample triggered from the sample level
        voiceParam->setLoopMode(_internalConfiguration.smplIsLoopEnabled ? 1 : 0);

        float pan = voiceParam->getFloat(champ_pan);
        if (_internalConfiguration.smplIsStereoEnabled)
        {
            if (pan < 0)
                voiceParam->setPan(-50.0f);
            else if (pan > 0.0f)
                voiceParam->setPan(50.0f);

            voice->setGain(_configuration.masterGain + _internalConfiguration.smplGain);
        }
        else
        {
            if (pan < 0.0f)
                voiceParam->setPan(-1.0f);
            else if (pan > 0.0f)
                voiceParam->setPan(1.0f);

            voice->setGain(voiceParam->getType() == 1 ? _configuration.masterGain + _internalConfiguration.smplGain : -1000.0f);
        }
    }
}

void Synth::configureVoices()
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    Voice * voice;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voice = i < count1 ? voices1[i] : voices2[i - count1];
        configureVoice(voice);
    }
}

void Synth::releaseVoices(int sf2Id, int presetId, int channel, int key)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    Voice * voice;
    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voice = i < count1 ? voices1[i] : voices2[i - count1];
        voiceParam = voice->getParam();

        // Channel filter
        if (channel != -2 && voiceParam->getChannel() != channel)
            continue;

        // Sf2 filter
        if (sf2Id != -1 && voiceParam->getSf2Id() != sf2Id)
            continue;

        // Preset filter
        if (presetId != -1 && voiceParam->getPresetId() != -1 && voiceParam->getPresetId() != presetId)
            continue;

        // Key filter
        if (key != -2 && (key != -1 || voiceParam->getKey() >= 0) && voiceParam->getKey() != key)
            continue;

        voice->release();
    }
}

void Synth::stop(bool allChannels)
{
    // Stop required for all voices
    releaseVoices(-1, -1, allChannels ? -2 : -1, -2);

    //Voice ** voices1, ** voices2;
    //int count1, count2;
    //_voices.getVoices(voices1, count1, voices2, count2);
    // for (int i = s_numberOfVoices - 1; i >= 0; i--)
    // {
    //     if (allChannels || s_voices[i]->getChannel() == -1)
    //     {
    //         // Signal emitted for the sample player (voice -1)
    //         if (s_voices[i]->getKey() == -1)
    //             s_voices[i]->triggerReadFinishedSignal();

    //         --s_numberOfVoices;
    //         Voice * voiceTmp = s_voices[s_numberOfVoices];
    //         s_voices[s_numberOfVoices] = s_voices[i];
    //         s_voices[i] = voiceTmp;
    //     }
    // }
}

void Synth::configure(SynthConfig * configuration)
{
    this->stop(true);
    _configuration.copy(configuration);

    // Update reverb and chorus

    // Possibly wait if another thread is using the reverb or the chorus
    _internalConfiguration.reverbOn = false;
    _internalConfiguration.chorusOn = false;
    while (!_effectsInUse.testAndSetRelaxed(0, 1))
        QThread::msleep(2);

    _internalConfiguration.reverbOn = _configuration.revLevel > 0;
    _reverb.setEffectMix(0.01 * _configuration.revLevel);
    _reverb.setRoomSize(0.01 * _configuration.revSize);
    _reverb.setWidth(0.01 * _configuration.revWidth);
    _reverb.setDamping(0.01 * _configuration.revDamping);

    _internalConfiguration.chorusOn = _configuration.choLevel > 0;
    _chorusRevL.setEffectMix(0.005 * _configuration.choLevel);
    _chorusRevL.setModDepth(0.00025 * _configuration.choDepth);
    _chorusRevL.setModFrequency(0.06667 * _configuration.choFrequency);
    _chorusRevR.setEffectMix(0.005 * _configuration.choLevel);
    _chorusRevR.setModDepth(0.00025 * _configuration.choDepth);
    _chorusRevR.setModFrequency(0.06667 * _configuration.choFrequency);
    _chorusL.setEffectMix(0.005 * _configuration.choLevel);
    _chorusL.setModDepth(0.00025 * _configuration.choDepth);
    _chorusL.setModFrequency(0.06667 * _configuration.choFrequency);
    _chorusR.setEffectMix(0.005 * _configuration.choLevel);
    _chorusR.setModDepth(0.00025 * _configuration.choDepth);
    _chorusR.setModFrequency(0.06667 * _configuration.choFrequency);
    _effectsInUse.storeRelaxed(0);

    // Update the tuning fork and temperament
    Voice::setTuningFork(_configuration.tuningFork);

    if (_configuration.temperament.count() == 14)
    {
        float temperament[12];
        for (int i = 1; i < 13; i++)
            temperament[i - 1] = _configuration.temperament[i].toFloat();
        int temperamentRelativeKey = _configuration.temperament[13].toInt() % 12;
        Voice::setTemperament(temperament, temperamentRelativeKey);
    }
    else
    {
        float temperament[12];
        memset(temperament, 0, 12 * sizeof(float));
        Voice::setTemperament(temperament, 0);
    }
}

void Synth::setIMidiValues(IMidiValues * midiValues)
{
    ParameterModulator::setIMidiValues(midiValues);
}

void Synth::setSampleGain(double gain)
{
    // Modification du gain des samples
    _internalConfiguration.smplGain = gain;
    configureVoices();
}

void Synth::setStereo(bool isStereoEnabled)
{
    // Enable the stereo when playing a sample
    _internalConfiguration.smplIsStereoEnabled = isStereoEnabled;
    configureVoices();
}

bool Synth::isStereoEnabled()
{
    return _internalConfiguration.smplIsStereoEnabled;
}

void Synth::setLoopEnabled(bool isEnabled)
{
    // Enable the loop when playing a sample
    _internalConfiguration.smplIsLoopEnabled = isEnabled;
    configureVoices();
}

void Synth::setSinus(bool isOn, int rootKey)
{
    _sinus.setPitch(rootKey);
    if (isOn)
        _sinus.on();
    else
        _sinus.off();
}

void Synth::activateSmplEq(bool isActivated)
{
    if (isActivated)
        _eq.on();
    else
        _eq.off();
}

void Synth::setSmplEqValues(int values[10])
{
    _eq.setValues(values);
}

void Synth::setStartLoop(quint32 startLoop, bool withLinkedSample)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getType() == 1 || (voiceParam->getType() == 2 && withLinkedSample))
            voiceParam->setLoopStart(startLoop);
    }
}

void Synth::setEndLoop(quint32 endLoop, bool withLinkedSample)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getType() == 1 || (voiceParam->getType() == 2 && withLinkedSample))
            voiceParam->setLoopEnd(endLoop);
    }
}

void Synth::setPitchCorrection(qint16 correction, bool withLinkedSample)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getType() == 1 || (voiceParam->getType() == 2 && withLinkedSample))
            voiceParam->setFineTune(correction);
    }
}

void Synth::startNewRecord(QString fileName)
{
    _recorder.startNewRecord(fileName, _internalConfiguration.sampleRate);
}

void Synth::endRecord()
{
    _recorder.endRecord();
}

void Synth::pause(bool isOn)
{
    _recorder.pause(isOn);
}

bool Synth::processKey(int channel, int key, int vel)
{
    Q_UNUSED(channel)
    Q_UNUSED(key)
    Q_UNUSED(vel)
    return false;
}

bool Synth::processPolyPressureChanged(int channel, int key, int pressure)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getChannel() == channel && voiceParam->getKey() == key)
            voiceParam->processPolyPressureChanged(pressure);
    }

    return false;
}

bool Synth::processMonoPressureChanged(int channel, int value)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getChannel() == channel)
            voiceParam->processMonoPressureChanged(value);
    }

    return false;
}

bool Synth::processControllerChanged(int channel, int num, int value)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getChannel() == channel)
            voiceParam->processControllerChanged(num, value);
    }

    return false;
}

bool Synth::processBendChanged(int channel, float value)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getChannel() == channel)
            voiceParam->processBendChanged(value);
    }

    return false;
}

bool Synth::processBendSensitivityChanged(int channel, float semitones)
{
    Voice ** voices1, ** voices2;
    int count1, count2;
    _voices.getVoices(voices1, count1, voices2, count2);

    VoiceParam * voiceParam;
    for (int i = 0; i < count1 + count2; ++i)
    {
        voiceParam = i < count1 ? voices1[i]->getParam() : voices2[i - count1]->getParam();
        if (voiceParam->getChannel() == channel)
            voiceParam->processBendSensitivityChanged(semitones);
    }

    return false;
}

void Synth::setSampleRateAndBufferSize(quint32 sampleRate, quint32 bufferSize)
{
    // Mutex not mandatory: no data generation when "setFormat" is called
    _internalConfiguration.sampleRate = sampleRate;

    // Reset
    this->stop(true);

    // Sample rate update
    _sinus.setSampleRate(sampleRate);
    _eq.setSampleRate(sampleRate);

    // Update buffer size
    bufferSize *= 2;
    if (_bufferSize != bufferSize)
    {
        _bufferSize = bufferSize;
        destroySoundEnginesAndBuffers();
        createSoundEnginesAndBuffers();
    }
}

void Synth::readData(float * dataL, float * dataR, quint32 maxlen)
{
    if (_soundEngineCount == 0)
    {
        memset(dataL, 0, maxlen * sizeof(float));
        memset(dataR, 0, maxlen * sizeof(float));
        return;
    }

    // Stop the current computation, in case it's not finished yet
    _voices.endComputation();
    _semRunningSoundEngines.acquire(_soundEngineCount);

    // Get the data of all sound engines
    gatherSoundEngineData(maxlen);

    // Wake up the sound engines (as soon as possible)
    _voices.prepareComputation();
    for (int i = 0; i < _soundEngineCount; ++i)
        _soundEngines[i]->prepareData(maxlen);

    // Apply chorus / reverb on the gathered data
    applyChoRev(dataL, dataR, maxlen);

    // EQ filter (live preview of filtered samples)
    _eq.filterData(dataL, dataR, maxlen);

    // Add calibrating sinus
    _sinus.addData(dataL, dataR, maxlen);

    // Clipping
    for (unsigned int i = 0; i < maxlen; ++i)
    {
        if (dataL[i] > 1.0f)
            dataL[i] = 1.0f;
        else if (dataL[i] < -1.0f)
            dataL[i] = -1.0f;
    }
    for (unsigned int i = 0; i < maxlen; ++i)
    {
        if (dataR[i] > 1.0f)
            dataR[i] = 1.0f;
        else if (dataR[i] < -1.0f)
            dataR[i] = -1.0f;
    }

    // Possible recording
    _recorder.process(dataL, dataR, maxlen);
}

void Synth::gatherSoundEngineData(quint32 maxlen)
{
    // Gather the different parts of the sound
    _soundEngines[0]->setData(_dataL, _dataR, _dataChoL, _dataChoR, _dataRevL, _dataRevR, _dataChoRevL, _dataChoRevR, maxlen);
    for (int i = 1; i < _soundEngineCount; i++)
        _soundEngines[i]->addData(_dataL, _dataR, _dataChoL, _dataChoR, _dataRevL, _dataRevR, _dataChoRevL, _dataChoRevR, maxlen);
}

void Synth::applyChoRev(float * dataL, float * dataR, quint32 maxlen)
{
    // Copy the data rev + chorus
    memcpy(dataL, _dataChoRevL, maxlen * sizeof(float));
    memcpy(dataR, _dataChoRevR, maxlen * sizeof(float));

    bool lockAcquired = false;
    if (_internalConfiguration.chorusOn || _internalConfiguration.reverbOn)
        lockAcquired = _effectsInUse.testAndSetRelaxed(0, 1);
    if (_internalConfiguration.chorusOn && lockAcquired)
    {
        // Apply chorus
        for (quint32 i = 0; i < maxlen; i++)
            dataL[i] = static_cast<float>(_chorusRevL.tick(static_cast<double>(dataL[i])));
        for (quint32 i = 0; i < maxlen; i++)
            dataR[i] = static_cast<float>(_chorusRevR.tick(static_cast<double>(dataR[i])));
    }

    // Add the data rev
    for (quint32 i = 0; i < maxlen; i++)
        dataL[i] += _dataRevL[i];
    for (quint32 i = 0; i < maxlen; i++)
        dataR[i] += _dataRevR[i];

    if (_internalConfiguration.reverbOn && lockAcquired)
    {
        // Apply reverb
        for (quint32 i = 0; i < maxlen; i++)
        {
            dataL[i] = static_cast<float>(_reverb.tick(static_cast<double>(dataL[i]), static_cast<double>(dataR[i])));
            dataR[i] = static_cast<float>(_reverb.lastOut(1));
        }
    }

    // Add the data cho, process the effect first
    if (_internalConfiguration.chorusOn && lockAcquired)
    {
        // Apply chorus
        for (quint32 i = 0; i < maxlen; i++)
            _dataChoL[i] = static_cast<float>(_chorusL.tick(static_cast<double>(_dataChoL[i])));
        for (quint32 i = 0; i < maxlen; i++)
            _dataChoR[i] = static_cast<float>(_chorusR.tick(static_cast<double>(_dataChoR[i])));
    }

    if (lockAcquired)
        _effectsInUse.storeRelaxed(0);

    for (quint32 i = 0; i < maxlen; i++)
        dataL[i] += _dataChoL[i];
    for (quint32 i = 0; i < maxlen; i++)
        dataR[i] += _dataChoR[i];

    // Add the data with no effects
    for (quint32 i = 0; i < maxlen; i++)
        dataL[i] += _dataL[i];
    for (quint32 i = 0; i < maxlen; i++)
        dataR[i] += _dataR[i];
}
