/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "voice.h"
#include "voicecontext.h"
#include "fastmaths.h"
#include "synthmath.h"

alignas(32) float Voice::s_sinc_tables[8][RESAMPLING_SUBDIVISION][RESAMPLING_ORDER];
float Voice::s_floatConversionCoef24;

void Voice::prepareTables()
{
    // Create sinc tables with different windows (polyphase algorithm)
    // beta = 5   => Hamming window
    // beta = 6   => Hann window
    // beta = 8.6 => Blackman window
    for (int i = 0; i < 8; i++)
        SynthMath::fillSincTable(&s_sinc_tables[i][0][0], RESAMPLING_ORDER, RESAMPLING_SUBDIVISION, 4.3 + 0.5 * i);

    // Integer => float conversion
    s_floatConversionCoef24 = 1.0f / (0.5f + static_cast<float>(0x7FFFFF));
}

Voice::Voice(VoiceParam * voiceParam, ISynthListener * listener) :
    _listener(listener),
    _context(voiceParam->getContext()),
    _dataSmpl16(nullptr),
    _dataSmpl24(nullptr),
    _voiceParam(voiceParam)
{
    // Array initialization
    _arrayLength = 0;
    _dataVolArray = nullptr;
    _dataModArray = nullptr;
    _modLfoArray = nullptr;
    _vibLfoArray = nullptr;
    reserve(DEFAULT_MAX_BLOCK_SIZE >> COMPUTATION_CHUNK_SHIFT);
}

Voice::~Voice()
{
    deleteArrays();
    if (_voiceParam->getType() != 0 && _dataSmpl16 != nullptr)
    {
        if ((void*)_dataSmpl16 != (void*)_dataSmpl24)
            delete [] _dataSmpl24;
        delete [] _dataSmpl16;
        _dataSmpl16 = nullptr;
        _dataSmpl24 = nullptr;
    }
}

void Voice::deleteArrays()
{
    delete [] _dataVolArray;
    delete [] _dataModArray;
    delete [] _modLfoArray;
    delete [] _vibLfoArray;
}

void Voice::reserve(uint32_t chunkCount)
{
    if (chunkCount <= _arrayLength)
        return;

    deleteArrays();
    _arrayLength = chunkCount;
    _dataVolArray = new float[_arrayLength];
    _dataModArray = new float[_arrayLength];
    _modLfoArray = new float[_arrayLength];
    _vibLfoArray = new float[_arrayLength];
}

void Voice::initialize(int sf2Id, ISynthInstPrst * prst, ISynthZone * prstZone, ISynthInstPrst * inst, ISynthZone * instZone,
                       ISynthSample * smpl, int8_t channel, int8_t key, int8_t vel, int8_t type, uint32_t audioSmplRate, int token)
{
    // Possibly delete previous sample data that has been copied
    if (_voiceParam->getType() != 0 && _dataSmpl16 != nullptr)
    {
        if ((void*)_dataSmpl16 != (void*)_dataSmpl24)
            delete [] _dataSmpl24;
        delete [] _dataSmpl16;
        _dataSmpl16 = nullptr;
        _dataSmpl24 = nullptr;
    }

    _voiceParam->initialize(sf2Id, prst, prstZone, inst, instZone, smpl, channel, key, vel, type);

    smpl->getData(_dataSmplLength, _dataSmpl16, _dataSmpl24, _voiceParam->getType() != 0 /* copy data at the sample level */);
    if (_dataSmpl24 == nullptr)
        _dataSmpl24 = (uint8_t *)_dataSmpl16;
    _sincWindow = nullptr;

    _smplRate = smpl->getSampleRate();
    _smplPeak = smpl->getPeak();
    _sampleRootKey = smpl->getRootKey();
    _audioSmplRate = audioSmplRate;
    _audioSmplRateInv = 1.0f / _audioSmplRate;
    _gain = 0.0f;
    _token = token;
    _currentSmplPos = 0xFFFFFFFF;
    _elapsedSmplPos = 0;
    _time = 0;
    _release = false;
    _isFinished = false;
    _x1 = 0;
    _x2 = 0;
    _y1 = 0;
    _y2 = 0;
    _filterFrequency = -1.0f; // Coefficients not computed yet
    _filterInvQ = -1.0f;
    _isFilterOn = false;
    _volumeCoeff = 0;

    float sampleRateForChunks = static_cast<float>(_audioSmplRate) * static_cast<float>(COMPUTATION_CHUNK_INVERT);
    _modLFO.initialize(sampleRateForChunks);
    _vibLFO.initialize(sampleRateForChunks);
    _volEnvelope.initialize(sampleRateForChunks, false);
    _modEnvelope.initialize(sampleRateForChunks, true);

    // Resampling initialization
    memset(_srcData16, 0, sizeof(_srcData16));
    memset(_srcData24, 0, sizeof(_srcData24));
    _lastDistanceFraction = 0;
    _moreAvailable = -1;
}

