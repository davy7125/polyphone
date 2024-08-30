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

void Voice::prepareSincTable()
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

            s_sinc_table7[256 - i2 - 1][i] = v;
        }
    }
}

// Constructeur, destructeur
Voice::Voice() : QObject(nullptr)
{
    // Array initialization
    _srcDataLength = INITIAL_ARRAY_LENGTH;
    _srcData = new float[_srcDataLength + 6];

    _arrayLength = INITIAL_ARRAY_LENGTH;
    _dataModArray = new float[_arrayLength];
    _modLfoArray = new float[_arrayLength];
    _vibLfoArray = new float[_arrayLength];
    _modFreqArray = new float[_arrayLength];
    _pointDistanceArray = new quint32[_arrayLength + 1];
}

Voice::~Voice()
{
    delete [] _dataModArray;
    delete [] _modLfoArray;
    delete [] _vibLfoArray;
    delete [] _modFreqArray;
    delete [] _pointDistanceArray;
    delete [] _srcData;
}

void Voice::initialize(VoiceInitializer * voiceInitializer)
{
    _voiceParam.initialize(voiceInitializer->prst,
                           voiceInitializer->prstDiv,
                           voiceInitializer->inst,
                           voiceInitializer->instDiv,
                           voiceInitializer->smpl,
                           voiceInitializer->channel,
                           voiceInitializer->key,
                           voiceInitializer->vel);

    _chorusLevel = 0;
    _baData = voiceInitializer->smpl->_sound.getData();
    _smplRate = voiceInitializer->smpl->_sound.getUInt32(champ_dwSampleRate);
    _audioSmplRate = voiceInitializer->audioSmplRate;
    _gain = 0;
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
    _pointDistanceArray[0] = 0;
}

