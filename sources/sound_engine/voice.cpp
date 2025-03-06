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
#include "qmath.h"
#include "smpl.h"

volatile int Voice::s_tuningFork = 440;
volatile float Voice::s_temperament[12] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
volatile int Voice::s_temperamentRelativeKey = 0;
float Voice::s_sinc_table7[2048][7];
float Voice::s_sin_table[2048];
float Voice::s_pow10_table[2048];

void Voice::prepareTables()
{
    double v, i_shifted;
    for (int i = 0; i < 7; i++) // i: Offset in terms of whole samples
    {
        // i2: Offset in terms of fractional samples ('subsamples')
        for (int i2 = 0; i2 < 2048; i2++)
        {
            // Center on middle of table
            i_shifted = (double)i - (7.0 / 2.0) + (double)i2 / 2048.0;

            // sinc(0) cannot be calculated straightforward (limit needed for 0/0)
            if (fabs (i_shifted) > 0.000001)
            {
                v = sin(i_shifted * M_PI) / (M_PI * i_shifted);

                // Hamming window
                v *= 0.5 * (1.0 + cos (2.0 * M_PI * i_shifted / 7));
            }
            else
                v = 1.0;

            s_sinc_table7[2048 - i2 - 1][i] = (float)v;
        }
    }

    // Lookup table for sinus, values in [0; pi/2[
    for (int i = 0; i < 2048; i++)
        s_sin_table[i] = (float)sin(static_cast<double>(i) * M_PI_2 / 2048.0);

    // Lookup table for pow10, values in [-102.4; 102.3]
    for (int i = 0; i < 2048; i++)
        s_pow10_table[i] = qPow(10.0, 0.1 * (double)(i - 1024));
}

Voice::Voice(VoiceParam * voiceParam) : QObject(nullptr),
    _dataSmpl(nullptr),
    _voiceParam(voiceParam)
{
    // Array initialization
    _arrayLength = INITIAL_ARRAY_LENGTH;
    _dataVolArray = new float[_arrayLength];
    _dataModArray = new float[_arrayLength];
    _modLfoArray = new float[_arrayLength];
    _vibLfoArray = new float[_arrayLength];
}

Voice::~Voice()
{
    delete [] _dataVolArray;
    delete [] _dataModArray;
    delete [] _modLfoArray;
    delete [] _vibLfoArray;
    if (_voiceParam->getType() != 0 && _dataSmpl != nullptr)
    {
        delete [] _dataSmpl;
        _dataSmpl = nullptr;
    }
}

void Voice::initialize(InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv, Smpl * smpl,
                       qint8 channel, qint8 key, qint8 vel, qint8 type, quint32 audioSmplRate, int token)
{
    // Possibly delete previous sample data that has been copied
    if (_voiceParam->getType() != 0 && _dataSmpl != nullptr)
    {
        delete [] _dataSmpl;
        _dataSmpl = nullptr;
    }

    _voiceParam->initialize(prst, prstDiv, inst, instDiv, smpl, channel, key, vel, type);

    _dataSmpl = smpl->_sound.getData(_dataSmplLength, false, _voiceParam->getType() != 0 /* copy data at the sample level */);
    _smplRate = smpl->_sound.getUInt32(champ_dwSampleRate);
    _audioSmplRate = audioSmplRate;
    _audioSmplRateInv = 1.0f / _audioSmplRate;
    _gain = 0.0f;
    _token = token;
    _currentSmplPos = 0xFFFFFFFF;
    _elapsedSmplPos = 0;
    _time = 0;
    _release = false;
    _isFinished = false;
    _isRunning = false;
    _x1 = 0;
    _x2 = 0;
    _y1 = 0;
    _y2 = 0;

    _modLFO.initialize(_audioSmplRate);
    _vibLFO.initialize(_audioSmplRate);
    _volEnvelope.initialize(_audioSmplRate, false);
    _modEnvelope.initialize(_audioSmplRate, true);

    // Resampling initialization
    memset(_srcData, 0, 7 * sizeof(float));
    _lastDistanceFraction = 0;
    _moreAvailable = -1;
}

