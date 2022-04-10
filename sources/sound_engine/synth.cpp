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

int Synth::s_sampleVoiceTokenCounter = 0;
const int Synth::MAX_NUMBER_OF_VOICES_TO_ADD = 128;

// Constructeur, destructeur
Synth::Synth(ConfManager *configuration) : QObject(nullptr),
    _sf2(SoundfontManager::getInstance()),
    _soundEngines(nullptr),
    _soundEngineCount(0),
    _voicesToAdd(new Voice * [MAX_NUMBER_OF_VOICES_TO_ADD]),
    _numberOfVoicesToAdd(0),
    _gain(0),
    _choLevel(0), _choDepth(0), _choFrequency(0),
    _recordFile(nullptr),
    _isRecording(false),
    _isWritingInStream(0),
    _dataWav(nullptr),
    _bufferSize(0),
    _configuration(configuration)
{
    // Initial configuration
    updateConfiguration();
}

Synth::~Synth()
{
    destroySoundEnginesAndBuffers();
    delete [] _voicesToAdd;
}

void Synth::destroySoundEnginesAndBuffers()
{
    // Stop sound engines
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->stop();

    // Stop threads
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->thread()->quit();

    // Delete sound engines and threads
    for (int i = 0; i < _soundEngineCount; i++)
    {
        QThread * thread = _soundEngines[i]->thread();
        thread->wait(50);
        delete _soundEngines[i];
        delete thread;
    }
    delete [] _soundEngines;

    delete [] _dataWav;
}

void Synth::createSoundEnginesAndBuffers()
{
    _dataWav = new float[8 * _bufferSize];

    _soundEngineCount = qMax(QThread::idealThreadCount() - 2, 1);
    _soundEngineCount = 3;
    _soundEngines = new SoundEngine * [_soundEngineCount];
    for (int i = 0; i < _soundEngineCount; i++)
    {
        SoundEngine * soundEngine = new SoundEngine(&_semRunningSoundEngines, _bufferSize);
        connect(soundEngine, SIGNAL(readFinished(int)), this, SIGNAL(readFinished(int)));
        soundEngine->moveToThread(new QThread());
        soundEngine->thread()->start(QThread::TimeCriticalPriority);
        QMetaObject::invokeMethod(soundEngine, "start");
        _soundEngines[i] = soundEngine;
    }
    SoundEngine::setInstanceList(_soundEngines, _soundEngineCount);
}

int Synth::play(EltID id, int channel, int key, int velocity)
{
    //qWarning() << "PLAY on channel" << channel << "key" << key << "vel" << velocity << id.toString();

    // Reset the number of voices to add
    _numberOfVoicesToAdd = 0;

    if (velocity == 0)
    {
        // Release of voices
        SoundEngine::releaseVoices(id.indexSf2, id.indexElt, channel, key);
        return -1;
    }

    // A key is pressed
    int playingToken = -1;
    switch (id.typeElement)
    {
    case elementSmpl:
        playingToken = playSmpl(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    case elementInst: case elementInstSmpl:
        playInst(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    case elementPrst: case elementPrstInst:
        playPrst(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    default:
        return -1;
    }

    // Add all voices to the sound engines
    SoundEngine::addVoice(_voicesToAdd, _numberOfVoicesToAdd);

    return playingToken;
}

void Synth::playPrst(int idSf2, int idElt, int channel, int key, int velocity)
{
    // Default preset range
    EltID idPrst(elementPrst, idSf2, idElt, 0, 0);
    RangesType defaultKeyRange, defaultVelRange;
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

    // Browse the ranges of all linked instruments
    EltID idPrstInst(elementPrstInst, idSf2, idElt, 0, 0);
    int keyMin, keyMax, velMin, velMax;
    RangesType rangeTmp;
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

        // Check {key, vel} is in the division and go inside the instruments
        if (keyMin <= key && key <= keyMax && velMin <= velocity && velocity <= velMax)
            this->playInst(idSf2, _sf2->get(idPrstInst, champ_instrument).wValue, channel, key, velocity, idPrstInst);
    }
}

void Synth::playInst(int idSf2, int idElt, int channel, int key, int velocity, EltID idPrstInst)
{
    // Default instrument range
    EltID idInst(elementInst, idSf2, idElt, 0, 0);
    idInst.typeElement = elementInst;
    RangesType defaultKeyRange, defaultVelRange;
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

    // Browse the range of all linked samples
    EltID idInstSmpl(elementInstSmpl, idSf2, idElt, 0, 0);
    int keyMin, keyMax, velMin, velMax;
    RangesType rangeTmp;
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

        // Check {key, vel} is in the division and go inside the samples
        if (keyMin <= key && key <= keyMax && velMin <= velocity && velocity <= velMax)
            this->playSmpl(idSf2, _sf2->get(idInstSmpl, champ_sampleID).wValue, channel, key, velocity, idInstSmpl, idPrstInst);
    }
}

int Synth::playSmpl(int idSf2, int idElt, int channel, int key, int velocity, EltID idInstSmpl, EltID idPrstInst)
{
    EltID idSmpl(elementSmpl, idSf2, idElt, 0, 0);

    // Prepare the parameters for the voice
    VoiceParam * voiceParam = new VoiceParam(idPrstInst, idInstSmpl, idSmpl, channel, key, velocity);

    if (key < 0) // Smpl area
        voiceParam->prepareForSmpl(key, _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue);

    // Create a voice
    int currentToken = s_sampleVoiceTokenCounter++;
    Voice * voiceTmp = new Voice(_sf2->getData(idSmpl, champ_sampleData32),
                                 _sf2->get(idSmpl, champ_dwSampleRate).dwValue,
                                 _format.sampleRate(), voiceParam, currentToken);

    // Initialize chorus and gain
    if (key < 0)
        voiceTmp->setChorus(0, 0, 0);
    else
    {
        voiceTmp->setChorus(_choLevel, _choDepth, _choFrequency);
        voiceTmp->setGain(_gain);
    }

    // Store the voice in the list of voices to add
    if (_numberOfVoicesToAdd < MAX_NUMBER_OF_VOICES_TO_ADD)
        _voicesToAdd[_numberOfVoicesToAdd++] = voiceTmp;
    else
    {
        delete voiceTmp;
        return currentToken;
    }

    if (key == -1) // -2 is the linked sample
    {
        // Position in the graphics
        connect(voiceTmp, SIGNAL(currentPosChanged(quint32)), this, SIGNAL(currentPosChanged(quint32)));

        // Stereo link?
        SFSampleLink typeLien = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLien != monoSample && typeLien != RomMonoSample)
            this->playSmpl(idSf2, _sf2->get(idSmpl, champ_wSampleLink).wValue, channel, -2, 127);
    }

    return currentToken;
}

