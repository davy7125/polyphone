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
                v = sin (i_shifted * M_PI) / (M_PI * i_shifted);

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
    _distanceArray = new quint32[_arrayLength];
    _dataVolArray = new float[_arrayLength];
    _dataModArray = new float[_arrayLength];
    _modLfoArray = new float[_arrayLength];
    _vibLfoArray = new float[_arrayLength];
}

Voice::~Voice()
{
    delete [] _distanceArray;
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
}

void Voice::generateData(float * data, quint32 len)
{
    // Get voice current parameters
    if (_currentSmplPos == 0xFFFFFFFF)
        _currentSmplPos = _voiceParam->getPosition(champ_dwStart16); // This value is read only once
    qint32 v_rootkey = _voiceParam->getInteger(champ_overridingRootKey);
    qint32 playedNote = _voiceParam->getInteger(champ_keynum);
    qint32 v_scaleTune = _voiceParam->getInteger(champ_scaleTuning);
    qint32 v_fineTune = _voiceParam->getInteger(champ_fineTune);
    qint32 v_coarseTune = _voiceParam->getInteger(champ_coarseTune);

    float v_modLfoFreq = _voiceParam->getFloat(champ_freqModLFO);
    float v_modLfoDelay = _voiceParam->getFloat(champ_delayModLFO);
    qint32 v_modLfoToPitch = _voiceParam->getInteger(champ_modLfoToPitch);
    qint32 v_modLfoToFilterFreq = _voiceParam->getInteger(champ_modLfoToFilterFc);
    float v_modLfoToVolume = _voiceParam->getFloat(champ_modLfoToVolume);

    float v_vibLfoFreq = _voiceParam->getFloat(champ_freqVibLFO);
    float v_vibLfoDelay = _voiceParam->getFloat(champ_delayVibLFO);
    qint32 v_vibLfoToPitch = _voiceParam->getInteger(champ_vibLfoToPitch);

    qint32 v_modEnvToPitch = _voiceParam->getInteger(champ_modEnvToPitch);
    qint32 v_modEnvToFilterFc = _voiceParam->getInteger(champ_modEnvToFilterFc);

    float v_filterQ = _voiceParam->getFloat(champ_initialFilterQ);
    float v_filterFreq = _voiceParam->getFloat(champ_initialFilterFc);
    qint32 v_loopMode = _voiceParam->getInteger(champ_sampleModes);

    float v_attenuation = _voiceParam->getFloat(champ_initialAttenuation);

    // Number of chunks
    quint32 chunkCount = (len >> COMPUTATION_CHUNK_SHIFT);

    // Possibly increase the length of arrays
    if (chunkCount > _arrayLength)
    {
        delete [] _distanceArray;
        delete [] _dataVolArray;
        delete [] _dataModArray;
        delete [] _modLfoArray;
        delete [] _vibLfoArray;

        _distanceArray = new quint32[chunkCount];
        _dataVolArray = new float[chunkCount];
        _dataModArray = new float[chunkCount];
        _modLfoArray = new float[chunkCount];
        _vibLfoArray = new float[chunkCount];
        _arrayLength = chunkCount;
    }

    /// ENVELOPES ///
    if (_volEnvelope.getEnvelope(_dataVolArray, chunkCount, _release, playedNote, fastPow10(0.05f * (_gain - v_attenuation - 0.5f * v_filterQ)), _voiceParam))
        if (v_loopMode != 3)
            _isFinished = true;
    _modEnvelope.getEnvelope(_dataModArray, chunkCount, _release, playedNote, 1.0f, _voiceParam);

    /// LFOs ///
    _modLFO.getData(_modLfoArray, chunkCount, v_modLfoFreq, v_modLfoDelay);
    _vibLFO.getData(_vibLfoArray, chunkCount, v_vibLfoFreq, v_vibLfoDelay);

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
        // Pitch modulation
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

        for (quint32 chunk = 0; chunk < chunkCount; ++chunk)
        {
            quint32 sampleStart = (chunk << COMPUTATION_CHUNK_SHIFT);
            quint32 sampleEnd = sampleStart + COMPUTATION_CHUNK;

            // Distance between the points
            float currentDeltaPitch = deltaPitchFixed + v_modEnvToPitch * _dataModArray[chunk] + v_modLfoToPitch * _modLfoArray[chunk] + v_vibLfoToPitch * _vibLfoArray[chunk];
            int fixedGap = static_cast<quint32>(
                EnveloppeVol::fastPow2(currentDeltaPitch * 0.000833333f /* 1:1200 */ + 11.0f /* multiply by 2048, which is 2^11 */) + 0.5f);

            // Resampling
            float * coeffs;
            int goOn, pointsToLoad;
            for (quint32 i = sampleStart; i < sampleEnd; ++i)
            {
                // New position and number of points to skip / load
                _lastDistanceFraction += fixedGap;
                goOn = _lastDistanceFraction >> 11;
                pointsToLoad = goOn > 7 ? 7 : goOn;
                _lastDistanceFraction &= 0x7FF;

                // Update the current data needed for resampling
                memcpy(&_srcData[0], &_srcData[pointsToLoad], (7 - pointsToLoad) * sizeof(float));
                takeData(&_srcData[7 - pointsToLoad], goOn - pointsToLoad, pointsToLoad, v_loopMode, loopStart, loopEnd);

                // 7th order sinc interpolation
                coeffs = s_sinc_table7[_lastDistanceFraction];
                data[i] = coeffs[0] * _srcData[0] +
                          coeffs[1] * _srcData[1] +
                          coeffs[2] * _srcData[2] +
                          coeffs[3] * _srcData[3] +
                          coeffs[4] * _srcData[4] +
                          coeffs[5] * _srcData[5] +
                          coeffs[6] * _srcData[6];
            }

            // Volume envelope and volume modulation with values from the mod LFO converted to dB
            float coeff = _dataVolArray[chunk] * fastPow10(0.05f * v_modLfoToVolume * _modLfoArray[chunk]);

            // Low-pass filter
            // - 3.01f => so that a value of 0 gives a non-resonant low pass
            // If v_filterQ is 0, inv_q_lin is sqrt(2)
            float inv_q_lin = fastPow10(-0.05f * (v_filterQ - 3.01f));
            float a0, a1, a2, b1, b2, valTmp;
            valTmp = v_filterFreq * EnveloppeVol::fastPow2(
                         (_dataModArray[chunk] * v_modEnvToFilterFc + _modLfoArray[chunk] * v_modLfoToFilterFreq) * 0.000833333f /* 1:1200 */);
            biQuadCoefficients(a0, a1, a2, b1, b2, valTmp, inv_q_lin);
            for (quint32 i = sampleStart; i < sampleEnd; ++i)
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

    // No need to go further if only the release is needed
    if (v_loopMode == 2 && !_release)
        return;

    if (_isFinished)
    {
        if (_voiceParam->getKey() == -1)
        {
            emit currentPosChanged(0);
            _elapsedSmplPos = 0;
        }
    }
    else if (_voiceParam->getKey() == -1)
    {
        _elapsedSmplPos += len;
        if (_elapsedSmplPos > (_smplRate >> 5))
        {
            emit currentPosChanged(_currentSmplPos);
            _elapsedSmplPos = 0;
        }
    }
}

void Voice::takeData(float * data, quint32 nbSkip, quint32 nbRead, qint32 loopMode, quint32 loopStart, quint32 loopEnd)
{
    if (loopMode == 1 || (loopMode == 3 && !_release)) // Loop?
    {
        // Skip
        _currentSmplPos += nbSkip;
        while (_currentSmplPos >= loopEnd)
            _currentSmplPos += loopStart - loopEnd;

        // Read
        quint32 total = 0;
        while (nbRead - total > 0)
        {
            const quint32 chunk = qMin(loopEnd - _currentSmplPos, nbRead - total);
            memcpy(&data[total], &_dataSmpl[_currentSmplPos], chunk * sizeof(float));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= loopEnd)
                _currentSmplPos = loopStart;
            total += chunk;
        }
    }
    else
    {
        // Skip
        _currentSmplPos += nbSkip;

        // Read
        quint32 sampleEnd = _dataSmplLength;
        if (_currentSmplPos > sampleEnd)
        {
            // No more data, fill with 0
            memset(data, 0, nbRead * sizeof(float));
            _isFinished = true;
        }
        else if (sampleEnd - _currentSmplPos < nbRead)
        {
            // Copy what is possible to copy, fill the rest with 0
            memcpy(data, &_dataSmpl[_currentSmplPos], sizeof(qint32) * (sampleEnd - _currentSmplPos));
            memset(&data[sampleEnd - _currentSmplPos], 0, (nbRead - sampleEnd + _currentSmplPos) * sizeof(float));

            // We are now at the end
            _currentSmplPos = sampleEnd;
            _isFinished = true;
        }
        else
        {
            // Copy data
            memcpy(data, &_dataSmpl[_currentSmplPos], nbRead * sizeof(float));
            _currentSmplPos += nbRead;
        }
    }
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
