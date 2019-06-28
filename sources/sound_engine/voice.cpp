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
Voice::Voice(QByteArray baData, quint32 smplRate, quint32 audioSmplRate, int note, int velocity,
             VoiceParam * voiceParam, QObject * parent) : QObject(parent),
    _modLFO(audioSmplRate, voiceParam->modLfoDelay),
    _vibLFO(audioSmplRate, voiceParam->vibLfoDelay),
    _enveloppeVol(voiceParam, audioSmplRate, 0),
    _enveloppeMod(voiceParam, audioSmplRate, 1),
    _baData(baData),
    _smplRate(smplRate),
    _audioSmplRate(audioSmplRate),
    _note(note),
    _velocity(velocity),
    _gain(0),
    _voiceParam(voiceParam),
    _currentSmplPos(voiceParam->sampleStart),
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
    // Synchronization delay
    int nbNullValues = qMin(len, _delayStart);
    for (qint32 i = 0; i < nbNullValues; i++)
    {
        dataL[i] = 0;
        dataR[i] = 0;
    }
    _delayStart -= nbNullValues;
    len -= nbNullValues;
    if (len == 0) return;
    dataL = &dataL[nbNullValues];
    dataR = &dataR[nbNullValues];

    _mutexParam.lock();

    bool endSample = false;
    double gainLowPassFilter = 0;

    // Construction des tableaux
    float * dataMod = new float[len];
    float * modLfo = new float[len];
    float * vibLfo = new float[len];
    float * modPitch = new float[len+1];
    double * modFreq = new double[len];

    /// ENVELOPPE DE MODULATION ///
    _enveloppeMod.applyEnveloppe(dataMod, len, _release, _note, 127, _voiceParam, 0, true);

    /// LFOs ///
    _modLFO.getSinus(modLfo, len, _voiceParam->modLfoFreq);
    _vibLFO.getSinus(vibLfo, len, _voiceParam->vibLfoFreq);

    // MODULATION DU PITCH
    double deltaPitchFixe = 0;
    if (_note < 0)
        deltaPitchFixe = -12. * qLn((double)_audioSmplRate / _smplRate) / 0.69314718056 +
                _voiceParam->getPitchDifference(_voiceParam->rootkey);
    else
        deltaPitchFixe = -12. * qLn((double)_audioSmplRate / _smplRate) / 0.69314718056 +
                _voiceParam->getPitchDifference(_note);
    for (quint32 i = 0; i < len; i++)
        modPitch[i + 1] = deltaPitchFixe + 0.01 * (dataMod[i] * _voiceParam->modEnvToPitch
                                            + modLfo[i] * _voiceParam->modLfoToPitch
                                            + vibLfo[i] * _voiceParam->vibLfoToPitch);

    // Conversion en distance cumulée entre points
    modPitch[0] = _deltaPos + 1;
    for (quint32 i = 1; i <= len; i++)
        modPitch[i] = qMin(qMax(0.001f, EnveloppeVol::fastPow2(modPitch[i] / 12)), 64.f) + modPitch[i-1];
    _deltaPos = modPitch[len];
    _deltaPos = _deltaPos - ceil(_deltaPos);

    // Constitution des données
    quint32 nbDataTmp = static_cast<quint32>(ceil(modPitch[len])) - 1;
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

    // FILTRE PASSE-BAS
    for (quint32 i = 0; i < len; i++)
    {
        modFreq[i] = _voiceParam->filterFreq * EnveloppeVol::fastPow2((dataMod[i] * _voiceParam->modEnvToFilterFc +
                                                                       modLfo[i] * _voiceParam->modLfoToFilterFreq) / 1200);
        if (modFreq[i] > 20000)
            modFreq[i] = 20000;
        else if (modFreq[i] < 20)
            modFreq[i] = 20;
    }
    double a0, a1, a2, b1, b2, valTmp;
    double filterQ = _voiceParam->filterQ - 3.01;
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

    // Modulation du volume, conversion des dB
    int signe;
    if (_voiceParam->modLfoToVolume < 0)
        signe = 1;
    else
        signe = -1;
    for (quint32 i = 0; i < len; i++)
    {
        valTmp = qMax(signe * static_cast<double>(modLfo[i]), 0.);
        dataL[i] *= static_cast<float>(qPow(10., signe * 0.005 * _voiceParam->modLfoToVolume * valTmp));
    }

    // Destruction des tableaux
    delete [] dataMod;
    delete [] modLfo;
    delete [] vibLfo;
    delete [] modPitch;
    delete [] modFreq;

    // Application de l'enveloppe de volume
    bool bRet2 = false;
    if (_note < 0)
        bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, _release, _voiceParam->rootkey,
                                             _velocity, _voiceParam, _gain - gainLowPassFilter);
    else
        bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, _release, _note,
                                             _velocity, _voiceParam, _gain - gainLowPassFilter);

    if ((bRet2 && _voiceParam->loopMode != 3) || endSample)
    {
        _isFinished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(_currentSmplPos));

    //// APPLICATION BALANCE ET CHORUS ////

    double pan = (_voiceParam->pan + 50) * M_PI / 200.;    // Entre 0 et pi / 2
    double coef1 = sin(pan);
    double coef2 = cos(pan);
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

    if ((_voiceParam->loopMode == 1 || _voiceParam->loopMode == 2 ||
         (_voiceParam->loopMode == 3 && !_release)) &&
            _voiceParam->loopStart != _voiceParam->loopEnd)
    {
        // Boucle
        if (_currentSmplPos >= _voiceParam->loopEnd)
            _currentSmplPos = _voiceParam->loopStart;
        quint32 total = 0;
        while (nbRead - total > 0)
        {
            const quint32 chunk = qMin(_voiceParam->loopEnd - _currentSmplPos, nbRead - total);
            memcpy(&data[total], &dataSmpl[_currentSmplPos], chunk * sizeof(qint32));
            _currentSmplPos += chunk;
            if (_currentSmplPos >= _voiceParam->loopEnd)
                _currentSmplPos = _voiceParam->loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (_voiceParam->sampleEnd - _currentSmplPos < nbRead)
        {
            memcpy(data, &dataSmpl[_currentSmplPos], 4 * (_voiceParam->sampleEnd - _currentSmplPos));
            for (quint32 i = _voiceParam->sampleEnd - _currentSmplPos; i < nbRead; i++)
                data[i] = 0;
            _currentSmplPos = _voiceParam->sampleEnd;
            _delayEnd--;
            if (_delayEnd == 0)
                endSample = true;
        }
        else
        {
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
        // Arrêt par classe exclusive, arrêt rapide
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
    _chorus.setEffectMix(0.005 * level * 0.01 * static_cast<double>(_voiceParam->chorus));
    _chorus.setModDepth(0.00025 * depth);
    _chorus.setModFrequency(0.06667 * frequency);
    _mutexParam.unlock();
}

void Voice::biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2,
                               double freq, double Q)
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
    double val = _voiceParam->pan;
    _mutexParam.unlock();
    return val;
}

