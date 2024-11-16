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
float Voice::s_sinc_table7[256][7];
float Voice::s_sin_table[256];
float Voice::s_pow10_table[2048];

void Voice::prepareTables()
{
    double v, i_shifted;
    for (int i = 0; i < 7; i++) // i: Offset in terms of whole samples
    {
        // i2: Offset in terms of fractional samples ('subsamples')
        for (int i2 = 0; i2 < 256; i2++)
        {
            // Center on middle of table
            i_shifted = (double)i - (7.0 / 2.0) + (double)i2 / 256.0;

            // sinc(0) cannot be calculated straightforward (limit needed for 0/0)
            if (fabs (i_shifted) > 0.000001)
            {
                v = sin (i_shifted * M_PI) / (M_PI * i_shifted);

                // Hamming window
                v *= 0.5 * (1.0 + cos (2.0 * M_PI * i_shifted / 7));
            }
            else
                v = 1.0;

            s_sinc_table7[256 - i2 - 1][i] = (float)v;
        }
    }

    // Lookup table for sinus, values in [0; pi/2[
    for (int i = 0; i < 256; i++)
        s_sin_table[i] = (float)sin(static_cast<double>(i) * M_PI_2 / 256.0);

    // Lookup table for pow10, values in [-102.4; 102.3]
    for (int i = 0; i < 2048; i++)
        s_pow10_table[i] = qPow(10.0, 0.1 * (double)(i - 1024));
}

Voice::Voice(VoiceParam * voiceParam) : QObject(nullptr),
    _dataSmpl(nullptr),
    _voiceParam(voiceParam)
{
    // Array initialization
    _srcDataLength = INITIAL_ARRAY_LENGTH;
    _srcData = new float[_srcDataLength + 7];

    _arrayLength = INITIAL_ARRAY_LENGTH;
    _dataModArray = new float[_arrayLength];
    _modLfoArray = new float[_arrayLength];
    _vibLfoArray = new float[_arrayLength];
    _modFreqArray = new float[_arrayLength];
    _pointDistanceArray = new quint32[_arrayLength];
}

Voice::~Voice()
{
    delete [] _dataModArray;
    delete [] _modLfoArray;
    delete [] _vibLfoArray;
    delete [] _modFreqArray;
    delete [] _pointDistanceArray;
    delete [] _srcData;
    if (_voiceParam->getType() != 0 && _dataSmpl != nullptr)
    {
        delete [] _dataSmpl;
        _dataSmpl = nullptr;
    }
}