bool Voice::generateData(float * data, uint32_t len)
{
    // Values read only once
    if (_currentSmplPos == 0xFFFFFFFF)
    {
        _currentSmplPos = _voiceParam->getPosition(champ_dwStart16);
        _loopMode = (char)_voiceParam->getInteger(champ_sampleModes);
    }

    int32_t playedNote = _voiceParam->getInteger(champ_keynum);
    float v_filterQ = _voiceParam->getFloat(champ_initialFilterQ);
    float v_filterFreq = _voiceParam->getFloat(champ_initialFilterFc);
    float v_modLfoToVolume = _voiceParam->getFloat(champ_modLfoToVolume);

    // Number of chunks (len is a multiple of COMPUTATION_CHUNK, the arrays have been reserved for it)
    uint32_t chunkCount = (len >> COMPUTATION_CHUNK_SHIFT);

    /// ENVELOPES ///
    float v_attenuation = _voiceParam->getFloat(champ_initialAttenuation);
    if (_volEnvelope.getEnvelope(_dataVolArray, chunkCount, _release, playedNote, FastMaths::fastPow10(0.05f * (_gain - v_attenuation - 0.5f * v_filterQ)), _voiceParam))
    {
        if (_loopMode != 3)
            _isFinished = true;
    }
    _modEnvelope.getEnvelope(_dataModArray, chunkCount, _release, playedNote, 1.0f, _voiceParam);

    // During the release, the voice ends after these chunks if it cannot be heard anymore: largest amplitude possible
    // under the noise floor, with the peak of the sample, the volume modulated by the LFO and the resonance of the
    // filter (Q dB, half of them being compensated by the gain of the envelope)
    if (_volEnvelope.isReleasing() && chunkCount > 0 &&
        _dataVolArray[chunkCount - 1] * _smplPeak *
        FastMaths::fastPow10(0.05f * SynthMath::abs(v_modLfoToVolume) + 0.025f * v_filterQ) < NOISE_FLOOR)
        _isFinished = true;

    /// LFOs ///
    _modLFO.getData(_modLfoArray, chunkCount,
                    _voiceParam->getFloat(champ_freqModLFO),
                    _voiceParam->getFloat(champ_delayModLFO));
    _vibLFO.getData(_vibLfoArray, chunkCount,
                    _voiceParam->getFloat(champ_freqVibLFO),
                    _voiceParam->getFloat(champ_delayVibLFO));

    bool isAudible = false;
    if (_dataSmpl16 == nullptr)
    {
        // Nothing to play
        finish();
    }
    else if (_loopMode == 2 && !_release)
    {
        // Blank until the release
    }
    else
    {
        // LFOs and modulation effects
        int32_t v_modLfoToPitch = _voiceParam->getInteger(champ_modLfoToPitch);
        int32_t v_modLfoToFilterFreq = _voiceParam->getInteger(champ_modLfoToFilterFc);
        int32_t v_vibLfoToPitch = _voiceParam->getInteger(champ_vibLfoToPitch);
        int32_t v_modEnvToPitch = _voiceParam->getInteger(champ_modEnvToPitch);
        int32_t v_modEnvToFilterFc = _voiceParam->getInteger(champ_modEnvToFilterFc);

        // Pitch modulation
        int32_t v_rootkey = _voiceParam->getInteger(champ_overridingRootKey);
        int32_t v_scaleTune = _voiceParam->getInteger(champ_scaleTuning);
        int32_t v_fineTune = _voiceParam->getInteger(champ_fineTune);
        int32_t v_coarseTune = _voiceParam->getInteger(champ_coarseTune);
        float temperamentFineTune = _voiceParam->getKey() < 0 ? 0.0f : _context->getTemperamentFineTune(playedNote);
        float pitchShift = (playedNote - v_rootkey) * v_scaleTune + (temperamentFineTune + static_cast<float>(v_fineTune)) + 100.0f * v_coarseTune;
        float deltaPitchFixed = -1731.234f /* -1200 / ln(2) */ *
                                    std::log(static_cast<double>(_audioSmplRate) * 4400.f / (_smplRate * _context->getReferencePitch())) +
                                pitchShift;
        if (_sincWindow == nullptr)
            _sincWindow = s_sinc_tables[whichSincTable(_sampleRootKey, deltaPitchFixed)];

        // Exact ratio between the speed in the sample and the speed of the output, without the modulations
        double fixedRatio = static_cast<double>(_smplRate) * _context->getReferencePitch() /
                            (static_cast<double>(_audioSmplRate) * 4400.0) * std::exp2(pitchShift / 1200.0);

        // Loop
        uint32_t loopStart = _voiceParam->getPosition(champ_dwStartLoop);
        uint32_t loopEnd = _voiceParam->getPosition(champ_dwEndLoop);
        if (loopStart >= loopEnd && _loopMode != 2)
            _loopMode = 0;

        // Parameter for the low-pass filter
        // - 3.01f => so that a value of 0 gives a non-resonant low pass
        // If v_filterQ is 0, inv_q_lin is sqrt(2)
        float inv_q_lin = FastMaths::fastPow10(-0.05f * (v_filterQ - 3.01f));

        // In the loops below, the members are copied into local variables: the compiler could not keep them in
        // registers, since the data written might be one of them
        float (*sincWindow)[RESAMPLING_ORDER] = _sincWindow;

        bool wasFinished = _isFinished;
        for (uint32_t chunk = 0; chunk < chunkCount; ++chunk)
        {
            uint32_t sampleStart = (chunk << COMPUTATION_CHUNK_SHIFT);
            float * pDataEnd = &data[sampleStart + COMPUTATION_CHUNK];

            // End of the sample reached in the previous chunk: silent until the voice is removed, as if the chunks
            // were computed one by one (otherwise the filter would ring with the following ones)
            if (_isFinished && !wasFinished)
            {
                memset(&data[sampleStart], 0, (len - sampleStart) * sizeof(float));
                break;
            }

            // Distance between the points, fixed point with 32 bits for the fraction
            float pitchModulation = v_modEnvToPitch * _dataModArray[chunk] + v_modLfoToPitch * _modLfoArray[chunk] + v_vibLfoToPitch * _vibLfoArray[chunk];
            double ratio = pitchModulation == 0.0f ? fixedRatio :
                               fixedRatio * FastMaths::fastPow2(pitchModulation * 0.000833333f /* 1:1200 */);
            uint64_t fixedGap = static_cast<uint64_t>(ratio * 4294967296.0 /* 2^32 */ + 0.5);

            // Resampling (sinc interpolation)
            int16_t * srcData16 = &_dataSmpl16[_currentSmplPos];
            uint8_t * srcData24 = &_dataSmpl24[_currentSmplPos];
            float * pData = &data[sampleStart];
            uint32_t nextPosition = 0;
            void(Voice::*slowExtractFunction)(uint32_t,uint32_t,uint32_t,int16_t*&,uint8_t*&) = (_loopMode == 1 || (_loopMode == 3 && !_release)) ? &Voice::getDataWithLoop : &Voice::getData;

resample_fast:
            // Fast extraction, if possible
            {
                uint64_t distanceFraction = _lastDistanceFraction;
                int32_t moreAvailable = _moreAvailable;
                while (true)
                {
                    distanceFraction += fixedGap;
                    if ((int32_t)(nextPosition = static_cast<uint32_t>(distanceFraction >> 32)) > moreAvailable)
                    {
                        // Revert the last jump and update the current position
                        distanceFraction -= fixedGap;
                        _currentSmplPos += static_cast<uint32_t>(distanceFraction >> 32);
                        _lastDistanceFraction = distanceFraction & 0xFFFFFFFF;
                        goto resample_slow;
                    }

                    (*pData++) = FastMaths::multiply8(sincWindow[(distanceFraction >> 24) & 0xFF], &srcData16[nextPosition], &srcData24[nextPosition]);
                                // + FastMaths::multiply8(&sincWindow[(distanceFraction >> 24) & 0xFF][8], &srcData16[nextPosition + 8], &srcData24[nextPosition + 8]);

                    if (pData >= pDataEnd)
                    {
                        // Update the current position
                        _currentSmplPos += nextPosition;
                        _moreAvailable = moreAvailable - static_cast<int32_t>(nextPosition);
                        _lastDistanceFraction = distanceFraction & 0xFFFFFFFF;
                        goto resample_end;
                    }
                }
            }

resample_slow:
            // First data extraction or complex extraction (the members are modified by the extraction functions)
            while (true)
            {
                _lastDistanceFraction += fixedGap;
                (*this.*slowExtractFunction)(static_cast<uint32_t>(_lastDistanceFraction >> 32), loopStart, loopEnd, srcData16, srcData24);
                _lastDistanceFraction &= 0xFFFFFFFF;
                (*pData++) = FastMaths::multiply8(sincWindow[(_lastDistanceFraction >> 24) & 0xFF], srcData16, srcData24);
                            // + FastMaths::multiply8(&sincWindow[(_lastDistanceFraction >> 24) & 0xFF][8], &srcData16[8], &srcData24[8]);

                if (pData >= pDataEnd)
                    goto resample_end;
                if (_moreAvailable > 0)
                    goto resample_fast;
            }
resample_end:

            // deltaCoeff is the linear evolution of the volume on a chunk
            // The target volume is based on:
            // * the volume envelope
            // * the volume modulation with values from the mod LFO converted to dB
            // The conversion from int16 to float is included here
            float deltaCoeff = (s_floatConversionCoef24 *
                                    _dataVolArray[chunk] *
                                    FastMaths::fastPow10(0.05f * v_modLfoToVolume * _modLfoArray[chunk])
                                - _volumeCoeff) * COMPUTATION_CHUNK_INVERT;

            // Silent chunk if the volume stays at 0 (delay of the envelope, end of the release)
            if (_volumeCoeff != 0.0f || deltaCoeff != 0.0f)
                isAudible = true;

            // Low-pass filter, its coefficients being computed again only if the frequency or the resonance changes
            pData = &data[sampleStart];
            float valTmp = v_filterFreq * FastMaths::fastPow2(
                         (v_modEnvToFilterFc * _dataModArray[chunk] + v_modLfoToFilterFreq * _modLfoArray[chunk]) * 0.000833333f /* 1:1200 */);
            if (valTmp != _filterFrequency || inv_q_lin != _filterInvQ)
            {
                _filterFrequency = valTmp;
                _filterInvQ = inv_q_lin;
                _isFilterOn = biQuadCoefficients(_filterCoeffs, valTmp, inv_q_lin);
            }
            float volumeCoeff = _volumeCoeff;
            if (_isFilterOn)
            {
                const float c0 = _filterCoeffs[0], c1 = _filterCoeffs[1], c2 = _filterCoeffs[2];
                float x1 = _x1, x2 = _x2, y1 = _y1, y2 = _y2, x;
                while (pData < pDataEnd)
                {
                    volumeCoeff += deltaCoeff;
                    x = *pData;
                    valTmp = c0 * (x + x1 + x1 + x2) + c1 * y1 + c2 * y2;
                    x2 = x1;
                    x1 = x;
                    y2 = y1;
                    y1 = valTmp;
                    (*pData++) = volumeCoeff * valTmp;
                }
                _x1 = x1;
                _x2 = x2;
                _y1 = y1;
                _y2 = y2;
            }
            else
            {
                if (deltaCoeff == 0)
                {
                    while (pData < pDataEnd)
                        (*pData++) *= volumeCoeff;
                }
                else
                {
                    while (pData < pDataEnd)
                    {
                        volumeCoeff += deltaCoeff;
                        (*pData++) *= volumeCoeff;
                    }
                }
            }
            _volumeCoeff = volumeCoeff;
        }
    }

    // Update the playback bar at the sample level
    if (_voiceParam->getKey() == -1 && _listener != nullptr)
    {
        if (_isFinished)
        {
            _listener->onSamplePositionChanged(0);
            _elapsedSmplPos = 0;
        }
        else
        {
            _elapsedSmplPos += len;
            if (_elapsedSmplPos > (_smplRate >> 5))
            {
                _listener->onSamplePositionChanged(_currentSmplPos);
                _elapsedSmplPos = 0;
            }
        }
    }

    return isAudible;
}

