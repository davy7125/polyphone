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


#ifndef PLUGINENGINE_H
#define PLUGINENGINE_H

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "synthcore.h"
#include "synthcoreconfig.h"
#include "synthqueue.h"
class SynthChannels;
class SynthSoundfontFile;
class SynthSoundfontSet;
class MidiProcessor;

// Synth of the plugin, without JUCE: a soundfont played on the 16 MIDI channels by the synth core of Polyphone, the
// audio thread never waiting for the others.
// - The "owner" threads (interface, host restoring a state) give the soundfont and choose the presets. Their requests
//   are queued and applied by the audio thread before it computes the next block. Their calls are serialized by a mutex
//   that the audio thread never takes.
// - The audio thread processes the MIDI messages of the host at their position in a block and computes the sound at the
//   frame (SynthCore::renderExact): a note starts at the frame of its message, whatever the size of the blocks.
class PluginEngine
{
public:
    // A preset of the soundfont, its program being "bank * 128 + number"
    struct Preset
    {
        int program;
        std::string name;
    };

    // How the channels choose their presets
    enum Mode
    {
        MODE_ONE_PRESET, // The same preset for all channels
        MODE_CHANNELS,   // A preset for each channel, or the one chosen by its program changes (General MIDI)
        MODE_MPE         // The same preset for all channels, played by an MPE controller (MidiProcessor::setMpeEnabled)
    };

    PluginEngine();
    ~PluginEngine();

    /// Owner threads

    // Soundfont replacing the current one (null for none), possibly shared with other engines. False if it cannot replace
    // it now (too many soundfonts are still played by voices fading out).
    bool setSoundfont(std::shared_ptr<SynthSoundfontFile> soundfont);
    bool hasSoundfont();
    std::string getSoundfontInfo(const std::string &key); // Text of the INFO chunk, "INAM" for the name
    std::vector<Preset> getPresets(); // By program

    // Presets played: one for all channels, or one for each channel (-1 for the one chosen by the program changes)
    void setMode(Mode mode);
    Mode getMode();
    void setProgram(int program); // Modes MODE_ONE_PRESET and MODE_MPE
    int getProgram();
    void setChannelProgram(int channel, int program); // Mode MODE_CHANNELS
    int getChannelProgram(int channel);

    // All sounds stopped, the pedals and the controllers back to their initial values
    void panic();

    // Delete the soundfonts replaced that are not played anymore, to call regularly
    void deleteUnusedSoundfonts();

    /// Any thread: what is played (updated by the audio thread after each block)

    int getPlayedProgram(int channel) const; // -1 if the channel plays nothing
    uint32_t getChannelActivity(int channel) const; // Changes each time a note starts
    int getVoiceCount() const;
    float getLoad() const; // Share of the duration of the audio taken by its computation

    /// Audio thread, or another thread when no audio is computed

    // When no audio is computed: sample rate, maximum number of frames computed at once and polyphony (the notes stop)
    void prepare(uint32_t sampleRate, uint32_t maxBlockSize, int polyphony);

    // Settings of the synth, reached progressively (no click), and transposition of the next notes
    void setConfiguration(const SynthCoreConfig &configuration, int transposition);

    // A block: beginBlock (requests of the owner threads), the MIDI messages and the frames in their order, endBlock
    // The frames go to the main output, and possibly to the outputs of the channels and of the effects
    void beginBlock(bool isRealTime);
    void processMidi(const uint8_t * message, size_t length);
    void render(const SynthCore::Outputs &outputs, uint32_t frameCount);
    void endBlock();

private:
    struct Command
    {
        enum Type
        {
            COMMAND_SOUNDFONT,
            COMMAND_PROGRAMS,
            COMMAND_PANIC
        };

        Type type = COMMAND_PANIC;
        int sf2Index = -1;
        bool isMpe = false;
        SynthSoundfontFile * soundfont = nullptr; // Kept by _soundfonts until the audio thread removes it
        int programs[16] = {};
    };

    void sendPrograms(); // Owner
    void applyCommand(const Command &command); // Audio thread
    void applyPrograms(); // Audio thread

    // Synth, used by the audio thread (the soundfonts are declared first: deleted after the voices reading them)
    std::unique_ptr<SynthSoundfontSet> _soundfonts;
    std::unique_ptr<SynthCore> _core;
    std::unique_ptr<MidiProcessor> _midiProcessor;
    std::unique_ptr<SynthChannels> _channels;
    SynthQueue<Command> _commands;
    int _sf2Index;
    int _programs[16]; // Forced program of each channel, -1 for the program changes
    uint32_t _sampleRate;
    uint32_t _blockFrameCount;
    std::chrono::steady_clock::time_point _blockStart;

    // Owner threads
    std::mutex _ownerMutex;
    std::shared_ptr<SynthSoundfontFile> _soundfont; // Current one
    std::vector<Preset> _presets;
    Mode _mode;
    int _program;
    int _channelPrograms[16];
    bool _programsPending; // Not sent, the queue being full

    // What is played
    std::atomic<int> _playedPrograms[16];
    std::atomic<uint32_t> _channelActivity[16];
    std::atomic<int> _voiceCount;
    std::atomic<float> _load;
};

#endif // PLUGINENGINE_H
