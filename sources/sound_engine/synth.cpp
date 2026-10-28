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
#include "synthmath.h"

Synth::Synth(ISynthSoundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts) : QObject(nullptr),
    _mutexSoundfonts(mutexSoundfonts),
    _core(soundfonts, this),
    _soundEngines(nullptr),
    _soundEngineCount(0),
    _bufferSize(0),
    _preparedLength(0),
    _outputL(nullptr),
    _outputR(nullptr),
    _outputCapacity(0),
    _outputPosition(0),
    _outputAvailable(0)
{
    connect(&_recorder, SIGNAL(dataWritten(quint32,quint32)), this, SIGNAL(dataWritten(quint32,quint32)));
}

Synth::~Synth()
{
    destroySoundEngines();
    FastMaths::deleteAlignedBuffer(_outputL);
    FastMaths::deleteAlignedBuffer(_outputR);
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
    _core.setMaxBlockSize(_bufferSize);

    // Output: the frames not returned (less than a chunk if the length asked does not change) followed by the
    // chunks computed, written at an aligned position
    FastMaths::deleteAlignedBuffer(_outputL);
    FastMaths::deleteAlignedBuffer(_outputR);
    _outputCapacity = _buses.capacity() + COMPUTATION_CHUNK + BUFFER_ALIGNMENT / sizeof(float);
    _outputL = FastMaths::newAlignedBuffer(_outputCapacity);
    _outputR = FastMaths::newAlignedBuffer(_outputCapacity);
    _outputPosition = 0;
    _outputAvailable = 0;
    _preparedLength = 0;

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

    // The sound engines start computing at the first call of readData, which knows the length to prepare
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
    _outputAvailable = 0; // Frames computed with the previous sample rate

    // Update buffer size
    bufferSize *= 2;
    if (_bufferSize != bufferSize)
    {
        _bufferSize = bufferSize;
        destroySoundEngines();
        createSoundEngines();
    }
}

void Synth::readData(float * dataL, float * dataR, quint32 len)
{
    if (_soundEngineCount == 0)
    {
        memset(dataL, 0, len * sizeof(float));
        memset(dataR, 0, len * sizeof(float));
        return;
    }

    DenormalsDisabled denormalsDisabled;

    // Chunks computed by the sound engines since the previous call
    VoiceList * voices = _core.getVoiceList();
    quint32 computedLength = _preparedLength;
    if (computedLength > 0)
    {
        // No more voice processing (will not stop immediately)
        voices->endComputation();
        for (int i = 0; i < _soundEngineCount; ++i)
            _soundEngines[i]->endCurrentProcessing();

        // Get the data of all sound engines, even if they are still working
        _soundEngines[0]->copyData(_buses, computedLength);
        for (int i = 1; i < _soundEngineCount; i++)
            _soundEngines[i]->addData(_buses, computedLength);
    }

    // Frames that will remain after this call, the chunks missing now being silent (first call or longer length)
    quint32 available = _outputAvailable + computedLength;
    quint32 remaining = len > available ? SynthMath::roundToChunks(len - available) + available - len : available - len;

    // Tell the sound engines to restart (or continue) the processing with the chunks missing for a next call of
    // the same length, which must fit after the remaining frames
    _preparedLength = 0;
    if (remaining < len)
    {
        quint32 alignment = BUFFER_ALIGNMENT / sizeof(float);
        quint32 freeLength = _outputCapacity - (remaining + alignment - 1) / alignment * alignment;
        _preparedLength = qMin(qMin(SynthMath::roundToChunks(len - remaining), _buses.capacity()),
                               (freeLength >> COMPUTATION_CHUNK_SHIFT) << COMPUTATION_CHUNK_SHIFT);
    }
    if (_preparedLength > 0)
    {
        voices->prepareComputation();
        for (int i = 0; i < _soundEngineCount; ++i)
            _soundEngines[i]->prepareData(_preparedLength);
    }

    // Effects for the data gathered
    if (computedLength > 0)
        appendOutput(computedLength);

    // Return the data, possibly completed by silent chunks
    quint32 done = 0;
    while (true)
    {
        quint32 count = qMin(len - done, _outputAvailable);
        memcpy(&dataL[done], &_outputL[_outputPosition], count * sizeof(float));
        memcpy(&dataR[done], &_outputR[_outputPosition], count * sizeof(float));
        _outputPosition += count;
        _outputAvailable -= count;
        done += count;
        if (done == len)
            break;

        quint32 chunkLength = qMin(SynthMath::roundToChunks(len - done), _buses.capacity());
        _buses.clear(chunkLength);
        appendOutput(chunkLength);
    }

    // Possible recording
    _recorder.process(dataL, dataR, len);
}

void Synth::appendOutput(quint32 len)
{
    // The frames not returned are moved just before an aligned position, where the new frames are written
    quint32 alignment = BUFFER_ALIGNMENT / sizeof(float);
    quint32 position = (_outputAvailable + alignment - 1) / alignment * alignment;
    if (_outputPosition + _outputAvailable != position)
    {
        memmove(&_outputL[position - _outputAvailable], &_outputL[_outputPosition], _outputAvailable * sizeof(float));
        memmove(&_outputR[position - _outputAvailable], &_outputR[_outputPosition], _outputAvailable * sizeof(float));
        _outputPosition = position - _outputAvailable;
    }
    float * dataL = &_outputL[position];
    float * dataR = &_outputR[position];

    // Apply chorus / reverb on the gathered data
    _core.applyEffects(dataL, dataR, _buses, len);

    // EQ filter (live preview of filtered samples)
    _eq.filterData(dataL, dataR, len);

    // Add calibrating sinus
    _sinus.addData(dataL, dataR, len);

    // Clipping
    FastMaths::clamp(dataL, len);
    FastMaths::clamp(dataR, len);

    _outputAvailable += len;
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
