/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
             VoiceParam * voiceParam, QObject * parent) :
    CircularBuffer(BUFFER_VOICE_SIZE, BUFFER_VOICE_AVANCE, parent),
    m_sinusOsc(audioSmplRate),
    m_modLFO(audioSmplRate, voiceParam->modLfoDelay),
    m_vibLFO(audioSmplRate, voiceParam->vibLfoDelay),
    m_baData(baData),
    m_smplRate(smplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(note),
    m_velocity(velocity),
    m_voiceParam(voiceParam),
    m_currentSmplPos(voiceParam->sampleStart),
    m_time(0),
    m_release(false),
    m_finished(false),
    m_enveloppeVol(voiceParam, audioSmplRate, 0),
    m_enveloppeMod(voiceParam, audioSmplRate, 1),
    m_deltaPos(0),
    m_valPrec(0),
    m_x1(0), m_x2(0), m_y1(0), m_y2(0)
{
    // Initialisation resampling
    takeData(&m_valBase, 1);
}

// Sinus
Voice::Voice(DWORD audioSmplRate, VoiceParam *voiceParam, QObject *parent) :
    CircularBuffer(BUFFER_VOICE_SIZE, BUFFER_VOICE_AVANCE, parent),
    m_sinusOsc(audioSmplRate),
    m_modLFO(audioSmplRate, voiceParam->modLfoDelay),
    m_vibLFO(audioSmplRate, voiceParam->vibLfoDelay),
    m_smplRate(audioSmplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(-3),
    m_velocity(127),
    m_voiceParam(voiceParam),
    m_currentSmplPos(0),
    m_time(0),
    m_release(false),
    m_finished(false),
    m_enveloppeVol(voiceParam, audioSmplRate, 0),
    m_enveloppeMod(voiceParam, audioSmplRate, 1),
    m_deltaPos(0),
    m_valPrec(0),
    m_x1(0), m_x2(0), m_y1(0), m_y2(0)
{
    // Initialisation resampling
    takeData(&m_valBase, 1);
}

Voice::~Voice()
{
    delete m_voiceParam;
}

qint64 Voice::readData(char *data, qint64 maxlen)
{
    this->generateData(maxlen - bytesAvailable());
    qint64 len = CircularBuffer::readData(data, maxlen);
    if (m_finished)
        return -len;
    else
        return len;
}

void Voice::generateData(qint64 nbData)
{
    if (nbData <= 0) return;
    double data[nbData];
    bool bRet = true;
    double gainLowPassFilter = 0;
    if (m_note == -3)
    {
        // Génération d'un sinus
        m_sinusOsc.getSinus(data, nbData, 440.0 * pow(2.0, (double)(m_voiceParam->rootkey - 69.) / 12.));
    }
    else
    {
        /// ENVELOPPE DE MODULATION ///
        double dataMod[nbData];
        m_enveloppeMod.applyEnveloppe(dataMod, nbData, m_release, m_note, 127, m_voiceParam, 0, true);
        /// LFOs ///
        double modLfo[nbData];
        m_modLFO.getSinus(modLfo, nbData, m_voiceParam->modLfoFreq);
        double vibLfo[nbData];
        m_vibLFO.getSinus(vibLfo, nbData, m_voiceParam->vibLfoFreq);

        // MODULATION DU PITCH
        double deltaPitchFixe = 0;
        if (m_note < 0)
            deltaPitchFixe = -12 * log2((double)m_audioSmplRate / m_smplRate) +
                    m_voiceParam->getPitchDifference(m_voiceParam->rootkey);
        else
            deltaPitchFixe = -12 * log2((double)m_audioSmplRate / m_smplRate) +
                    m_voiceParam->getPitchDifference(this->m_note);
        double modPitch[nbData+1];
        for (int i = 0; i < nbData; i++)
            modPitch[i + 1] = deltaPitchFixe
                    + (dataMod[i] * m_voiceParam->modEnvToPitch
                    + modLfo[i] * m_voiceParam->modLfoToPitch
                    + vibLfo[i] * m_voiceParam->vibLfoToPitch)/ 100;
        // Conversion en distance cumulée entre points
        modPitch[0] = m_deltaPos + 1;
        for (int i = 1; i <= nbData; i++)
            modPitch[i] = qMin(qMax(0.001, pow(2, modPitch[i] / 12)), 64.) + modPitch[i-1];
        m_deltaPos = modPitch[nbData];
        m_deltaPos = m_deltaPos - ceil(m_deltaPos);

        // Constitution des données
        int nbDataTmp = ceil(modPitch[nbData]) - 1;
        qint32 dataTmp[nbDataTmp + 2];
        dataTmp[0] = m_valPrec;
        dataTmp[1] = m_valBase;
        bRet = takeData(&dataTmp[2], nbDataTmp);
        m_valPrec = dataTmp[nbDataTmp];
        m_valBase = dataTmp[nbDataTmp + 1];
        double pos;
        qint32 val1, val2;
        for (int i = 0; i < nbData; i++)
        {
            pos = modPitch[i];
            val1 = dataTmp[(int)floor(pos)];
            val2 = dataTmp[(int)ceil(pos)];
            pos -= floor(pos);
            data[i] = ((1. - pos) * val1 + pos * val2) / 2147483648LL; // Passage en double de -1 à 1
        }

        // FILTRE PASSE-BAS
        double modFreq[nbData];
        for (int i = 0; i < nbData; i++)
        {
            modFreq[i] = m_voiceParam->filterFreq * pow(2., (dataMod[i] * m_voiceParam->modEnvToFilterFc
                    + modLfo[i] * m_voiceParam->modLfoToFilterFreq) / 1200);
            if (modFreq[i] > 20000)
                modFreq[i] = 20000;
            else if (modFreq[i] < 20)
                modFreq[i] = 20;
        }
        double a0, a1, a2, b1, b2, valTmp;
        double filterQ = m_voiceParam->filterQ - 3.01;
        double q_lin = pow(10, filterQ / 20.);
        for (int i = 0; i < nbData; i++)
        {
            biQuadCoefficients(a0, a1, a2, b1, b2, modFreq[i], q_lin);
            valTmp = a0 * data[i] + a1 * m_x1 + a2 * m_x2 - b1 * m_y1 - b2 * m_y2;
            m_x2 = m_x1;
            m_x1 = data[i];
            m_y2 = m_y1;
            m_y1 = valTmp;
            data[i] = valTmp;
        }
        gainLowPassFilter = qMax(1.33 * filterQ, 0.);

        // Modulation du volume, conversion des dB
        int signe;
        if (m_voiceParam->modLfoToVolume < 0)
            signe = 1;
        else
            signe = -1;
        for (int i = 0; i < nbData; i++)
        {
            valTmp = qMax(signe * modLfo[i], 0.);
            data[i] *= pow(10., signe * (double)m_voiceParam->modLfoToVolume / 20. * valTmp);
        }
    }
    // Application de l'enveloppe de volume
    bool bRet2 = false;
    if (m_note < 0)
        bRet2 = m_enveloppeVol.applyEnveloppe(data, nbData, m_release, m_voiceParam->rootkey,
                                              m_velocity, m_voiceParam, m_gain - gainLowPassFilter);
    else
        bRet2 = m_enveloppeVol.applyEnveloppe(data, nbData, m_release, m_note,
                                              m_velocity, m_voiceParam, m_gain - gainLowPassFilter);
    if ((bRet2 && m_voiceParam->loopMode != 3) || !bRet)
    {
        m_finished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(m_currentSmplPos));

    //// ECRITURE SUR LE BUFFER ////
    this->writeData((char *)data, nbData * 8);
}

bool Voice::takeData(qint32 * data, qint64 nbRead)
{
    bool ok = true;
    const qint32 * dataSmpl = (qint32 *)m_baData.constData();

    if ((m_voiceParam->loopMode == 1 || m_voiceParam->loopMode == 2 ||
         (m_voiceParam->loopMode == 3 && !m_release)) &&
            m_voiceParam->loopStart != m_voiceParam->loopEnd)
    {
        // Boucle
        if (m_currentSmplPos >= m_voiceParam->loopEnd) m_currentSmplPos = m_voiceParam->loopStart;
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(m_voiceParam->loopEnd - m_currentSmplPos, nbRead - total);
            memcpy(&data[total], &dataSmpl[m_currentSmplPos], chunk * sizeof(qint32));
            m_currentSmplPos += chunk;
            if (m_currentSmplPos >= m_voiceParam->loopEnd) m_currentSmplPos = m_voiceParam->loopStart;
            total += chunk;
        }
    }
    else
    {
        // Pas de boucle
        if (m_voiceParam->sampleEnd - m_currentSmplPos < nbRead)
        {
            memcpy(data, &dataSmpl[m_currentSmplPos], 4 * (m_voiceParam->sampleEnd - m_currentSmplPos));
            for (int i = m_voiceParam->sampleEnd - m_currentSmplPos; i < nbRead; i++)
                data[i] = 0;
            m_currentSmplPos = m_voiceParam->sampleEnd;
            ok = false;
        }
        else
        {
            memcpy(data, &dataSmpl[m_currentSmplPos], nbRead * sizeof(qint32));
            m_currentSmplPos += nbRead;
        }
    }
    return ok;
}

void Voice::release(bool quick)
{
    if (quick)
    {
        // Arrêt par classe exclusive, arrêt rapide
        m_enveloppeVol.quickRelease();
    }
    m_release = true;
}

void Voice::setGain(double gain)
{
    m_gain = gain;
}
void Voice::setChorus(int level, int depth, int frequency)
{
    m_chorus.setEffectMix((double)level / 200. * (double)m_voiceParam->chorus / 100.);
    m_chorus.setModDepth((double)depth / 4000.);
    m_chorus.setModFrequency((double)frequency / 15.);
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

Voice* Voice::readData(double * data1, double * data2, qint64 size)
{
    Voice * voiceRet = NULL;
    // Récupération des données
    double data[size];
    qint64 nbRead = this->readData((char *)data, 8 * size) / 8;
    if (nbRead < 0)
    {
        nbRead = -nbRead;
        voiceRet = this;
    }
    if (nbRead != size)
        qDebug() << "warning: synth asked" << size << "samples and got" << nbRead;
    // Ajout en séparant les voix et application chorus
    double pan = (this->getVoiceParam()->pan + 50) * PI / 200.;    // Entre 0 et pi / 2
    double coef1 = sin(pan);
    double coef2 = cos(pan);
    for (quint32 i = 0; i < size; i++)
    {
        data1[i] = coef1 * m_chorus.tick(data[i]);
        data2[i] = coef2 * m_chorus.lastOut(1);
    }
    return voiceRet;
}
