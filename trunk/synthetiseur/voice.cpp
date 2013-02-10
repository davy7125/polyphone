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
    m_baData(baData),
    m_smplRate(smplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(note),
    m_velocity(velocity),
    m_voiceParam(voiceParam),
    m_currentSmplPos(0),
    m_time(0),
    m_release(false),
    m_finished(false),
    m_enveloppeVol(voiceParam, audioSmplRate, 0),
    m_enveloppeMod(voiceParam, audioSmplRate, 1),
    m_deltaPos(0),
    m_valPrec(0)
{
    // Initialisation resampling
    takeData(&m_valBase, 1);
}

// Sinus
Voice::Voice(DWORD audioSmplRate, VoiceParam *voiceParam, QObject *parent) :
    CircularBuffer(BUFFER_VOICE_SIZE, BUFFER_VOICE_AVANCE, parent),
    m_sinusOsc(audioSmplRate),
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
    m_valPrec(0)
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
    qint32 data[nbData];
    bool bRet = true;
    if (m_note == -3)
    {
        // Génération d'un sinus
        double f = 440.0 * pow(2.0, (double)(m_voiceParam->rootkey - 69) / 12);
        m_sinusOsc.getSinus((char*)data, 4*nbData, f);
    }
    else
    {
        /// ENVELOPPE DE MODULATION ///

        qint32 dataMod[nbData+20];///////
        m_enveloppeMod.applyEnveloppe(dataMod, nbData, m_release, m_note, 127, m_voiceParam, 0, 1000);

        // MODULATION DU PITCH
        double deltaPitchFixe = 0;
        if (m_note < 0)
            deltaPitchFixe = 12 * log2((double)m_audioSmplRate / m_smplRate) +
                    m_voiceParam->getPitchDifference(m_voiceParam->rootkey);
        else
            deltaPitchFixe = -12 * log2((double)m_audioSmplRate / m_smplRate) +
                    m_voiceParam->getPitchDifference(this->m_note);
        double modPitch[nbData+19];////////////
        for (int i = 0; i < nbData; i++)
            modPitch[i+1] = deltaPitchFixe + (double)(dataMod[i] * m_voiceParam->modEnvToPitch) / 100000;

        // Conversion en distance cumulée entre points
        modPitch[0] = m_deltaPos + 1;
        for (int i = 1; i <= nbData; i++)
            modPitch[i] = pow(2, modPitch[i] / 12) + modPitch[i-1];
        m_deltaPos = modPitch[nbData];
        m_deltaPos = m_deltaPos - ceil(m_deltaPos);

        // Constitution des données
        int nbDataTmp = ceil(modPitch[nbData]) - 1;
        qint32 dataTmp[nbDataTmp + 20];////////////
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
            data[i] = (1. - pos) * val1 + pos * val2;
        }
    }
    // Application de l'enveloppe de volume
    bool bRet2 = false;
    if (m_note < 0)
        bRet2 = m_enveloppeVol.applyEnveloppe(data, nbData, m_release, m_voiceParam->rootkey,
                                              m_velocity, m_voiceParam, m_gain);
    else
        bRet2 = m_enveloppeVol.applyEnveloppe(data, nbData, m_release, m_note,
                                              m_velocity, m_voiceParam, m_gain);
    if (bRet2 || !bRet)
    {
        m_finished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(m_currentSmplPos));
    //// ECRITURE SUR LE BUFFER ////
    this->writeData((char *)data, nbData * 4);
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
        if (m_baData.size()/4 - m_currentSmplPos < nbRead)
        {
            memcpy(data, &dataSmpl[m_currentSmplPos], m_baData.size() - 4 * m_currentSmplPos);
            for (int i = m_baData.size()/4 - m_currentSmplPos; i < nbRead; i++)
                data[i] = 0;
            m_currentSmplPos = m_baData.size() / 4;
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

void Voice::setGain(double gain)
{
    this->m_gain = gain;
}

