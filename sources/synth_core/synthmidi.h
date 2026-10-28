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

#ifndef SYNTHMIDI_H
#define SYNTHMIDI_H

#include "imidilistener.h"
#include "isynthsoundfont.h"
class SynthCore;

// Plays a soundfont with the MIDI events processed by MidiProcessor, a program change choosing the preset of a
// channel by its bank and its number (bank 0 and program 0 by default, bank 128 on the channel 10)
// The events of the channels 0 to 15 are consumed
class SynthMidi: public IMidiListener
{
public:
    SynthMidi(SynthCore * core, ISynthSoundfonts * soundfonts, int sf2Id);

    // Index of the preset having this bank and this number, -1 if there is none
    static int findPreset(ISynthSoundfont * soundfont, int bank, int preset);

    // IMidiListener implementation
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override;
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override;
    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

private:
    int findPreset(int bank, int preset);

    SynthCore * _core;
    ISynthSoundfonts * _soundfonts;
    int _sf2Id;
    int _presetIndexes[16]; // -2 until a program is chosen
};

#endif // SYNTHMIDI_H
