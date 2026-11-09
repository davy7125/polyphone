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
    _transposition(0)
{
    for (int i = 0; i < 16; i++)
    {
        _sf2Indexes[i] = -1;
        _forcedPrograms[i] = -1;
        _programs[i] = getDefaultProgram(i);
        for (int key = 0; key < 128; key++)
            _playedKeys[i][key] = -1;
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

bool SynthChannels::processKey(int channel, int key, int vel)
{
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
    int program = _forcedPrograms[channel] >= 0 ? _forcedPrograms[channel] : _programs[channel];
    if (sf2Index < 0 || program < 0)
        return true;

    // Transposition, except for the percussions
    int playedKey = key;
    if (program / 128 != 128)
        playedKey += _transposition;
    if (playedKey < 0 || playedKey > 127)
        return true;

    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(sf2Index);
    int presetIndex = soundfont == nullptr ? -1 : SynthMidi::findPreset(soundfont, program / 128, program % 128);
    if (presetIndex >= 0)
    {
        _playedKeys[channel][key] = playedKey;
        _core->playPreset(sf2Index, presetIndex, channel, playedKey, vel);
    }
    return true;
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
