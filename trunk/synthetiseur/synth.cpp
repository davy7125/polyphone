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

#include "synth.h"


// Constructeur, destructeur
Synth::Synth(Pile_sf2 *sf2, QWidget * parent) : QObject(parent),
    m_sf2(sf2),
    m_gain(0),
    m_choLevel(0), m_choDepth(0), m_choFrequency(0),
    m_clipCoef(1),
    m_recordFile(NULL),
    m_isRecording(true)
{
    // Création des sound engines
    int nbEngines = qMax(QThread::idealThreadCount() - 2, 1);

    nbEngines = 1;

    for (int i = 0; i < nbEngines; i++)
    {
        SoundEngine * soundEngine = new SoundEngine();
        connect(soundEngine, SIGNAL(readFinished()), this, SIGNAL(readFinished()));
        soundEngine->moveToThread(soundEngine->getThread());
        soundEngine->getThread()->start(QThread::TimeCriticalPriority);
        QMetaObject::invokeMethod(soundEngine, "start");
        _soundEngines << soundEngine;
    }

    _fTmpSumRev1 = new float [2 * BUFFER_ENGINE_MAX_DATA];
    _fTmpSumRev2 = new float [2 * BUFFER_ENGINE_MAX_DATA];
    _dataWav = new float[4 * BUFFER_ENGINE_MAX_DATA];
}

Synth::~Synth()
{
    // Arrêt des soundEngines
    for (int i = 0; i < _soundEngines.size(); i++)
        _soundEngines.at(i)->stop();

    // Arrêt des threads
    for (int i = 0; i < _soundEngines.size(); i++)
        _soundEngines.at(i)->getThread()->quit();

    // Suppression des sound engines et des threads
    while (_soundEngines.size())
    {
        QThread * thread = _soundEngines.last()->getThread();
        thread->wait(50);
        delete _soundEngines.takeLast();
        delete thread;
    }

    delete [] _fTmpSumRev1;
    delete [] _fTmpSumRev2;
    delete [] _dataWav;
}

