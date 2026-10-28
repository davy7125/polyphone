/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "offlinerenderer.h"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QRegularExpression>
#include <algorithm>
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "synth.h"
#include "synthcore.h"
#include "midiprocessor.h"
#include "synthmidi.h"
#include "basetypes.h"

OfflineRenderer::OfflineRenderer() :
    _midiProcessor(new MidiProcessor()),
    _midiProcessorOwned(true),
    _sampleRate(48000),
    _blockSize(COMPUTATION_CHUNK),
    _duration(0),
    _masterGain(0),
    _referencePitch(4400),
    _sampleLoop(true),
    _sampleStereo(false),
    _sampleGain(0)
{
    for (int i = 0; i < 4; i++)
        _reverb[i] = 0;
    for (int i = 0; i < 3; i++)
        _chorus[i] = 0;

    // The parser emits this signal for each modulator source that is a MIDI controller
    _modulatorConnection = QObject::connect(SoundfontManager::getInstance(), &SoundfontManager::inputModulatorChanged,
                                            [this](int controllerNumber, bool isBipolar, bool isDescending) {
        _midiProcessor->setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
    });
}

OfflineRenderer::~OfflineRenderer()
{
    QObject::disconnect(_modulatorConnection);
    if (_midiProcessorOwned)
        delete _midiProcessor;
}

bool OfflineRenderer::readEvents(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        _error = "cannot open \"" + filePath + "\"";
        return false;
    }

    // A line is either a setting or an event starting with its time in seconds
    QTextStream stream(&file);
    int line = 0;
    while (!stream.atEnd())
    {
        QString text = stream.readLine();
        line++;
        int commentPosition = text.indexOf('#');
        if (commentPosition >= 0)
            text.truncate(commentPosition);
        QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (words.isEmpty())
            continue;

        bool isTime;
        double time = words[0].toDouble(&isTime);
        if (isTime ? !readEvent(time, words.mid(1), line) : !readSetting(words, line))
            return false;
    }

    if (_duration <= 0)
    {
        _error = "the duration is missing";
        return false;
    }

    // Position of the events in frames, keeping the order of the file for a same position
    quint64 frameCount = static_cast<quint64>(qRound64(_duration * _sampleRate));
    for (int i = 0; i < _events.count(); i++)
    {
        _events[i].frame = static_cast<quint64>(qRound64(_events[i].time * _sampleRate));
        if (_events[i].frame >= frameCount)
        {
            _error = QString("line %1: the event is after the end").arg(_events[i].line);
            return false;
        }
    }
    std::stable_sort(_events.begin(), _events.end(), [](const Event &a, const Event &b) {
        return a.frame < b.frame;
    });

    return true;
}

