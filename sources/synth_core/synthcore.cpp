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

// Set the value if the current one is the expected one
static bool testAndSetRelaxed(std::atomic<int> &value, int expected, int newValue)
{
    return value.compare_exchange_strong(expected, newValue, std::memory_order_relaxed);
}

SynthCore::SynthCore(ISynthSoundfonts * soundfonts, ISynthListener * listener) :
    _soundfonts(soundfonts),
    _sampleRate(44100),
    _reverbOn(false),
    _chorusOn(false),
    _smplGain(0),
    _smplIsStereoEnabled(false),
    _smplIsLoopEnabled(true),
    _sampleVoiceTokenCounter(0),
    _effectsInUse(0)
{
    _voices.initialize(listener, &_context);
}

SynthCore::~SynthCore()
{

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
                      _sampleRate, currentToken);
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

        float pan = voiceParam->getFloat(champ_pan);
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
    this->stop(true, true);
    _configuration = configuration;

    // Update reverb and chorus

    // Possibly wait if another thread is using the reverb or the chorus
    _reverbOn = false;
    _chorusOn = false;
    while (!testAndSetRelaxed(_effectsInUse, 0, 1))
        std::this_thread::sleep_for(std::chrono::milliseconds(2));

    _reverbOn = _configuration.revLevel > 0;
    _reverb.setEffectMix(0.01 * _configuration.revLevel);
    _reverb.setRoomSize(0.01 * _configuration.revSize);
    _reverb.setWidth(0.01 * _configuration.revWidth);
    _reverb.setDamping(0.01 * _configuration.revDamping);

    _chorusOn = _configuration.choLevel > 0;
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
    _effectsInUse.store(0, std::memory_order_relaxed);

    // Settings read by the voices
    _context.setTuning(_configuration.referencePitch, _configuration.temperament, _configuration.temperamentRelativeKey);
    _context.setModulatorVelToFilterType(_configuration.modulatorVelToFilterType);
}

void SynthCore::setMidiValues(IMidiValues * midiValues)
{
    _context.setMidiValues(midiValues);
}

void SynthCore::setSampleRate(uint32_t sampleRate)
{
    // Mutex not mandatory: no data generation when the sample rate changes
    _sampleRate = sampleRate;

    // Reset
    this->stop(true, true);

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

bool SynthCore::render(float * dataL, float * dataR, uint32_t len)
{
    // The voices only compute whole chunks
    if (len % COMPUTATION_CHUNK != 0)
    {
        memset(dataL, 0, len * sizeof(float));
        memset(dataR, 0, len * sizeof(float));
        return false;
    }
    _buses.reserve(len);

    // Include the voices added and remove the voices that are finished
    _voices.prepareComputation();

    // Compute all voices
    _buses.clear(len);
    bool leftInitialized = false;
    bool rightInitialized = false;
    Voice * voice;
    while ((voice = _voices.getNextVoiceToCompute()) != nullptr)
        _buses.addVoice(voice, len, leftInitialized, rightInitialized);

    // Effects and clipping
    applyEffects(dataL, dataR, _buses, len);
    FastMaths::clamp(dataL, len);
    FastMaths::clamp(dataR, len);
    return true;
}

void SynthCore::applyEffects(float * dataL, float * dataR, SynthBuses &buses, uint32_t len)
{
    // Copy the data rev + chorus
    memcpy(dataL, buses.dataChoRevL, len * sizeof(float));
    memcpy(dataR, buses.dataChoRevR, len * sizeof(float));

    bool lockAcquired = false;
    if (_chorusOn || _reverbOn)
        lockAcquired = testAndSetRelaxed(_effectsInUse, 0, 1);
    if (_chorusOn && lockAcquired)
    {
        // Apply chorus
        for (uint32_t i = 0; i < len; i++)
            dataL[i] = static_cast<float>(_chorusRevL.tick(static_cast<double>(dataL[i])));
        for (uint32_t i = 0; i < len; i++)
            dataR[i] = static_cast<float>(_chorusRevR.tick(static_cast<double>(dataR[i])));
    }

    // Add the data rev
    FastMaths::addVectors(dataL, buses.dataRevL, len);
    FastMaths::addVectors(dataR, buses.dataRevR, len);

    if (_reverbOn && lockAcquired)
    {
        // Apply reverb
        for (uint32_t i = 0; i < len; i++)
        {
            dataL[i] = static_cast<float>(_reverb.tick(static_cast<double>(dataL[i]), static_cast<double>(dataR[i])));
            dataR[i] = static_cast<float>(_reverb.lastOut(1));
        }
    }

    // Add the data cho, process the effect first
    if (_chorusOn && lockAcquired)
    {
        // Apply chorus
        for (uint32_t i = 0; i < len; i++)
            buses.dataChoL[i] = static_cast<float>(_chorusL.tick(static_cast<double>(buses.dataChoL[i])));
        for (uint32_t i = 0; i < len; i++)
            buses.dataChoR[i] = static_cast<float>(_chorusR.tick(static_cast<double>(buses.dataChoR[i])));
    }

    if (lockAcquired)
        _effectsInUse.store(0, std::memory_order_relaxed);

    FastMaths::addVectors(dataL, buses.dataChoL, len);
    FastMaths::addVectors(dataR, buses.dataChoR, len);

    // Add the data with no effects
    FastMaths::addVectors(dataL, buses.dataL, len);
    FastMaths::addVectors(dataR, buses.dataR, len);
}
