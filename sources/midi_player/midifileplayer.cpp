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

#include "midifileplayer.h"
#include <QMutexLocker>
#include "synth.h"
#include "synthmidi.h"

// Programs chosen for the notes of a channel, with the same rules as the player for the banks
class MidiFileProgramCollector: public IMidiListener
{
public:
    MidiFileProgramCollector(int channel) :
        _channel(channel),
        _program(MidiFilePlayer::getDefaultProgram(channel))
    {}

    bool processKey(int channel, int key, int vel) override
    {
        Q_UNUSED(key)
        if (channel == _channel && vel > 0 && !_programs.contains(_program))
            _programs << _program;
        return true;
    }

    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override
    {
        if (channel == _channel)
            _program = bank * 128 + preset;
        return true;
    }

    QList<int> getPrograms() { return _programs; }

private:
    int _channel;
    int _program;
    QList<int> _programs;
};

MidiFilePlayer::MidiFilePlayer(Synth * synth, ISynthSoundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts) :
    _synth(synth),
    _soundfonts(soundfonts),
    _mutexSoundfonts(mutexSoundfonts),
    _fileChanged(false),
    _isPlaying(false),
    _isLooping(false),
    _startPosition(0),
    _preparationRequired(false),
    _fastRelease(false),
    _releaseRequired(false),
    _quit(false),
    _nextEventIndex(0)
{
    for (int i = 0; i < 16; i++)
    {
        _sf2Indexes[i] = -1;
        _isForced[i] = false;
        _forcedPrograms[i] = 0;
        _filePrograms[i] = getDefaultProgram(i);
        _noteCounts[i] = 0;
    }
    _processor.setListener(this);
    _thread = std::thread(&MidiFilePlayer::run, this);
}

MidiFilePlayer::~MidiFilePlayer()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _quit = true;
    }
    _condition.notify_one();
    _thread.join();
    releaseAll(true);
}

void MidiFilePlayer::setFile(std::shared_ptr<const MidiFile> file)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _file = file;
        _fileChanged = true;
        _isPlaying = false;
        _startPosition = 0;
        _preparationRequired = true;
        _fastRelease = true;
    }
    _condition.notify_one();
}

void MidiFilePlayer::play()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_isPlaying || _file == nullptr)
            return;
        _isPlaying = true;
        _startTime = Clock::now();
    }
    _condition.notify_one();
}

void MidiFilePlayer::pause()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_isPlaying)
            return;
        _startPosition = getPositionLocked();
        _isPlaying = false;
        _releaseRequired = true;
    }
    _condition.notify_one();
}

void MidiFilePlayer::stop()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _isPlaying = false;
        _startPosition = 0;
        _preparationRequired = true;
        _fastRelease = false;
    }
    _condition.notify_one();
}

void MidiFilePlayer::setPosition(qint64 position)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_file == nullptr)
            return;
        _startPosition = qBound(static_cast<qint64>(0), position, _file->getDuration());
        _startTime = Clock::now();
        _preparationRequired = true;
        _fastRelease = true;
    }
    _condition.notify_one();
}

void MidiFilePlayer::setLoop(bool isLooping)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isLooping = isLooping;
}

bool MidiFilePlayer::isPlaying()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _isPlaying;
}

qint64 MidiFilePlayer::getPosition()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _file == nullptr ? 0 : qMin(getPositionLocked(), _file->getDuration());
}

qint64 MidiFilePlayer::getPositionLocked()
{
    if (!_isPlaying)
        return _startPosition;
    return _startPosition + std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - _startTime).count();
}

void MidiFilePlayer::setSoundfont(int channel, int sf2Index)
{
    if (channel >= 0 && channel < 16)
        _sf2Indexes[channel] = sf2Index;
}

void MidiFilePlayer::setForcedProgram(int channel, bool isForced, int program)
{
    if (channel < 0 || channel >= 16)
        return;
    _forcedPrograms[channel] = program;
    _isForced[channel] = isForced;
}

void MidiFilePlayer::setGain(int channel, float gain)
{
    if (channel >= 0 && channel < 16)
        _synth->setChannelGain(MIDI_FILE_CHANNEL_OFFSET + channel, gain);
}

int MidiFilePlayer::getFileProgram(int channel)
{
    return (channel >= 0 && channel < 16) ? _filePrograms[channel].load() : -1;
}

quint32 MidiFilePlayer::getNoteCount(int channel)
{
    return (channel >= 0 && channel < 16) ? _noteCounts[channel].load() : 0;
}