bool OfflineRenderer::readSetting(QStringList words, int line)
{
    QString name = words.takeFirst();
    int value;
    float floatValue;
    if (name == "samplerate" && words.count() == 1)
    {
        if (!readInt(words[0], 8000, 192000, value, line))
            return false;
        _sampleRate = static_cast<quint32>(value);
    }
    else if (name == "block" && words.count() == 1)
    {
        // The voices compute their envelopes by chunks, a remainder would be silent
        if (!readInt(words[0], COMPUTATION_CHUNK, 16 * COMPUTATION_CHUNK, value, line))
            return false;
        if (value % COMPUTATION_CHUNK != 0)
        {
            _error = QString("line %1: the block size must be a multiple of %2").arg(line).arg(COMPUTATION_CHUNK);
            return false;
        }
        _blockSize = static_cast<quint32>(value);
    }
    else if (name == "duration" && words.count() == 1)
    {
        if (!readFloat(words[0], 0.001f, 3600.f, floatValue, line))
            return false;
        _duration = static_cast<double>(floatValue);
    }
    else if (name == "gain" && words.count() == 1)
    {
        if (!readFloat(words[0], -50.f, 50.f, floatValue, line))
            return false;
        _masterGain = static_cast<double>(floatValue);
    }
    else if (name == "tuning" && words.count() == 1)
    {
        // Reference pitch in Hz, stored in 0.1 Hz
        if (!readFloat(words[0], 400.f, 480.f, floatValue, line))
            return false;
        _referencePitch = qRound(10.f * floatValue);
    }
    else if (name == "reverb" && words.count() == 4)
    {
        for (int i = 0; i < 4; i++)
            if (!readInt(words[i], 0, 100, _reverb[i], line))
                return false;
    }
    else if (name == "chorus" && words.count() == 3)
    {
        for (int i = 0; i < 3; i++)
            if (!readInt(words[i], 0, 100, _chorus[i], line))
                return false;
    }
    else if ((name == "sampleloop" || name == "samplestereo") && words.count() == 1)
    {
        if (!readInt(words[0], 0, 1, value, line))
            return false;
        if (name == "sampleloop")
            _sampleLoop = (value == 1);
        else
            _sampleStereo = (value == 1);
    }
    else if (name == "samplegain" && words.count() == 1)
    {
        if (!readFloat(words[0], -50.f, 50.f, floatValue, line))
            return false;
        _sampleGain = static_cast<double>(floatValue);
    }
    else if (((name == "mute" && words.count() == 3) || (name == "alwaysplayed" && words.count() == 2)) &&
             (words[0] == "instrument" || words[0] == "preset"))
    {
        // A zone of an instrument or a preset muted, or an instrument or a preset always played
        ModelChange change;
        change.mute = (name == "mute");
        change.isPreset = (words[0] == "preset");
        change.zone = -1;
        change.line = line;
        if (!readInt(words[1], 0, 65535, change.index, line) ||
            (change.mute && !readInt(words[2], 0, 65535, change.zone, line)))
            return false;
        _modelChanges << change;
    }
    else
    {
        _error = QString("line %1: unknown setting \"%2\" or wrong number of values").arg(line).arg(name);
        return false;
    }

    return true;
}

bool OfflineRenderer::readEvent(double time, QStringList words, int line)
{
    if (time < 0)
    {
        _error = QString("line %1: negative time").arg(line);
        return false;
    }

    // Event name, then the channel and the values
    Event event;
    event.time = time;
    event.frame = 0;
    event.number = 0;
    event.value = 0;
    event.floatValue = 0;
    event.index = -1;
    event.line = line;

    QString name = words.isEmpty() ? "" : words.takeFirst();
    if (name == "instrument" || name == "sample")
    {
        // Played alone on the channel of the editor: index, key (-1: a sample at its root key) and velocity
        event.type = (name == "instrument" ? EVENT_INSTRUMENT : EVENT_SAMPLE);
        event.channel = -1;
        if (words.count() != 3)
        {
            _error = QString("line %1: 3 values expected after \"%2\"").arg(line).arg(name);
            return false;
        }
        if (!readInt(words[0], 0, 65535, event.index, line) || !readInt(words[1], -1, 127, event.number, line) ||
            !readInt(words[2], 0, 127, event.value, line))
            return false;
        _events << event;
        return true;
    }

    int expectedCount;
    if (name == "program")
    {
        event.type = EVENT_PROGRAM;
        expectedCount = 3;
    }
    else if (name == "on")
    {
        event.type = EVENT_NOTE_ON;
        expectedCount = 3;
    }
    else if (name == "off")
    {
        event.type = EVENT_NOTE_OFF;
        expectedCount = 2;
    }
    else if (name == "cc")
    {
        event.type = EVENT_CONTROLLER;
        expectedCount = 3;
    }
    else if (name == "bend")
    {
        event.type = EVENT_BEND;
        expectedCount = 2;
    }
    else if (name == "bendrange")
    {
        event.type = EVENT_BEND_RANGE;
        expectedCount = 2;
    }
    else if (name == "pressure")
    {
        event.type = EVENT_PRESSURE;
        expectedCount = 2;
    }
    else if (name == "polypressure")
    {
        event.type = EVENT_POLY_PRESSURE;
        expectedCount = 3;
    }
    else
    {
        _error = QString("line %1: unknown event \"%2\"").arg(line).arg(name);
        return false;
    }

    if (words.count() != expectedCount)
    {
        _error = QString("line %1: %2 values expected after \"%3\"").arg(line).arg(expectedCount).arg(name);
        return false;
    }

    if (!readInt(words[0], 0, 15, event.channel, line))
        return false;

    bool ok = true;
    switch (event.type)
    {
    case EVENT_PROGRAM:
        ok = readInt(words[1], 0, 128, event.number, line) && readInt(words[2], 0, 127, event.value, line);
        break;
    case EVENT_NOTE_ON: case EVENT_CONTROLLER: case EVENT_POLY_PRESSURE:
        ok = readInt(words[1], 0, 127, event.number, line) && readInt(words[2], 0, 127, event.value, line);
        break;
    case EVENT_NOTE_OFF:
        ok = readInt(words[1], 0, 127, event.number, line);
        break;
    case EVENT_BEND:
        ok = readFloat(words[1], -1.f, 1.f, event.floatValue, line);
        break;
    case EVENT_BEND_RANGE:
        ok = readFloat(words[1], 0.f, 127.f, event.floatValue, line);
        break;
    case EVENT_PRESSURE:
        ok = readInt(words[1], 0, 127, event.value, line);
        break;
    case EVENT_INSTRUMENT: case EVENT_SAMPLE:
        break;
    }
    if (!ok)
        return false;

    _events << event;
    return true;
}

