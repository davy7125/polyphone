/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "voice.h"
#include "qmath.h"

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
Voice::Voice(QByteArray baData, quint32 smplRate, quint32 audioSmplRate, VoiceParam * voiceParam, int token) : QObject(nullptr),
    _modLFO(audioSmplRate),
    _vibLFO(audioSmplRate),
    _enveloppeVol(audioSmplRate, false),
    _enveloppeMod(audioSmplRate, true),
    _chorusLevel(0),
    _baData(baData),
    _smplRate(smplRate),
    _audioSmplRate(audioSmplRate),
    _gain(0),
    _voiceParam(voiceParam),
    _token(token),
    _currentSmplPos(voiceParam->getPosition(champ_dwStart16)), // This value is read only once
    _time(0),
    _release(false),
    _delayEnd(10),
    _delayStart(0),
    _isFinished(false),
    _isRunning(false),
    _lastFraction(0),
    _valPrec3(0),
    _valPrec2(0),
    _valPrec1(0),
    _x1(0), _x2(0), _y1(0), _y2(0),
    _arrayLength(0),
    _srcDataLength(0)
{
    // Resampling initialization
    takeData(_firstVal, 3);
}

Voice::~Voice()
{
    if (_arrayLength != 0)
    {
        delete [] _dataModArray;
        delete [] _modLfoArray;
        delete [] _vibLfoArray;
        delete [] _modFreqArray;
        delete [] _pointDistanceArray;
    }
    if (_srcDataLength != 0)
        delete [] _srcData;
    delete _voiceParam;
}