void Voice::generateData(float *dataL, float *dataR, quint32 len)
{
    memset(dataL, 0, len * sizeof(float));
    memset(dataR, 0, len * sizeof(float));

    // Get voice current parameters
    _voiceParam.computeModulations();
    if (_currentSmplPos == 0xFFFFFFFF)
        _currentSmplPos = _voiceParam.getPosition(champ_dwStart16); // This value is read only once
    qint32 v_rootkey = _voiceParam.getInteger(champ_overridingRootKey);
    qint32 playedNote = _voiceParam.getInteger(champ_keynum);
    qint32 v_scaleTune = _voiceParam.getInteger(champ_scaleTuning);
    qint32 v_fineTune = _voiceParam.getInteger(champ_fineTune);
    qint32 v_coarseTune = _voiceParam.getInteger(champ_coarseTune);

    double v_modLfoFreq = _voiceParam.getDouble(champ_freqModLFO);
    double v_modLfoDelay = _voiceParam.getDouble(champ_delayModLFO);
    qint32 v_modLfoToPitch = _voiceParam.getInteger(champ_modLfoToPitch);
    qint32 v_modLfoToFilterFreq = _voiceParam.getInteger(champ_modLfoToFilterFc);
    double v_modLfoToVolume = _voiceParam.getDouble(champ_modLfoToVolume);

    double v_vibLfoFreq = _voiceParam.getDouble(champ_freqVibLFO);
    double v_vibLfoDelay = _voiceParam.getDouble(champ_delayVibLFO);
    qint32 v_vibLfoToPitch = _voiceParam.getInteger(champ_vibLfoToPitch);

    qint32 v_modEnvToPitch = _voiceParam.getInteger(champ_modEnvToPitch);
    qint32 v_modEnvToFilterFc = _voiceParam.getInteger(champ_modEnvToFilterFc);

    double v_filterQ = _voiceParam.getDouble(champ_initialFilterQ);
    double v_filterFreq = _voiceParam.getDouble(champ_initialFilterFc);
    qint32 v_loopMode = _voiceParam.getInteger(champ_sampleModes);
    double v_pan = _voiceParam.getDouble(champ_pan);
    double v_chorusEffect = _voiceParam.getDouble(champ_chorusEffectsSend);

    double v_attenuation = _voiceParam.getDouble(champ_initialAttenuation);

    bool endSample = false;

    // Possible increase the length of arrays
    if (len > _arrayLength)
    {
        unsigned int tmp = _pointDistanceArray[0];

        delete [] _dataModArray;
        delete [] _modLfoArray;
        delete [] _vibLfoArray;
        delete [] _modFreqArray;
        delete [] _pointDistanceArray;

        _dataModArray = new float[len];
        _modLfoArray = new float[len];
        _vibLfoArray = new float[len];
        _modFreqArray = new float[len];
        _pointDistanceArray = new quint32[len + 1];
        _pointDistanceArray[0] = tmp;

        _arrayLength = len;
    }

    /// ENVELOPPE DE MODULATION ///
    _enveloppeMod.applyEnveloppe(_dataModArray, len, _release, playedNote, 1.0f, &_voiceParam);

    /// LFOs ///
    _modLFO.getData(_modLfoArray, len, static_cast<float>(v_modLfoFreq), v_modLfoDelay);
    _vibLFO.getData(_vibLfoArray, len, static_cast<float>(v_vibLfoFreq), v_vibLfoDelay);

    // Skip steps if the sample is read only when the key is released
    if (_release || v_loopMode != 2)
    {
        // Pitch modulation
        float temperamentFineTune = _voiceParam.getKey() < 0 ? 0.0f : (s_temperament[(playedNote - s_temperamentRelativeKey + 12) % 12] -
                                                                s_temperament[(21 - s_temperamentRelativeKey) % 12]); // Correction so that the tuning fork is accurate
        float deltaPitchFixed = -1200.f * qLn(static_cast<double>(_audioSmplRate) / _smplRate * 440.f / s_tuningFork) / 0.69314718056f +
                (playedNote - v_rootkey) * v_scaleTune + (temperamentFineTune + v_fineTune) + 100.0f * v_coarseTune;

        // Compute the distance of each point
        if (v_modEnvToPitch == 0 && v_modLfoToPitch == 0 && v_vibLfoToPitch == 0)
        {
            float tmp = static_cast<float>(_pointDistanceArray[0]);
            for (quint32 i = 0; i < len; i++)
            {
                tmp += EnveloppeVol::fastPow2(deltaPitchFixed * 0.000833333f /* 1:1200 */ + 8.0f /* multiply by 256, which is 2^8 */);
                _pointDistanceArray[i + 1] = static_cast<int>(0.5f + tmp);
            }
        }
        else
        {
            float tmp = static_cast<float>(_pointDistanceArray[0]);
            float currentDeltaPitch;
            for (quint32 i = 0; i < len; i++)
            {
                currentDeltaPitch = deltaPitchFixed + _dataModArray[i] * v_modEnvToPitch + _modLfoArray[i] * v_modLfoToPitch + _vibLfoArray[i] * v_vibLfoToPitch;
                tmp += EnveloppeVol::fastPow2(currentDeltaPitch * 0.000833333f /* 1:1200 */ + 8.0f /* multiply by 256, which is 2^8 */);
                _pointDistanceArray[i + 1] = static_cast<int>(0.5f + tmp);
            }
        }

        // Resample data
        quint32 nbDataTmp = (_pointDistanceArray[len] >> 8);
        if (nbDataTmp > _srcDataLength)
        {
            delete [] _srcData;
            _srcData = new float[nbDataTmp + 6];
            _srcDataLength = nbDataTmp;
        }

        memcpy(_srcData, _firstVal, 6 * sizeof(float));
        endSample = takeData(&_srcData[6], nbDataTmp, v_loopMode);
        memcpy(_firstVal, &_srcData[nbDataTmp], 6 * sizeof(float));

        float * coeffs;
        quint32 currentPos;
        for (quint32 i = 0; i < len; i++)
        {
            // Sinc interpolation 7th order
            coeffs = s_sinc_table7[_pointDistanceArray[i] & 0xFF];
            currentPos = (_pointDistanceArray[i] >> 8);
            dataL[i] = coeffs[0] * _srcData[currentPos] +
                    coeffs[1] * _srcData[currentPos + 1] +
                    coeffs[2] * _srcData[currentPos + 2] +
                    coeffs[3] * _srcData[currentPos + 3] +
                    coeffs[4] * _srcData[currentPos + 4] +
                    coeffs[5] * _srcData[currentPos + 5] +
                    coeffs[6] * _srcData[currentPos + 6];
        }
        _pointDistanceArray[0] = (_pointDistanceArray[len] & 0xFF);

        // Low-pass filter
        for (quint32 i = 0; i < len; i++)
        {
            _modFreqArray[i] = v_filterFreq *
                    EnveloppeVol::fastPow2((_dataModArray[i] * v_modEnvToFilterFc + _modLfoArray[i] * v_modLfoToFilterFreq) / 1200.f);
            if (_modFreqArray[i] > 20000.0f)
                _modFreqArray[i] = 20000.0f;
            else if (_modFreqArray[i] < 20.0f)
                _modFreqArray[i] = 20.0f;
        }
        float a0, a1, a2, b1, b2, valTmp;
        double filterQ = v_filterQ - 3.01f; // So that a value of 0 gives a non-resonant low pass
        float q_lin = qPow(10, filterQ / 20.); // If filterQ is -3.01, q_lin is 1/sqrt(2)
        for (quint32 i = 0; i < len; i++)
        {
            biQuadCoefficients(a0, a1, a2, b1, b2, _modFreqArray[i], q_lin);
            valTmp = a0 * dataL[i] + a1 * _x1 + a2 * _x2 - b1 * _y1 - b2 * _y2;
            _x2 = _x1;
            _x1 = dataL[i];
            _y2 = _y1;
            _y1 = valTmp;
            dataL[i] = valTmp;
        }

        // Volume modulation with values from the mod LFO converted to dB
        for (quint32 i = 0; i < len; i++)
            dataL[i] *= static_cast<float>(qPow(10., 0.05 * v_modLfoToVolume * static_cast<double>(_modLfoArray[i])));
    }

    // Apply the volume envelop
    bool bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, _release, playedNote,
                                              static_cast<float>(qPow(10, 0.05 * (_gain - v_attenuation - v_filterQ / 2))),
                                              &_voiceParam);

    // No need to go further of only the release is needed
    if (v_loopMode == 2 && !_release)
        return;

    if ((bRet2 && v_loopMode != 3) || endSample)
    {
        _isFinished = true;
        if (_voiceParam.getKey() == -1)
        {
            emit(currentPosChanged(0));
            _elapsedSmplPos = 0;
        }
    }
    else if (_voiceParam.getKey() == -1)
    {
        _elapsedSmplPos += len;
        if (_elapsedSmplPos > (_smplRate >> 5))
        {
            emit(currentPosChanged(_currentSmplPos));
            _elapsedSmplPos = 0;
        }
    }

    //// APPLY PAN AND CHORUS ////

    double pan = (v_pan + 50) * M_PI / 200.; // Between 0 and PI/2
    float coefL = cos(pan);
    float coefR = sin(pan);
    if (_chorusLevel > 0)
    {
        _chorus.setEffectMix(0.005 * _chorusLevel * 0.01 * v_chorusEffect);
        for (quint32 i = 0; i < len; i++)
        {
            dataL[i] = static_cast<float>(coefL * _chorus.tick(static_cast<double>(dataL[i])));
            dataR[i] = static_cast<float>(coefR * _chorus.lastOut(1));
        }
    }
    else
    {
        // Or just pan the sound
        for (quint32 i = 0; i < len; i++)
        {
            dataR[i] = static_cast<float>(coefR * dataL[i]);
            dataL[i] *= coefL;
        }
    }
}

