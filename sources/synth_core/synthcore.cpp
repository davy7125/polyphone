/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "synthcore.h"
#include <cstring>
#include <thread>
#include <chrono>
#include <mutex>
#include "modulatordata.h"
#include "voice.h"
#include "fastmaths.h"
#include "synthmath.h"

// Value of an attribute in a zone, 0 if it is not set
static AttributeValue getAttribute(ISynthZone * zone, AttributeType champ)
{
    if (zone->getAttributeSet()[champ])
        return zone->getAttributeValues()[champ];

    AttributeValue value;
    value.wValue = 0;
    return value;
}

// Key or velocity range of a zone, defaultRange if it is not set
static RangesType getRange(ISynthZone * zone, AttributeType champ, RangesType defaultRange)
{
    return zone->getAttributeSet()[champ] ? zone->getAttributeValues()[champ].rValue : defaultRange;
}

// Lock shared with the audio thread, which only tries to take it
static bool tryLock(std::atomic<int> &lock)
{
    int expected = 0;
    return lock.compare_exchange_strong(expected, 1, std::memory_order_acquire);
}

static void unlock(std::atomic<int> &lock)
{
    lock.store(0, std::memory_order_release);
}

SynthCore::SynthCore(ISynthSoundfonts * soundfonts, ISynthListener * listener) :
    _soundfonts(soundfonts),
    _sampleRate(44100),
    _smplGain(0),
    _smplIsStereoEnabled(false),
    _smplIsLoopEnabled(true),
    _sampleVoiceTokenCounter(0),
    _noteId(0),
    _outputL(nullptr),
    _outputR(nullptr),
    _outputPosition(0),
    _outputAvailable(0),
    _effectConfigurationChanged(false),
    _effectConfigurationLock(0)
{
    _voices.initialize(listener, &_context);
    setMaxBlockSize(DEFAULT_MAX_BLOCK_SIZE);
}

SynthCore::~SynthCore()
{
    FastMaths::deleteAlignedBuffer(_outputL);
    FastMaths::deleteAlignedBuffer(_outputR);
}

void SynthCore::setMaxBlockSize(uint32_t maxBlockSize)
{
    uint32_t previousCapacity = _buses.capacity();
    _buses.reserve(maxBlockSize);
    if (_buses.capacity() == previousCapacity)
        return;

    _voices.reserve(_buses.capacity() >> COMPUTATION_CHUNK_SHIFT);

    // The frames not returned yet are lost
    FastMaths::deleteAlignedBuffer(_outputL);
    FastMaths::deleteAlignedBuffer(_outputR);
    _outputL = FastMaths::newAlignedBuffer(_buses.capacity());
    _outputR = FastMaths::newAlignedBuffer(_buses.capacity());
    _outputPosition = 0;
    _outputAvailable = 0;
}

void SynthCore::setMaxPolyphony(int maxPolyphony)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
    _voices.setMaxPolyphony(maxPolyphony);
}

void SynthCore::setRealTime(bool isRealTime)
{
    _voices.setRealTime(isRealTime);
}

void SynthCore::prepareTables()
{
    // Synths can be created by several threads at the same time
    static std::once_flag once;
    std::call_once(once, []() {
        SFModulator::prepareConversionTables();
        FastMaths::initialize();
        Voice::prepareTables();
    });
}

int SynthCore::playSample(int sf2Id, int sampleIndex, int channel, int key, int velocity)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
    _noteId++; // Shared by all voices started here

    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Id);
    if (soundfont == nullptr)
        return -1;
    ISynthSample * smpl = soundfont->getSynthSample(sampleIndex);
    if (smpl == nullptr)
        return -1;
    int playingToken = playSmpl(sf2Id, smpl, channel, key, velocity, false);

    // Stereo link?
    if (smpl->getSampleType() != monoSample && smpl->getSampleType() != RomMonoSample)
    {
        ISynthSample * otherSmpl = soundfont->getSynthSample(smpl->getLinkedSampleIndex());
        if (otherSmpl != nullptr)
            playSmpl(sf2Id, otherSmpl, channel, key, velocity, true);
    }

    // Prepared voices are now ready to be computed
    _voices.addPreparedVoices();

    return playingToken;
}

void SynthCore::playInstrument(int sf2Id, int instrumentIndex, int channel, int key, int velocity)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
    _noteId++; // Shared by all voices started here

    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Id);
    if (soundfont == nullptr)
        return;

    // Launch the instrument and all other instruments that are "always played"
    for (int i = 0; i < soundfont->getInstrumentCount(); ++i)
    {
        ISynthInstPrst * inst = soundfont->getInstrumentAt(i);
        if (inst->isAlwaysPlayed() || inst->getIndex() == instrumentIndex)
            playInst(sf2Id, soundfont, inst, channel, key, velocity);
    }

    // Prepared voices are now ready to be computed
    _voices.addPreparedVoices();
}

