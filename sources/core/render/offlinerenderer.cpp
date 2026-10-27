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
#include "imidivalues.h"
#include "basetypes.h"

/// MIDI values read by the modulators, with the same defaults as MidiDevice
class RenderMidiValues: public IMidiValues
{
public:
    RenderMidiValues()
    {
        for (int channel = 0; channel < 16; channel++)
        {
            for (int i = 0; i < 128; i++)
            {
                _controllerValues[channel][i] = 0;
                _polyPressureValues[channel][i] = 0;
            }
            _controllerValues[channel][7] = 127; // Main volume
            _controllerValues[channel][8] = 64; // Balance
            _controllerValues[channel][10] = 64; // Pan position
            _controllerValues[channel][11] = 127; // Expression
            _bendValues[channel] = 0.f;
            _bendSensitivityValues[channel] = 2.f;
            _monoPressureValues[channel] = 0;
        }
    }

    // Same as MidiDevice::onInputModulatorChanged, called while the soundfont is loaded
    void setDefaultControllerValue(int controllerNumber, bool isBipolar, bool isDescending)
    {
        for (int channel = 0; channel < 16; channel++)
            _controllerValues[channel][controllerNumber] = isBipolar ? 64 : (isDescending ? 127 : 0);
    }

    void setControllerValue(int channel, int controllerNumber, int value) { _controllerValues[channel][controllerNumber] = value; }
    void setBendValue(int channel, float value) { _bendValues[channel] = value; }
    void setBendSensitivityValue(int channel, float value) { _bendSensitivityValues[channel] = value; }
    void setMonoPressure(int channel, int value) { _monoPressureValues[channel] = value; }
    void setPolyPressure(int channel, int key, int value) { _polyPressureValues[channel][key] = value; }

    int getControllerValue(int channel, int controllerNumber) override { return isValid(channel) ? _controllerValues[channel][controllerNumber] : 0; }
    float getBendValue(int channel) override { return isValid(channel) ? _bendValues[channel] : 0.f; }
    float getBendSensitivityValue(int channel) override { return isValid(channel) ? _bendSensitivityValues[channel] : 2.f; }
    int getMonoPressure(int channel) override { return isValid(channel) ? _monoPressureValues[channel] : 0; }
    int getPolyPressure(int channel, int key) override { return isValid(channel) ? _polyPressureValues[channel][key] : 0; }

private:
    bool isValid(int channel) { return channel >= 0 && channel < 16; }

    int _controllerValues[16][128];
    float _bendValues[16];
    float _bendSensitivityValues[16];
    int _monoPressureValues[16];
    int _polyPressureValues[16][128];
};

OfflineRenderer::OfflineRenderer() :
    _midiValues(new RenderMidiValues()),
    _midiValuesOwned(true),
    _sampleRate(48000),
    _blockSize(COMPUTATION_CHUNK),
    _duration(0),
    _masterGain(0),
    _referencePitch(4400)
{
    for (int i = 0; i < 4; i++)
        _reverb[i] = 0;
    for (int i = 0; i < 3; i++)
        _chorus[i] = 0;
    for (int i = 0; i < 16; i++)
        _channelPresetIndexes[i] = -1;

    // The parser emits this signal for each modulator source that is a MIDI controller
    _modulatorConnection = QObject::connect(SoundfontManager::getInstance(), &SoundfontManager::inputModulatorChanged,
                                            [this](int controllerNumber, bool isBipolar, bool isDescending) {
        _midiValues->setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
    });
}

