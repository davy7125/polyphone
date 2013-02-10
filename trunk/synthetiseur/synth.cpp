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

#include "synth.h"


// Constructeur, destructeur
Synth::Synth(Pile_sf2 *sf2, QObject *parent) :
    CircularBuffer(BUFFER_SYNTH_SIZE, BUFFER_SYNTH_AVANCE, parent),
    m_sf2(sf2),
    m_isJack(false),
    m_gain(0),
    m_gainSmpl(0),
    m_isStereo(false),
    m_isLoopEnabled(false),
    m_isSinusEnabled(false),
    m_interrupt(false)
{
    m_mutexInterrupt.unlock();
    m_mutexVoices.unlock();
    m_mutexCompleted.unlock();
}
Synth::~Synth()
{
}

// signal de lecture ou de fin
void Synth::play(int type, int idSf2, int idElt, int note, int velocity, VoiceParam * voiceParamTmp)
{
    if (velocity == 0)
    {
        // Relachement d'une note, parcours de toutes les voix
        m_mutexVoices.lock();
        if (note == -1)
        {
            // Arret lecture d'un sample
            for (int i = m_listeVoix.size()-1; i >= 0; i--)
                if (m_listeVoix.at(i)->getNote() < 0)
                    m_listeVoix.at(i)->release();
        }
        else
        {
            for (int i = m_listeVoix.size()-1; i >= 0; i--)
                if (m_listeVoix.at(i)->getNote() == note)
                    m_listeVoix.at(i)->release();
        }
        m_mutexVoices.unlock();
        return;
    }
    // Appui sur une note
    switch (type)
    {
    case 0: // sample
        if (note == -1)
        {
            // Lecture d'un sample, association possible
            EltID idSmpl = {elementSmpl, idSf2, idElt, 0, 0};
            VoiceParam * voiceParam1 = new VoiceParam(m_sf2, idSmpl, voiceParamTmp);
            voiceParam1->volReleaseTime = 0.2;
            voiceParam1->loopMode = this->m_isLoopEnabled;
            Voice *voiceTmp1 = NULL;
            Voice *voiceTmp2 = NULL;
            // Lien ?
            SFSampleLink typeLien = m_sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
            {
                // Smpl lie
                EltID idSmpl2 = {elementSmpl, idSf2, m_sf2->get(idSmpl, champ_wSampleLink).wValue, 0, 0};
                VoiceParam * voiceParam2 = new VoiceParam(m_sf2, idSmpl2, voiceParamTmp);
                voiceParam2->volReleaseTime = 0.2;
                voiceParam2->loopMode = this->m_isLoopEnabled;
                // Balance
                if (typeLien == leftSample || typeLien == RomLeftSample)
                {
                    voiceParam1->pan = -50;
                    voiceParam2->pan = 50;
                }
                else
                {
                    voiceParam1->pan = 50;
                    voiceParam2->pan = -50;
                }
                // Creation voix 2
                voiceTmp2 = new Voice(m_sf2->getData(idSmpl2, champ_sampleData32),
                                             m_sf2->get(idSmpl2, champ_dwSampleRate).dwValue,
                                             m_format.sampleRate(), -2, 127, voiceParam2);
            }
            // Creation voix 1
            voiceTmp1 = new Voice(m_sf2->getData(idSmpl, champ_sampleData32),
                                         m_sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                         m_format.sampleRate(), -1, 127, voiceParam1);
            connect(voiceTmp1, SIGNAL(currentPosChanged(int)), this, SLOT(emitCurrentPosChanged(int)));
            // Creation sinus
            VoiceParam *voiceParam3 = new VoiceParam(m_sf2, idSmpl);
            voiceParam3->volReleaseTime = 0.2;
            Voice *voiceTmp3 = new Voice(m_format.sampleRate(), voiceParam3);
            // Ajout des voix dans la liste
            m_mutexVoices.lock();
            m_listeVoix.append(voiceTmp1);
            if (voiceTmp2)
                m_listeVoix.append(voiceTmp2);
            m_listeVoix.append(voiceTmp3);
            this->setStereo(m_isStereo, false);
            this->setSinusEnabled(m_isSinusEnabled, false);
            m_mutexVoices.unlock();
        }
        else
        {
            EltID idSmpl = {elementSmpl, idSf2, idElt, 0, 0};
            // Recuperation des parametres
            VoiceParam * voiceParam = new VoiceParam(m_sf2, idSmpl, voiceParamTmp);
            // Creation voix
            Voice * voiceTmp = new Voice(m_sf2->getData(idSmpl, champ_sampleData32),
                                         m_sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                         m_format.sampleRate(), note, velocity,
                                         voiceParam);
            m_mutexVoices.lock();
            m_listeVoix.append(voiceTmp);
            m_listeVoix.last()->setGain(this->m_gain);
            m_mutexVoices.unlock();
        }
        break;
    case 1:{ // instrument
        // Parcours de tous les samples lies
        EltID idInstSmpl = {elementInstSmpl, idSf2, idElt, 0, 0};
        for (int i = 0; i < m_sf2->count(idInstSmpl); i++)
        {
            idInstSmpl.indexElt2 = i;
            if (!m_sf2->get(idInstSmpl, champ_hidden).bValue)
            {
                int keyMin = 0;
                int keyMax = 127;
                if (m_sf2->isSet(idInstSmpl, champ_keyRange))
                {
                    keyMin = m_sf2->get(idInstSmpl, champ_keyRange).rValue.byLo;
                    keyMax = m_sf2->get(idInstSmpl, champ_keyRange).rValue.byHi;
                }
                int velMin = 0;
                int velMax = 127;
                if (m_sf2->isSet(idInstSmpl, champ_velRange))
                {
                    velMin = m_sf2->get(idInstSmpl, champ_velRange).rValue.byLo;
                    velMax = m_sf2->get(idInstSmpl, champ_velRange).rValue.byHi;
                }
                if (keyMin <= note     && keyMax >= note &&
                    velMin <= velocity && velMax >= velocity)
                {
                    // Recuperation des parametres
                    VoiceParam * voiceParam = new VoiceParam(m_sf2, idInstSmpl, voiceParamTmp);
                    // Lecture de l'instrument associé
                    this->play(0, idSf2, m_sf2->get(idInstSmpl, champ_sampleID).wValue,
                               note, velocity, voiceParam);
                    delete voiceParam;
                }
            }
        }
        }break;
    case 2:{ // preset
        // Parcours de tous les instruments lies
        EltID idPrstInst = {elementPrstInst, idSf2, idElt, 0, 0};
        for (int i = 0; i < m_sf2->count(idPrstInst); i++)
        {
            idPrstInst.indexElt2 = i;
            if (!m_sf2->get(idPrstInst, champ_hidden).bValue)
            {
                int keyMin = 0;
                int keyMax = 127;
                if (m_sf2->isSet(idPrstInst, champ_keyRange))
                {
                    keyMin = m_sf2->get(idPrstInst, champ_keyRange).rValue.byLo;
                    keyMax = m_sf2->get(idPrstInst, champ_keyRange).rValue.byHi;
                }
                int velMin = 0;
                int velMax = 127;
                if (m_sf2->isSet(idPrstInst, champ_velRange))
                {
                    velMin = m_sf2->get(idPrstInst, champ_velRange).rValue.byLo;
                    velMax = m_sf2->get(idPrstInst, champ_velRange).rValue.byHi;
                }
                if (keyMin <= note     && keyMax >= note &&
                    velMin <= velocity && velMax >= velocity)
                {
                    // Recuperation des parametres
                    VoiceParam * voiceParam = new VoiceParam(m_sf2, idPrstInst);
                    // Lecture de l'instrument associe
                    this->play(1, idSf2, m_sf2->get(idPrstInst, champ_instrument).wValue,
                               note, velocity, voiceParam);
                    delete voiceParam;
                }
            }
        }
        }break;
    default:
        return;
    }
}
void Synth::stop()
{
    // Arret demande de toutes les voix
    m_mutexVoices.lock();
    for (int i = m_listeVoix.size()-1; i >= 0; i--)
        delete m_listeVoix.takeAt(i);
    m_mutexVoices.unlock();
    // reinitialisation buffer
    this->reinit();
}