void SynthCore::playPreset(int sf2Id, int presetIndex, int channel, int key, int velocity)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
    _noteId++; // Shared by all voices started here

    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Id);
    if (soundfont == nullptr)
        return;

    // Launch the preset and all other presets that are "always played"
    for (int i = 0; i < soundfont->getPresetCount(); ++i)
    {
        ISynthInstPrst * prst = soundfont->getPresetAt(i);
        if (prst->isAlwaysPlayed() || prst->getIndex() == presetIndex)
            playPrst(sf2Id, soundfont, prst, channel, key, velocity);
    }

    // Prepared voices are now ready to be computed
    _voices.addPreparedVoices();
}

void SynthCore::playPrst(int sf2Id, ISynthSoundfont * soundfont, ISynthInstPrst * prst, int channel, int key, int velocity)
{
    // Default preset ranges
    RangesType fullRange;
    fullRange.byLo = 0;
    fullRange.byHi = 127;
    RangesType defaultKeyRange = getRange(prst->getGlobalZone(), champ_keyRange, fullRange);
    RangesType defaultVelRange = getRange(prst->getGlobalZone(), champ_velRange, fullRange);

    // Browse the ranges of all linked instruments
    RangesType currentKeyRange, currentVelRange;
    for (int i = 0; i < prst->getZoneCount(); ++i)
    {
        // Skip hidden or muted zones
        ISynthZone * prstZone = prst->getZone(i);
        if (!prstZone->isPlayable())
            continue;

        // Check that {key, vel} is in the zone and go inside the instruments
        currentKeyRange = getRange(prstZone, champ_keyRange, defaultKeyRange);
        currentVelRange = getRange(prstZone, champ_velRange, defaultVelRange);
        if (currentKeyRange.byLo <= key && key <= currentKeyRange.byHi && currentVelRange.byLo <= velocity && velocity <= currentVelRange.byHi)
        {
            ISynthInstPrst * inst = soundfont->getSynthInstrument(getAttribute(prstZone, champ_instrument).wValue);
            if (inst != nullptr)
                this->playInst(sf2Id, soundfont, inst, channel, key, velocity, prst, prstZone);
        }
    }
}

void SynthCore::playInst(int sf2Id, ISynthSoundfont * soundfont, ISynthInstPrst * inst, int channel, int key, int velocity,
                         ISynthInstPrst * prst, ISynthZone * prstZone)
{
    // Default instrument ranges
    RangesType fullRange;
    fullRange.byLo = 0;
    fullRange.byHi = 127;
    RangesType defaultKeyRange = getRange(inst->getGlobalZone(), champ_keyRange, fullRange);
    RangesType defaultVelRange = getRange(inst->getGlobalZone(), champ_velRange, fullRange);

    // Browse the range of all linked samples
    RangesType currentKeyRange, currentVelRange;
    for (int i = 0; i < inst->getZoneCount(); ++i)
    {
        // Skip hidden or muted zones
        ISynthZone * instZone = inst->getZone(i);
        if (!instZone->isPlayable())
            continue;

        // Check that {key, vel} is in the zone and go inside the samples
        currentKeyRange = getRange(instZone, champ_keyRange, defaultKeyRange);
        currentVelRange = getRange(instZone, champ_velRange, defaultVelRange);
        if (currentKeyRange.byLo <= key && key <= currentKeyRange.byHi && currentVelRange.byLo <= velocity && velocity <= currentVelRange.byHi)
        {
            ISynthSample * smpl = soundfont->getSynthSample(getAttribute(instZone, champ_sampleID).wValue);
            if (smpl != nullptr)
                this->playSmpl(sf2Id, smpl, channel, key, velocity, false,
                               inst, instZone, prst, prstZone);
        }
    }
}

int SynthCore::playSmpl(int sf2Id, ISynthSample * smpl, int channel, int key, int velocity, bool other,
                        ISynthInstPrst * inst, ISynthZone * instZone,
                        ISynthInstPrst * prst, ISynthZone * prstZone)
{
    // Load the sound
    smpl->loadData();

    int currentToken = _sampleVoiceTokenCounter.fetch_add(1, std::memory_order_relaxed);

    // Get a new voice to prepare
    Voice * voice = _voices.getVoiceToPrepare();
    if (voice == nullptr) // Maybe nothing else can be added
        return currentToken;

    // Configure it
    voice->initialize(sf2Id, prst, prstZone, inst, instZone, smpl,
                      channel, key, velocity, inst == nullptr ? (other ? 2 : 1) : 0,
                      _sampleRate, currentToken, _noteId);
    configureVoice(voice);

    return currentToken;
}

