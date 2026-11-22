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

#include "synthchannels.h"
#include "synthcore.h"
#include "synthmidi.h"

SynthChannels::SynthChannels(SynthCore * core, ISynthSoundfonts * soundfonts) :
    _core(core),
    _soundfonts(soundfonts),
    _transposition(0),
    _playMode(PLAY_MODE_POLY),
    _notePriority(PRIORITY_LATEST),
    _glideSeconds(0.0f)
{
    for (int i = 0; i < 16; i++)
    {
        _sf2Indexes[i] = -1;
        _forcedPrograms[i] = -1;
        _programs[i] = getDefaultProgram(i);
        _monoChannels[i].batchDepth = 0;
    }
    forgetKeys();
}

void SynthChannels::setPlayMode(PlayMode mode, NotePriority priority, float glideSeconds)
{
    if (mode != _playMode)
    {
        // The notes heard are released as in the polyphonic mode (_playedKeys)
        for (MonoChannel &mono : _monoChannels)
        {
            for (uint32_t &order : mono.keyOrders)
                order = 0;
            mono.heardKey = -1;
        }
        _playMode = mode;
    }
    _notePriority = priority;
    _glideSeconds = glideSeconds < 0.0f ? 0.0f : glideSeconds;
}

void SynthChannels::forgetKeys()
{
    for (int i = 0; i < 16; i++)
    {
        for (int key = 0; key < 128; key++)
        {
            _playedKeys[i][key] = -1;
            _monoChannels[i].keyOrders[key] = 0;
            _monoChannels[i].velocities[key] = 0;
        }
        _monoChannels[i].lastOrder = 0;
        _monoChannels[i].heardKey = -1;
        _monoChannels[i].heardSf2Index = -1;
        _monoChannels[i].heardPresetIndex = -1;
    }
}

void SynthChannels::setChannel(int channel, int sf2Index, int forcedProgram)
{
    if (channel < 0 || channel >= 16)
        return;
    _sf2Indexes[channel] = sf2Index;
    _forcedPrograms[channel] = forcedProgram;
}

int SynthChannels::getProgram(int channel) const
{
    if (channel < 0 || channel >= 16)
        return -1;
    return _forcedPrograms[channel] >= 0 ? _forcedPrograms[channel] : _programs[channel];
}

bool SynthChannels::getPreset(int channel, int key, int &sf2Index, int &presetIndex, int &playedKey)
{
    sf2Index = _sf2Indexes[channel];
    int program = _forcedPrograms[channel] >= 0 ? _forcedPrograms[channel] : _programs[channel];
    if (sf2Index < 0 || program < 0)
        return false;

    // Transposition, except for the percussions
    playedKey = key;
    if (program / 128 != 128)
        playedKey += _transposition;
    if (playedKey < 0 || playedKey > 127)
        return false;

    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Index);
    presetIndex = soundfont == nullptr ? -1 : SynthMidi::findPreset(soundfont, program / 128, program % 128);
    return presetIndex >= 0;
}

bool SynthChannels::isMonophonic(int channel) const
{
    return _playMode != PLAY_MODE_POLY && getProgram(channel) / 128 != 128;
}

bool SynthChannels::processKey(int channel, int key, int vel)
{
    if (channel < 0 || channel >= 16)
        return true;

    if (vel == 0)
    {
        // Key held in the monophonic mode: possibly another key heard
        MonoChannel &mono = _monoChannels[channel];
        if (mono.keyOrders[key] != 0)
        {
            mono.keyOrders[key] = 0;
            if (mono.batchDepth == 0)
                updateMonoNote(channel, -1);
            return true;
        }

        // Key released whatever the preset playing it, with the transposition it had when it started
        int playedKey = _playedKeys[channel][key];
        if (playedKey >= 0)
        {
            _playedKeys[channel][key] = -1;
            _core->releaseVoices(-1, -1, channel, playedKey, false);
        }
        return true;
    }

    int sf2Index, presetIndex, playedKey;
    if (!getPreset(channel, key, sf2Index, presetIndex, playedKey))
        return true;

    // Monophonic mode: the key is held, the note heard depending on the priority
    if (isMonophonic(channel))
    {
        MonoChannel &mono = _monoChannels[channel];
        mono.keyOrders[key] = ++mono.lastOrder;
        mono.velocities[key] = static_cast<uint8_t>(vel);
        if (mono.batchDepth == 0)
            updateMonoNote(channel, key);
        return true;
    }

    // A key played again with another transposition first stops its previous note
    int previousKey = _playedKeys[channel][key];
    if (previousKey >= 0 && previousKey != playedKey)
        _core->releaseVoices(-1, -1, channel, previousKey, false);
    _playedKeys[channel][key] = playedKey;
    _core->playPreset(sf2Index, presetIndex, channel, playedKey, vel);
    return true;
}