void Synth::generateData(qint64 nbData)
{
    //// GENERATION DES DONNEES ////
    if (nbData <= 0) return;
    // Nombre de valeurs à ajouter
    nbData = 4 * nbData / m_format.sampleSize() + 1; // division par (nb voix * octets) et 0 interdit
    if (!m_isJack)
    {
        // Initialisation des donnees a ecrire
        char data[nbData * 8]; // 2 voix de 4 octets
        for (int i = 0; i < nbData * 8; i++)
            data[i] = 0;
        // Fusion des voix
        m_mutexVoices.lock();
        QList<Voice *> listeVoice;
        for (int i = 0; i < m_listeVoix.size(); i++)
            listeVoice.append(this->fusion(data, nbData * 8, m_listeVoix.at(i)));
        // Suppression des voix terminees
        for (int i = 0; i < listeVoice.size(); i++)
            this->endVoice(listeVoice.at(i));
        m_mutexVoices.unlock();
        //// ADAPTATION AU FORMAT AUDIO ////
        // adaptation signed / unsigned
        if (m_format.sampleType() == QAudioFormat::UnSignedInt)
        {
            // Conversion signed => unsigned
            const qint32 * data32 = (const qint32 *)data;
            quint32 * dataU32 = (quint32 *)data;
            for (int i = 0; i < nbData * 2; i++)
                dataU32[i] = (qint64)data32[i] + 2147483648LL;
        }
        // adaptation bits par sample et endianness
        char data2[nbData * m_format.sampleSize() / 4];
        Sound::bpsConversion(data2, data, 8 * nbData, 32, m_format.sampleSize(),
                             m_format.byteOrder() == QAudioFormat::BigEndian);
        //// ECRITURE SUR LE BUFFER ////
        this->writeData(data2, nbData * m_format.sampleSize() / 4);
    }
    else
    {
        // Initialisation donnees a ecrire
        QByteArray baData1, baData2;
        baData1.resize(nbData * 4);
        baData2.resize(nbData * 4); // 2 voix de 4 octets
        baData1.fill(0);
        baData2.fill(0);
        // Fusion des voix
        m_mutexVoices.lock();
        QList<Voice *> listeVoice;
        for (int i = 0; i < m_listeVoix.size(); i++)
            listeVoice.append(this->fusion(baData1.data(), baData2.data(), nbData * 4, m_listeVoix.at(i)));
        // Suppression des voix terminees
        for (int i = 0; i < listeVoice.size(); i++)
            this->endVoice(listeVoice.at(i));
        m_mutexVoices.unlock();
        //// ECRITURE SUR LE BUFFER ////
        this->writeData(baData1.constData(), baData2.constData(), nbData * 4);
    }
}
Voice* Synth::fusion(char * data1, qint64 size, Voice * voice)
{
    Voice * voiceRet = NULL;
    // Recuperation des donnees
    char data[size/2];
    qint64 nbRead = voice->readData(data, size / 2); // passage mono
    if (nbRead < 0)
    {
        nbRead = -nbRead;
        voiceRet = voice;
    }
    if (nbRead != size / 2)
        qDebug() << "warning: synth asked" << size/2 << "samples and got" << nbRead;
    // Ajout en separant les voix
    double pan = (voice->getVoiceParam()->pan + 50) / 100; // entre 0 et 1
    qint32 * data1I = (qint32 *)data1;
    qint32 * dataI = (qint32 *)data;
    // Ajout en separant les voix (stereo)
    for (quint32 i = 0; i < nbRead / 4; i++)
    {
        data1I[2*i]   += pan        * dataI[i];
        data1I[2*i+1] += (1. - pan) * dataI[i];
    }
    return voiceRet;
}
Voice* Synth::fusion(char * data1, char * data2, qint64 size, Voice * voice)
{
    Voice * voiceRet = NULL;
    // Recuperation des donnees
    char data[size];
    qint64 nbRead = voice->readData(data, size);
    if (nbRead < 0)
    {
        nbRead = -nbRead;
        voiceRet = voice;
    }
    if (nbRead != size)
        qDebug() << "warning: synth asked" << size << "samples and got" << nbRead;
    // Ajout en separant les voix, conversion en float en meme temps
    double pan = (voice->getVoiceParam()->pan + 50) / 100; // entre 0 et 1
    float * data1F = (float *)data1;
    float * data2F = (float *)data2;
    qint32 * dataI = (qint32 *)data;
    double dTmp;
    for (quint32 i = 0; i < size / 4; i++)
    {
        dTmp = (double)dataI[i] / 2147483647;
        data1F[i] += pan        * dTmp;
        data2F[i] += (1. - pan) * dTmp;
    }
    return voiceRet;
}
void Synth::endVoice(Voice * voice)
{
    // Suppression d'une voix
    for (int i = m_listeVoix.size() - 1; i >= 0; i--)
    {
        if (m_listeVoix.at(i) == voice)
        {
            if (m_listeVoix.at(i)->getNote() == -1)
                this->readFinished();
            delete m_listeVoix.takeAt(i);
            return;
        }
    }
}