void SynthCore::configureVoice(Voice * voice)
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
        voiceParam->setLoopMode(_smplIsLoopEnabled ? 1 : 0);

        float pan = voiceParam->getCurrentFloat(champ_pan); // Possibly not the audio thread
        if (_smplIsStereoEnabled)
        {
            if (pan < 0)
                voiceParam->setPan(-50.0f);
            else if (pan > 0.0f)
                voiceParam->setPan(50.0f);

            voice->setGain(_configuration.masterGain + _smplGain);
        }
        else
        {
            if (pan < 0.0f)
                voiceParam->setPan(-1.0f);
            else if (pan > 0.0f)
                voiceParam->setPan(1.0f);

            voice->setGain(voiceParam->getType() == 1 ? _configuration.masterGain + _smplGain : -1000.0f);
        }
    }
}

void SynthCore::configureVoices()
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

void SynthCore::releaseVoices(int sf2Id, int presetId, int channel, int key, bool fast)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

        voice->release(fast);
    }
}

void SynthCore::stop(bool allChannels, bool fast)
{
    // Stop required for all voices
    releaseVoices(-1, -1, allChannels ? -2 : -1, -2, fast);
}

void SynthCore::configure(const SynthCoreConfig &configuration)
{
    // The notes go on
    bool gainChanged;
    {
        std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
        gainChanged = (configuration.masterGain != _configuration.masterGain);
        _configuration = configuration;
    }

    // Reverb and chorus, updated by the audio thread before computing its next chunks so that it never stops them
    while (!tryLock(_effectConfigurationLock))
        std::this_thread::yield();
    _effectConfiguration = configuration;
    _effectConfigurationChanged.store(true, std::memory_order_relaxed);
    unlock(_effectConfigurationLock);

    // Share of the duration of the audio that computing the voices can take in real time
    _voices.setCpuBudget(configuration.cpuBudget);

    // Settings read by the voices
    _context.setTuning(configuration.referencePitch, configuration.temperament, configuration.temperamentRelativeKey);
    _context.setModulatorVelToFilterType(configuration.modulatorVelToFilterType);

    // The voices playing reach the new gain progressively (linear evolution of the volume on each chunk)
    if (gainChanged)
        configureVoices();
}

void SynthCore::updateEffects(const SynthCoreConfig &configuration)
{
    _reverb.setTargetMix(0.01 * configuration.revLevel);
    _reverb.setRoomSize(0.01 * configuration.revSize);
    _reverb.setWidth(0.01 * configuration.revWidth);
    _reverb.setDamping(0.01 * configuration.revDamping);

    SynthChorus * choruses[4] = { &_chorusRevL, &_chorusRevR, &_chorusL, &_chorusR };
    for (SynthChorus * chorus : choruses)
    {
        chorus->setTargetMix(0.005 * configuration.choLevel);
        chorus->setTargetModDepth(0.00025 * configuration.choDepth);
        chorus->setModFrequency(0.06667 * configuration.choFrequency);
    }
}

void SynthCore::setMidiValues(IMidiValues * midiValues)
{
    _context.setMidiValues(midiValues);
}

void SynthCore::setSampleRate(uint32_t sampleRate)
{
    // Mutex not mandatory: no data generation when the sample rate changes
    _sampleRate = sampleRate;

    // Reset, the frames computed with the previous sample rate are not returned
    this->stop(true, true);
    _outputAvailable = 0;
    _voices.setSampleRate(sampleRate);

    // Sample rate update
    stk::Chorus::setSampleRate(sampleRate);
}

void SynthCore::setSampleGain(double gain)
{
    // Modification du gain des samples
    _smplGain = gain;
    configureVoices();
}

void SynthCore::setStereo(bool isStereoEnabled)
{
    // Enable the stereo when playing a sample
    _smplIsStereoEnabled = isStereoEnabled;
    configureVoices();
}

void SynthCore::setLoopEnabled(bool isEnabled)
{
    // Enable the loop when playing a sample
    _smplIsLoopEnabled = isEnabled;
    configureVoices();
}

void SynthCore::setStartLoop(uint32_t startLoop, bool withLinkedSample)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

void SynthCore::setEndLoop(uint32_t endLoop, bool withLinkedSample)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

void SynthCore::setPitchCorrection(int16_t correction, bool withLinkedSample)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

void SynthCore::processPolyPressureChanged(int channel, int key, int pressure)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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
}

void SynthCore::processMonoPressureChanged(int channel, int value)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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
}

void SynthCore::processControllerChanged(int channel, int num, int value)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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

    // Stop required for all voices?
    if (num == 120)
        releaseVoices(-1, -1, channel, -2, true);
    else if (num == 123)
        releaseVoices(-1, -1, channel, -2, false);
}

void SynthCore::processBendChanged(int channel, float value)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
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
}

