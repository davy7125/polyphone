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

#ifndef MIDIFILEPLAYER_H
#define MIDIFILEPLAYER_H

#include <QList>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "midiprocessor.h"
#include "midifile.h"
class Synth;
class ISynthSoundfonts;
class QRecursiveMutex;

// Plays a MIDI file with the synth of the editor, in its own thread. The channels 0 to 15 of the file are the channels
// MIDI_FILE_CHANNEL_OFFSET to MIDI_FILE_CHANNEL_OFFSET + 15 of the synth, with their own states (controllers, pedals,
// programs) so that the MIDI input still plays the editor. Each channel plays the presets of a soundfont, chosen by the
// program changes of the file or forced.
// The events are given to the synth on time, and are heard from the next block of audio it computes.
class MidiFilePlayer: public IMidiListener
{
public:
    MidiFilePlayer(Synth * synth, ISynthSoundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts);
    ~MidiFilePlayer() override;

    // File to play (null: none), the playback being stopped
    void setFile(std::shared_ptr<const MidiFile> file);

    // Transport, the position being in microseconds
    void play();
    void pause();
    void stop(); // And back to the beginning
    void setPosition(qint64 position);
    void setLoop(bool isLooping);
    bool isPlaying();
    qint64 getPosition();

    // Configuration of a channel (0 to 15): soundfont (-1 for none) and possibly a forced program, a program being
    // "bank * 128 + preset" (-1 for none)
    void setSoundfont(int channel, int sf2Index);
    void setForcedProgram(int channel, bool isForced, int program);
    void setGain(int channel, float gain); // dB

    // Program chosen by the file for a channel, and number of notes started (activity)
    int getFileProgram(int channel);
    quint32 getNoteCount(int channel);

    // Programs used by the notes of a channel in a file, for loading the samples beforehand
    static QList<int> getUsedPrograms(const MidiFile * file, int channel);

    // Values of the controllers read by the synth for the channels of the player, and their default values (depending
    // on the modulators)
    IMidiValues * getMidiValues() { return &_processor; }
    void setDefaultControllerValue(int controllerNumber, bool isBipolar, bool isDescending);

    // IMidiListener implementation (thread of the player): events processed by MidiProcessor
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override;
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override;
    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

    // Default program of a channel, before any program change
    static int getDefaultProgram(int channel) { return channel == 9 ? 128 * 128 : 0; }

private:
    typedef std::chrono::steady_clock Clock;

    void run();
    qint64 getPositionLocked();
    void prepare(const MidiFile * file, qint64 position, bool fastRelease);
    void dispatch(const MidiFile::Event &event);
    void releaseAll(bool fastRelease);

    Synth * _synth;
    ISynthSoundfonts * _soundfonts;
    QRecursiveMutex * _mutexSoundfonts;

    // Channel states, used by the thread of the player and read by the synth
    MidiProcessor _processor;

    // Configuration of the channels
    std::atomic<int> _sf2Indexes[16];
    std::atomic<bool> _isForced[16];
    std::atomic<int> _forcedPrograms[16];
    std::atomic<int> _filePrograms[16];
    std::atomic<quint32> _noteCounts[16];

    // Transport, shared with the thread under the mutex
    std::mutex _mutex;
    std::condition_variable _condition;
    std::shared_ptr<const MidiFile> _file;
    bool _fileChanged;
    bool _isPlaying;
    bool _isLooping;
    qint64 _startPosition; // Position when the playback started, or current position otherwise
    Clock::time_point _startTime;
    bool _preparationRequired; // The channels must be prepared for _startPosition
    bool _fastRelease; // For the preparation
    bool _releaseRequired;
    bool _quit;

    // Used by the thread only
    std::shared_ptr<const MidiFile> _currentFile;
    int _nextEventIndex;
    std::vector<bool> _isEventPrepared; // Events already processed when the channels were prepared

    std::thread _thread;
};

#endif // MIDIFILEPLAYER_H