void Synth::stop(bool allChannels)
{
    // Stop required for all voices
    SoundEngine::stopAllVoices(allChannels);
}

void Synth::updateConfiguration()
{
    this->stop(true);

    // Update chorus
    _choLevel = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_level", 0).toInt();
    _choDepth = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_depth", 0).toInt();
    _choFrequency = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "cho_frequency", 0).toInt();
    SoundEngine::setChorus(_choLevel, _choDepth, _choFrequency);

    // Update reverb
    double revLevel = 0.01 * _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_level", 0).toInt();
    double revSize = 0.01 * _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_size", 0).toInt();
    double revWidth = 0.01 * _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_width", 0).toInt();
    double revDamping = 0.01 * _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "rev_damping", 0).toInt();

    _mutexReverb.lock();
    _reverbOn = revLevel > 0.001;
    _reverb.setEffectMix(revLevel);
    _reverb.setRoomSize(revSize);
    _reverb.setWidth(revWidth);
    _reverb.setDamping(revDamping);
    _mutexReverb.unlock();

    // Update gain, tuning fork and temperament
    _gain = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "gain", 0).toInt();
    SoundEngine::setGain(_gain);

    int tuningFork = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "tuning_fork", 440).toInt();
    Voice::setTuningFork(tuningFork);

    QStringList temperamentArgs = _configuration->getValue(ConfManager::SECTION_SOUND_ENGINE, "temperament", "").toString().split(",");
    if (temperamentArgs.count() == 14)
    {
        float temperament[12];
        for (int i = 1; i < 13; i++)
            temperament[i - 1] = temperamentArgs[i].toFloat();
        int temperamentRelativeKey = temperamentArgs[13].toInt() % 12;
        Voice::setTemperament(temperament, temperamentRelativeKey);
    }
    else
    {
        float temperament[12];
        memset(temperament, 0, 12 * sizeof(float));
        Voice::setTemperament(temperament, 0);
    }

    // Update buffer size
    quint32 bufferSize = 2 * _configuration->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 512).toUInt();
    if (_bufferSize != bufferSize)
    {
        _bufferSize = bufferSize;
        destroySoundEnginesAndBuffers();
        createSoundEnginesAndBuffers();
    }
}

void Synth::setGainSample(int gain)
{
    // Modification du gain des samples
    SoundEngine::setGainSample(gain);
}

void Synth::setStereo(bool isStereo)
{
    // Enable the stereo when playing a sample
    SoundEngine::setStereo(isStereo);
}

bool Synth::isStereo()
{
    return SoundEngine::isStereo();
}