void SynthCore::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    std::lock_guard<std::recursive_mutex> lock(_voicesMutex);
    if (isRegistered && parameter == 0 && trigger == 4)
    {
        // Bend sensitivity changed
        float semitones = static_cast<float>(value >> 7) + 0.01f * static_cast<float>(value & 0x7F);

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
    }
}

void SynthCore::render(float * dataL, float * dataR, uint32_t len)
{
    DenormalsDisabled denormalsDisabled;
    while (len > 0)
    {
        // When everything has been returned, compute the chunks covering what remains (as for fluidsynth),
        // as much as the buffers can contain: the frames not asked are always less than a chunk
        if (_outputAvailable == 0)
        {
            uint32_t chunkLength = SynthMath::min(SynthMath::roundToChunks(len), _buses.capacity());
            renderChunks(chunkLength);
            _outputPosition = 0;
            _outputAvailable = chunkLength;
        }

        uint32_t count = SynthMath::min(len, _outputAvailable);
        memcpy(dataL, &_outputL[_outputPosition], count * sizeof(float));
        memcpy(dataR, &_outputR[_outputPosition], count * sizeof(float));
        dataL += count;
        dataR += count;
        len -= count;
        _outputPosition += count;
        _outputAvailable -= count;
    }
}

void SynthCore::renderChunks(uint32_t len)
{
    // Include the voices added and remove the voices that are finished (or fade out some of them)
    _voices.prepareComputation();

    // Compute all voices
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    _buses.clear(len);
    bool leftInitialized = false;
    bool rightInitialized = false;
    Voice * voice;
    uint32_t voiceCount = 0;
    while ((voice = _voices.getNextVoiceToCompute()) != nullptr)
    {
        _buses.addVoice(voice, len, leftInitialized, rightInitialized);
        voiceCount++;
    }
    std::chrono::steady_clock::time_point voicesComputed = std::chrono::steady_clock::now();

    // Effects and clipping
    applyEffects(_outputL, _outputR, _buses, len);
    FastMaths::clamp(_outputL, len);
    FastMaths::clamp(_outputR, len);

    // Measures for limiting the voices in real time
    uint32_t chunkCount = len >> COMPUTATION_CHUNK_SHIFT;
    _voices.addComputationTime(std::chrono::duration<double>(voicesComputed - start).count(), voiceCount * chunkCount,
                               std::chrono::duration<double>(std::chrono::steady_clock::now() - voicesComputed).count(),
                               chunkCount);
}

void SynthCore::applyEffects(float * dataL, float * dataR, SynthBuses &buses, uint32_t len)
{
    // New settings given by configure (taken at the next call if configure is writing them)
    if (_effectConfigurationChanged.load(std::memory_order_relaxed) && tryLock(_effectConfigurationLock))
    {
        SynthCoreConfig configuration = _effectConfiguration;
        _effectConfigurationChanged.store(false, std::memory_order_relaxed);
        unlock(_effectConfigurationLock);
        updateEffects(configuration);
    }

    // Copy the data rev + chorus
    memcpy(dataL, buses.dataChoRevL, len * sizeof(float));
    memcpy(dataR, buses.dataChoRevR, len * sizeof(float));

    // The effects are processed chunk by chunk: they decide for each one whether there is something to compute
    // (nothing when they are off)
    if (_chorusRevL.isOn() || _chorusRevR.isOn())
    {
        // Apply chorus
        for (uint32_t pos = 0; pos < len; pos += COMPUTATION_CHUNK)
        {
            _chorusRevL.process(&dataL[pos], COMPUTATION_CHUNK);
            _chorusRevR.process(&dataR[pos], COMPUTATION_CHUNK);
        }
    }

    // Add the data rev
    FastMaths::addVectors(dataL, buses.dataRevL, len);
    FastMaths::addVectors(dataR, buses.dataRevR, len);

    if (_reverb.isOn())
    {
        // Apply reverb
        for (uint32_t pos = 0; pos < len; pos += COMPUTATION_CHUNK)
            _reverb.process(&dataL[pos], &dataR[pos], COMPUTATION_CHUNK);
    }

    // Add the data cho, process the effect first
    if (_chorusL.isOn() || _chorusR.isOn())
    {
        // Apply chorus
        for (uint32_t pos = 0; pos < len; pos += COMPUTATION_CHUNK)
        {
            _chorusL.process(&buses.dataChoL[pos], COMPUTATION_CHUNK);
            _chorusR.process(&buses.dataChoR[pos], COMPUTATION_CHUNK);
        }
    }

    FastMaths::addVectors(dataL, buses.dataChoL, len);
    FastMaths::addVectors(dataR, buses.dataChoR, len);

    // Add the data with no effects
    FastMaths::addVectors(dataL, buses.dataL, len);
    FastMaths::addVectors(dataR, buses.dataR, len);
}
