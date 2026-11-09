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


#include "pluginengine.h"
#include <algorithm>
#include "synthcore.h"
#include "synthchannels.h"
#include "midiprocessor.h"
#include "synthsoundfontfile.h"
#include "synthsoundfontset.h"

// Soundfonts at the same time (the current one and the ones replaced whose voices fade out), requests waiting for the
// audio thread
static const int SOUNDFONT_CAPACITY = 8;
static const uint32_t COMMAND_CAPACITY = 256;

PluginEngine::PluginEngine() :
    _soundfonts(new SynthSoundfontSet(SOUNDFONT_CAPACITY)),
    _commands(COMMAND_CAPACITY),
    _sf2Index(-1),
    _sampleRate(48000),
    _blockFrameCount(0),
    _mode(MODE_ONE_PRESET),
    _program(0),
    _programsPending(false),
    _voiceCount(0),
    _load(0)
{
    SynthCore::prepareTables();
    _core.reset(new SynthCore(_soundfonts.get()));
    _midiProcessor.reset(new MidiProcessor());
    _channels.reset(new SynthChannels(_core.get(), _soundfonts.get()));
    _midiProcessor->setListener(_channels.get());
    _core->setMidiValues(_midiProcessor.get());

    for (int channel = 0; channel < 16; channel++)
    {
        _programs[channel] = _program;
        _channelPrograms[channel] = -1;
        _playedPrograms[channel].store(-1, std::memory_order_relaxed);
        _channelActivity[channel].store(0, std::memory_order_relaxed);
    }
}

PluginEngine::~PluginEngine()
{
    // The voices are deleted before the soundfonts whose data they read (including the ones of requests not applied)
    _midiProcessor->setListener(nullptr);
    _channels.reset();
    _core.reset();
    _midiProcessor.reset();
}

/// Owner threads

bool PluginEngine::setSoundfont(std::shared_ptr<SynthSoundfontFile> soundfont)
{
    std::lock_guard<std::mutex> lock(_ownerMutex);

    // The audio thread must be able to receive the request (one producer: the space cannot decrease meanwhile)
    if (_commands.isFull())
        return false;

    // Index of the soundfont, possibly after deleting the ones not played anymore
    int index = -1;
    if (soundfont != nullptr)
    {
        index = _soundfonts->add(soundfont);
        if (index < 0)
        {
            _soundfonts->deleteUnused();
            index = _soundfonts->add(soundfont);
        }
        if (index < 0)
            return false;
    }

    Command command;
    command.type = Command::COMMAND_SOUNDFONT;
    command.sf2Index = index;
    command.soundfont = soundfont.get();
    _commands.push(command);
    _soundfont = std::move(soundfont);

    // Presets by program
    _presets.clear();
    if (_soundfont != nullptr)
    {
        for (int i = 0; i < _soundfont->getPresetCount(); i++)
        {
            ISynthInstPrst * preset = _soundfont->getPresetAt(i);
            _presets.push_back({ preset->getBankNumber() * 128 + preset->getPresetNumber(),
                                 _soundfont->getPresetName(preset->getIndex()) });
        }
    }
    std::stable_sort(_presets.begin(), _presets.end(), [](const Preset &a, const Preset &b) {
        return a.program < b.program;
    });

    // The preset played by all channels is the first one if the soundfont does not have it
    if (_mode != MODE_CHANNELS && !_presets.empty() &&
        std::none_of(_presets.begin(), _presets.end(), [this](const Preset &preset) { return preset.program == _program; }))
    {
        _program = _presets.front().program;
        sendPrograms();
    }

    return true;
}

bool PluginEngine::hasSoundfont()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _soundfont != nullptr;
}

std::string PluginEngine::getSoundfontInfo(const std::string &key)
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _soundfont == nullptr ? "" : _soundfont->getInfo(key);
}

std::vector<PluginEngine::Preset> PluginEngine::getPresets()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _presets;
}

void PluginEngine::setMode(Mode mode)
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    _mode = mode;
    sendPrograms();
}

PluginEngine::Mode PluginEngine::getMode()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _mode;
}

void PluginEngine::setProgram(int program)
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    _program = program;
    sendPrograms();
}

int PluginEngine::getProgram()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _program;
}

void PluginEngine::setChannelProgram(int channel, int program)
{
    if (channel < 0 || channel >= 16)
        return;
    std::lock_guard<std::mutex> lock(_ownerMutex);
    _channelPrograms[channel] = program;
    sendPrograms();
}

int PluginEngine::getChannelProgram(int channel)
{
    if (channel < 0 || channel >= 16)
        return -1;
    std::lock_guard<std::mutex> lock(_ownerMutex);
    return _channelPrograms[channel];
}

void PluginEngine::sendPrograms()
{
    // Sent again by deleteUnusedSoundfonts if the queue is full
    Command command;
    command.type = Command::COMMAND_PROGRAMS;
    for (int channel = 0; channel < 16; channel++)
        command.programs[channel] = (_mode == MODE_CHANNELS) ? _channelPrograms[channel] : _program;
    command.isMpe = (_mode == MODE_MPE);
    _programsPending = !_commands.push(command);
}

