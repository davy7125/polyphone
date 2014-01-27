/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "voice.h"

// Constructeur, destructeur
Voice::Voice(QByteArray baData, DWORD smplRate, DWORD audioSmplRate, int note, int velocity,
             VoiceParam * voiceParam, QObject * parent) : QObject(parent),
    _sinusOsc(audioSmplRate),
    _modLFO(audioSmplRate, voiceParam->modLfoDelay),
    _vibLFO(audioSmplRate, voiceParam->vibLfoDelay),
    _enveloppeVol(voiceParam, audioSmplRate, 0),
    _enveloppeMod(voiceParam, audioSmplRate, 1),
    _baData(baData),
    m_smplRate(smplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(note),
    m_velocity(velocity),
    _voiceParam(voiceParam),
    m_currentSmplPos(voiceParam->sampleStart),
    m_time(0),
    m_release(false),
    _delayEnd(10),
    _isFinished(false),
    m_deltaPos(0),
    m_valPrec(0),
    m_x1(0), m_x2(0), m_y1(0), m_y2(0)
{
    // Initialisation resampling
    takeData(&m_valBase, 1);
}

// Sinus
Voice::Voice(DWORD audioSmplRate, VoiceParam *voiceParam, QObject *parent) : QObject(parent),
    _sinusOsc(audioSmplRate),
    _modLFO(audioSmplRate, voiceParam->modLfoDelay),
    _vibLFO(audioSmplRate, voiceParam->vibLfoDelay),
    _enveloppeVol(voiceParam, audioSmplRate, 0),
    _enveloppeMod(voiceParam, audioSmplRate, 1),
    m_smplRate(audioSmplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(-3),
    m_velocity(127),
    _voiceParam(voiceParam),
    m_currentSmplPos(0),
    m_time(0),
    m_release(false),
    _delayEnd(5),
    _isFinished(false),
    m_deltaPos(0),
    m_valPrec(0),
    m_x1(0), m_x2(0), m_y1(0), m_y2(0)
{
    // Initialisation resampling
    takeData(&m_valBase, 1);
}

Voice::~Voice()
{
    delete _voiceParam;
}

void Voice::generateData(float *dataL, float *dataR, qint64 len)
{
    _mutexParam.lock();
    bool endSample = false;
    double gainLowPassFilter = 0;
    if (m_note == -3)
    {
        // Génération d'un sinus
        _sinusOsc.getSinus(dataL, len, 440.0 * EnveloppeVol::fastPow2((double)(_voiceParam->rootkey - 69.) / 12.));
    }
    else
    {
        /// ENVELOPPE DE MODULATION ///
        float dataMod[len];
        _enveloppeMod.applyEnveloppe(dataMod, len, m_release, m_note, 127, _voiceParam, 0, true);

        /// LFOs ///
        float modLfo[len];
        _modLFO.getSinus(modLfo, len, _voiceParam->modLfoFreq);
        float vibLfo[len];
        _vibLFO.getSinus(vibLfo, len, _voiceParam->vibLfoFreq);

        // MODULATION DU PITCH
        float deltaPitchFixe = 0;
        if (m_note < 0)
            deltaPitchFixe = -12 * log2((double)m_audioSmplRate / m_smplRate) +
                    _voiceParam->getPitchDifference(_voiceParam->rootkey);
        else
            deltaPitchFixe = -12 * log2((double)m_audioSmplRate / m_smplRate) +
                    _voiceParam->getPitchDifference(this->m_note);
        float modPitch[len+1];
        for (int i = 0; i < len; i++)
            modPitch[i + 1] = deltaPitchFixe
                    + (dataMod[i] * _voiceParam->modEnvToPitch
                       + modLfo[i] * _voiceParam->modLfoToPitch
                       + vibLfo[i] * _voiceParam->vibLfoToPitch)/ 100;

        // Conversion en distance cumulée entre points
        modPitch[0] = m_deltaPos + 1;
        for (int i = 1; i <= len; i++)
            modPitch[i] = qMin(qMax(0.001f, EnveloppeVol::fastPow2(modPitch[i] / 12)), 64.f) + modPitch[i-1];
        m_deltaPos = modPitch[len];
        m_deltaPos = m_deltaPos - ceil(m_deltaPos);

        // Constitution des données
        int nbDataTmp = ceil(modPitch[len]) - 1;
        qint32 dataTmp[nbDataTmp + 2];
        dataTmp[0] = m_valPrec;
        dataTmp[1] = m_valBase;
        endSample = takeData(&dataTmp[2], nbDataTmp);
        m_valPrec = dataTmp[nbDataTmp];
        m_valBase = dataTmp[nbDataTmp + 1];
        double pos;
        qint32 val1, val2;
        for (int i = 0; i < len; i++)
        {
            pos = modPitch[i];
            val1 = dataTmp[(int)floor(pos)];
            val2 = dataTmp[(int)ceil(pos)];
            pos -= floor(pos);
            dataL[i] = ((1. - pos) * val1 + pos * val2) / 2147483648LL; // Passage en double de -1 à 1
        }

        // FILTRE PASSE-BAS
        double modFreq[len];
        for (int i = 0; i < len; i++)
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
        for (int i = 0; i < len; i++)
        {
            biQuadCoefficients(a0, a1, a2, b1, b2, modFreq[i], q_lin);
            valTmp = a0 * dataL[i] + a1 * m_x1 + a2 * m_x2 - b1 * m_y1 - b2 * m_y2;
            m_x2 = m_x1;
            m_x1 = dataL[i];
            m_y2 = m_y1;
            m_y1 = valTmp;
            dataL[i] = valTmp;
        }
        gainLowPassFilter = qMax(1.33 * filterQ, 0.);

        // Modulation du volume, conversion des dB
        int signe;
        if (_voiceParam->modLfoToVolume < 0)
            signe = 1;
        else
            signe = -1;
        for (int i = 0; i < len; i++)
        {
            valTmp = qMax(signe * modLfo[i], 0.f);
            dataL[i] *= qPow(10., signe * (double)_voiceParam->modLfoToVolume / 20. * valTmp);
        }
    }

    // Application de l'enveloppe de volume
    bool bRet2 = false;
    if (m_note < 0)
        bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, m_release, _voiceParam->rootkey,
                                             m_velocity, _voiceParam, m_gain - gainLowPassFilter);
    else
        bRet2 = _enveloppeVol.applyEnveloppe(dataL, len, m_release, m_note,
                                             m_velocity, _voiceParam, m_gain - gainLowPassFilter);

    if ((bRet2 && _voiceParam->loopMode != 3) || endSample)
    {
        _isFinished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(m_currentSmplPos));

    //// APPLICATION BALANCE ET CHORUS ////

    double pan = (_voiceParam->pan + 50) * PI / 200.;    // Entre 0 et pi / 2
    float coef1 = sin(pan);
    float coef2 = cos(pan);
    for (quint32 i = 0; i < len; i++)
    {
        dataL[i] = coef1 * _chorus.tick(dataL[i]);
        dataR[i] = coef2 * _chorus.lastOut(1);
    }
    _mutexParam.unlock();
}

bool Voice::takeData(qint32 * data, qint64 nbRead)
{
    bool endSample = false;
    const qint32 * dataSmpl = (qint32 *)_baData.constData();

    if ((_voiceParam->loopMode == 1 || _voiceParam->loopMode == 2 ||
         (_voiceParam->loopMode == 3 && !m_release)) &&
            _voiceParam->loopStart != _voiceParam->loopEnd)
    {
        // Boucle
        if (m_currentSmplPos >= _voiceParam->loopEnd)
            m_currentSmplPos = _voiceParam->loopStart;
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(_voiceParam->loopEnd - m_currentSmplPos, nbRead - total);
            memcpy(&data[total], &dataSmpl[m_currentSmplPos], chunk * sizeof(qint32));
            m_currentSmplPos += chunk;
            if (m_currentSmplPos >= _voiceParam->loopEnd)
                m_currentSmplPos = _voiceParam->loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (_voiceParam->sampleEnd - m_currentSmplPos < nbRead)
        {
            memcpy(data, &dataSmpl[m_currentSmplPos], 4 * (_voiceParam->sampleEnd - m_currentSmplPos));
            for (int i = _voiceParam->sampleEnd - m_currentSmplPos; i < nbRead; i++)
                data[i] = 0;
            m_currentSmplPos = _voiceParam->sampleEnd;
            _delayEnd--;
            if (_delayEnd < 0)
                endSample = true;
        }
        else
        {
            memcpy(data, &dataSmpl[m_currentSmplPos], nbRead * sizeof(qint32));
            m_currentSmplPos += nbRead;
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
    m_release = true;
    _mutexParam.unlock();
}

void Voice::setGain(double gain)
{
    _mutexParam.lock();
    m_gain = gain;
    _mutexParam.unlock();
}
void Voice::attackToMax()
{
    _mutexParam.lock();
    _enveloppeVol.attackToMax();
    _mutexParam.unlock();
}

void Voice::decayToMin()
{
    _mutexParam.lock();
    _enveloppeVol.decayToMin();
    _mutexParam.unlock();
}

void Voice::setChorus(int level, int depth, int frequency)
{
    _mutexParam.lock();
    _chorus.setEffectMix((double)level / 200. * (double)_voiceParam->chorus / 100.);
    _chorus.setModDepth((double)depth / 4000.);
    _chorus.setModFrequency((double)frequency / 15.);
    _mutexParam.unlock();
}

void Voice::biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2,
                               double freq, double Q)
{
    // Calcul des coefficients d'une structure bi-quad pour un passe-bas
    double theta = 2. * PI * freq / m_audioSmplRate;

    if (Q == 0)
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
        if (dTmp == -1)
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

double Voice::getReverb()
{
    _mutexParam.lock();
    double val = _voiceParam->reverb;
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

void Voice::setLoopStart(qint32 val)
{
    _mutexParam.lock();
    _voiceParam->loopStart = val;
    _mutexParam.unlock();
}

void Voice::setLoopEnd(qint32 val)
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