void Voice::initialize(VoiceInitializer * voiceInitializer)
{
    if (_voiceParam->getType() != 0 && _dataSmpl != nullptr)
    {
        delete [] _dataSmpl;
        _dataSmpl = nullptr;
    }

    _voiceParam->initialize(voiceInitializer->prst,
                            voiceInitializer->prstDiv,
                            voiceInitializer->inst,
                            voiceInitializer->instDiv,
                            voiceInitializer->smpl,
                            voiceInitializer->channel,
                            voiceInitializer->key,
                            voiceInitializer->vel,
                            voiceInitializer->type);

    _dataSmpl = voiceInitializer->smpl->_sound.getData(_dataSmplLength, false, _voiceParam->getType() != 0 /* copy data at the sample level */);
    _smplRate = voiceInitializer->smpl->_sound.getUInt32(champ_dwSampleRate);
    _audioSmplRate = voiceInitializer->audioSmplRate;
    _audioSmplRateInv = 1.0f / _audioSmplRate;
    _gain = 0.0f;
    _token = voiceInitializer->token;
    _currentSmplPos = 0xFFFFFFFF;
    _elapsedSmplPos = 0;
    _time = 0;
    _release = false;
    _delayEnd = 10;
    _isFinished = false;
    _isRunning = false;
    _x1 = 0;
    _x2 = 0;
    _y1 = 0;
    _y2 = 0;
    _arrayLength = 0;

    _modLFO.initialize(_audioSmplRate);
    _vibLFO.initialize(_audioSmplRate);
    _enveloppeVol.initialize(_audioSmplRate, false);
    _enveloppeMod.initialize(_audioSmplRate, true);

    // Resampling initialization
    memset(_firstVal, 0, 6 * sizeof(float));
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

    bool endSample = false;

    // Possible increase the length of arrays
    if (len > _arrayLength)
    {
        delete [] _dataModArray;
        delete [] _modLfoArray;
        delete [] _vibLfoArray;
        delete [] _modFreqArray;
        delete [] _pointDistanceArray;

        _dataModArray = new float[len];
        _modLfoArray = new float[len];
        _vibLfoArray = new float[len];
        _modFreqArray = new float[len];
        _pointDistanceArray = new quint32[len];
        _arrayLength = len;
    }

    /// ENVELOPPE DE MODULATION ///
    _enveloppeMod.applyEnveloppe(_dataModArray, len, _release, playedNote, 1.0f, _voiceParam);

    /// LFOs ///
    _modLFO.getData(_modLfoArray, len, v_modLfoFreq, v_modLfoDelay);
    _vibLFO.getData(_vibLfoArray, len, v_vibLfoFreq, v_vibLfoDelay);

    if (v_loopMode == 2 && !_release)
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

        // Compute the distance of each point
        if (v_modEnvToPitch == 0 && v_modLfoToPitch == 0 && v_vibLfoToPitch == 0)
        {
            // Double required for tuning accuracy
            double gap = EnveloppeVol::fastPow2(deltaPitchFixed * 0.000833333f /* 1:1200 */ + 8.0f /* multiply by 256, which is 2^8 */);
            double initialDistance = static_cast<double>(_lastDistanceFraction) + gap;
            for (quint32 i = 0; i < len; i++)
                _pointDistanceArray[i] = static_cast<int>(0.5 + initialDistance + gap * i);
        }
        else
        {
            // Double required for tuning accuracy
            double distance = static_cast<double>(_lastDistanceFraction);
            float currentDeltaPitch;
            for (quint32 i = 0; i < len; i++)
            {
                currentDeltaPitch = deltaPitchFixed + _dataModArray[i] * v_modEnvToPitch + _modLfoArray[i] * v_modLfoToPitch + _vibLfoArray[i] * v_vibLfoToPitch;
                distance += EnveloppeVol::fastPow2(currentDeltaPitch * 0.000833333f /* 1:1200 */ + 8.0f /* multiply by 256, which is 2^8 */);
                _pointDistanceArray[i] = static_cast<int>(0.5f + distance);
            }
        }
        _lastDistanceFraction = (_pointDistanceArray[len - 1] & 0xFF);

        // Resample data
        quint32 nbDataTmp = (_pointDistanceArray[len - 1] >> 8);
        if (nbDataTmp > _srcDataLength)
        {
            delete [] _srcData;
            _srcData = new float[nbDataTmp + 7];
            _srcDataLength = nbDataTmp;
        }

        memcpy(_srcData, _firstVal, 6 * sizeof(float));
        endSample = takeData(&_srcData[6], nbDataTmp, v_loopMode);
        _srcData[6 + nbDataTmp] = 0; // Extra 0 needed
        memcpy(_firstVal, &_srcData[nbDataTmp], 6 * sizeof(float));

        float * coeffs;
        quint32 currentPos;
        for (quint32 i = 0; i < len; i++)
        {
            // Sinc interpolation 7th order
            coeffs = s_sinc_table7[_pointDistanceArray[i] & 0xFF];
            currentPos = (_pointDistanceArray[i] >> 8);
            data[i] = coeffs[0] * _srcData[currentPos] +
                      coeffs[1] * _srcData[currentPos + 1] +
                      coeffs[2] * _srcData[currentPos + 2] +
                      coeffs[3] * _srcData[currentPos + 3] +
                      coeffs[4] * _srcData[currentPos + 4] +
                      coeffs[5] * _srcData[currentPos + 5] +
                      coeffs[6] * _srcData[currentPos + 6];
        }

        // Low-pass filter
        for (quint32 i = 0; i < len; i++)
        {
            _modFreqArray[i] = v_filterFreq *
                               EnveloppeVol::fastPow2((_dataModArray[i] * v_modEnvToFilterFc + _modLfoArray[i] * v_modLfoToFilterFreq) * 0.000833333f /* 1:1200 */);
            if (_modFreqArray[i] > 0.5f * _audioSmplRate)
                _modFreqArray[i] = 0.5f * _audioSmplRate;
            else if (_modFreqArray[i] < 20.0f)
                _modFreqArray[i] = 20.0f;
        }
        float a0, a1, a2, b1, b2, valTmp;
        float filterQ = v_filterQ - 3.01f; // So that a value of 0 gives a non-resonant low pass
        float inv_q_lin = fastPow10(-0.05f * filterQ); // If filterQ is -3.01, inv_q_lin is sqrt(2)
        for (quint32 i = 0; i < len; ++i)
        {
            biQuadCoefficients(a0, a1, a2, b1, b2, _modFreqArray[i], inv_q_lin);
            valTmp = a0 * data[i] + a1 * _x1 + a2 * _x2 - b1 * _y1 - b2 * _y2;
            _x2 = _x1;
            _x1 = data[i];
            _y2 = _y1;
            _y1 = valTmp;
            data[i] = valTmp;
        }

        // Volume modulation with values from the mod LFO converted to dB
        for (quint32 i = 0; i < len; ++i)
            data[i] *= fastPow10(0.05f * v_modLfoToVolume * _modLfoArray[i]);
    }

    // Apply the volume envelop
    bool bRet2 = _enveloppeVol.applyEnveloppe(data, len, _release, playedNote,
                                              fastPow10(0.05f * (_gain - v_attenuation - 0.5f * v_filterQ)),
                                              _voiceParam);

    // No need to go further if only the release is needed
    if (v_loopMode == 2 && !_release)
        return;

    if ((bRet2 && v_loopMode != 3) || endSample)
    {
        _isFinished = true;
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

bool Voice::takeData(float * data, quint32 nbRead, qint32 loopMode)
{
    if (_dataSmpl == nullptr)
    {
        memset(data, 0, nbRead * sizeof(float));
        return true;
    }

    bool endSample = false;
    quint32 loopStart = _voiceParam->getPosition(champ_dwStartLoop);
    quint32 loopEnd = _voiceParam->getPosition(champ_dwEndLoop);

    if ((loopMode == 1 || (loopMode == 3 && !_release)) && loopStart < loopEnd)
    {
        // Loop
        if (_currentSmplPos >= loopEnd)
            _currentSmplPos = loopStart;
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
        // No loop
        quint32 sampleEnd = _dataSmplLength;
        if (_currentSmplPos > sampleEnd)
        {
            // No more data, fill with 0
            memset(data, 0, nbRead * sizeof(float));
            endSample = true;
        }
        else if (sampleEnd - _currentSmplPos < nbRead)
        {
            // Copy what is possible to copy, fill the rest with 0
            memcpy(data, &_dataSmpl[_currentSmplPos], sizeof(qint32) * (sampleEnd - _currentSmplPos));
            memset(&data[sampleEnd - _currentSmplPos], 0, (nbRead - sampleEnd + _currentSmplPos) * sizeof(float));

            // We are now at the end
            _currentSmplPos = sampleEnd;
            _delayEnd--;
            if (_delayEnd == 0)
                endSample = true;
        }
        else
        {
            // Copy data
            memcpy(data, &_dataSmpl[_currentSmplPos], nbRead * sizeof(float));
            _currentSmplPos += nbRead;
        }
    }

    return endSample;
}

void Voice::release(bool quick)
{
    if (quick)
    {
        // Stopped by an exclusive class => quick release
        _enveloppeVol.quickRelease();
    }
    else if (_voiceParam->getInteger(champ_sampleModes) == 2)
    {
        // Start of a sample in the release mode
        _enveloppeVol.quickAttack();
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

    // The maximum frequency is half the audio sample rate
    // Range of theta is [0; 1] for [0; pi]
    float theta = 2.f * freq * _audioSmplRateInv;

    if (inv_Q <= 0)
    {
        a0 = 1;
        a1 = 0;
        a2 = 0;
        b1 = 0;
        b2 = 0;
    }
    else
    {
        float dTmp = inv_Q * fastSin(theta) * 0.5f;
        if (dTmp <= -1.0f)
        {
            a0 = 1;
            a1 = 0;
            a2 = 0;
            b1 = 0;
            b2 = 0;
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
    value *= 512.f;
    int indexBefore = static_cast<int>(value);
    float diff = value - static_cast<float>(indexBefore);

    if (indexBefore < 0)
        return 0.f;
    if (indexBefore > 255)
        return 1.f;

    // Linear interpolation
    return indexBefore >= 255 ?
               s_sin_table[255] + (1.f - s_sin_table[255]) * diff :
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

float Voice::getFloatAttribute(AttributeType attribute)
{
    return _voiceParam->getFloat(attribute);
}

qint32 Voice::getIntAttribute(AttributeType attribute)
{
    return _voiceParam->getInteger(attribute);
}

void Voice::setPan(float val)
{
    _voiceParam->setPan(val);
}

void Voice::setLoopMode(quint16 val)
{
    _voiceParam->setLoopMode(val);
}

void Voice::setLoopStart(quint32 val)
{
    _voiceParam->setLoopStart(val);
}

void Voice::setLoopEnd(quint32 val)
{
    _voiceParam->setLoopEnd(val);
}

void Voice::setFineTune(qint16 val)
{
    _voiceParam->setFineTune(val);
}