// signal de lecture ou de fin
void Synth::play(int type, int idSf2, int idElt, int note, int velocity, VoiceParam * voiceParamTmp)
{
    if (velocity == 0)
    {
        // Relachement d'une note
        SoundEngine::releaseNote(note);
        return;
    }

    // Appui sur une note
    switch (type)
    {
    case 0:{ // sample
        EltID idSmpl(elementSmpl, idSf2, idElt, 0, 0);

        // Récupération des paramètres
        VoiceParam * voiceParam = new VoiceParam(m_sf2, idSmpl, voiceParamTmp);

        if (note < 0)
        {
            // Temps de fin, exclusive class et balance
            voiceParam->volReleaseTime = 0.2;
            voiceParam->exclusiveClass = note;
            SFSampleLink typeLien = m_sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
            switch (typeLien)
            {
            case leftSample: case RomLeftSample:
                voiceParam->pan = -50;
                break;
            case rightSample: case RomRightSample:
                voiceParam->pan = 50;
                break;
            default:
                break;
            }
        }

        // Création voix
        Voice * voiceTmp = new Voice(m_sf2->getData(idSmpl, champ_sampleData32),
                                     m_sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                     m_format.sampleRate(), note, velocity,
                                     voiceParam);

        // Initialisation chorus et gain
        if (note < 0)
            voiceTmp->setChorus(0, 0, 0);
        else
        {
            voiceTmp->setChorus(m_choLevel, m_choDepth, m_choFrequency);
            voiceTmp->setGain(m_gain);
        }

        // Ajout de la voix
        _listVoixTmp << voiceTmp;
        SoundEngine::addVoice(voiceTmp, _listVoixTmp);

        if (note == -1)
        {
            // Avancement du graphique
            connect(voiceTmp, SIGNAL(currentPosChanged(int)), this, SLOT(emitCurrentPosChanged(int)));

            // Lien ?
            SFSampleLink typeLien = m_sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
                this->play(0, idSf2, m_sf2->get(idSmpl, champ_wSampleLink).wValue, -2, 127);

            // Création sinus
            playSinus(idSmpl);
        }
    }break;
    case 1:{ // instrument
        // Parcours de tous les samples liés
        EltID idInstSmpl(elementInstSmpl, idSf2, idElt, 0, 0);
        EltID idInst = idInstSmpl;
        idInst.typeElement = elementInst;
        rangesType defaultKeyRange, defaultVelRange;
        if (m_sf2->isSet(idInst, champ_keyRange))
            defaultKeyRange = m_sf2->get(idInst, champ_keyRange).rValue;
        else
        {
            defaultKeyRange.byLo = 0;
            defaultKeyRange.byHi = 127;
        }
        if (m_sf2->isSet(idInst, champ_velRange))
            defaultVelRange = m_sf2->get(idInst, champ_velRange).rValue;
        else
        {
            defaultVelRange.byLo = 0;
            defaultVelRange.byHi = 127;
        }
        int keyMin, keyMax, velMin, velMax;
        rangesType rangeTmp;
        for (int i = 0; i < m_sf2->count(idInstSmpl); i++)
        {
            idInstSmpl.indexElt2 = i;
            if (!m_sf2->get(idInstSmpl, champ_hidden).bValue)
            {
                if (m_sf2->isSet(idInstSmpl, champ_keyRange))
                {
                    rangeTmp = m_sf2->get(idInstSmpl, champ_keyRange).rValue;
                    keyMin = rangeTmp.byLo;
                    keyMax = rangeTmp.byHi;
                }
                else
                {
                    keyMin = defaultKeyRange.byLo;
                    keyMax = defaultKeyRange.byHi;
                }
                if (m_sf2->isSet(idInstSmpl, champ_velRange))
                {
                    rangeTmp = m_sf2->get(idInstSmpl, champ_velRange).rValue;
                    velMin = rangeTmp.byLo;
                    velMax = rangeTmp.byHi;
                }
                else
                {
                    velMin = defaultVelRange.byLo;
                    velMax = defaultVelRange.byHi;
                }
                if (keyMin <= note && keyMax >= note &&
                        velMin <= velocity && velMax >= velocity)
                {
                    // Récupération des paramètres et lecture du sample associé
                    VoiceParam * voiceParam = new VoiceParam(m_sf2, idInstSmpl, voiceParamTmp);
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
        EltID idPrst = idPrstInst;
        idPrst.typeElement = elementPrst;
        rangesType defaultKeyRange, defaultVelRange;
        if (m_sf2->isSet(idPrst, champ_keyRange))
            defaultKeyRange = m_sf2->get(idPrst, champ_keyRange).rValue;
        else
        {
            defaultKeyRange.byLo = 0;
            defaultKeyRange.byHi = 127;
        }
        if (m_sf2->isSet(idPrst, champ_velRange))
            defaultVelRange = m_sf2->get(idPrst, champ_velRange).rValue;
        else
        {
            defaultVelRange.byLo = 0;
            defaultVelRange.byHi = 127;
        }
        int keyMin, keyMax, velMin, velMax;
        rangesType rangeTmp;
        for (int i = 0; i < m_sf2->count(idPrstInst); i++)
        {
            idPrstInst.indexElt2 = i;
            if (!m_sf2->get(idPrstInst, champ_hidden).bValue)
            {
                if (m_sf2->isSet(idPrstInst, champ_keyRange))
                {
                    rangeTmp = m_sf2->get(idPrstInst, champ_keyRange).rValue;
                    keyMin = rangeTmp.byLo;
                    keyMax = rangeTmp.byHi;
                }
                else
                {
                    keyMin = defaultKeyRange.byLo;
                    keyMax = defaultKeyRange.byHi;
                }
                if (m_sf2->isSet(idPrstInst, champ_velRange))
                {
                    rangeTmp = m_sf2->get(idPrstInst, champ_velRange).rValue;
                    velMin = rangeTmp.byLo;
                    velMax = rangeTmp.byHi;
                }
                else
                {
                    velMin = defaultVelRange.byLo;
                    velMax = defaultVelRange.byHi;
                }
                if (keyMin <= note     && keyMax >= note &&
                        velMin <= velocity && velMax >= velocity)
                {
                    // Récupération des paramètres et lecture de l'instrument associé
                    VoiceParam * voiceParam = new VoiceParam(m_sf2, idPrstInst);
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

    // Réinitialisation de la liste temporaire de voix (utilisée pour exclusive class)
    if (!voiceParamTmp)
        _listVoixTmp.clear();
}

void Synth::playSinus(EltID idSmpl)
{
    VoiceParam *voiceParamSinus = new VoiceParam(m_sf2, idSmpl);
    voiceParamSinus->volAttackTime = 0.2;
    voiceParamSinus->volDecayTime = 0.2;
    voiceParamSinus->volReleaseTime = 0.2;
    voiceParamSinus->exclusiveClass = -3;
    Voice *voiceSinus = new Voice(m_format.sampleRate(), voiceParamSinus);
    voiceSinus->setChorus(0, 0, 0);
    SoundEngine::addVoice(voiceSinus);
}

void Synth::stop()
{
    // Arrêt demandé de toutes les voix
    SoundEngine::stopAllVoices();
}

void Synth::setGain(double gain)
{
    // Modification du gain
    m_gain = gain;
    SoundEngine::setGain(gain);
}
void Synth::setReverb(int level, int size, int width, int damping)
{
    // Mise à jour de la réverbération
    _mutexReverb.lock();
    _reverb.setEffectMix((double)level / 100.);
    _reverb.setRoomSize((double)size / 100.);
    _reverb.setWidth((double)width / 100.);
    _reverb.setDamping((double)damping / 100.);
    _mutexReverb.unlock();
}
void Synth::setChorus(int level, int depth, int frequency)
{
    // Mise à jour du chorus
    m_choLevel = level;
    m_choDepth = depth;
    m_choFrequency = frequency;
    SoundEngine::setChorus(level, depth, frequency);
}
void Synth::setGainSample(int gain)
{
    // Modification du gain des samples
    SoundEngine::setGainSample(gain);
}
void Synth::setStereo(bool isStereo)
{
    // Modification lecture mono ou stereo
    SoundEngine::setStereo(isStereo);
}
void Synth::setLoopEnabled(bool isEnabled)
{
    // Modification lecture en boucle ou non
    SoundEngine::setLoopEnabled(isEnabled);
}
void Synth::setSinusEnabled(bool isEnabled)
{
    // Modification lecture avec sinus non
    SoundEngine::setSinusEnabled(isEnabled);
}
void Synth::setStartLoop(int startLoop, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    SoundEngine::setStartLoop(startLoop, repercute);
}
void Synth::setEndLoop(int endLoop, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    SoundEngine::setEndLoop(endLoop, repercute);
}
void Synth::setPitchCorrection(int correction, bool repercute)
{
    // mise à jour voix -1 et -2 si répercussion
    SoundEngine::setPitchCorrection(correction, repercute);
}
void Synth::setRootKey(int rootKey)
{
    // mise à jour voix -3 (sinus)
    SoundEngine::setRootKey(rootKey);
}

void Synth::emitCurrentPosChanged(int pos)
{
    this->currentPosChanged(pos);
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