void Voice::generateData(float * data, quint32 len)
{
    if (_currentSmplPos == 0xFFFFFFFF)
        _currentSmplPos = _voiceParam->getPosition(champ_dwStart16); // This value is read only once
    qint32 playedNote = _voiceParam->getInteger(champ_keynum);
    qint32 v_loopMode = _voiceParam->getInteger(champ_sampleModes);

    float v_filterQ = _voiceParam->getFloat(champ_initialFilterQ);
    float v_filterFreq = _voiceParam->getFloat(champ_initialFilterFc);

    // Number of chunks
    quint32 chunkCount = (len >> COMPUTATION_CHUNK_SHIFT);

    // Possibly increase the length of arrays
    if (chunkCount > _arrayLength)
    {
        delete [] _dataVolArray;
        delete [] _dataModArray;
        delete [] _modLfoArray;
        delete [] _vibLfoArray;

        _arrayLength = chunkCount;
        _dataVolArray = new float[_arrayLength];
        _dataModArray = new float[_arrayLength];
        _modLfoArray = new float[_arrayLength];
        _vibLfoArray = new float[_arrayLength];
    }

    /// ENVELOPES ///
    float v_attenuation = _voiceParam->getFloat(champ_initialAttenuation);
    if (_volEnvelope.getEnvelope(_dataVolArray, chunkCount, _release, playedNote, fastPow10(0.05f * (_gain - v_attenuation - 0.5f * v_filterQ)), _voiceParam))
        if (v_loopMode != 3)
            _isFinished = true;
    _modEnvelope.getEnvelope(_dataModArray, chunkCount, _release, playedNote, 1.0f, _voiceParam);

    /// LFOs ///
    _modLFO.getData(_modLfoArray, chunkCount,
                    _voiceParam->getFloat(champ_freqModLFO),
                    _voiceParam->getFloat(champ_delayModLFO));
    _vibLFO.getData(_vibLfoArray, chunkCount,
                    _voiceParam->getFloat(champ_freqVibLFO),
                    _voiceParam->getFloat(champ_delayVibLFO));

    if (_dataSmpl == nullptr)
    {
        memset(data, 0, len * sizeof(float));
        _isFinished = true;
    }
    else if (v_loopMode == 2 && !_release)
    {
        // Blank
        memset(data, 0, len * sizeof(float));
    }
    else
    {
        // LFOs and modulation effects
        qint32 v_modLfoToPitch = _voiceParam->getInteger(champ_modLfoToPitch);
        qint32 v_modLfoToFilterFreq = _voiceParam->getInteger(champ_modLfoToFilterFc);
        float v_modLfoToVolume = _voiceParam->getFloat(champ_modLfoToVolume);
        qint32 v_vibLfoToPitch = _voiceParam->getInteger(champ_vibLfoToPitch);
        qint32 v_modEnvToPitch = _voiceParam->getInteger(champ_modEnvToPitch);
        qint32 v_modEnvToFilterFc = _voiceParam->getInteger(champ_modEnvToFilterFc);

        // Pitch modulation
        qint32 v_rootkey = _voiceParam->getInteger(champ_overridingRootKey);
        qint32 v_scaleTune = _voiceParam->getInteger(champ_scaleTuning);
        qint32 v_fineTune = _voiceParam->getInteger(champ_fineTune);
        qint32 v_coarseTune = _voiceParam->getInteger(champ_coarseTune);
        float temperamentFineTune = _voiceParam->getKey() < 0 ? 0.0f : (s_temperament[(playedNote - s_temperamentRelativeKey + 12) % 12] -
                                                                        s_temperament[(21 - s_temperamentRelativeKey) % 12]); // Correction so that the tuning fork is accurate
        float deltaPitchFixed = -1731.234f /* -1200 / ln(2) */ *
                                    qLn(static_cast<double>(_audioSmplRate) * 440.f / (_smplRate * s_tuningFork)) +
                                (playedNote - v_rootkey) * v_scaleTune + (temperamentFineTune + v_fineTune) + 100.0f * v_coarseTune;

        // Loop
        quint32 loopStart = _voiceParam->getPosition(champ_dwStartLoop);
        quint32 loopEnd = _voiceParam->getPosition(champ_dwEndLoop);
        if (loopStart >= loopEnd)
            v_loopMode = 0;

        // Parameter for the low-pass filter
        // - 3.01f => so that a value of 0 gives a non-resonant low pass
        // If v_filterQ is 0, inv_q_lin is sqrt(2)
        float inv_q_lin = fastPow10(-0.05f * (v_filterQ - 3.01f));

        for (quint32 chunk = 0; chunk < chunkCount; ++chunk)
        {
            quint32 sampleStart = (chunk << COMPUTATION_CHUNK_SHIFT);
            quint32 sampleEnd = sampleStart + COMPUTATION_CHUNK;

            // Distance between the points
            float currentDeltaPitch = deltaPitchFixed + v_modEnvToPitch * _dataModArray[chunk] + v_modLfoToPitch * _modLfoArray[chunk] + v_vibLfoToPitch * _vibLfoArray[chunk];
            int fixedGap = static_cast<quint32>(
                EnveloppeVol::fastPow2(currentDeltaPitch * 0.000833333f /* 1:1200 */ + 11.0f /* multiply by 2048, which is 2^11 */) + 0.5f);

            // Resampling (7th order sinc interpolation)
            float * srcData = &_dataSmpl[_currentSmplPos];
            quint32 i = sampleStart;
            quint32 nextPosition = 0;
            float *(Voice::*slowExtractFunction)(quint32,quint32,quint32) = (v_loopMode == 1 || (v_loopMode == 3 && !_release)) ? &Voice::getDataWithLoop : &Voice::getData;

resample_fast:
            // Fast extraction, if possible
            while (1)
            {
                _lastDistanceFraction += fixedGap;
                if ((qint32)(nextPosition = (_lastDistanceFraction >> 11)) > _moreAvailable)
                {
                    // Revert the last jump and update the current position
                    _lastDistanceFraction -= fixedGap;
                    _currentSmplPos += (_lastDistanceFraction >> 11);
                    _lastDistanceFraction &= 0x7FF;
                    goto resample_slow;
                }

                data[i++] = multiply(s_sinc_table7[_lastDistanceFraction & 0x7FF], &srcData[nextPosition]);
                if (i >= sampleEnd)
                {
                    // Update the current position
                    _currentSmplPos += nextPosition;
                    _moreAvailable -= nextPosition;
                    _lastDistanceFraction &= 0x7FF;
                    goto resample_end;
                }
            }
resample_slow:
            // First data extraction or complex extraction
            while (1)
            {
                _lastDistanceFraction += fixedGap;
                srcData = (*this.*slowExtractFunction)(_lastDistanceFraction >> 11, loopStart, loopEnd);
                _lastDistanceFraction &= 0x7FF;
                data[i++] = multiply(s_sinc_table7[_lastDistanceFraction], srcData);

                if (i >= sampleEnd)
                    goto resample_end;
                if (_moreAvailable > 0)
                    goto resample_fast;
            }
resample_end:

            // Volume envelope and volume modulation with values from the mod LFO converted to dB
            float coeff = _dataVolArray[chunk] * fastPow10(0.05f * v_modLfoToVolume * _modLfoArray[chunk]);

            // Low-pass filter
            float valTmp = v_filterFreq * EnveloppeVol::fastPow2(
                         (v_modEnvToFilterFc * _dataModArray[chunk] + v_modLfoToFilterFreq * _modLfoArray[chunk]) * 0.000833333f /* 1:1200 */);
            float a0, a1, a2, b1, b2;
            biQuadCoefficients(a0, a1, a2, b1, b2, valTmp, inv_q_lin);
            for (i = sampleStart; i < sampleEnd; ++i)
            {
                valTmp = a0 * data[i] + a1 * _x1 + a2 * _x2 - b1 * _y1 - b2 * _y2;
                _x2 = _x1;
                _x1 = data[i];
                _y2 = _y1;
                _y1 = valTmp;
                data[i] = coeff * valTmp;
            }
        }
    }

    // Update the playback bar at the sample level
    if (_voiceParam->getKey() == -1)
    {
        if (_isFinished)
        {
            emit currentPosChanged(0);
            _elapsedSmplPos = 0;
        }
        else
        {
            _elapsedSmplPos += len;
            if (_elapsedSmplPos > (_smplRate >> 5))
            {
                emit currentPosChanged(_currentSmplPos);
                _elapsedSmplPos = 0;
            }
        }
    }
}