OfflineRenderer::~OfflineRenderer()
{
    QObject::disconnect(_modulatorConnection);
    if (_midiValuesOwned)
        delete _midiValues;
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
    event.presetIndex = -1;
    event.line = line;

    QString name = words.isEmpty() ? "" : words.takeFirst();
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

bool OfflineRenderer::resolvePrograms(int sf2Index)
{
    Soundfont * soundfont = SoundfontManager::getInstance()->getSoundfonts()->getSoundfont(sf2Index);
    if (soundfont == nullptr)
    {
        _error = "the soundfont is not loaded";
        return false;
    }

    // Find the preset of each program change, and check that the notes have one
    bool channelHasPreset[16] = {};
    QVector<InstPrst *> presets = soundfont->getPresets().values();
    for (int i = 0; i < _events.count(); i++)
    {
        Event &event = _events[i];
        if (event.type == EVENT_PROGRAM)
        {
            for (int j = 0; j < presets.count(); j++)
            {
                if (presets[j]->getExtraField(champ_wBank) == event.number &&
                    presets[j]->getExtraField(champ_wPreset) == event.value)
                {
                    event.presetIndex = presets[j]->getId().indexElt;
                    break;
                }
            }
            if (event.presetIndex == -1)
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

bool OfflineRenderer::render(int sf2Index, QString outputFilePath)
{
    if (!resolvePrograms(sf2Index))
        return false;

    // Same order as the application: creation, configuration, then sample rate
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = new Synth(sm->getSoundfonts(), sm->getMutex());
    synth->setIMidiValues(_midiValues);
    _midiValuesOwned = false; // Deleted by the synth from now on, still updated here

    SynthConfig configuration;
    configuration.masterGain = _masterGain;
    configuration.referencePitch = _referencePitch;
    configuration.revLevel = _reverb[0];
    configuration.revSize = _reverb[1];
    configuration.revWidth = _reverb[2];
    configuration.revDamping = _reverb[3];
    configuration.choLevel = _chorus[0];
    configuration.choDepth = _chorus[1];
    configuration.choFrequency = _chorus[2];
    synth->configure(&configuration);
    synth->setSampleRateAndBufferSize(_sampleRate, _blockSize);

    // Each call to readData returns what has been computed since the previous call: the events of a block
    // are heard in the next one, so the first block is skipped and one more block is computed
    quint32 frameCount = static_cast<quint32>(qRound64(_duration * _sampleRate));
    quint32 blockCount = (frameCount + _blockSize - 1) / _blockSize;
    QVector<float> data(2 * static_cast<int>(blockCount * _blockSize), 0.f);
    float * dataL = new float[_blockSize];
    float * dataR = new float[_blockSize];
    int eventIndex = 0;
    for (quint32 block = 0; block <= blockCount; block++)
    {
        // Events are processed while no voice is being computed
        synth->waitForComputation();
        quint64 blockEnd = static_cast<quint64>(block + 1) * _blockSize;
        while (eventIndex < _events.count() && _events[eventIndex].frame < blockEnd)
            processEvent(synth, sf2Index, _events[eventIndex++]);

        synth->readData(dataL, dataR, _blockSize);
        if (block > 0)
        {
            float * destination = data.data() + 2 * (block - 1) * _blockSize;
            for (quint32 i = 0; i < _blockSize; i++)
            {
                destination[2 * i] = dataL[i];
                destination[2 * i + 1] = dataR[i];
            }
        }
    }
    delete [] dataL;
    delete [] dataR;
    delete synth;

    return writeWavFile(outputFilePath, data, frameCount);
}

void OfflineRenderer::processEvent(Synth * synth, int sf2Index, Event &event)
{
    // As MidiDevice: the values read by the modulators are updated before the synth is notified
    switch (event.type)
    {
    case EVENT_PROGRAM:
        _channelPresetIndexes[event.channel] = event.presetIndex;
        break;
    case EVENT_NOTE_ON:
        if (event.value > 0)
        {
            _midiValues->setPolyPressure(event.channel, event.number, event.value);
            synth->play(EltID(elementPrst, sf2Index, _channelPresetIndexes[event.channel]),
                        event.channel, event.number, event.value);
            break;
        }
        // A velocity of 0 is a note off
        Q_FALLTHROUGH();
    case EVENT_NOTE_OFF:
        // The key is released whatever the preset (-1)
        synth->play(EltID(elementPrst, sf2Index, -1), event.channel, event.number, 0);
        break;
    case EVENT_CONTROLLER:
        _midiValues->setControllerValue(event.channel, event.number, event.value);
        synth->processControllerChanged(event.channel, event.number, event.value);
        break;
    case EVENT_BEND:
        _midiValues->setBendValue(event.channel, event.floatValue);
        synth->processBendChanged(event.channel, event.floatValue);
        break;
    case EVENT_BEND_RANGE: {
        // RPN 0: semitones in the MSB, cents in the LSB
        int semitones = static_cast<int>(event.floatValue);
        int value = (semitones << 7) + qRound(100.f * (event.floatValue - static_cast<float>(semitones)));
        _midiValues->setBendSensitivityValue(event.channel, static_cast<float>(value >> 7) + 0.01f * static_cast<float>(value & 0x7F));
        synth->processRPNChanged(event.channel, 0, value, true, 4);
    } break;
    case EVENT_PRESSURE:
        _midiValues->setMonoPressure(event.channel, event.value);
        synth->processMonoPressureChanged(event.channel, event.value);
        break;
    case EVENT_POLY_PRESSURE:
        _midiValues->setPolyPressure(event.channel, event.number, event.value);
        synth->processPolyPressureChanged(event.channel, event.number, event.value);
        break;
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