bool OfflineRenderer::readInt(QString word, int min, int max, int &value, int line)
{
    bool ok;
    value = word.toInt(&ok);
    if (!ok || value < min || value > max)
    {
        _error = QString("line %1: \"%2\" is not an integer between %3 and %4").arg(line).arg(word).arg(min).arg(max);
        return false;
    }
    return true;
}

bool OfflineRenderer::readFloat(QString word, float min, float max, float &value, int line)
{
    bool ok;
    value = word.toFloat(&ok);
    if (!ok || value < min || value > max)
    {
        _error = QString("line %1: \"%2\" is not a number between %3 and %4").arg(line).arg(word).arg(min).arg(max);
        return false;
    }
    return true;
}

bool OfflineRenderer::applyModelChanges(int sf2Index)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    Soundfont * soundfont = sm->getSoundfonts()->getSoundfont(sf2Index);
    if (soundfont == nullptr)
    {
        _error = "the soundfont is not loaded";
        return false;
    }

    AttributeValue value;
    value.bValue = 1;
    foreach (ModelChange change, _modelChanges)
    {
        InstPrst * element = change.isPreset ? soundfont->getPreset(change.index) : soundfont->getInstrument(change.index);
        if (element == nullptr || (change.mute && element->getDivision(change.zone) == nullptr))
        {
            _error = QString("line %1: no such element in the soundfont").arg(change.line);
            return false;
        }

        if (change.mute)
            sm->set(EltID(change.isPreset ? elementPrstInst : elementInstSmpl, sf2Index, change.index, change.zone), champ_mute, value);
        else
            sm->set(EltID(change.isPreset ? elementPrst : elementInst, sf2Index, change.index), champ_alwaysPlayed, value);
    }

    return true;
}