int Voice::getExclusiveClass()
{
    _mutexParam.lock();
    int val = _voiceParam->exclusiveClass;
    _mutexParam.unlock();
    return val;
}

int Voice::getPresetNumber()
{
    _mutexParam.lock();
    int val = _voiceParam->numPreset;
    _mutexParam.unlock();
    return val;
}

float Voice::getReverb()
{
    _mutexParam.lock();
    float val = _voiceParam->reverb;
    _mutexParam.unlock();
    return val;
}

void Voice::setPan(double val)
{
    _mutexParam.lock();
    _voiceParam->pan = val;
    _mutexParam.unlock();
}

void Voice::setLoopMode(int val)
{
    _mutexParam.lock();
    _voiceParam->loopMode = val;
    _mutexParam.unlock();
}

void Voice::setLoopStart(quint32 val)
{
    _mutexParam.lock();
    _voiceParam->loopStart = val;
    _mutexParam.unlock();
}

void Voice::setLoopEnd(quint32 val)
{
    _mutexParam.lock();
    _voiceParam->loopEnd = val;
    _mutexParam.unlock();
}

void Voice::setFineTune(qint32 val)
{
    _mutexParam.lock();
    _voiceParam->fineTune = val;
    _mutexParam.unlock();
}

void Voice::setRootKey(double val)
{
    _mutexParam.lock();
    _voiceParam->rootkey = val;
    _mutexParam.unlock();
}