// Lecture d'un sample
void Synth::setGain(double gain)
{
    // Modification du gain
    m_mutexVoices.lock();
    m_gain = gain;
    for (int i = 0; i < m_listeVoix.size(); i++)
        if (m_listeVoix.at(i)->getNote() >= 0)
            m_listeVoix.at(i)->setGain(m_gain);
    m_mutexVoices.unlock();
}
void Synth::setGainSample(int gain)
{
    // Modification du gain des samples
    m_mutexVoices.lock();
    m_gainSmpl = gain - 50;
    // Mise a jour voix -1 et -2
    for (int i = 0; i < m_listeVoix.size(); i++)
        if (m_listeVoix.at(i)->getNote() == -1)
        {
            if (m_isStereo)
                m_listeVoix.at(i)->setGain(m_gainSmpl);
            else
                m_listeVoix.at(i)->setGain(m_gainSmpl+12);
        }
        else if (m_listeVoix.at(i)->getNote() == -2 && m_isStereo)
            m_listeVoix.at(i)->setGain(m_gainSmpl);
    m_mutexVoices.unlock();
}
void Synth::setStereo(int isStereo, bool withMutex)
{
    // Modification lecture mono ou stereo
    if (withMutex)
        m_mutexVoices.lock();
    m_isStereo = isStereo;
    // Mise a jour voix -1 et -2
    Voice * voice1 = NULL;
    Voice * voice2 = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1)
            voice1 = m_listeVoix.at(i);
        else if (m_listeVoix.at(i)->getNote() == -2)
            voice2 = m_listeVoix.at(i);
    }
    if (isStereo)
    {
        if (voice1)
        {
            if (voice1->getVoiceParam()->pan < 0)
                voice1->getVoiceParam()->pan = -50;
            else if (voice1->getVoiceParam()->pan > 0)
                voice1->getVoiceParam()->pan = 50;
            if (voice2)
                voice1->setGain(m_gainSmpl);
            else
                voice1->setGain(m_gainSmpl+12);
        }
        if (voice2)
            voice2->setGain(m_gainSmpl);
    }
    else
    {
        if (voice1)
        {
            if (voice1->getVoiceParam()->pan < 0)
                voice1->getVoiceParam()->pan = -1;
            else if (voice1->getVoiceParam()->pan > 0)
                voice1->getVoiceParam()->pan = 1;
            voice1->setGain(m_gainSmpl+12);
        }
        if (voice2)
            voice2->setGain(-1000);
    }
    if (withMutex)
        m_mutexVoices.unlock();
}
void Synth::setLoopEnabled(int isEnabled)
{
    // Modification lecture en boucle ou non
    m_mutexVoices.lock();
    m_isLoopEnabled = isEnabled;
    // Mise a jour voix -1 et -2
    Voice * voice1 = NULL;
    Voice * voice2 = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1)
            voice1 = m_listeVoix.at(i);
        else if (m_listeVoix.at(i)->getNote() == -2)
            voice2 = m_listeVoix.at(i);
    }
    if (voice1)
        voice1->getVoiceParam()->loopMode = isEnabled;
    if (voice2)
        voice2->getVoiceParam()->loopMode = isEnabled;
    m_mutexVoices.unlock();
}
void Synth::setSinusEnabled(int isEnabled, bool withMutex)
{
    // Modification lecture avec sinus non
    if (withMutex)
        m_mutexVoices.lock();
    m_isSinusEnabled = isEnabled;
    // Mise a jour voix -3
    Voice * voice3 = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -3)
            voice3 = m_listeVoix.at(i);
    }
    if (voice3)
    {
        if (isEnabled)
            voice3->setGain(0);
        else
            voice3->setGain(-1000);
    }
    if (withMutex)
        m_mutexVoices.unlock();
}
void Synth::setStartLoop(int startLoop)
{
    // mise a jour voix -1
    m_mutexVoices.lock();
    Voice * voice = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1)
            voice = m_listeVoix.at(i);
    }
    if (voice)
        voice->getVoiceParam()->loopStart = startLoop;
    m_mutexVoices.unlock();
}
void Synth::setEndLoop(int endLoop)
{
    // mise a jour voix -1
    m_mutexVoices.lock();
    Voice * voice = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1)
            voice = m_listeVoix.at(i);
    }
    if (voice)
        voice->getVoiceParam()->loopEnd = endLoop;
    m_mutexVoices.unlock();
}
void Synth::setPitchCorrection(int correction)
{
    // mise a jour voix -1
    m_mutexVoices.lock();
    Voice * voice = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1)
            voice = m_listeVoix.at(i);
    }
    if (voice)
        voice->getVoiceParam()->fineTune = correction;
    m_mutexVoices.unlock();
}
void Synth::setRootKey(int rootKey)
{
    // mise a jour voix -3 (sinus)
    m_mutexVoices.lock();
    Voice * voice = NULL;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -3)
            voice = m_listeVoix.at(i);
    }
    if (voice)
        voice->getVoiceParam()->rootkey = rootKey;
    m_mutexVoices.unlock();
}

