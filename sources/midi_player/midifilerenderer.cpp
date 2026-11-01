/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "midifilerenderer.h"
#include <QFile>
#include <QDataStream>
#include <QMutexLocker>
#include <cmath>
#include "soundfontmanager.h"
#include "soundfonts.h"
#include "synthcore.h"
#include "synthmidi.h"
#include "midifile.h"
#include "midifileplayer.h"

// Frames computed at once, a whole chunk: an event is heard from the beginning of the chunk where it occurs
static const quint32 BLOCK_SIZE = COMPUTATION_CHUNK;

// After the end of the file, the rendering stops once no voice remains and the sound stays under -90 dBFS during
// 0.1 s, or after 60 s
static const float SILENCE_LEVEL = 0.0000316f;
static const quint32 SILENCE_DURATION_DIVIDER = 10;
static const quint32 MAX_TAIL_DURATION = 60;

// Frames of a wav file whose size does not exceed 4 GB (the length is stored on 32 bits)
static const quint64 MAX_FRAME_COUNT = (0xFFFFFFFFULL - 50) / 8;

MidiFileRenderer::MidiFileRenderer() :
    _sampleRate(44100),
    _speed(1.0),
    _transposition(0),
    _progress(0),
    _isCanceled(false)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    _soundfonts = sm->getSoundfonts();
    _mutexSoundfonts = sm->getMutex();

    // The sample rate is given by configure: it is shared by the effects of all synths
    _core = new SynthCore(_soundfonts);
    _core->setMidiValues(&_processor);
    _core->setMaxBlockSize(BLOCK_SIZE);
    _core->setRealTime(false);
    _processor.setListener(this);

    for (int i = 0; i < 16; i++)
    {
        _sf2Indexes[i] = -1;
        _forcedPrograms[i] = -1;
        _filePrograms[i] = MidiFilePlayer::getDefaultProgram(i);
        for (int key = 0; key < 128; key++)
            _playedKeys[i][key] = -1;
    }

    // The parser emits this signal for each modulator source that is a MIDI controller
    _modulatorConnection = QObject::connect(sm, &SoundfontManager::inputModulatorChanged,
                                            [this](int controllerNumber, bool isBipolar, bool isDescending) {
        _processor.setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
    });
}

MidiFileRenderer::~MidiFileRenderer()
{
    QObject::disconnect(_modulatorConnection);
    _processor.setListener(nullptr);
    delete _core;
}

void MidiFileRenderer::configure(const SynthCoreConfig &configuration, int polyphony, quint32 sampleRate)
{
    // Same order as the application: configuration, then sample rate
    _sampleRate = sampleRate;
    _core->configure(configuration);
    _core->setSampleRate(sampleRate);
    _core->setMaxPolyphony(polyphony);
}

void MidiFileRenderer::setChannel(int channel, int sf2Index, int forcedProgram, float gain)
{
    if (channel < 0 || channel >= 16)
        return;
    _sf2Indexes[channel] = sf2Index;
    _forcedPrograms[channel] = forcedProgram;
    _core->setChannelGain(channel, gain);
}