float * Voice::getData(quint32 goOn, quint32 loopStart, quint32 loopEnd)
{
    Q_UNUSED(loopStart)
    Q_UNUSED(loopEnd)

    // New sample position
    _currentSmplPos += goOn;

    // If at least 7 points are available after the current position, use a pointer to the data directly
    if ((_moreAvailable = _dataSmplLength - _currentSmplPos - 7) >= 0)
        return &_dataSmpl[_currentSmplPos];

    qint32 remainingSamples = _dataSmplLength - _currentSmplPos;
    if (remainingSamples > 0)
    {
        // Copy what is possible to copy, fill the rest with 0
        memcpy(_srcData, &_dataSmpl[_currentSmplPos], remainingSamples * sizeof(float));
        memset(&_srcData[remainingSamples], 0, (7 - remainingSamples) * sizeof(float));
    }
    else
    {
        // No more data, fill with 0
        memset(_srcData, 0, 7 * sizeof(float));
    }

    _isFinished = true;
    return _srcData;
}

float * Voice::getDataWithLoop(quint32 goOn, quint32 loopStart, quint32 loopEnd)
{
    // New sample position
    _currentSmplPos += goOn;

    // Possibly go back to the beginning of the loop
    while (_currentSmplPos >= loopEnd)
        _currentSmplPos += loopStart - loopEnd;

    // If at least 7 points are available after the current position, use a pointer to the data directly
    if ((_moreAvailable = loopEnd - _currentSmplPos - 7) >= 0)
        return &_dataSmpl[_currentSmplPos];

    if (loopEnd >= 7 + loopStart)
    {
        // Or take the end + the beginning of the loop
        quint32 remainingSamples = loopEnd - _currentSmplPos;
        memcpy(_srcData, &_dataSmpl[_currentSmplPos], remainingSamples * sizeof(float));
        memcpy(&_srcData[remainingSamples], &_dataSmpl[loopStart], (7 - remainingSamples) * sizeof(float));
    }
    else
    {
        // Or, if the loop is too short, use the loop several times
        quint32 total = 0;
        while (total < 7)
        {
            const quint32 chunk = qMin(loopEnd - _currentSmplPos, 7 - total);
            memcpy(&_srcData[total], &_dataSmpl[_currentSmplPos], chunk * sizeof(float));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= loopEnd)
                _currentSmplPos = loopStart;
            total += chunk;
        }
    }

    return _srcData;
}