QList<int> MidiFilePlayer::getUsedPrograms(const MidiFile * file, int channel)
{
    MidiFileProgramCollector collector(channel);
    std::unique_ptr<MidiProcessor> processor(new MidiProcessor());
    processor->setListener(&collector);
    foreach (const MidiFile::Event &event, file->getEvents())
    {
        if ((event.status & 0x0F) != channel)
            continue;
        switch (event.status & 0xF0)
        {
        case 0x90:
            processor->processKeyOn(channel, event.data1, event.data2);
            break;
        case 0xB0:
            if (event.data1 == 0 || event.data1 == 32) // Bank
                processor->processControllerChanged(false, channel, event.data1, event.data2);
            break;
        case 0xC0:
            processor->processProgramChanged(channel, event.data1);
            break;
        default:
            break;
        }
    }
    return collector.getPrograms();
}

void MidiFilePlayer::setDefaultControllerValue(int controllerNumber, bool isBipolar, bool isDescending)
{
    _processor.setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
}

void MidiFilePlayer::run()
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (!_quit)
    {
        if (_fileChanged)
        {
            _fileChanged = false;
            _currentFile = _file;
        }

        // The synth is used without the mutex
        if (_preparationRequired)
        {
            _preparationRequired = false;
            _releaseRequired = false;
            qint64 position = _startPosition;
            bool fastRelease = _fastRelease;
            lock.unlock();
            prepare(_currentFile.get(), position, fastRelease);
            lock.lock();
            continue;
        }
        if (_releaseRequired)
        {
            _releaseRequired = false;
            lock.unlock();
            releaseAll(false);
            lock.lock();
            continue;
        }

        if (!_isPlaying || _currentFile == nullptr)
        {
            _condition.wait(lock);
            continue;
        }

        // Events to play now
        const QVector<MidiFile::Event> &events = _currentFile->getEvents();
        qint64 position = getPositionLocked();
        int firstEventIndex = _nextEventIndex;
        while (_nextEventIndex < events.size() && events[_nextEventIndex].time <= position)
            _nextEventIndex++;
        if (_nextEventIndex > firstEventIndex)
        {
            int lastEventIndex = _nextEventIndex;
            lock.unlock();
            for (int i = firstEventIndex; i < lastEventIndex; i++)
                if (!_isEventPrepared[i])
                    dispatch(events[i]);
            lock.lock();
            continue;
        }

        // End of the file: start again or stop at the beginning
        if (_nextEventIndex >= events.size() && position >= _currentFile->getDuration())
        {
            _startPosition = 0;
            _preparationRequired = true;
            _fastRelease = false;
            if (_isLooping)
                _startTime = Clock::now();
            else
                _isPlaying = false;
            continue;
        }

        // Wait for the next event, the end of the file or a change
        qint64 nextTime = _nextEventIndex < events.size() ? events[_nextEventIndex].time : _currentFile->getDuration();
        _condition.wait_until(lock, _startTime + std::chrono::microseconds(nextTime - _startPosition));
    }
}

void MidiFilePlayer::prepare(const MidiFile * file, qint64 position, bool fastRelease)
{
    // Notes released and channels back to their initial state
    releaseAll(fastRelease);
    _processor.resetChannels();
    for (int i = 0; i < 16; i++)
        _filePrograms[i] = getDefaultProgram(i);
    _nextEventIndex = 0;
    _isEventPrepared.clear();
    if (file == nullptr)
        return;

    // First event from the position, and next note of each channel
    const QVector<MidiFile::Event> &events = file->getEvents();
    while (_nextEventIndex < events.size() && events[_nextEventIndex].time < position)
        _nextEventIndex++;
    int nextNoteIndexes[16];
    for (int i = 0; i < 16; i++)
        nextNoteIndexes[i] = -1;
    for (int i = _nextEventIndex; i < events.size(); i++)
        if ((events[i].status & 0xF0) == 0x90 && nextNoteIndexes[events[i].status & 0x0F] < 0)
            nextNoteIndexes[events[i].status & 0x0F] = i;

    // Each channel is prepared with its events except the notes (controllers, programs, ...) before the position and
    // before its next note, so that the program of this note is known: these events are then not played again
    _isEventPrepared.assign(events.size(), false);
    for (int i = 0; i < events.size(); i++)
    {
        const MidiFile::Event &event = events[i];
        if ((event.status & 0xF0) == 0x80 || (event.status & 0xF0) == 0x90)
            continue;
        if (i < _nextEventIndex || i < nextNoteIndexes[event.status & 0x0F])
        {
            dispatch(event);
            _isEventPrepared[i] = true;
        }
    }
}

