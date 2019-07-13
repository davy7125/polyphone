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

// Constructeur, destructeur
Voice::Voice(QByteArray baData, quint32 smplRate, quint32 audioSmplRate, int initialKey,
             VoiceParam * voiceParam) : QObject(nullptr),
    _modLFO(audioSmplRate),
    _vibLFO(audioSmplRate),
    _enveloppeVol(audioSmplRate, false),
    _enveloppeMod(audioSmplRate, true),
    _chorusLevel(0),
    _baData(baData),
    _smplRate(smplRate),
    _audioSmplRate(audioSmplRate),
    _gain(0),
    _initialKey(initialKey),
    _voiceParam(voiceParam),
    _currentSmplPos(voiceParam->getPosition(champ_dwStart16)), // This value is read only once
    _time(0),
    _release(false),
    _delayEnd(10),
    _delayStart(0),
    _isFinished(false),
    _isRunning(false),
    _deltaPos(0),
    _valPrec(0),
    _x1(0), _x2(0), _y1(0), _y2(0)
{
    // Initialisation resampling
    takeData(&_valBase, 1);
}

Voice::~Voice()
{
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
    int nbNullValues = qMin(len, _delayStart);
    for (qint32 i = 0; i < nbNullValues; i++)
    {
        dataL[i] = 0;
        dataR[i] = 0;
    }
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
    double gainLowPassFilter = 0;

    // Prepare arrays
    float * dataMod = new float[len];
    float * modLfo = new float[len];
    float * vibLfo = new float[len];
    float * modPitch = new float[len+1];
    double * modFreq = new double[len];

    /// ENVELOPPE DE MODULATION ///
    _enveloppeMod.applyEnveloppe(dataMod, len, _release, playedNote, 1.0f, _voiceParam);

    /// LFOs ///
    _modLFO.getSinus(modLfo, len, static_cast<float>(v_modLfoFreq), v_modLfoDelay);
    _vibLFO.getSinus(vibLfo, len, static_cast<float>(v_vibLfoFreq), v_vibLfoDelay);

    // Pitch modulation
    double deltaPitchFixed = -12. * qLn((double)_audioSmplRate / _smplRate) / 0.69314718056 +
            (playedNote - v_rootkey) * 0.01 * v_scaleTune + 0.01 * v_fineTune + v_coarseTune;
    for (quint32 i = 0; i < len; i++)
        modPitch[i + 1] = deltaPitchFixed + 0.01 * (dataMod[i] * v_modEnvToPitch
                                                    + modLfo[i] * v_modLfoToPitch
                                                    + vibLfo[i] * v_vibLfoToPitch);

    // Convert into a cumulated distance between points
    modPitch[0] = _deltaPos + 1;
    for (quint32 i = 1; i <= len; i++)
        modPitch[i] = qMin(qMax(0.001f, EnveloppeVol::fastPow2(modPitch[i] / 12)), 64.f) + modPitch[i-1];
    _deltaPos = modPitch[len];
    _deltaPos = _deltaPos - ceil(_deltaPos);

    // Resample data
    quint32 nbDataTmp = static_cast<quint32>(ceil(static_cast<double>(modPitch[len]))) - 1;
    qint32 * dataTmp = new qint32[nbDataTmp + 2];
    dataTmp[0] = _valPrec;
    dataTmp[1] = _valBase;
    endSample = takeData(&dataTmp[2], nbDataTmp);
    _valPrec = dataTmp[nbDataTmp];
    _valBase = dataTmp[nbDataTmp + 1];
    double pos;
    qint32 val1, val2;
    for (quint32 i = 0; i < len; i++)
    {
        pos = static_cast<double>(modPitch[i]);
        val1 = dataTmp[static_cast<quint32>(floor(pos))];
        val2 = dataTmp[static_cast<quint32>(ceil(pos))];
        pos -= floor(pos);
        dataL[i] = static_cast<float>(((1. - pos) * val1 + pos * val2) / 2147483648LL); // Passage en double de -1 à 1
    }
    delete [] dataTmp;

    // Low-pass filter
    for (quint32 i = 0; i < len; i++)
    {
        modFreq[i] = v_filterFreq * EnveloppeVol::fastPow2((dataMod[i] * v_modEnvToFilterFc +
                                                            modLfo[i] * v_modLfoToFilterFreq) / 1200);
        if (modFreq[i] > 20000)
            modFreq[i] = 20000;
        else if (modFreq[i] < 20)
            modFreq[i] = 20;
    }
    double a0, a1, a2, b1, b2, valTmp;
    double filterQ = v_filterQ - 3.01;
    double q_lin = qPow(10, filterQ / 20.);
    for (quint32 i = 0; i < len; i++)
    {
        biQuadCoefficients(a0, a1, a2, b1, b2, modFreq[i], q_lin);
        valTmp = a0 * static_cast<double>(dataL[i]) + a1 * _x1 + a2 * _x2 - b1 * _y1 - b2 * _y2;
        _x2 = _x1;
        _x1 = static_cast<double>(dataL[i]);
        _y2 = _y1;
        _y1 = valTmp;
        dataL[i] = static_cast<float>(valTmp);
    }
    gainLowPassFilter = qMax(1.33 * filterQ, 0.);

    // Volume modulation, dB conversion
    int signe;
    if (v_modLfoToVolume < 0)
        signe = 1;
    else
        signe = -1;
    for (quint32 i = 0; i < len; i++)
    {
        valTmp = qMax(signe * static_cast<double>(modLfo[i]), 0.);
        dataL[i] *= static_cast<float>(qPow(10., signe * 0.005 * v_modLfoToVolume * valTmp));
    }

    // Destroy arrays
    delete [] dataMod;
    delete [] modLfo;
    delete [] vibLfo;
    delete [] modPitch;
    delete [] modFreq;

    // Apply the volume envelop
    bool bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, _release, playedNote,
                                              // Gain: should have been a multiplication by 0.1 but FluidSynth do this
                                              static_cast<float>(qPow(10, 0.02 * (_gain - gainLowPassFilter - v_attenuation))),
                                              _voiceParam);

    if ((bRet2 && v_loopMode != 3) || endSample)
    {
        _isFinished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(_currentSmplPos));

    //// APPLY PAN AND CHORUS ////

    double pan = (v_pan + 50) * M_PI / 200.; // Between 0 and π/2
    double coef1 = sin(pan);
    double coef2 = cos(pan);
    _chorus.setEffectMix(0.005 * _chorusLevel * 0.01 * v_chorusEffect);
    for (quint32 i = 0; i < len; i++)
    {
        dataL[i] = static_cast<float>(coef1 * _chorus.tick(static_cast<double>(dataL[i])));
        dataR[i] = static_cast<float>(coef2 * _chorus.lastOut(1));
    }

    _mutexParam.unlock();

    dataL = &dataL[-nbNullValues];
    dataR = &dataR[-nbNullValues];
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

void Voice::setChorus(int level, int depth, int frequency)
{
    _mutexParam.lock();
    _chorusLevel = level;
    _chorus.setModDepth(0.00025 * depth);
    _chorus.setModFrequency(0.06667 * frequency);
    _mutexParam.unlock();
}

void Voice::biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2, double freq, double Q)
{
    // Calcul des coefficients d'une structure bi-quad pour un passe-bas
    double theta = 2. * M_PI * freq / _audioSmplRate;

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
        double d = 1. / Q;
        double dTmp = d * sin(theta) / 2.;
        if (dTmp <= -1)
        {
            a0 = 1;
            a1 = 0;
            a2 = 0;
            b1 = 0;
            b2 = 0;
        }
        else
        {
            double beta = 0.5 * (1. - dTmp) / (1. + dTmp);
            double gamma = (0.5 + beta) * cos(theta);
            a0 = (0.5 + beta - gamma) / 2.;
            a1 = 2. * a0;
            a2 = a0;
            b1 = -2. * gamma;
            b2 = 2. * beta;
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