void Voice::getData(uint32_t goOn, uint32_t loopStart, uint32_t loopEnd, int16_t * &srcData16, uint8_t * &srcData24)
{
    (void)loopStart;
    (void)loopEnd;

    // New sample position
    _currentSmplPos += goOn;

    // If at least RESAMPLING_ORDER points are available after the current position, use a pointer to the data directly
    if ((_moreAvailable = _dataSmplLength - _currentSmplPos - RESAMPLING_ORDER) >= 0)
    {
        srcData16 = &_dataSmpl16[_currentSmplPos];
        srcData24 = &_dataSmpl24[_currentSmplPos];
        return;
    }

    int32_t remainingSamples = _dataSmplLength - _currentSmplPos;
    if (remainingSamples > 0)
    {
        // Copy what is possible to copy, fill the rest with 0
        memcpy(_srcData16, &_dataSmpl16[_currentSmplPos], remainingSamples * sizeof(int16_t));
        memset(&_srcData16[remainingSamples], 0, (RESAMPLING_ORDER - remainingSamples) * sizeof(int16_t));
        memcpy(_srcData24, &_dataSmpl24[_currentSmplPos], remainingSamples * sizeof(uint8_t));
        memset(&_srcData24[remainingSamples], 0, (RESAMPLING_ORDER - remainingSamples) * sizeof(uint8_t));
    }
    else
    {
        // No more data, fill with 0
        memset(_srcData16, 0, RESAMPLING_ORDER * sizeof(int16_t));
        memset(_srcData24, 0, RESAMPLING_ORDER * sizeof(uint8_t));
    }

    finish();
    srcData16 = _srcData16;
    srcData24 = _srcData24;
}

