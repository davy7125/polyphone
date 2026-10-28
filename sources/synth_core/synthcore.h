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

#ifndef SYNTHCORE_H
#define SYNTHCORE_H

#include <atomic>
#include <mutex>
#include "voicelist.h"
#include "synthbuses.h"
#include "isynthsoundfont.h"
#include "isynthlistener.h"
#include "imidivalues.h"
#include "voicecontext.h"
#include "synthcoreconfig.h"
#include "syntheffects.h"

// Soundfont synthesizer without dependency: it plays the soundfonts given by ISynthSoundfonts
// The editor adds the multi-threaded computation, the effects for the samples and the recording (Synth)
class SynthCore
{
public:
    // The listener (possibly null) is notified when a sample is played alone
    SynthCore(ISynthSoundfonts * soundfonts, ISynthListener * listener = nullptr);
    ~SynthCore();

    // Tables shared by all the synths, to prepare before using one (only computed the first time)
    static void prepareTables();

    // Configuration (not by the audio thread)
    void configure(const SynthCoreConfig &configuration);
    void setMidiValues(IMidiValues * midiValues); // Not deleted by the synth (null: default values)
    void setSampleRate(uint32_t sampleRate);
    uint32_t getSampleRate() { return _sampleRate; }

    // Notes, by a MIDI or GUI thread while the soundfonts do not change
    // A sample is played with its stereo part, an instrument or a preset with the ones that are "always played"
    // The token identifies the reading of a sample (-1 otherwise)
    int playSample(int sf2Id, int sampleIndex, int channel, int key, int velocity);
    void playInstrument(int sf2Id, int instrumentIndex, int channel, int key, int velocity);
    void playPreset(int sf2Id, int presetIndex, int channel, int key, int velocity);

    // sf2Id: -1 (no filter) or specific sf2 id
    // presetId: -1 (no filter) or specific preset id
    // channel: -2 (all channels), -1 (GUI channel) or [0 - 15] (MIDI channel)
    // key: -2 (all keys), -1 (all keys < 0) or a specific key
    void releaseVoices(int sf2Id, int presetId, int channel, int key, bool fast);
    void stop(bool allChannels, bool fast);

    // Configuration for reading samples alone
    void setSampleGain(double gain);
    void setStereo(bool isStereoEnabled);
    bool isStereoEnabled() { return _smplIsStereoEnabled; }
    void setLoopEnabled(bool isEnabled);

    // Editing parameters of the playing samples
    void setStartLoop(uint32_t startLoop, bool withLinkedSample);
    void setEndLoop(uint32_t endLoop, bool withLinkedSample);
    void setPitchCorrection(int16_t correction, bool withLinkedSample);

    // MIDI controllers (the values read by the modulators are given by IMidiValues)
    void processPolyPressureChanged(int channel, int key, int pressure);
    void processMonoPressureChanged(int channel, int value);
    void processControllerChanged(int channel, int num, int value);
    void processBendChanged(int channel, float value);
    void processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger);

    // Maximum number of frames computed at once, for allocating the buffers (not by the audio thread)
    // The default is DEFAULT_MAX_BLOCK_SIZE, render accepts more but computes them in several times
    void setMaxBlockSize(uint32_t maxBlockSize);

    // Maximum number of voices that are not fading out (DEFAULT_POLYPHONY by default), the quietest notes fading out
    // above it. The voices are allocated for it: if the number changes all notes are stopped, and no audio must be
    // computed meanwhile.
    void setMaxPolyphony(int maxPolyphony);

    // In real time (default), the quietest notes fade out if computing the voices takes more than a share of the
    // duration of the audio (configuration). Offline (a rendering to a file), the computation time is not limited.
    void setRealTime(bool isRealTime);

    // Compute len frames in the calling thread, any length: voices, effects and clipping
    // The voices are computed by whole chunks (COMPUTATION_CHUNK): the frames computed and not asked are returned
    // first by the next call, and an event (note, controller...) is heard from the next chunk computed
    void render(float * dataL, float * dataR, uint32_t len);

    // For a computation of the voices by several threads (editor)
    VoiceList * getVoiceList() { return &_voices; }
    void applyEffects(float * dataL, float * dataR, SynthBuses &buses, uint32_t len); // Aligned as the buses, whole chunks

private:
    void playPrst(int sf2Id, ISynthSoundfont * soundfont, ISynthInstPrst * prst, int channel, int key, int velocity);
    void playInst(int sf2Id, ISynthSoundfont * soundfont, ISynthInstPrst * inst, int channel, int key, int velocity,
                  ISynthInstPrst * prst = nullptr, ISynthZone * prstZone = nullptr);
    int playSmpl(int sf2Id, ISynthSample * smpl, int channel, int key, int velocity, bool other,
                 ISynthInstPrst * inst = nullptr, ISynthZone * instZone = nullptr,
                 ISynthInstPrst * prst = nullptr, ISynthZone * prstZone = nullptr);
    void configureVoice(Voice * voice);
    void configureVoices();
    void renderChunks(uint32_t len);

    // Soundfonts
    ISynthSoundfonts * _soundfonts;

    // Configuration
    SynthCoreConfig _configuration;
    uint32_t _sampleRate;
    bool _reverbOn;
    bool _chorusOn;
    double _smplGain;
    bool _smplIsStereoEnabled;
    bool _smplIsLoopEnabled;
    VoiceContext _context; // Read by the voices

    // Voices, the other threads than the audio one using them one at a time
    VoiceList _voices;
    SynthBuses _buses;
    std::atomic<int> _sampleVoiceTokenCounter;
    std::recursive_mutex _voicesMutex;
    int _noteId; // Shared by the voices started by a key

    // Result of the last chunks computed, aligned as the buses
    float * _outputL, * _outputR;
    uint32_t _outputPosition;  // First frame not returned
    uint32_t _outputAvailable; // Frames not returned

    // Effects
    SynthReverb _reverb;
    SynthChorus _chorusRevL, _chorusRevR, _chorusL, _chorusR;
    std::atomic<int> _effectsInUse;
};

#endif // SYNTHCORE_H
