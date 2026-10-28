/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "synth.h"
#include <QThread>
#include "fastmaths.h"

Synth::Synth(ISynthSoundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts) : QObject(nullptr),
    _mutexSoundfonts(mutexSoundfonts),
    _core(soundfonts, this),
    _soundEngines(nullptr),
    _soundEngineCount(0),
    _bufferSize(0)
{
    connect(&_recorder, SIGNAL(dataWritten(quint32,quint32)), this, SIGNAL(dataWritten(quint32,quint32)));
}

Synth::~Synth()
{
    destroySoundEngines();
}

void Synth::destroySoundEngines()
{
    if (_soundEngineCount == 0)
        return;

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
}

void Synth::createSoundEngines()
{
    _recorder.setBufferSize(_bufferSize);
    _buses.reserve(_bufferSize);

    _soundEngineCount = qMax(QThread::idealThreadCount() - 1, 1);
    _soundEngines = new SoundEngine * [_soundEngineCount];
    //qWarning() << _soundEngineCount << "sound engines created";

    for (int i = 0; i < _soundEngineCount; i++)
    {
        SoundEngine * soundEngine = new SoundEngine(_bufferSize);
        soundEngine->moveToThread(new QThread());
        soundEngine->thread()->start(QThread::HighPriority);
        QMetaObject::invokeMethod(soundEngine, "start");
        _soundEngines[i] = soundEngine;
    }

    _core.getVoiceList()->setThreadCount(_soundEngineCount);
    SoundEngine::initialize(_core.getVoiceList());

    // Start producing data
    for (int i = 0; i < _soundEngineCount; i++)
        _soundEngines[i]->prepareData(_bufferSize);
}

int Synth::play(EltID id, int channel, int key, int velocity)
{
    //qWarning() << "PLAY on channel" << channel << "key" << key << "vel" << velocity << id.toString();

    // Possibly release voices
    if (velocity == 0)
    {
        _core.releaseVoices(id.indexSf2, id.indexElt, channel, key, false);
        return -1;
    }

    // A key is pressed, the soundfonts must not change while the voices are prepared
    QMutexLocker mutexLocker(_mutexSoundfonts);  // Will do nothing if _mutexSoundfonts is null
    int playingToken = -1;
    switch (id.typeElement)
    {
    case elementSmpl:
        playingToken = _core.playSample(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    case elementInst: case elementInstSmpl:
        _core.playInstrument(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    case elementPrst: case elementPrstInst:
        _core.playPreset(id.indexSf2, id.indexElt, channel, key, velocity);
        break;
    default:
        break;
    }

    return playingToken;
}

void Synth::stop(bool allChannels, bool fast)
{
    _core.stop(allChannels, fast);
}

void Synth::configure(const SynthCoreConfig &configuration)
{
    _core.configure(configuration);
}

void Synth::setIMidiValues(IMidiValues * midiValues)
{
    _core.setMidiValues(midiValues);
}

void Synth::setSampleGain(double gain)
{
    _core.setSampleGain(gain);
}

void Synth::setStereo(bool isStereoEnabled)
{
    _core.setStereo(isStereoEnabled);
}

bool Synth::isStereoEnabled()
{
    return _core.isStereoEnabled();
}

void Synth::setLoopEnabled(bool isEnabled)
{
    _core.setLoopEnabled(isEnabled);
}

void Synth::setSinus(bool isOn, int rootKey)
{
    _sinus.setPitch(rootKey);
    if (isOn)
        _sinus.on();
    else
        _sinus.off();
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

void Synth::setStartLoop(quint32 startLoop, bool withLinkedSample)
{
    _core.setStartLoop(startLoop, withLinkedSample);
}

void Synth::setEndLoop(quint32 endLoop, bool withLinkedSample)
{
    _core.setEndLoop(endLoop, withLinkedSample);
}

void Synth::setPitchCorrection(qint16 correction, bool withLinkedSample)
{
    _core.setPitchCorrection(correction, withLinkedSample);
}

void Synth::startNewRecord(QString fileName)
{
    _recorder.startNewRecord(fileName, _core.getSampleRate());
}

void Synth::endRecord()
{
    _recorder.endRecord();
}

void Synth::pause(bool isOn)
{
    _recorder.pause(isOn);
}

bool Synth::processKey(int channel, int key, int vel)
{
    Q_UNUSED(channel)
    Q_UNUSED(key)
    Q_UNUSED(vel)
    return false;
}

bool Synth::processPolyPressureChanged(int channel, int key, int pressure)
{
    _core.processPolyPressureChanged(channel, key, pressure);
    return false;
}

bool Synth::processMonoPressureChanged(int channel, int value)
{
    _core.processMonoPressureChanged(channel, value);
    return false;
}

bool Synth::processControllerChanged(int channel, int num, int value)
{
    _core.processControllerChanged(channel, num, value);
    return false;
}

bool Synth::processBendChanged(int channel, float value)
{
    _core.processBendChanged(channel, value);
    return false;
}

bool Synth::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    _core.processRPNChanged(channel, parameter, value, isRegistered, trigger);
    return false;
}

bool Synth::processProgramChanged(int channel, quint16 bank, quint8 preset)
{
    Q_UNUSED(channel)
    Q_UNUSED(bank)
    Q_UNUSED(preset)
    return false;
}

void Synth::setSampleRateAndBufferSize(quint32 sampleRate, quint32 bufferSize)
{
    // Mutex not mandatory: no data generation when "setFormat" is called
    _core.setSampleRate(sampleRate); // Voices are stopped
    _sinus.setSampleRate(sampleRate);
    _eq.setSampleRate(sampleRate);

    // Update buffer size
    bufferSize *= 2;
    if (_bufferSize != bufferSize)
    {
        _bufferSize = bufferSize;
        destroySoundEngines();
        createSoundEngines();
    }
}

void Synth::readData(float * dataL, float * dataR, quint32 maxlen)
{
    if (_soundEngineCount == 0)
    {
        memset(dataL, 0, maxlen * sizeof(float));
        memset(dataR, 0, maxlen * sizeof(float));
        return;
    }

    // No more voice processing (will not stop immediately)
    VoiceList * voices = _core.getVoiceList();
    voices->endComputation();
    for (int i = 0; i < _soundEngineCount; ++i)
        _soundEngines[i]->endCurrentProcessing();

    // Get the data of all sound engines, even if they are still working
    _soundEngines[0]->copyData(_buses, maxlen);
    for (int i = 1; i < _soundEngineCount; i++)
        _soundEngines[i]->addData(_buses, maxlen);

    // Tell the sound engines to restart (or continue) the processing
    voices->prepareComputation();
    for (int i = 0; i < _soundEngineCount; ++i)
        _soundEngines[i]->prepareData(maxlen);

    // Apply chorus / reverb on the gathered data
    _core.applyEffects(dataL, dataR, _buses, maxlen);

    // EQ filter (live preview of filtered samples)
    _eq.filterData(dataL, dataR, maxlen);

    // Add calibrating sinus
    _sinus.addData(dataL, dataR, maxlen);

    // Clipping
    FastMaths::clamp(dataL, maxlen);
    FastMaths::clamp(dataR, maxlen);

    // Possible recording
    _recorder.process(dataL, dataR, maxlen);
}

void Synth::waitForComputation()
{
    for (int i = 0; i < _soundEngineCount; i++)
        while (!_soundEngines[i]->isIdle())
            QThread::yieldCurrentThread();
}

void Synth::onSamplePositionChanged(uint32_t position)
{
    emit currentPosChanged(position);
}

void Synth::onSampleReadFinished(int token)
{
    emit readFinished(token);
}
