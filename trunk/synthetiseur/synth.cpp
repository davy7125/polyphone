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
    m_gain(0),
    m_gainSmpl(0),
    m_isStereo(false),
    m_isLoopEnabled(false),
    m_isSinusEnabled(false),
    m_choLevel(0), m_choDepth(0), m_choFrequency(0),
    m_clipCoef(1),
    m_interrupt(false),
    m_recordFile(NULL),
    m_isRecording(true)
{
    m_mutexInterrupt.unlock();
    m_mutexVoices.unlock();
    m_mutexCompleted.unlock();
    m_mutexRecord.unlock();
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
            EltID idSmpl(elementSmpl, idSf2, idElt, 0, 0);
            VoiceParam * voiceParam1 = new VoiceParam(m_sf2, idSmpl);
            voiceParam1->volReleaseTime = 0.2;
            voiceParam1->loopMode = this->m_isLoopEnabled;
            Voice *voiceTmp1 = NULL;
            Voice *voiceTmp2 = NULL;
            // Lien ?
            SFSampleLink typeLien = m_sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
            {
                // Smpl lié
                EltID idSmpl2(elementSmpl, idSf2, m_sf2->get(idSmpl, champ_wSampleLink).wValue, 0, 0);
                VoiceParam * voiceParam2 = new VoiceParam(m_sf2, idSmpl2);
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
                // Création voix 2
                voiceTmp2 = new Voice(m_sf2->getData(idSmpl2, champ_sampleData32),
                                             m_sf2->get(idSmpl2, champ_dwSampleRate).dwValue,
                                             m_format.sampleRate(), -2, 127, voiceParam2);
                // Initialisation chorus
                voiceTmp2->setChorus(0, 0, 0);
            }
            // Création voix 1
            voiceTmp1 = new Voice(m_sf2->getData(idSmpl, champ_sampleData32),
                                         m_sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                         m_format.sampleRate(), -1, 127, voiceParam1);
            connect(voiceTmp1, SIGNAL(currentPosChanged(int)), this, SLOT(emitCurrentPosChanged(int)));
            // Création sinus
            VoiceParam *voiceParam3 = new VoiceParam(m_sf2, idSmpl);
            voiceParam3->volReleaseTime = 0.2;
            Voice *voiceTmp3 = new Voice(m_format.sampleRate(), voiceParam3);
            // Initialisation chorus
            voiceTmp1->setChorus(0, 0, 0);
            voiceTmp3->setChorus(0, 0, 0);
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
            EltID idSmpl(elementSmpl, idSf2, idElt, 0, 0);
            // Récupération des paramètres
            VoiceParam * voiceParam = new VoiceParam(m_sf2, idSmpl, voiceParamTmp);
            // Création voix
            Voice * voiceTmp = new Voice(m_sf2->getData(idSmpl, champ_sampleData32),
                                         m_sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                         m_format.sampleRate(), note, velocity,
                                         voiceParam);
            m_listeVoixTmp << voiceTmp;
            // Initialisation chorus
            voiceTmp->setChorus(m_choLevel, m_choDepth, m_choFrequency);
            m_mutexVoices.lock();
            if (voiceParam->exclusiveClass != 0)
            {
                // Parcours de toutes les voix
                for (int i = m_listeVoix.size()-1; i >= 0; i--)
                    if (m_listeVoix.at(i)->getVoiceParam()->exclusiveClass == voiceParam->exclusiveClass &&
                            m_listeVoix.at(i)->getVoiceParam()->numPreset == voiceParam->numPreset &&
                            m_listeVoixTmp.indexOf(m_listeVoix.at(i)) == -1)
                        m_listeVoix.at(i)->release(true);
            }
            m_listeVoix.append(voiceTmp);
            m_listeVoix.last()->setGain(this->m_gain);
            m_mutexVoices.unlock();
        }
        break;
    case 1:{ // instrument
        // Parcours de tous les samples liés
        EltID idInstSmpl(elementInstSmpl, idSf2, idElt, 0, 0);
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
                    // Récupération des paramètres
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
        EltID idPrstInst(elementPrstInst, idSf2, idElt, 0, 0);
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
                    // Récupération des paramètres
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
    if (!voiceParamTmp)
        m_listeVoixTmp.clear();
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
    // Initialisation données à écrire
    double data1[nbData];
    double data2[nbData];
    double data1Rev[nbData];
    double data2Rev[nbData];
    for (int i = 0; i < nbData; i++)
    {
        data1[i] = 0;
        data2[i] = 0;
        data1Rev[i] = 0;
        data2Rev[i] = 0;
    }
    double data1Tmp[nbData];
    double data2Tmp[nbData];
    double coef1, coef2;
    // Fusion des voix
    m_mutexVoices.lock();
    QList<Voice *> listeVoice;
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        listeVoice.append(m_listeVoix.at(i)->readData(data1Tmp, data2Tmp, nbData));
        coef1 = m_listeVoix.at(i)->getVoiceParam()->reverb / 100.;
        coef2 = 1. - coef1;
        // Fusion des données
        for (int j = 0; j < nbData; j++)
        {
            data1Rev[j] += coef1 * data1Tmp[j];
            data2Rev[j] += coef1 * data2Tmp[j];
            data1[j]    += coef2 * data1Tmp[j];
            data2[j]    += coef2 * data2Tmp[j];
        }
    }
    // Suppression des voix terminées
    for (int i = 0; i < listeVoice.size(); i++)
        this->endVoice(listeVoice.at(i));
    m_mutexVoices.unlock();
    // Application réverbération et addition
    for (int i = 0; i < nbData; i++)
    {
        data1[i] += m_reverb.tick(data1Rev[i], data2Rev[i]);
        data2[i] += m_reverb.lastOut(1);
    }
    // vérif -1, 1 et passage en float
    clip(data1, data2, nbData);
    float * dataF1 = (float *)data1;
    float * dataF2 = (float *)data2;
    for (qint64 i = 0; i < nbData; i++)
    {
        dataF1[i] = (float)data1[i];
        dataF2[i] = (float)data2[i];
    }

    //// ECRITURE SUR LE DOUBLE BUFFER ////
    this->writeData((char *)data1, (char *)data2, nbData * 4);

    // Enregistrement dans un fichier si demandé
    m_mutexRecord.lock();
    if (m_recordFile && m_isRecording)
    {
        // Entrelacement et écriture
        for (int i = nbData - 1; i >= 0; i--)
        {
            dataF1[2 * i + 1] = dataF2[i];
            dataF1[2 * i]     = dataF1[i];
        }
        m_recordStream.writeRawData((char*)dataF1, nbData * 8);
        // Prise en compte de l'avance
        m_recordLength += nbData * 8;
        this->samplesRead(nbData);
    }
    m_mutexRecord.unlock();
}
void Synth::clip(double * data1, double * data2, qint64 size)
{
    // Recherche valeur maxi
    double dMax = 0;
    qint32 pos = -1;
    double dTmp;
    for (qint32 i = 0; i < size; i++)
    {
        dTmp = qAbs(data1[i]);
        if (dTmp > dMax)
        {
            dMax = dTmp;
            pos = i;
        }
        dTmp = qAbs(data2[i]);
        if (dTmp > dMax)
        {
            dMax = dTmp;
            pos = i;
        }
    }
    if (dMax > .99)
    {
        double coef = .99 / dMax;
        for (int i = 0; i < pos; i++)
        {
            dTmp = (double)(pos - i) / pos * m_clipCoef
                    + (double)i / pos * coef;
            data1[i] *= dTmp;
            data2[i] *= dTmp;
        }
        m_clipCoef = coef;
        for (int i = pos; i < size; i++)
        {
            data1[i] *= coef;
            data2[i] *= coef;
        }
    }
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
    m_gain = gain;
    m_mutexVoices.lock();
    for (int i = 0; i < m_listeVoix.size(); i++)
        if (m_listeVoix.at(i)->getNote() >= 0)
            m_listeVoix.at(i)->setGain(m_gain);
    m_mutexVoices.unlock();
}
void Synth::setReverb(int level, int size, int width, int damping)
{
    m_reverb.setEffectMix((double)level / 100.);
    m_reverb.setRoomSize((double)size / 100.);
    m_reverb.setWidth((double)width / 100.);
    m_reverb.setDamping((double)damping / 100.);
}
void Synth::setChorus(int level, int depth, int frequency)
{
    // Mise à jour du chorus
    m_choLevel = level;
    m_choDepth = depth;
    m_choFrequency = frequency;
    m_mutexVoices.lock();
    for (int i = 0; i < m_listeVoix.size(); i++)
        if (m_listeVoix.at(i)->getNote() >= 0)
            m_listeVoix.at(i)->setChorus(level, depth, frequency);
    m_mutexVoices.unlock();
}
void Synth::setGainSample(int gain)
{
    // Modification du gain des samples
    m_mutexVoices.lock();
    m_gainSmpl = gain;
    // Mise a jour voix -1 et -2
    for (int i = 0; i < m_listeVoix.size(); i++)
        if (m_listeVoix.at(i)->getNote() == -1)
        {
            if (m_isStereo)
                m_listeVoix.at(i)->setGain(m_gainSmpl - 12);
            else
                m_listeVoix.at(i)->setGain(m_gainSmpl);
        }
        else if (m_listeVoix.at(i)->getNote() == -2 && m_isStereo)
            m_listeVoix.at(i)->setGain(m_gainSmpl - 12);
    m_mutexVoices.unlock();
}
void Synth::setStereo(int isStereo, bool withMutex)
{
    // Modification lecture mono ou stereo
    if (withMutex)
        m_mutexVoices.lock();
    m_isStereo = isStereo;

    // Mise à jour voix -1 et -2
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
                voice1->setGain(m_gainSmpl - 12);
            else
                voice1->setGain(m_gainSmpl);
        }
        if (voice2)
            voice2->setGain(m_gainSmpl - 12);
    }
    else
    {
        if (voice1)
        {
            if (voice1->getVoiceParam()->pan < 0)
                voice1->getVoiceParam()->pan = -1;
            else if (voice1->getVoiceParam()->pan > 0)
                voice1->getVoiceParam()->pan = 1;
            voice1->setGain(m_gainSmpl);
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

    // Mise à jour voix -3
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
void Synth::setStartLoop(int startLoop, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    m_mutexVoices.lock();
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1 || (m_listeVoix.at(i)->getNote() == -2 && repercute))
            m_listeVoix[i]->getVoiceParam()->loopStart = startLoop;
    }
    m_mutexVoices.unlock();
}
void Synth::setEndLoop(int endLoop, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    m_mutexVoices.lock();
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1 || (m_listeVoix.at(i)->getNote() == -2 && repercute))
            m_listeVoix[i]->getVoiceParam()->loopEnd = endLoop;
    }
    m_mutexVoices.unlock();
}
void Synth::setPitchCorrection(int correction, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    m_mutexVoices.lock();
    for (int i = 0; i < m_listeVoix.size(); i++)
    {
        if (m_listeVoix.at(i)->getNote() == -1 || (m_listeVoix.at(i)->getNote() == -2 && repercute))
            m_listeVoix[i]->getVoiceParam()->fineTune = correction;
    }
    m_mutexVoices.unlock();
}
void Synth::setRootKey(int rootKey)
{
    // mise à jour voix -3 (sinus)
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

// Boucle déclenchant la génération de données
void Synth::start()
{
    qint64 nbData;
    m_mutexInterrupt.lock();
    m_interrupt = false;
    m_mutexInterrupt.unlock();
    // Surveillance du buffer après chaque lecture
    while (!getInterrupt())
    {
        nbData = this->dataNeeded();
        if (nbData)
            this->generateData(nbData);
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
    m_mutexInterrupt.unlock();
    m_mutexCompleted.unlock();
}
qint64 Synth::readData(char *data1, char *data2, qint64 maxlen)
{
    int valRet = CircularBuffer::readData(data1, data2, maxlen);
    for (int i = valRet; i < maxlen; i++)
    {
        data1[i] = 0;
        data2[i] = 0;
    }
    m_mutexCompleted.unlock();
    return valRet;
}

void Synth::setFormat(AudioFormat format)
{
    // Mutex inutile : pas de génération de données lors de l'appel à setFormat
    m_format = format;
    // Réinitialisation
    this->stop();
    // Envoi signal de modification de l'échantillonnage
    this->sampleRateChanged(format.sampleRate());
}

void Synth::startNewRecord(QString fileName)
{
    m_mutexRecord.lock();
    if (m_recordFile)
        this->endRecord();
    m_recordFile = new QFile(fileName);
    if (m_recordFile->open(QIODevice::WriteOnly))
    {
        // Création de l'entête
        quint32 dwTemp = 0;
        quint16 wTemp;
        m_recordStream.setDevice(m_recordFile);
        m_recordStream.setByteOrder(QDataStream::LittleEndian);
        m_recordLength = 0;
        // Entete
        m_recordStream.writeRawData("RIFF", 4);
        m_recordStream << (quint32)(m_recordLength + 18 + 4 + 8 + 8);
        m_recordStream.writeRawData("WAVE", 4);
        ///////////// BLOC FMT /////////////
        m_recordStream.writeRawData("fmt ", 4);
        dwTemp = 18;
        m_recordStream << dwTemp;
        // Compression code
        wTemp = 3;
        m_recordStream << wTemp;
        // Number of channels
        wTemp = 2;
        m_recordStream << wTemp;
        // Sample rate
        dwTemp = m_format.sampleRate();
        m_recordStream << dwTemp;
        // Average byte per second
        dwTemp *= 2 * 4;
        m_recordStream << dwTemp;
        // Block align
        wTemp = 2 * 4;
        m_recordStream << wTemp;
        // Significants bits per smpl
        m_recordStream << (quint16)32;
        // Extra format bytes
        wTemp = 0;
        m_recordStream << wTemp;
        ///////////// BLOC DATA /////////////
        m_recordStream.writeRawData("data", 4);
        m_recordStream << m_recordLength;

        m_isRecording = true;
    }
    else
    {
        delete m_recordFile;
        m_recordFile = NULL;
    }
    m_mutexRecord.unlock();
}
void Synth::endRecord()
{
    m_mutexRecord.lock();
    if (m_recordFile)
    {
        // Ajustement des dimensions du fichier
        m_recordFile->seek(4);
        m_recordStream << (quint32)(m_recordLength + 18 + 4 + 8 + 8);
        m_recordFile->seek(42);
        m_recordStream << m_recordLength;

        // Fermeture
        m_recordStream.setDevice(NULL);
        m_recordFile->close();
        delete m_recordFile;
        m_recordFile = NULL;

        m_isRecording = false;
    }
    m_mutexRecord.unlock();
}
void Synth::pause(bool isOn)
{
    m_mutexRecord.lock();
    m_isRecording = !isOn;
    m_mutexRecord.unlock();
}
