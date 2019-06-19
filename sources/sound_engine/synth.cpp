/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "synth.h"
#include <QThread>
#include <QFile>
#include "contextmanager.h"
#include "soundfontmanager.h"

// Constructeur, destructeur
Synth::Synth(ConfManager *configuration) : QObject(nullptr),
    _sf2(SoundfontManager::getInstance()),
    m_gain(0),
    m_choLevel(0), m_choDepth(0), m_choFrequency(0),
    m_clipCoef(1),
    m_recordFile(nullptr),
    m_isRecording(true),
    _fTmpSumRev1(nullptr),
    _fTmpSumRev2(nullptr),
    _dataWav(nullptr)
{

    // Création des buffers et sound engines
    _bufferSize = 2 * configuration->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 512).toInt();
    createSoundEnginesAndBuffers();
}

Synth::~Synth()
{
    destroySoundEnginesAndBuffers();
}

void Synth::destroySoundEnginesAndBuffers()
{
    // Arrêt des soundEngines
    for (int i = 0; i < _soundEngines.size(); i++)
        _soundEngines.at(i)->stop();

    // Arrêt des threads
    for (int i = 0; i < _soundEngines.size(); i++)
        _soundEngines.at(i)->thread()->quit();

    // Suppression des sound engines et des threads
    while (_soundEngines.size())
    {
        QThread * thread = _soundEngines.last()->thread();
        thread->wait(50);
        delete _soundEngines.takeLast();
        delete thread;
    }

    delete [] _fTmpSumRev1;
    delete [] _fTmpSumRev2;
    delete [] _dataWav;
}

void Synth::createSoundEnginesAndBuffers()
{
    _fTmpSumRev1 = new float [4 * _bufferSize];
    _fTmpSumRev2 = new float [4 * _bufferSize];
    _dataWav = new float[8 * _bufferSize];

    int nbEngines = qMax(QThread::idealThreadCount() - 2, 1);
    for (int i = 0; i < nbEngines; i++)
    {
        SoundEngine * soundEngine = new SoundEngine(_bufferSize);
        connect(soundEngine, SIGNAL(readFinished()), this, SIGNAL(readFinished()));
        soundEngine->moveToThread(new QThread());
        soundEngine->thread()->start(QThread::TimeCriticalPriority);
        QMetaObject::invokeMethod(soundEngine, "start");
        _soundEngines << soundEngine;
    }
}

// signal de lecture ou de fin
void Synth::play(int type, int idSf2, int idElt, int note, int velocity, VoiceParam * voiceParamTmp)
{
    play_sub(type, idSf2, idElt, note, velocity, voiceParamTmp);

    if (velocity > 0)
    {
        // Synchronize new voices
        _mutexSynchro.lock();
        SoundEngine::syncNewVoices();
        _mutexSynchro.unlock();
    }
}