bool OfflineRenderer::resolveElements(int sf2Index)
{
    Soundfont * soundfont = SoundfontManager::getInstance()->getSoundfonts()->getSoundfont(sf2Index);
    if (soundfont == nullptr)
    {
        _error = "the soundfont is not loaded";
        return false;
    }

    // Check that each program change has a preset, that the notes have one and that the elements played alone exist
    bool channelHasPreset[16] = {};
    for (int i = 0; i < _events.count(); i++)
    {
        Event &event = _events[i];
        if ((event.type == EVENT_INSTRUMENT && soundfont->getInstrument(event.index) == nullptr) ||
            (event.type == EVENT_SAMPLE && soundfont->getSample(event.index) == nullptr))
        {
            _error = QString("line %1: no such element in the soundfont").arg(event.line);
            return false;
        }
        else if (event.type == EVENT_PROGRAM)
        {
            // The bank is selected by the controller 0, except on the channel 10 whose bank is 128
            if ((event.channel == 9) != (event.number == 128))
            {
                _error = QString("line %1: the bank is 128 on the channel 9 and from 0 to 127 on the others").arg(event.line);
                return false;
            }
            if (SynthMidi::findPreset(soundfont, event.number, event.value) == -1)
            {
                _error = QString("line %1: no preset %2 in bank %3").arg(event.line).arg(event.value).arg(event.number);
                return false;
            }
            channelHasPreset[event.channel] = true;
        }
        else if (event.type == EVENT_NOTE_ON && !channelHasPreset[event.channel])
        {
            _error = QString("line %1: no program on channel %2").arg(event.line).arg(event.channel);
            return false;
        }
    }

    return true;
}

// Receiver of the MIDI events for the synth of the editor, playing the preset chosen by the program change of a channel
// (the editor does the same with its listeners)
class EditorSynthListener: public IMidiListener
{
public:
    EditorSynthListener(Synth * synth, ISynthSoundfont * soundfont, int sf2Index) :
        _synth(synth),
        _soundfont(soundfont),
        _sf2Index(sf2Index)
    {
        for (int i = 0; i < 16; i++)
            _presetIndexes[i] = -1;
    }

    bool processKey(int channel, int key, int vel) override
    {
        // The key is released whatever the preset (-1)
        _synth->play(EltID(elementPrst, _sf2Index, vel > 0 ? _presetIndexes[channel] : -1), channel, key, vel);
        return true;
    }

    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override
    {
        _presetIndexes[channel] = SynthMidi::findPreset(_soundfont, bank, preset);
        return true;
    }

    bool processPolyPressureChanged(int channel, int key, int pressure) override
    {
        _synth->processPolyPressureChanged(channel, key, pressure);
        return true;
    }

    bool processMonoPressureChanged(int channel, int value) override
    {
        _synth->processMonoPressureChanged(channel, value);
        return true;
    }

    bool processControllerChanged(int channel, int num, int value) override
    {
        _synth->processControllerChanged(channel, num, value);
        return true;
    }

    bool processBendChanged(int channel, float value) override
    {
        _synth->processBendChanged(channel, value);
        return true;
    }

    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override
    {
        _synth->processRPNChanged(channel, parameter, value, isRegistered, trigger);
        return true;
    }

private:
    Synth * _synth;
    ISynthSoundfont * _soundfont;
    int _sf2Index;
    int _presetIndexes[16];
};

// Instruments and samples played alone on the channel of the editor (-1), as the editor does
static void playElement(Synth * synth, ElementType type, int sf2Index, int index, int key, int velocity)
{
    synth->play(EltID(type, sf2Index, index), -1, key, velocity);
}

static void playElement(SynthCore * core, ElementType type, int sf2Index, int index, int key, int velocity)
{
    if (velocity == 0)
        core->releaseVoices(sf2Index, index, -1, key, false);
    else if (type == elementInst)
        core->playInstrument(sf2Index, index, -1, key, velocity);
    else
        core->playSample(sf2Index, index, -1, key, velocity);
}

bool OfflineRenderer::render(int sf2Index, QString outputFilePath, bool threaded)
{
    if (!applyModelChanges(sf2Index) || !resolveElements(sf2Index))
        return false;

    quint32 frameCount = static_cast<quint32>(qRound64(_duration * _sampleRate));
    quint32 blockCount = (frameCount + _blockSize - 1) / _blockSize;
    QVector<float> data(2 * static_cast<int>(blockCount * _blockSize), 0.f);
    if (threaded)
        renderWithThreads(sf2Index, data, blockCount);
    else
        renderWithCore(sf2Index, data, blockCount);

    return writeWavFile(outputFilePath, data, frameCount);
}