void Synth::setLoopEnabled(bool isEnabled)
{
    // Enable the loop when playing a sample
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

void Synth::setStartLoop(quint32 startLoop, bool repercute)
{
    // Update voices -1 and -2 if repercussion
    SoundEngine::setStartLoop(startLoop, repercute);
}

void Synth::setEndLoop(quint32 endLoop, bool repercute)
{
    // Update voices -1 and -2  if repercussion
    SoundEngine::setEndLoop(endLoop, repercute);
}

void Synth::setPitchCorrection(qint16 correction, bool repercute)
{
    // Update voices -1 and -2  if repercussion
    SoundEngine::setPitchCorrection(correction, repercute);
}

void Synth::activateSmplEq(bool isActivated)
{
    if (isActivated)
        _eq.on();
    else
        _eq.off();
}

void Synth::setSmplEqValues(int values[10])
{
    _eq.setValues(values);
}

void Synth::setFormat(AudioFormat format)
{
    // Mutex not mandatory: no data generation when "setFormat" is called
    _format = format;

    // Reset
    this->stop(true);

    // Sample rate update
    _sinus.setSampleRate(format.sampleRate());
    _eq.setSampleRate(format.sampleRate());
}

void Synth::startNewRecord(QString fileName)
{
    if (_recordFile)
        this->endRecord();

    _recordFile = new QFile(fileName);
    if (_recordFile->open(QIODevice::WriteOnly))
    {
        // Create header
        quint32 dwTemp = 0;
        quint16 wTemp;
        _recordStream.setDevice(_recordFile);
        _recordStream.setByteOrder(QDataStream::LittleEndian);
        _recordLength = 0;

        // Write it
        _recordStream.writeRawData("RIFF", 4);
        _recordStream << static_cast<quint32>(_recordLength + 18 + 4 + 8 + 8);
        _recordStream.writeRawData("WAVE", 4);

        ///////////// BLOC FMT /////////////
        _recordStream.writeRawData("fmt ", 4);
        dwTemp = 18;
        _recordStream << dwTemp;
        // Compression code
        wTemp = 3;
        _recordStream << wTemp;
        // Number of channels
        wTemp = 2;
        _recordStream << wTemp;
        // Sample rate
        dwTemp = _format.sampleRate();
        _recordStream << dwTemp;
        // Average byte per second
        dwTemp *= 2 * 4;
        _recordStream << dwTemp;
        // Block align
        wTemp = 2 * 4;
        _recordStream << wTemp;
        // Significants bits per smpl
        _recordStream << static_cast<quint16>(32);
        // Extra format bytes
        wTemp = 0;
        _recordStream << wTemp;

        ///////////// BLOC DATA /////////////
        _recordStream.writeRawData("data", 4);
        _recordStream << _recordLength;

        _isRecording = true;
    }
    else
    {
        delete _recordFile;
        _recordFile = nullptr;
    }
}

void Synth::endRecord()
{
    _isRecording = false;

    // Possibly wait if another thread is writing
    while (!_isWritingInStream.testAndSetRelaxed(0, 1))
        QThread::msleep(5);

    if (_recordFile)
    {
        // Adjust file dimensions
        _recordFile->seek(4);
        _recordStream << static_cast<quint32>(_recordLength + 18 + 4 + 8 + 8);
        _recordFile->seek(42);
        _recordStream << _recordLength;

        // Close file
        _recordStream.setDevice(nullptr);
        _recordFile->close();
        delete _recordFile;
        _recordFile = nullptr;
    }

    // We are not writing anymore
    _isWritingInStream.storeRelaxed(0);
}

void Synth::pause(bool isOn)
{
    _isRecording = !isOn;
}

void Synth::readData(float *dataL, float *dataR, quint32 maxlen)
{
    memset(dataL, 0, maxlen * sizeof(float));
    memset(dataR, 0, maxlen * sizeof(float));

    // Wake up the sound engines
    for (int i = 0; i < _soundEngineCount; ++i)
        _soundEngines[i]->prepareData(maxlen);
    _semRunningSoundEngines.acquire(_soundEngineCount);

    // Get the reverberated part of the sound
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->addRevData(dataL, dataR, maxlen);
    if (_reverbOn)
    {
        // Apply reverb on the current data
        _mutexReverb.lock();
        for (quint32 i = 0; i < maxlen; i++)
        {
            dataL[i] = static_cast<float>(
                        _reverb.tick(static_cast<double>(dataL[i]), static_cast<double>(dataR[i])));
            dataR[i] = static_cast<float>(_reverb.lastOut(1));
        }
        _mutexReverb.unlock();
    }

    // Add the non-reverberated part of the sound
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->addNonRevData(dataL, dataR, maxlen);

    // EQ filter (live preview of filtered samples)
    _eq.filterData(dataL, dataR, maxlen);

    // Add calibrating sinus
    _sinus.addData(dataL, dataR, maxlen);

    // Clipping
    for (unsigned int i = 0; i < maxlen; ++i)
    {
        if (dataL[i] > 1.0f)
            dataL[i] = 1.0f;
        else if (dataL[i] < -1.0f)
            dataL[i] = -1.0f;
        if (dataR[i] > 1.0f)
            dataR[i] = 1.0f;
        else if (dataR[i] < -1.0f)
            dataR[i] = -1.0f;
    }

    // Possibly record in a file
    if (_isRecording)
    {
        // If someone else is writing, do nothing
        if (_recordFile && _isWritingInStream.testAndSetRelaxed(0, 1))
        {
            // Interleave and write
            for (quint32 i = 0; i < maxlen; i++)
            {
                _dataWav[2 * i + 1] = dataL[i];
                _dataWav[2 * i] = dataR[i];
            }
            _recordStream.writeRawData(reinterpret_cast<char*>(_dataWav), static_cast<int>(maxlen * 8));

            // Prise en compte de l'avance
            _recordLength += maxlen * 8;
            emit(dataWritten(_format.sampleRate(), maxlen));

            // We are not writing anymore
            _isWritingInStream.storeRelaxed(0);
        }
    }
}
