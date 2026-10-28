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

#include "synthmidi.h"
#include "synthcore.h"

SynthMidi::SynthMidi(SynthCore * core, ISynthSoundfonts * soundfonts, int sf2Id) :
    _core(core),
    _soundfonts(soundfonts),
    _sf2Id(sf2Id)
{
    for (int i = 0; i < 16; i++)
        _presetIndexes[i] = -2;
}

int SynthMidi::findPreset(ISynthSoundfont * soundfont, int bank, int preset)
{
    for (int i = 0; i < soundfont->getPresetCount(); i++)
    {
        ISynthInstPrst * prst = soundfont->getPresetAt(i);
        if (prst->getBankNumber() == bank && prst->getPresetNumber() == preset)
            return prst->getIndex();
    }
    return -1;
}

int SynthMidi::findPreset(int bank, int preset)
{
    ISynthSoundfont * soundfont = _soundfonts->getSynthSoundfont(_sf2Id);
    return soundfont == nullptr ? -1 : findPreset(soundfont, bank, preset);
}

bool SynthMidi::processKey(int channel, int key, int vel)
{
    if (channel < 0 || channel > 15)
        return false;

    if (vel == 0)
    {
        // The key is released whatever the preset
        _core->releaseVoices(_sf2Id, -1, channel, key, false);
        return true;
    }

    // Default program
    if (_presetIndexes[channel] == -2)
        _presetIndexes[channel] = findPreset(channel == 9 ? 128 : 0, 0);
    if (_presetIndexes[channel] >= 0)
        _core->playPreset(_sf2Id, _presetIndexes[channel], channel, key, vel);
    return true;
}

bool SynthMidi::processPolyPressureChanged(int channel, int key, int pressure)
{
    _core->processPolyPressureChanged(channel, key, pressure);
    return channel >= 0 && channel <= 15;
}

bool SynthMidi::processMonoPressureChanged(int channel, int value)
{
    _core->processMonoPressureChanged(channel, value);
    return channel >= 0 && channel <= 15;
}

bool SynthMidi::processControllerChanged(int channel, int num, int value)
{
    _core->processControllerChanged(channel, num, value);
    return channel >= 0 && channel <= 15;
}

bool SynthMidi::processBendChanged(int channel, float value)
{
    _core->processBendChanged(channel, value);
    return channel >= 0 && channel <= 15;
}

bool SynthMidi::processProgramChanged(int channel, uint16_t bank, uint8_t preset)
{
    if (channel < 0 || channel > 15)
        return false;
    _presetIndexes[channel] = findPreset(bank, preset);
    return true;
}

bool SynthMidi::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    _core->processRPNChanged(channel, parameter, value, isRegistered, trigger);
    return channel >= 0 && channel <= 15;
}