SynthCoreConfig OfflineRenderer::getSynthConfig()
{
    SynthCoreConfig configuration;
    configuration.masterGain = _masterGain;
    configuration.referencePitch = _referencePitch;
    configuration.revLevel = _reverb[0];
    configuration.revSize = _reverb[1];
    configuration.revWidth = _reverb[2];
    configuration.revDamping = _reverb[3];
    configuration.choLevel = _chorus[0];
    configuration.choDepth = _chorus[1];
    configuration.choFrequency = _chorus[2];
    return configuration;
}

void OfflineRenderer::renderWithCore(int sf2Index, QVector<float> &data, quint32 blockCount)
{
    SynthCore * core = new SynthCore(SoundfontManager::getInstance()->getSoundfonts());
    core->setMidiValues(_midiProcessor);
    _midiProcessorOwned = false; // Deleted by the synth from now on, still used here
    SynthMidi synthMidi(core, SoundfontManager::getInstance()->getSoundfonts(), sf2Index);
    _midiProcessor->setListener(&synthMidi);
    core->configure(getSynthConfig());
    core->setSampleRate(_sampleRate);
    core->setLoopEnabled(_sampleLoop);
    core->setStereo(_sampleStereo);
    core->setSampleGain(_sampleGain);

    // The synth of the editor applies the effects on a first block of silence (skipped in renderWithThreads):
    // the same is done here so that the modulation of the chorus has the same phase
    float * dataL = new float[_blockSize];
    float * dataR = new float[_blockSize];
    core->render(dataL, dataR, _blockSize);

    // The events of a block are heard from its beginning
    int eventIndex = 0;
    for (quint32 block = 0; block < blockCount; block++)
    {
        processEvents(core, sf2Index, static_cast<quint64>(block + 1) * _blockSize, eventIndex);
        core->render(dataL, dataR, _blockSize);
        storeBlock(data, block, dataL, dataR);
    }
    _midiProcessor->setListener(nullptr);
    delete [] dataL;
    delete [] dataR;
    delete core;
}

void OfflineRenderer::renderWithThreads(int sf2Index, QVector<float> &data, quint32 blockCount)
{
    // Same order as the application: creation, configuration, then sample rate
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = new Synth(sm->getSoundfonts(), sm->getMutex());
    synth->setIMidiValues(_midiProcessor);
    _midiProcessorOwned = false; // Deleted by the synth from now on, still used here
    EditorSynthListener listener(synth, sm->getSoundfonts()->getSynthSoundfont(sf2Index), sf2Index);
    _midiProcessor->setListener(&listener);
    synth->configure(getSynthConfig());
    synth->setSampleRateAndBufferSize(_sampleRate, _blockSize);
    synth->setLoopEnabled(_sampleLoop);
    synth->setStereo(_sampleStereo);
    synth->setSampleGain(_sampleGain);

    // Each call to readData returns what has been computed since the previous call: the events of a block
    // are heard in the next one, so the first block is skipped and one more block is computed
    float * dataL = new float[_blockSize];
    float * dataR = new float[_blockSize];
    int eventIndex = 0;
    for (quint32 block = 0; block <= blockCount; block++)
    {
        // Events are processed while no voice is being computed
        synth->waitForComputation();
        processEvents(synth, sf2Index, static_cast<quint64>(block + 1) * _blockSize, eventIndex);

        synth->readData(dataL, dataR, _blockSize);
        if (block > 0)
            storeBlock(data, block - 1, dataL, dataR);
    }
    _midiProcessor->setListener(nullptr);
    delete [] dataL;
    delete [] dataR;
    delete synth;
}