void Voice::generateData(float *dataL, float *dataR, quint32 len)
{
    // Get voice current parameters
    _mutexParam.lock();
    _voiceParam->computeModulations();
    qint32 v_rootkey = _voiceParam->getInteger(champ_overridingRootKey);
    qint32 playedNote = _voiceParam->getInteger(champ_keynum);
    qint32 v_scaleTune = _voiceParam->getInteger(champ_scaleTuning);
    qint32 v_fineTune = _voiceParam->getInteger(champ_fineTune);
    qint32 v_coarseTune = _voiceParam->getInteger(champ_coarseTune);

    double v_modLfoFreq = _voiceParam->getDouble(champ_freqModLFO);
    double v_modLfoDelay = _voiceParam->getDouble(champ_delayModLFO);
    qint32 v_modLfoToPitch = _voiceParam->getInteger(champ_modLfoToPitch);
    qint32 v_modLfoToFilterFreq = _voiceParam->getInteger(champ_modLfoToFilterFc);
    double v_modLfoToVolume = _voiceParam->getDouble(champ_modLfoToVolume);

    double v_vibLfoFreq = _voiceParam->getDouble(champ_freqVibLFO);
    double v_vibLfoDelay = _voiceParam->getDouble(champ_delayVibLFO);
    qint32 v_vibLfoToPitch = _voiceParam->getInteger(champ_vibLfoToPitch);

    qint32 v_modEnvToPitch = _voiceParam->getInteger(champ_modEnvToPitch);
    qint32 v_modEnvToFilterFc = _voiceParam->getInteger(champ_modEnvToFilterFc);

    double v_filterQ = _voiceParam->getDouble(champ_initialFilterQ);
    double v_filterFreq = _voiceParam->getDouble(champ_initialFilterFc);
    qint32 v_loopMode = _voiceParam->getInteger(champ_sampleModes);
    double v_pan = _voiceParam->getDouble(champ_pan);
    double v_chorusEffect = _voiceParam->getDouble(champ_chorusEffectsSend);

    double v_attenuation = _voiceParam->getDouble(champ_initialAttenuation);

    // Synchronization delay (stereo samples)
    quint32 nbNullValues = qMin(len, _delayStart);
    memset(dataL, 0, nbNullValues * sizeof(float));
    memset(dataR, 0, nbNullValues * sizeof(float));
    _delayStart -= nbNullValues;
    len -= nbNullValues;
    if (len == 0)
    {
        _mutexParam.unlock();
        return;
    }
    dataL = &dataL[nbNullValues];
    dataR = &dataR[nbNullValues];

    bool endSample = false;

    // Prepare arrays
    if (len > _arrayLength)
    {
        if (_arrayLength != 0)
        {
            delete [] _dataModArray;
            delete [] _modLfoArray;
            delete [] _vibLfoArray;
            delete [] _modFreqArray;
            delete [] _pointDistanceArray;
        }
        _dataModArray = new float[len];
        _modLfoArray = new float[len];
        _vibLfoArray = new float[len];
        _modFreqArray = new float[len];
        _pointDistanceArray = new quint32[len+1];

        _arrayLength = len;
    }

    /// ENVELOPPE DE MODULATION ///
    _enveloppeMod.applyEnveloppe(_dataModArray, len, _release, playedNote, 1.0f, _voiceParam);

    /// LFOs ///
    _modLFO.getData(_modLfoArray, len, static_cast<float>(v_modLfoFreq), v_modLfoDelay);
    _vibLFO.getData(_vibLfoArray, len, static_cast<float>(v_vibLfoFreq), v_vibLfoDelay);

    // Pitch modulation
    float temperamentFineTune = _voiceParam->getKey() < 0 ? 0.0f : (s_temperament[(playedNote - s_temperamentRelativeKey + 12) % 12] -
                                                  s_temperament[(21 - s_temperamentRelativeKey) % 12]); // Correction so that the tuning fork is accurate
    float deltaPitchFixed = -12.f * qLn(static_cast<double>(_audioSmplRate) / _smplRate * 440.f / s_tuningFork) / 0.69314718056f +
            (playedNote - v_rootkey) * 0.01f * v_scaleTune + 0.01f * (temperamentFineTune + v_fineTune) + v_coarseTune;

    // Compute the distance of each point
    _pointDistanceArray[0] = static_cast<quint32>(_lastFraction + 0.5f);
    float currentDeltaPitch;
    for (quint32 i = 0; i < len; i++)
    {
        currentDeltaPitch = deltaPitchFixed + 0.01f * (
                    _dataModArray[i] * v_modEnvToPitch + _modLfoArray[i] * v_modLfoToPitch + _vibLfoArray[i] * v_vibLfoToPitch);
        _lastFraction += EnveloppeVol::fastPow2(currentDeltaPitch / 12.f + 8.0f /* multiply by 256, which is 2^8 */);
        _pointDistanceArray[i + 1] = static_cast<quint32>(_lastFraction + 0.5f);
    }
    _lastFraction = (_pointDistanceArray[len] & 0xFF) ? static_cast<float>(_pointDistanceArray[len] & 0xFF) : 256.0f;

    // Resample data
    quint32 nbDataTmp = (_pointDistanceArray[len] >> 8) - ((_pointDistanceArray[len] & 0xFF) ? 0 : 1);
    if (nbDataTmp > _srcDataLength)
    {
        if (_srcDataLength != 0)
            delete [] _srcData;
        _srcData = new qint32[nbDataTmp + 6];
        _srcDataLength = nbDataTmp;
    }
    _srcData[0] = _valPrec3;
    _srcData[1] = _valPrec2;
    _srcData[2] = _valPrec1;
    _srcData[3] = _firstVal[0];
    _srcData[4] = _firstVal[1];
    _srcData[5] = _firstVal[2];
    endSample = takeData(&_srcData[6], nbDataTmp);
    _valPrec3 = _srcData[nbDataTmp];
    _valPrec2 = _srcData[nbDataTmp + 1];
    _valPrec1 = _srcData[nbDataTmp + 2];
    _firstVal[0] = _srcData[nbDataTmp + 3];
    _firstVal[1] = _srcData[nbDataTmp + 4];
    _firstVal[2] = _srcData[nbDataTmp + 5];

    float * coeffs;
    quint32 currentPos;
    for (quint32 i = 0; i < len; i++)
    {
        // Sinc interpolation 7th order
        coeffs = s_sinc_table7[_pointDistanceArray[i] & 0xFF];
        currentPos = (_pointDistanceArray[i] >> 8);
        dataL[i] = (coeffs[0] * _srcData[currentPos] +
                coeffs[1] * _srcData[currentPos + 1] +
                coeffs[2] * _srcData[currentPos + 2] +
                coeffs[3] * _srcData[currentPos + 3] +
                coeffs[4] * _srcData[currentPos + 4] +
                coeffs[5] * _srcData[currentPos + 5] +
                coeffs[6] * _srcData[currentPos + 6])
                / 2147483648LL; // Convert to float from -1.0 to 1.0
    }

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

    // Apply the volume envelop
    bool bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, _release, playedNote,
                                              static_cast<float>(qPow(10, 0.05 * (_gain - v_attenuation))),
                                              _voiceParam);

    if ((bRet2 && v_loopMode != 3) || endSample)
    {
        _isFinished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(_currentSmplPos));

    //// APPLY PAN AND CHORUS ////

    double pan = (v_pan + 50) * M_PI / 200.; // Between 0 and PI/2
    double coefL = sin(pan);
    double coefR = cos(pan);
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

    _mutexParam.unlock();

    dataL = &dataL[-static_cast<int>(nbNullValues)];
    dataR = &dataR[-static_cast<int>(nbNullValues)];
}