bool SynthChannels::processKeyRestarted(int channel, int key, int vel)
{
    // Monophonic mode: the key is pressed again without being released first (its note starting again if it is heard)
    if (channel >= 0 && channel < 16 && _monoChannels[channel].keyOrders[key] != 0 && isMonophonic(channel))
        return processKey(channel, key, vel);
    return IMidiListener::processKeyRestarted(channel, key, vel);
}

void SynthChannels::beginKeyBatch(int channel)
{
    if (channel >= 0 && channel < 16)
        _monoChannels[channel].batchDepth++;
}

void SynthChannels::endKeyBatch(int channel)
{
    if (channel < 0 || channel >= 16 || _monoChannels[channel].batchDepth == 0)
        return;
    if (--_monoChannels[channel].batchDepth == 0 && isMonophonic(channel))
        updateMonoNote(channel, -1);
}

void SynthChannels::updateMonoNote(int channel, int pressedKey)
{
    MonoChannel &mono = _monoChannels[channel];

    // Key having the priority among the keys held
    int key = -1;
    for (int i = 0; i < 128; i++)
    {
        if (mono.keyOrders[i] != 0 &&
            (key < 0 || _notePriority == PRIORITY_HIGHEST ||
             (_notePriority == PRIORITY_LATEST && mono.keyOrders[i] > mono.keyOrders[key])))
            key = i;
    }
    if (key == mono.heardKey && key != pressedKey)
        return; // Same note heard

    int previousPlayedKey = mono.heardKey >= 0 ? _playedKeys[channel][mono.heardKey] : -1;
    if (mono.heardKey >= 0)
        _playedKeys[channel][mono.heardKey] = -1;
    mono.heardKey = -1;

    // No key held anymore: the note ends with its release
    if (key < 0)
    {
        if (previousPlayedKey >= 0)
            _core->releaseVoices(-1, -1, channel, previousPlayedKey, false);
        return;
    }

    int sf2Index, presetIndex, playedKey;
    bool canPlay = getPreset(channel, key, sf2Index, presetIndex, playedKey);
    if (canPlay && _playMode == PLAY_MODE_LEGATO && previousPlayedKey >= 0 && previousPlayedKey != playedKey &&
        sf2Index == mono.heardSf2Index && presetIndex == mono.heardPresetIndex)
    {
        // The note continues with the new key
        _core->playPresetLegato(sf2Index, presetIndex, channel, previousPlayedKey, playedKey, mono.velocities[key],
                                _glideSeconds);
    }
    else
    {
        // The notes of the channel (including the ones in their release) stop quickly, the new one starts with its attack
        _core->releaseVoices(-1, -1, channel, -2, true);
        if (canPlay)
            _core->playPreset(sf2Index, presetIndex, channel, playedKey, mono.velocities[key]);
    }

    if (canPlay)
    {
        mono.heardKey = key;
        mono.heardSf2Index = sf2Index;
        mono.heardPresetIndex = presetIndex;
        _playedKeys[channel][key] = playedKey;
    }
}

bool SynthChannels::processPolyPressureChanged(int channel, int key, int pressure)
{
    if (channel >= 0 && channel < 16 && _playedKeys[channel][key] >= 0)
        _core->processPolyPressureChanged(channel, _playedKeys[channel][key], pressure);
    return true;
}

bool SynthChannels::processMonoPressureChanged(int channel, int value)
{
    if (channel >= 0 && channel < 16)
        _core->processMonoPressureChanged(channel, value);
    return true;
}

bool SynthChannels::processControllerChanged(int channel, int num, int value)
{
    if (channel >= 0 && channel < 16)
        _core->processControllerChanged(channel, num, value);
    return true;
}

bool SynthChannels::processBendChanged(int channel, float value)
{
    if (channel >= 0 && channel < 16)
        _core->processBendChanged(channel, value);
    return true;
}

bool SynthChannels::processProgramChanged(int channel, uint16_t bank, uint8_t preset)
{
    if (channel >= 0 && channel < 16)
        _programs[channel] = bank * 128 + preset;
    return true;
}

bool SynthChannels::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    if (channel >= 0 && channel < 16)
        _core->processRPNChanged(channel, parameter, value, isRegistered, trigger);
    return true;
}