void MidiFilePlayer::dispatch(const MidiFile::Event &event)
{
    // Values from the file, never relative
    int channel = event.status & 0x0F;
    switch (event.status & 0xF0)
    {
    case 0x80:
        _processor.processKeyOff(channel, event.data1);
        break;
    case 0x90:
        _processor.processKeyOn(channel, event.data1, event.data2);
        break;
    case 0xA0:
        _processor.processPolyPressureChanged(channel, event.data1, event.data2);
        break;
    case 0xB0:
        if (event.data1 == 121)
        {
            // Reset all controllers, not done by MidiProcessor: pitch bend, pressure, modulation, expression, pedals and
            // (N)RPN
            _processor.processBendChanged(channel, 0.0f);
            _processor.processMonoPressureChanged(channel, 0);
            _processor.processControllerChanged(false, channel, 1, 0);
            _processor.processControllerChanged(false, channel, 11, 127);
            for (int num = 64; num <= 67; num++)
                _processor.processControllerChanged(false, channel, num, 0);
            _processor.processControllerChanged(false, channel, 101, 127);
            _processor.processControllerChanged(false, channel, 100, 127);
        }
        else
            _processor.processControllerChanged(false, channel, event.data1, event.data2);
        break;
    case 0xC0:
        _processor.processProgramChanged(channel, event.data1);
        break;
    case 0xD0:
        _processor.processMonoPressureChanged(channel, event.data1);
        break;
    case 0xE0:
        _processor.processBendChanged(channel, (static_cast<float>((event.data2 << 7) | event.data1) - 8192.0f) / 8192.0f);
        break;
    default:
        break;
    }
}

void MidiFilePlayer::releaseAll(bool fastRelease)
{
    for (int channel = 0; channel < 16; channel++)
        _synth->releaseChannel(MIDI_FILE_CHANNEL_OFFSET + channel, fastRelease);
}

bool MidiFilePlayer::processKey(int channel, int key, int vel)
{
    // The events repeated on the channel -1 by MidiProcessor are ignored
    if (channel < 0 || channel >= 16)
        return true;

    if (vel == 0)
    {
        // Released whatever the preset playing the key
        _synth->play(EltID(elementPrst), MIDI_FILE_CHANNEL_OFFSET + channel, key, 0);
        return true;
    }
    _noteCounts[channel]++;

    int sf2Index = _sf2Indexes[channel];
    int program = _isForced[channel] ? _forcedPrograms[channel].load() : _filePrograms[channel].load();
    if (sf2Index < 0 || program < 0)
        return true;
    QMutexLocker locker(_mutexSoundfonts);
    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Index);
    int presetIndex = soundfont == nullptr ? -1 : SynthMidi::findPreset(soundfont, program / 128, program % 128);
    if (presetIndex >= 0)
        _synth->play(EltID(elementPrst, sf2Index, presetIndex), MIDI_FILE_CHANNEL_OFFSET + channel, key, vel);
    return true;
}

bool MidiFilePlayer::processPolyPressureChanged(int channel, int key, int pressure)
{
    if (channel >= 0 && channel < 16)
        _synth->processPolyPressureChanged(MIDI_FILE_CHANNEL_OFFSET + channel, key, pressure);
    return true;
}

bool MidiFilePlayer::processMonoPressureChanged(int channel, int value)
{
    if (channel >= 0 && channel < 16)
        _synth->processMonoPressureChanged(MIDI_FILE_CHANNEL_OFFSET + channel, value);
    return true;
}

bool MidiFilePlayer::processControllerChanged(int channel, int num, int value)
{
    if (channel >= 0 && channel < 16)
        _synth->processControllerChanged(MIDI_FILE_CHANNEL_OFFSET + channel, num, value);
    return true;
}

bool MidiFilePlayer::processBendChanged(int channel, float value)
{
    if (channel >= 0 && channel < 16)
        _synth->processBendChanged(MIDI_FILE_CHANNEL_OFFSET + channel, value);
    return true;
}

bool MidiFilePlayer::processProgramChanged(int channel, uint16_t bank, uint8_t preset)
{
    if (channel >= 0 && channel < 16)
        _filePrograms[channel] = bank * 128 + preset;
    return true;
}

bool MidiFilePlayer::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    if (channel >= 0 && channel < 16)
        _synth->processRPNChanged(MIDI_FILE_CHANNEL_OFFSET + channel, parameter, value, isRegistered, trigger);
    return true;
}