bool Voice::takeData(qint32 * data, quint32 nbRead)
{
    bool endSample = false;
    const qint32 * dataSmpl = reinterpret_cast<const qint32*>(_baData.constData());

    int loopMode = _voiceParam->getInteger(champ_sampleModes);
    quint32 loopStart = _voiceParam->getPosition(champ_dwStartLoop);
    quint32 loopEnd = _voiceParam->getPosition(champ_dwEndLoop);

    if ((loopMode == 1 || loopMode == 2 || (loopMode == 3 && !_release)) && loopStart < loopEnd)
    {
        // Loop
        if (_currentSmplPos >= loopEnd)
            _currentSmplPos = loopStart;
        quint32 total = 0;
        while (nbRead - total > 0)
        {
            const quint32 chunk = qMin(_currentSmplPos < loopEnd ? loopEnd - _currentSmplPos : 0, nbRead - total);
            memcpy(&data[total], &dataSmpl[_currentSmplPos], chunk * sizeof(qint32));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= loopEnd)
                _currentSmplPos = loopStart;
            total += chunk;
        }
    }
    else
    {
        // No loop
        quint32 sampleEnd = _voiceParam->getPosition(champ_dwLength);
        if (_currentSmplPos > sampleEnd)
        {
            // No more data, fill with 0
            memset(data, 0, nbRead * sizeof(qint32));
            endSample = true;
        }
        else if (sampleEnd - _currentSmplPos < nbRead)
        {
            // Copy what is possible to copy, fill the rest with 0
            memcpy(data, &dataSmpl[_currentSmplPos], sizeof(qint32) * (sampleEnd - _currentSmplPos));
            memset(&data[sampleEnd - _currentSmplPos], 0, (nbRead - sampleEnd + _currentSmplPos) * sizeof(qint32));

            // We are now at the end
            _currentSmplPos = sampleEnd;
            _delayEnd--;
            if (_delayEnd == 0)
                endSample = true;
        }
        else
        {
            // Copy data
            memcpy(data, &dataSmpl[_currentSmplPos], nbRead * sizeof(qint32));
            _currentSmplPos += nbRead;
        }
    }

    return endSample;
}

void Voice::release(bool quick)
{
    _mutexParam.lock();
    if (quick)
    {
        // Stopped by an exclusive class => quick release
        _enveloppeVol.quickRelease();
    }
    _release = true;
    _mutexParam.unlock();
}

void Voice::setGain(double gain)
{
    _mutexParam.lock();
    _gain = gain;
    _mutexParam.unlock();
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
    _mutexParam.lock();
    _chorusLevel = level;
    _chorus.setModDepth(0.00025 * depth);
    _chorus.setModFrequency(0.06667 * frequency);
    _mutexParam.unlock();
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
    _mutexParam.lock();
    double val = _voiceParam->getDouble(champ_pan);
    _mutexParam.unlock();
    return val;
}

int Voice::getExclusiveClass()
{
    _mutexParam.lock();
    int val = _voiceParam->getInteger(champ_exclusiveClass);
    _mutexParam.unlock();
    return val;
}

int Voice::getPresetNumber()
{
    _mutexParam.lock();
    int val = _voiceParam->getInteger(champ_wPreset);
    _mutexParam.unlock();
    return val;
}

float Voice::getReverb()
{
    _mutexParam.lock();
    float val = static_cast<float>(_voiceParam->getDouble(champ_reverbEffectsSend));
    _mutexParam.unlock();
    return val;
}

void Voice::setPan(double val)
{
    _mutexParam.lock();
    _voiceParam->setPan(val);
    _mutexParam.unlock();
}

void Voice::setLoopMode(quint16 val)
{
    _mutexParam.lock();
    _voiceParam->setLoopMode(val);
    _mutexParam.unlock();
}

void Voice::setLoopStart(quint32 val)
{
    _mutexParam.lock();
    _voiceParam->setLoopStart(val);
    _mutexParam.unlock();
}

void Voice::setLoopEnd(quint32 val)
{
    _mutexParam.lock();
    _voiceParam->setLoopEnd(val);
    _mutexParam.unlock();
}

void Voice::setFineTune(qint16 val)
{
    _mutexParam.lock();
    _voiceParam->setFineTune(val);
    _mutexParam.unlock();
}