void Voice::getDataWithLoop(uint32_t goOn, uint32_t loopStart, uint32_t loopEnd, int16_t * &srcData16, uint8_t * &srcData24)
{
    // New sample position
    _currentSmplPos += goOn;

    // Possibly go back to the beginning of the loop
    while (_currentSmplPos >= loopEnd)
        _currentSmplPos += loopStart - loopEnd;

    // If at least RESAMPLING_ORDER points are available after the current position, use a pointer to the data directly
    if ((_moreAvailable = loopEnd - _currentSmplPos - RESAMPLING_ORDER) >= 0)
    {
        srcData16 = &_dataSmpl16[_currentSmplPos];
        srcData24 = &_dataSmpl24[_currentSmplPos];
        return;
    }

    if (loopEnd >= RESAMPLING_ORDER + loopStart)
    {
        // Or take the end + the beginning of the loop
        uint32_t remainingSamples = loopEnd - _currentSmplPos;
        memcpy(_srcData16, &_dataSmpl16[_currentSmplPos], remainingSamples * sizeof(int16_t));
        memcpy(&_srcData16[remainingSamples], &_dataSmpl16[loopStart], (RESAMPLING_ORDER - remainingSamples) * sizeof(int16_t));
        memcpy(_srcData24, &_dataSmpl24[_currentSmplPos], remainingSamples * sizeof(uint8_t));
        memcpy(&_srcData24[remainingSamples], &_dataSmpl24[loopStart], (RESAMPLING_ORDER - remainingSamples) * sizeof(uint8_t));
    }
    else
    {
        // Or, if the loop is too short, use the loop several times
        uint32_t total = 0;
        while (total < RESAMPLING_ORDER)
        {
            const uint32_t chunk = SynthMath::min(loopEnd - _currentSmplPos, RESAMPLING_ORDER - total);
            memcpy(&_srcData16[total], &_dataSmpl16[_currentSmplPos], chunk * sizeof(int16_t));
            memcpy(&_srcData24[total], &_dataSmpl24[_currentSmplPos], chunk * sizeof(uint8_t));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= loopEnd)
                _currentSmplPos = loopStart;
            total += chunk;
        }
    }

    srcData16 = _srcData16;
    srcData24 = _srcData24;
}

