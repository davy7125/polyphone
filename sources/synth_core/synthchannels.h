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

#ifndef SYNTHCHANNELS_H
#define SYNTHCHANNELS_H

#include "imidilistener.h"
class SynthCore;
class ISynthSoundfonts;

// MIDI channels 0 to 15 playing presets with SynthCore, receiving the events processed by MidiProcessor (MIDI file
// renderer, plugin). Each channel plays a soundfont with the program chosen by the program changes (bank select then
// program change, the bank 128 being the default one of the channel 10) or with a forced program. The keys are
// transposed except for the percussions (bank 128), and a key is released with the transposition it had when it started.
// The events repeated by MidiProcessor on the channel -1 are ignored. The functions are called by the thread processing
// the MIDI events, while the soundfonts do not change.
class SynthChannels: public IMidiListener
{
public:
    SynthChannels(SynthCore * core, ISynthSoundfonts * soundfonts);

    // Soundfont played by a channel (-1 for none, the default) and forced program ("bank * 128 + preset", -1 for the
    // program chosen by the program changes)
    void setChannel(int channel, int sf2Index, int forcedProgram);

    // Transposition in semitones of the next notes
    void setTransposition(int semitones) { _transposition = semitones; }

    // Program of a channel before its first program change
    static int getDefaultProgram(int channel) { return channel == 9 ? 128 * 128 : 0; }

    // Program played by a channel: the forced one, otherwise the one chosen by the program changes
    int getProgram(int channel) const;

    // IMidiListener implementation
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override;
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override;
    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

private:
    SynthCore * _core;
    ISynthSoundfonts * _soundfonts;
    int _sf2Indexes[16];
    int _forcedPrograms[16];
    int _programs[16]; // Chosen by the program changes
    int _playedKeys[16][128]; // Key played (transposed) by each key held, -1 if none
    int _transposition;
};

#endif // SYNTHCHANNELS_H