void Voice::release(bool quick)
{
    if (quick)
    {
        // Stopped by an exclusive class => quick release
        _volEnvelope.quickRelease();
    }
    else if (_voiceParam->getInteger(champ_sampleModes) == 2)
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

void Voice::setTuningFork(int tuningFork)
{
    // Atomic operation
    s_tuningFork = tuningFork;
}

void Voice::setTemperament(float temperament[12], int relativeKey)
{
    // Atomic operations
    for (int i = 0; i < 12; i++)
        s_temperament[i] = temperament[i];
    s_temperamentRelativeKey = relativeKey;
}

void Voice::triggerReadFinishedSignal()
{
    emit readFinished(_token);
}

void Voice::biQuadCoefficients(float &a0, float &a1, float &a2, float &b1, float &b2, float freq, float inv_Q)
{
    // Calcul des coefficients d'une structure bi-quad pour un passe-bas

    // The maximum frequency must be before the half the audio sample rate (otherwise resulting values can be > 1)
    // Range of theta is [0; 1] for [0; pi]
    float theta = 2.f * freq * _audioSmplRateInv;
    if (theta > 0.98f)
        theta = 0.98f;

    if (inv_Q <= 0.f)
    {
        a0 = 1.f;
        a1 = 0.f;
        a2 = 0.f;
        b1 = 0.f;
        b2 = 0.f;
    }
    else
    {
        float dTmp = inv_Q * fastSin(theta) * 0.5f;
        if (dTmp <= -1.0f)
        {
            a0 = 1.f;
            a1 = 0.f;
            a2 = 0.f;
            b1 = 0.f;
            b2 = 0.f;
        }
        else
        {
            float beta = 0.5f * (1.f - dTmp) / (1.f + dTmp);
            float gamma = (0.5f + beta) * fastCos(theta);
            a0 = (0.5f + beta - gamma) * 0.5f;
            a1 = 2.f * a0;
            a2 = a0;
            b1 = -2.f * gamma;
            b2 = 2.f * beta;
        }
    }
}

float Voice::fastSin(float value)
{
    return value < 0.5f ? getSinValue(value) : getSinValue(1.f - value);
}

float Voice::fastCos(float value)
{
    return value < 0.5f ? getSinValue(0.5f - value) : -getSinValue(value - 0.5f);
}

float Voice::getSinValue(float value)
{
    value *= 4096.f;
    int indexBefore = static_cast<int>(value);
    float diff = value - static_cast<float>(indexBefore);

    if (indexBefore < 0)
        return 0.f;
    if (indexBefore > 2047)
        return 1.f;

    // Linear interpolation
    return indexBefore >= 2047 ?
               s_sin_table[2047] + (1.f - s_sin_table[2047]) * diff :
               s_sin_table[indexBefore] + (s_sin_table[indexBefore + 1] - s_sin_table[indexBefore]) * diff;
}

float Voice::fastPow10(float value)
{
    value = 10.0f * (value + 102.4f);
    int indexBefore = static_cast<int>(value);
    float diff = value - static_cast<float>(indexBefore);

    if (indexBefore < 0)
        return s_pow10_table[0];
    if (indexBefore > 2047)
        return s_pow10_table[2047];

    // Linear interpolation
    return s_pow10_table[indexBefore] + (s_pow10_table[indexBefore + 1] - s_pow10_table[indexBefore]) * diff;
}