bool Voice::takeData(float * data, quint32 nbRead, qint32 loopMode)
{
    bool endSample = false;
    const float * dataSmpl = _baData.constData();

    quint32 loopStart = _voiceParam.getPosition(champ_dwStartLoop);
    quint32 loopEnd = _voiceParam.getPosition(champ_dwEndLoop);

    if ((loopMode == 1 || (loopMode == 3 && !_release)) && loopStart < loopEnd)
    {
        // Loop
        if (_currentSmplPos >= loopEnd)
            _currentSmplPos = loopStart;
        quint32 total = 0;
        while (nbRead - total > 0)
        {
            const quint32 chunk = qMin(_currentSmplPos < loopEnd ? loopEnd - _currentSmplPos : 0, nbRead - total);
            memcpy(&data[total], &dataSmpl[_currentSmplPos], chunk * sizeof(float));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= loopEnd)
                _currentSmplPos = loopStart;
            total += chunk;
        }
    }
    else
    {
        // No loop
        quint32 sampleEnd = _voiceParam.getPosition(champ_dwLength);
        if (_currentSmplPos > sampleEnd)
        {
            // No more data, fill with 0
            memset(data, 0, nbRead * sizeof(float));
            endSample = true;
        }
        else if (sampleEnd - _currentSmplPos < nbRead)
        {
            // Copy what is possible to copy, fill the rest with 0
            memcpy(data, &dataSmpl[_currentSmplPos], sizeof(qint32) * (sampleEnd - _currentSmplPos));
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
            memcpy(data, &dataSmpl[_currentSmplPos], nbRead * sizeof(float));
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
    else if (_voiceParam.getInteger(champ_sampleModes) == 2)
    {
        // Start of a sample in the release mode
        _enveloppeVol.quickAttack();
    }
    _release = true;
}

void Voice::setGain(double gain)
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
        s_temperament[i]  = temperament[i];
    s_temperamentRelativeKey = relativeKey;
}

