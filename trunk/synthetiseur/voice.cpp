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
Voice::Voice(QByteArray baData, DWORD smplRate, DWORD audioSmplRate, int note,
             VoiceParam * voiceParam, QObject * parent) :
    CircularBuffer(BUFFER_VOICE_SIZE, BUFFER_VOICE_AVANCE, parent),
    m_sinusOsc(audioSmplRate),
    m_baData(baData),
    m_smplRate(smplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(note),
    m_voiceParam(voiceParam),
    m_currentSmplPos(0),
    m_time(0),
    m_release(false),
    m_finished(false),
    m_enveloppeVol(voiceParam, audioSmplRate, 0),
    m_enveloppeMod(voiceParam, audioSmplRate, 1),
    m_d(0)
{}

Voice::Voice(DWORD audioSmplRate, VoiceParam *voiceParam, QObject *parent) :
    CircularBuffer(BUFFER_VOICE_SIZE, BUFFER_VOICE_AVANCE, parent),
    m_sinusOsc(audioSmplRate),
    m_smplRate(audioSmplRate),
    m_audioSmplRate(audioSmplRate),
    m_note(-3),
    m_voiceParam(voiceParam),
    m_currentSmplPos(0),
    m_time(0),
    m_release(false),
    m_finished(false),
    m_enveloppeVol(voiceParam, audioSmplRate, 0),
    m_enveloppeMod(voiceParam, audioSmplRate, 1),
    m_d(0)
{}

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
    if (nbData == 0) return;
    QByteArray baData;
    bool bRet = true;
    if (m_note == -3)
    {
        // Génération d'un sinus
        double f = 440.0 * pow(2.0, (double)(m_voiceParam->rootkey - 69) / 12);
        baData.resize(nbData);
        m_sinusOsc.getSinus(baData.data(), baData.size(), f);
    }
    else
    {
        // Nombre d'échantillons à mettre dans le buffer
        int nbFinal = nbData / 4 + 100; // division par 4 car 1 voix et 4 octets par echantillon et une petite marge
        // Nombre d'échantillons à lire au départ
        int nbInit = 0;
        // Fréquence d'échantillonnage initiale fictive (pour accordage)
        double tuneMod = 0;
        int avanceModEnv = 0;
        if (m_voiceParam->modEnvToPitch != 0)
        {
            // Prise en compte de la modulation
            qint32 valTmp[1];
            valTmp[0] = m_voiceParam->modEnvToPitch;
            m_enveloppeMod.applyEnveloppe(valTmp, 1, m_release, m_note, m_voiceParam, 0);
            tuneMod = (double)valTmp[0] / 100;
            avanceModEnv++;
        }
        double bpsInit = 0;
        if (m_note < 0)
            bpsInit = (double)m_smplRate * pow(2, ((m_voiceParam->getPitchDifference(m_voiceParam->rootkey) + tuneMod) / 12));
        else
            bpsInit = (double)m_smplRate * pow(2, ((m_voiceParam->getPitchDifference(this->m_note) + tuneMod) / 12));

        if (bpsInit != m_audioSmplRate)
            nbInit = qMax(100.0, (nbFinal * bpsInit) / m_audioSmplRate);
        else
            nbInit = qMax(100, nbFinal);
        //// CONSTITUTION DES DONNEES ////
        baData = takeData(nbInit, bRet);
        // Resampling
        if (bpsInit != m_audioSmplRate)
        {
            // Récupération des dernières valeurs avant resampling
            qint32 * data = (qint32*)baData.data();
            qint32 fin = data[baData.size()/4-1];
            baData = Sound::resampleMono(baData, bpsInit, m_audioSmplRate, 32, m_valPrec, m_d);
            // Mise à jour des valeurs précédentes
            m_valPrec = fin;
        }
        // Avance dans la modulation
        if (m_voiceParam->modEnvToPitch != 0)
        {
            QByteArray baTmp;
            baTmp.resize(baData.size() - 4);
            qint32 * dataTmp = (qint32 *)baTmp.data();
            m_enveloppeMod.applyEnveloppe(dataTmp, baData.size()/4, m_release, m_note, m_voiceParam, 0);
        }
    }
    // Application de l'enveloppe
    qint32 * data = (qint32 *)baData.data();
    bool bRet2 = false;
    if (m_note < 0)
        bRet2 = m_enveloppeVol.applyEnveloppe(data, baData.size()/4, m_release, m_voiceParam->rootkey, m_voiceParam, m_gain);
    else
        bRet2 = m_enveloppeVol.applyEnveloppe(data, baData.size()/4, m_release, m_note, m_voiceParam, m_gain);
    if (bRet2 || !bRet)
    {
        m_finished = true;
        emit(currentPosChanged(0));
    }
    else
        emit(currentPosChanged(m_currentSmplPos));
    //// ECRITURE SUR LE BUFFER ////
    this->writeData(baData.constData(), baData.size());
}

QByteArray Voice::takeData(qint64 nbRead, bool &end)
{
    const char * data = m_baData.constData();
    QByteArray baDataRet;
    baDataRet.resize(nbRead * 4);
    baDataRet.fill(0);
    char * dataRet = baDataRet.data();
    if (m_voiceParam->loopMode && m_voiceParam->loopStart != m_voiceParam->loopEnd)
    {
        // Boucle
        if (m_currentSmplPos >= m_voiceParam->loopEnd) m_currentSmplPos = m_voiceParam->loopStart;
        qint64 total = 0;
        while (nbRead - total > 0)
        {
            const qint64 chunk = qMin(m_voiceParam->loopEnd - m_currentSmplPos, nbRead - total);
            memcpy(dataRet + 4 * total, data + 4 * m_currentSmplPos, 4 * chunk);
            m_currentSmplPos += chunk;
            if (m_currentSmplPos >= m_voiceParam->loopEnd) m_currentSmplPos = m_voiceParam->loopStart;
            total += chunk;
        }
        end = true;
    }
    else
    {
        // Pas de boucle
        if (m_baData.size()/4 - m_currentSmplPos < nbRead)
        {
            baDataRet.fill(0);
            memcpy(dataRet, data + 4 * m_currentSmplPos, m_baData.size() - 4 * m_currentSmplPos);
            m_currentSmplPos = m_baData.size() / 4;
            end = false;
        }
        else
        {
            memcpy(dataRet, data + 4 * m_currentSmplPos, 4 * nbRead);
            m_currentSmplPos += nbRead;
            end = true;
        }
    }
    return baDataRet;
}

void Voice::setGain(double gain)
{
    this->m_gain = gain;
}