void Synth::emitCurrentPosChanged(int pos)
{
    this->currentPosChanged(pos);
}

// Boucle declenchant la generation de donnees
void Synth::start()
{
    m_mutexInterrupt.lock();
    m_interrupt = false;
    m_mutexInterrupt.unlock();
    // Surveillance du buffer apres chaque lecture
    while (!getInterrupt())
    {
        if (this->dataNeeded())
            this->generateData(this->dataNeeded());
        m_mutexCompleted.lock();
    }
}
bool Synth::getInterrupt()
{
    m_mutexInterrupt.lock();
    bool bRet = m_interrupt;
    m_mutexInterrupt.unlock();
    return bRet;
}
void Synth::interruption()
{
    m_mutexInterrupt.lock();
    m_interrupt = true;
    m_mutexCompleted.unlock();
    m_mutexInterrupt.unlock();
}
qint64 Synth::readData(char *data, qint64 maxlen)
{
    int valRet = CircularBuffer::readData(data, maxlen);
    m_mutexCompleted.unlock();
    return valRet;
}
qint64 Synth::readData(char *data1, char *data2, qint64 maxlen)
{
    int valRet = CircularBuffer::readData(data1, data2, maxlen);
    m_mutexCompleted.unlock();
    return valRet;
}

void Synth::setFormat(QAudioFormat format, bool isJack)
{
    // Mutex inutile : pas de generation de donnees lors de l'appel a setFormat
    m_format = format;
    m_isJack = isJack;
    // Reinitialisation
    this->stop();
}