void Voice::setChorus(int level, int depth, int frequency)
{
    _chorusLevel = level;
    _chorus.setModDepth(0.00025 * depth);
    _chorus.setModFrequency(0.06667 * frequency);
}

void Voice::triggerReadFinishedSignal()
{
    emit(readFinished(_token));
}

void Voice::biQuadCoefficients(float &a0, float &a1, float &a2, float &b1, float &b2, float freq, float Q)
{
    // Calcul des coefficients d'une structure bi-quad pour un passe-bas
    float theta = 2.f * M_PI * freq / _audioSmplRate;

    if (Q <= 0)
    {
        a0 = 1;
        a1 = 0;
        a2 = 0;
        b1 = 0;
        b2 = 0;
    }
    else
    {
        float dTmp = sin(theta) / (2.f * Q);
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
            float gamma = (0.5f + beta) * cos(theta);
            a0 = (0.5 + beta - gamma) / 2.f;
            a1 = 2.f * a0;
            a2 = a0;
            b1 = -2.f * gamma;
            b2 = 2.f * beta;
        }
    }
}

double Voice::getPan()
{
    double val = _voiceParam.getDouble(champ_pan);
    return val;
}

int Voice::getExclusiveClass()
{
    int val = _voiceParam.getInteger(champ_exclusiveClass);
    return val;
}

int Voice::getPresetNumber()
{
    int val = _voiceParam.getInteger(champ_wPreset);
    return val;
}

float Voice::getReverb()
{
    float val = static_cast<float>(_voiceParam.getDouble(champ_reverbEffectsSend));
    return val;
}

void Voice::setPan(double val)
{
    _voiceParam.setPan(val);
}

void Voice::setLoopMode(quint16 val)
{
    _voiceParam.setLoopMode(val);
}

void Voice::setLoopStart(quint32 val)
{
    _voiceParam.setLoopStart(val);
}

void Voice::setLoopEnd(quint32 val)
{
    _voiceParam.setLoopEnd(val);
}

void Voice::setFineTune(qint16 val)
{
    _voiceParam.setFineTune(val);
}