void PluginEngine::panic()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    Command command;
    command.type = Command::COMMAND_PANIC;
    _commands.push(command);
}

void PluginEngine::deleteUnusedSoundfonts()
{
    std::lock_guard<std::mutex> lock(_ownerMutex);
    _soundfonts->deleteUnused();
    if (_programsPending)
        sendPrograms();
}

/// Any thread

int PluginEngine::getPlayedProgram(int channel) const
{
    return (channel >= 0 && channel < 16) ? _playedPrograms[channel].load(std::memory_order_relaxed) : -1;
}

uint32_t PluginEngine::getChannelActivity(int channel) const
{
    return (channel >= 0 && channel < 16) ? _channelActivity[channel].load(std::memory_order_relaxed) : 0;
}

int PluginEngine::getVoiceCount() const
{
    return _voiceCount.load(std::memory_order_relaxed);
}

float PluginEngine::getLoad() const
{
    return _load.load(std::memory_order_relaxed);
}

/// Audio thread

void PluginEngine::prepare(uint32_t sampleRate, uint32_t maxBlockSize, int polyphony)
{
    _sampleRate = sampleRate > 0 ? sampleRate : 48000;
    _core->setSampleRate(_sampleRate);
    _core->setMaxBlockSize(maxBlockSize, true);
    _core->setMaxPolyphony(polyphony);
}

void PluginEngine::setConfiguration(const SynthCoreConfig &configuration, int transposition)
{
    _core->configure(configuration);
    _channels->setTransposition(transposition);
}

void PluginEngine::beginBlock(bool isRealTime)
{
    _blockStart = std::chrono::steady_clock::now();
    _blockFrameCount = 0;
    _core->setRealTime(isRealTime);

    // Requests of the owner threads, then the soundfonts replaced that have no voice anymore
    Command command;
    while (_commands.pop(command))
        applyCommand(command);
    _soundfonts->checkRemoved(*_core);
}

void PluginEngine::applyCommand(const Command &command)
{
    switch (command.type)
    {
    case Command::COMMAND_SOUNDFONT:
        // The previous soundfont is not played anymore, its voices stop quickly
        if (_sf2Index >= 0)
        {
            _soundfonts->remove(_sf2Index);
            _core->releaseVoices(_sf2Index, -1, -2, -2, true);
        }
        _sf2Index = command.sf2Index;

        // Default values of the controllers used by its modulators
        _midiProcessor->resetDefaultControllerValues();
        if (command.soundfont != nullptr)
            command.soundfont->applyDefaultControllerValues(*_midiProcessor);
        applyPrograms();
        break;
    case Command::COMMAND_PROGRAMS:
        // The notes stop if the MPE starts or ends (the channels are reset)
        if (command.isMpe != _midiProcessor->isMpeEnabled())
        {
            _core->releaseVoices(-1, -1, -2, -2, true);
            _midiProcessor->setMpeEnabled(command.isMpe);
        }
        for (int channel = 0; channel < 16; channel++)
            _programs[channel] = command.programs[channel];
        applyPrograms();
        break;
    case Command::COMMAND_PANIC:
        _core->releaseVoices(-1, -1, -2, -2, true);
        _midiProcessor->resetChannels();
        break;
    }
}

void PluginEngine::applyPrograms()
{
    for (int channel = 0; channel < 16; channel++)
        _channels->setChannel(channel, _sf2Index, _programs[channel]);
}

void PluginEngine::processMidi(const uint8_t * message, size_t length)
{
    if (length >= 3 && (message[0] & 0xF0) == 0x90 && message[2] > 0)
        _channelActivity[message[0] & 0x0F].fetch_add(1, std::memory_order_relaxed);
    // The values of the controllers sent by a host are absolute (a host may send the same value again and again, which
    // would be taken for relative values coming from a device)
    _midiProcessor->processMidiMessage(message, length, false);
}

void PluginEngine::render(const SynthCore::Outputs &outputs, uint32_t frameCount)
{
    _core->renderExact(outputs, frameCount);
    _blockFrameCount += frameCount;
}

void PluginEngine::endBlock()
{
    _voiceCount.store(_core->getVoiceList()->getVoiceCount(), std::memory_order_relaxed);
    for (int channel = 0; channel < 16; channel++)
        _playedPrograms[channel].store(_sf2Index >= 0 ? _channels->getProgram(channel) : -1, std::memory_order_relaxed);

    // Computation time compared to the duration of the audio, smoothed
    if (_blockFrameCount > 0)
    {
        double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - _blockStart).count();
        float load = static_cast<float>(elapsed * _sampleRate / _blockFrameCount);
        _load.store(0.9f * _load.load(std::memory_order_relaxed) + 0.1f * load, std::memory_order_relaxed);
    }
}