void Voice::release(bool quick)
{
    if (quick)
    {
        // Stopped by an exclusive class => quick release
        _volEnvelope.quickRelease();
    }
    else if (_voiceParam->getCurrentInteger(champ_sampleModes) == 2) // Not the audio thread
    {
        // Start of a sample in the release mode
        _volEnvelope.quickAttack();
    }
    _release = true;
}

void Voice::setGain(float gain)
{
    _gain = gain;
}

bool Voice::biQuadCoefficients(float * coeffs, float freq, float inv_Q)
{
    // Calcul des coefficients d'une structure bi-quad pour un passe-bas

    // The maximum frequency must be before the half the audio sample rate (otherwise resulting values can be > 1)
    // Range of theta is [0; 1] for [0; pi]
    float theta = 2.f * freq * _audioSmplRateInv;
    if (theta > 0.98f)
        theta = 0.98f;

    if (inv_Q <= 0.f)
        return false;

    float dTmp = inv_Q * FastMaths::fastSin(theta) * 0.5f;
    if (dTmp <= -1.0f)
        return false;

    float beta = 0.5f * (1.f - dTmp) / (1.f + dTmp);
    float gamma = (0.5f + beta) * FastMaths::fastCos(theta);
    coeffs[0] = (0.5f + beta - gamma) * 0.5f;
    coeffs[1] = 2.f * gamma;
    coeffs[2] = -2.f * beta;
    return true;
}


void Voice::finish()
{
    // Notification for the sample player (voice -1)
    if (_voiceParam->getKey() == -1 && _listener != nullptr)
        _listener->onSampleReadFinished(_token);
    _isFinished = true;
}

int Voice::whichSincTable(unsigned char sampleRootKey, double shift)
{
    const double changeByRootKey = 0.03;
    const double changeByShift = 0.3;
    int index = static_cast<int>(3.0 + changeByRootKey * ((double)sampleRootKey - 64.) + changeByShift * shift + 0.5);
    if (index < 0)
        return 0;
    if (index > 7)
        return 7;
    return index;
}