void OfflineRenderer::storeBlock(QVector<float> &data, quint32 block, const float * dataL, const float * dataR)
{
    float * destination = data.data() + 2 * block * _blockSize;
    for (quint32 i = 0; i < _blockSize; i++)
    {
        destination[2 * i] = dataL[i];
        destination[2 * i + 1] = dataR[i];
    }
}

template <class T>
void OfflineRenderer::processEvents(T * synth, int sf2Index, quint64 blockEnd, int &eventIndex)
{
    // The MIDI events go through MidiProcessor, the instruments and samples played alone directly to the synth
    for (; eventIndex < _events.count() && _events[eventIndex].frame < blockEnd; eventIndex++)
    {
        Event &event = _events[eventIndex];
        switch (event.type)
        {
        case EVENT_PROGRAM:
            // Bank select (the bank of the channel 10 is 128), then program change
            if (event.channel != 9)
                _midiProcessor->processControllerChanged(false, event.channel, 0, event.number);
            _midiProcessor->processProgramChanged(event.channel, static_cast<uint8_t>(event.value));
            break;
        case EVENT_NOTE_ON:
            if (event.value > 0)
            {
                _midiProcessor->processKeyOn(event.channel, event.number, event.value);
                break;
            }
            // A velocity of 0 is a note off
            Q_FALLTHROUGH();
        case EVENT_NOTE_OFF:
            _midiProcessor->processKeyOff(event.channel, event.number);
            break;
        case EVENT_CONTROLLER:
            _midiProcessor->processControllerChanged(false, event.channel, event.number, event.value);
            break;
        case EVENT_BEND:
            _midiProcessor->processBendChanged(event.channel, event.floatValue);
            break;
        case EVENT_BEND_RANGE: {
            // RPN 0 (controllers 101 and 100), semitones (controller 6) and cents (controller 38)
            int semitones = static_cast<int>(event.floatValue);
            int cents = qRound(100.f * (event.floatValue - static_cast<float>(semitones)));
            _midiProcessor->processControllerChanged(false, event.channel, 101, 0);
            _midiProcessor->processControllerChanged(false, event.channel, 100, 0);
            _midiProcessor->processControllerChanged(false, event.channel, 6, semitones);
            _midiProcessor->processControllerChanged(false, event.channel, 38, cents);
        } break;
        case EVENT_PRESSURE:
            _midiProcessor->processMonoPressureChanged(event.channel, event.value);
            break;
        case EVENT_POLY_PRESSURE:
            _midiProcessor->processPolyPressureChanged(event.channel, event.number, event.value);
            break;
        case EVENT_INSTRUMENT:
            playElement(synth, elementInst, sf2Index, event.index, event.number, event.value);
            break;
        case EVENT_SAMPLE:
            playElement(synth, elementSmpl, sf2Index, event.index, event.number, event.value);
            break;
        }
    }
}

bool OfflineRenderer::writeWavFile(QString filePath, const QVector<float> &data, quint32 frameCount)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        _error = "cannot write \"" + filePath + "\"";
        return false;
    }

    // Stereo, 32-bit float (format 3)
    quint32 dataSize = frameCount * 8;
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.writeRawData("RIFF", 4);
    stream << static_cast<quint32>(4 + 26 + 12 + 8 + dataSize);
    stream.writeRawData("WAVE", 4);
    stream.writeRawData("fmt ", 4);
    stream << static_cast<quint32>(18) << static_cast<quint16>(3) << static_cast<quint16>(2) << _sampleRate
           << static_cast<quint32>(_sampleRate * 8) << static_cast<quint16>(8) << static_cast<quint16>(32)
           << static_cast<quint16>(0);
    stream.writeRawData("fact", 4);
    stream << static_cast<quint32>(4) << frameCount;
    stream.writeRawData("data", 4);
    stream << dataSize;
    for (quint32 i = 0; i < 2 * frameCount; i++)
        stream << data[static_cast<int>(i)];

    if (stream.status() != QDataStream::Ok)
    {
        _error = "cannot write \"" + filePath + "\"";
        return false;
    }
    return true;
}
