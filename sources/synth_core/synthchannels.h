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

    // Play mode of the channels, the percussions (bank 128) being always polyphonic: polyphonic (default), or one note at
    // a time, heard among the keys held according to a priority (the latest key pressed, the highest or the lowest one).
    // When the key heard is released, the key held that has the priority is heard again.
    // - Monophonic: each note starts with its attack, the notes of the channel stopping quickly.
    // - Legato: a note following another one that is still held continues it (SynthCore::playPresetLegato), its pitch
    //   gliding from the previous one during glideSeconds (0: no glide). A note following no other one starts with its
    //   attack, as in the monophonic mode.
    // The keys held are forgotten when the mode changes (the notes heard are released as in the polyphonic mode).
    enum PlayMode
    {
        PLAY_MODE_POLY,
        PLAY_MODE_MONO,
        PLAY_MODE_LEGATO
    };
    enum NotePriority
    {
        PRIORITY_LATEST,
        PRIORITY_HIGHEST,
        PRIORITY_LOWEST
    };
    void setPlayMode(PlayMode mode, NotePriority priority, float glideSeconds);

    // Forget the keys held (all sounds stopped)
    void forgetKeys();

    // Program of a channel before its first program change
    static int getDefaultProgram(int channel) { return channel == 9 ? 128 * 128 : 0; }

    // Program played by a channel: the forced one, otherwise the one chosen by the program changes
    int getProgram(int channel) const;

    // IMidiListener implementation
    bool processKey(int channel, int key, int vel) override;
    bool processKeyRestarted(int channel, int key, int vel) override;
    void beginKeyBatch(int channel) override;
    void endKeyBatch(int channel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override;
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override;
    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

private:
    // Soundfont, preset and key (transposed) played by a key of a channel, false if nothing can be played
    bool getPreset(int channel, int key, int &sf2Index, int &presetIndex, int &playedKey);

    // Monophonic play mode: key heard after a key is pressed (pressedKey, possibly the key heard pressed again) or released
    // (pressedKey -1)
    bool isMonophonic(int channel) const;
    void updateMonoNote(int channel, int pressedKey);

    SynthCore * _core;
    ISynthSoundfonts * _soundfonts;
    int _sf2Indexes[16];
    int _forcedPrograms[16];
    int _programs[16]; // Chosen by the program changes
    int _playedKeys[16][128]; // Key played (transposed) by each key held, -1 if none
    int _transposition;

    // Monophonic play mode
    PlayMode _playMode;
    NotePriority _notePriority;
    float _glideSeconds;
    struct MonoChannel
    {
        uint32_t keyOrders[128]; // 0 if the key is not held, otherwise the order in which it was pressed
        uint8_t velocities[128];
        uint32_t lastOrder;
        int heardKey; // Key whose note is heard, -1 if none
        int heardSf2Index, heardPresetIndex;
        int batchDepth; // Keys released at once
    };
    MonoChannel _monoChannels[16];
};

#endif // SYNTHCHANNELS_H