bool MidiFileRenderer::render(const MidiFile * file, QString outputFilePath)
{
    _error = "";
    _progress = 0;
    QFile output(outputFilePath);
    if (!output.open(QIODevice::WriteOnly))
    {
        _error = tr("Cannot write file \"%1\".").arg(outputFilePath);
        return false;
    }

    // Header written again at the end with the length
    QDataStream stream(&output);
    stream.setByteOrder(QDataStream::LittleEndian);
    writeHeader(stream, 0);

    // Times of the file in frames, at the speed chosen
    const QVector<MidiFile::Event> &events = file->getEvents();
    double framesPerMicrosecond = static_cast<double>(_sampleRate) / (1000000.0 * _speed);
    quint64 endFrame = static_cast<quint64>(qRound64(static_cast<double>(file->getDuration()) * framesPerMicrosecond));
    quint64 lastFrame = endFrame + static_cast<quint64>(MAX_TAIL_DURATION) * _sampleRate;

    float dataL[BLOCK_SIZE];
    float dataR[BLOCK_SIZE];
    float data[2 * BLOCK_SIZE];
    quint64 frameCount = 0;
    quint64 silentFrameCount = 0;
    int eventIndex = 0;
    bool isReleased = false;
    bool isFinished = false;
    while (!isFinished && !_isCanceled)
    {
        if (frameCount + BLOCK_SIZE > MAX_FRAME_COUNT)
        {
            _error = tr("The audio file cannot exceed 4 GB.");
            break;
        }

        // Events of the block, then the notes still held are released at the end of the file
        quint64 blockEnd = frameCount + BLOCK_SIZE;
        for (; eventIndex < events.size(); eventIndex++)
        {
            if (static_cast<quint64>(qRound64(static_cast<double>(events[eventIndex].time) * framesPerMicrosecond)) >= blockEnd)
                break;
            MidiFilePlayer::dispatch(_processor, events[eventIndex]);
        }
        if (!isReleased && endFrame < blockEnd)
        {
            _core->releaseVoices(-1, -1, -2, -2, false);
            isReleased = true;
        }

        // Interleaved samples, written in the order of the processor (little endian), as the recorder does
        _core->render(dataL, dataR, BLOCK_SIZE);
        float peak = 0;
        for (quint32 i = 0; i < BLOCK_SIZE; i++)
        {
            data[2 * i] = dataL[i];
            data[2 * i + 1] = dataR[i];
            peak = qMax(peak, qMax(std::fabs(dataL[i]), std::fabs(dataR[i])));
        }
        stream.writeRawData(reinterpret_cast<const char *>(data), static_cast<int>(sizeof(data)));
        if (stream.status() != QDataStream::Ok)
        {
            _error = tr("Cannot write file \"%1\".").arg(outputFilePath);
            break;
        }
        frameCount = blockEnd;

        // Progress until the end of the file, then the sound fades out
        if (isReleased)
        {
            if (peak < SILENCE_LEVEL && _core->getVoiceList()->getVoiceCount() == 0)
                silentFrameCount += BLOCK_SIZE;
            else
                silentFrameCount = 0;
            isFinished = (silentFrameCount >= _sampleRate / SILENCE_DURATION_DIVIDER || frameCount >= lastFrame);
        }
        else
            _progress = static_cast<int>(1000 * frameCount / endFrame); // endFrame >= frameCount > 0 here
    }

    // Length of the file, or no file if it is not complete
    if (isFinished)
    {
        output.seek(0);
        writeHeader(stream, static_cast<quint32>(frameCount));
        if (stream.status() != QDataStream::Ok)
        {
            _error = tr("Cannot write file \"%1\".").arg(outputFilePath);
            isFinished = false;
        }
    }
    output.close();
    if (!isFinished)
    {
        output.remove();
        return false;
    }
    _progress = 1000;
    return true;
}

void MidiFileRenderer::writeHeader(QDataStream &stream, quint32 frameCount)
{
    // Stereo, 32-bit float (format 3)
    quint32 dataSize = frameCount * 8;
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
}

bool MidiFileRenderer::processKey(int channel, int key, int vel)
{
    // The events repeated on the channel -1 by MidiProcessor are ignored
    if (channel < 0 || channel >= 16)
        return true;

    if (vel == 0)
    {
        // Key released whatever the preset playing it, with the transposition it had when it started
        int playedKey = _playedKeys[channel][key];
        if (playedKey >= 0)
        {
            _playedKeys[channel][key] = -1;
            _core->releaseVoices(-1, -1, channel, playedKey, false);
        }
        return true;
    }

    int sf2Index = _sf2Indexes[channel];
    int program = _forcedPrograms[channel] >= 0 ? _forcedPrograms[channel] : _filePrograms[channel];
    if (sf2Index < 0 || program < 0)
        return true;

    // Transposition, except for the percussions
    int playedKey = key;
    if (program / 128 != 128)
        playedKey += _transposition;
    if (playedKey < 0 || playedKey > 127)
        return true;

    // The soundfonts must not change while the voices are prepared
    QMutexLocker locker(_mutexSoundfonts);
    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Index);
    int presetIndex = soundfont == nullptr ? -1 : SynthMidi::findPreset(soundfont, program / 128, program % 128);
    if (presetIndex >= 0)
    {
        _playedKeys[channel][key] = playedKey;
        _core->playPreset(sf2Index, presetIndex, channel, playedKey, vel);
    }
    return true;
}

bool MidiFileRenderer::processPolyPressureChanged(int channel, int key, int pressure)
{
    if (channel >= 0 && channel < 16 && _playedKeys[channel][key] >= 0)
        _core->processPolyPressureChanged(channel, _playedKeys[channel][key], pressure);
    return true;
}

bool MidiFileRenderer::processMonoPressureChanged(int channel, int value)
{
    if (channel >= 0 && channel < 16)
        _core->processMonoPressureChanged(channel, value);
    return true;
}

bool MidiFileRenderer::processControllerChanged(int channel, int num, int value)
{
    if (channel >= 0 && channel < 16)
        _core->processControllerChanged(channel, num, value);
    return true;
}

bool MidiFileRenderer::processBendChanged(int channel, float value)
{
    if (channel >= 0 && channel < 16)
        _core->processBendChanged(channel, value);
    return true;
}

bool MidiFileRenderer::processProgramChanged(int channel, uint16_t bank, uint8_t preset)
{
    if (channel >= 0 && channel < 16)
        _filePrograms[channel] = bank * 128 + preset;
    return true;
}

bool MidiFileRenderer::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    if (channel >= 0 && channel < 16)
        _core->processRPNChanged(channel, parameter, value, isRegistered, trigger);
    return true;
}