void Synth::play_sub(int type, int idSf2, int idElt, int note, int velocity, VoiceParam * voiceParamTmp)
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
        VoiceParam * voiceParam = new VoiceParam(_sf2, idSmpl, voiceParamTmp);

        if (note < 0)
        {
            // Temps de fin, exclusive class et balance
            voiceParam->volReleaseTime = 0.2;
            voiceParam->exclusiveClass = note;
            SFSampleLink typeLien = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
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
        Voice * voiceTmp = new Voice(_sf2->getData(idSmpl, champ_sampleData32),
                                     _sf2->get(idSmpl, champ_dwSampleRate).dwValue,
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
            connect(voiceTmp, SIGNAL(currentPosChanged(int)), this, SIGNAL(currentPosChanged(int)));

            // Lien ?
            SFSampleLink typeLien = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
            if (typeLien != monoSample && typeLien != RomMonoSample)
                this->play(0, idSf2, _sf2->get(idSmpl, champ_wSampleLink).wValue, -2, 127);
        }
    }break;
    case 1:{ // instrument
        // Parcours de tous les samples liés
        EltID idInstSmpl(elementInstSmpl, idSf2, idElt, 0, 0);
        EltID idInst = idInstSmpl;
        idInst.typeElement = elementInst;
        rangesType defaultKeyRange, defaultVelRange;
        if (_sf2->isSet(idInst, champ_keyRange))
            defaultKeyRange = _sf2->get(idInst, champ_keyRange).rValue;
        else
        {
            defaultKeyRange.byLo = 0;
            defaultKeyRange.byHi = 127;
        }
        if (_sf2->isSet(idInst, champ_velRange))
            defaultVelRange = _sf2->get(idInst, champ_velRange).rValue;
        else
        {
            defaultVelRange.byLo = 0;
            defaultVelRange.byHi = 127;
        }
        int keyMin, keyMax, velMin, velMax;
        rangesType rangeTmp;
        foreach (int i, _sf2->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = i;

            // Skip muted divisions
            if (_sf2->get(idInstSmpl, champ_mute).bValue > 0)
                continue;

            if (_sf2->isSet(idInstSmpl, champ_keyRange))
            {
                rangeTmp = _sf2->get(idInstSmpl, champ_keyRange).rValue;
                keyMin = rangeTmp.byLo;
                keyMax = rangeTmp.byHi;
            }
            else
            {
                keyMin = defaultKeyRange.byLo;
                keyMax = defaultKeyRange.byHi;
            }
            if (_sf2->isSet(idInstSmpl, champ_velRange))
            {
                rangeTmp = _sf2->get(idInstSmpl, champ_velRange).rValue;
                velMin = rangeTmp.byLo;
                velMax = rangeTmp.byHi;
            }
            else
            {
                velMin = defaultVelRange.byLo;
                velMax = defaultVelRange.byHi;
            }
            if (keyMin <= note && keyMax >= note && velMin <= velocity && velMax >= velocity)
            {
                // Récupération des paramètres et lecture du sample associé
                VoiceParam * voiceParam = new VoiceParam(_sf2, idInstSmpl, voiceParamTmp);
                this->play_sub(0, idSf2, _sf2->get(idInstSmpl, champ_sampleID).wValue,
                               note, velocity, voiceParam);
                delete voiceParam;
            }
        }
    }break;
    case 2:{ // preset
        // Parcours de tous les instruments liés
        EltID idPrstInst(elementPrstInst, idSf2, idElt, 0, 0);
        EltID idPrst = idPrstInst;
        idPrst.typeElement = elementPrst;
        rangesType defaultKeyRange, defaultVelRange;
        if (_sf2->isSet(idPrst, champ_keyRange))
            defaultKeyRange = _sf2->get(idPrst, champ_keyRange).rValue;
        else
        {
            defaultKeyRange.byLo = 0;
            defaultKeyRange.byHi = 127;
        }
        if (_sf2->isSet(idPrst, champ_velRange))
            defaultVelRange = _sf2->get(idPrst, champ_velRange).rValue;
        else
        {
            defaultVelRange.byLo = 0;
            defaultVelRange.byHi = 127;
        }
        int keyMin, keyMax, velMin, velMax;
        rangesType rangeTmp;
        foreach (int i, _sf2->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = i;

            // Skip muted divisions
            if (_sf2->get(idPrstInst, champ_mute).bValue > 0)
                continue;

            if (_sf2->isSet(idPrstInst, champ_keyRange))
            {
                rangeTmp = _sf2->get(idPrstInst, champ_keyRange).rValue;
                keyMin = rangeTmp.byLo;
                keyMax = rangeTmp.byHi;
            }
            else
            {
                keyMin = defaultKeyRange.byLo;
                keyMax = defaultKeyRange.byHi;
            }
            if (_sf2->isSet(idPrstInst, champ_velRange))
            {
                rangeTmp = _sf2->get(idPrstInst, champ_velRange).rValue;
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
                VoiceParam * voiceParam = new VoiceParam(_sf2, idPrstInst);
                this->play_sub(1, idSf2, _sf2->get(idPrstInst, champ_instrument).wValue,
                               note, velocity, voiceParam);
                delete voiceParam;
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

void Synth::stop()
{
    // Arrêt demandé de toutes les voix
    SoundEngine::stopAllVoices();
}

void Synth::updateConfiguration()
{
    this->stop();

    // Update chorus
    m_choLevel = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt();
    m_choDepth = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt();
    m_choFrequency = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt();
    SoundEngine::setChorus(m_choLevel, m_choDepth, m_choFrequency);

    // Update reverb
    double revLevel = (double)ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt() / 100.;
    double revSize = (double)ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt() / 100.;
    double revWidth = (double)ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt() / 100.;
    double revDamping = (double)ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt() / 100.;

    _mutexReverb.lock();
    _reverb.setEffectMix(revLevel);
    _reverb.setRoomSize(revSize);
    _reverb.setWidth(revWidth);
    _reverb.setDamping(revDamping);
    _mutexReverb.unlock();

    // Update gain
    m_gain = ContextManager::configuration()->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    SoundEngine::setGain(m_gain);

    // Update buffer size
    int bufferSize = 2 * ContextManager::configuration()->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 512).toInt();
    if (_bufferSize != bufferSize)
    {
        _bufferSize = bufferSize;
        _mutexSynchro.lock();
        destroySoundEnginesAndBuffers();
        createSoundEnginesAndBuffers();
        _mutexSynchro.unlock();
    }
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

void Synth::setSinus(bool isOn, int rootKey)
{
    _sinus.setPitch(rootKey);
    if (isOn)
        _sinus.on();
    else
        _sinus.off();
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

void Synth::setFormat(AudioFormat format)
{
    // Mutex inutile : pas de génération de données lors de l'appel à setFormat
    m_format = format;

    // Réinitialisation
    this->stop();

    // Mise à jour échantillonnage
    _sinus.setSampleRate(format.sampleRate());
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
        m_recordFile = nullptr;
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
        m_recordStream.setDevice(nullptr);
        m_recordFile->close();
        delete m_recordFile;
        m_recordFile = nullptr;

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

void Synth::readData(float *data1, float *data2, qint64 maxlen)
{
    for (int i = 0; i < maxlen; i++)
        data1[i] = data2[i] = _fTmpSumRev1[i] = _fTmpSumRev2[i] = 0;

    // Fusion des sound engines
    _mutexSynchro.lock();
    for (int i = 0; i < _soundEngines.size(); i++)
        _soundEngines.at(i)->addData(data1, data2, _fTmpSumRev1, _fTmpSumRev2, maxlen);
    _mutexSynchro.unlock();

    // Application réverbération et addition
    _mutexReverb.lock();
    for (int i = 0; i < maxlen; i++)
    {
        data1[i] += _reverb.tick(_fTmpSumRev1[i], _fTmpSumRev2[i]);
        data2[i] += _reverb.lastOut(1);
    }
    _mutexReverb.unlock();

    // Ajout sinus de calibration
    _sinus.addData(data1, data2, maxlen);

    // Clipping
    clip(data1, data2, maxlen);

    // Enregistrement dans un fichier si demandé
    m_mutexRecord.lock();
    if (m_recordFile && m_isRecording)
    {
        // Entrelacement et écriture
        for (int i = 0; i < maxlen; i++)
        {
            _dataWav[2 * i + 1] = data1[i];
            _dataWav[2 * i]     = data2[i];
        }
        m_recordStream.writeRawData((char*)_dataWav, maxlen * 8);

        // Prise en compte de l'avance
        m_recordLength += maxlen * 8;
        emit(dataWritten(m_format.sampleRate(), maxlen));
    }
    m_mutexRecord.unlock();
}
