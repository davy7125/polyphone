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

#ifndef SYNTHEFFECTS_H
#define SYNTHEFFECTS_H

#include <cstdint>
#include <vector>
#include "synthconstants.h"

// Reverb and chorus processing chunks in place, which stop computing when their input is silent and nothing more can
// be heard. The decision is made for each chunk (COMPUTATION_CHUNK frames), whatever the number of chunks computed
// at once.
// The settings that would make a click (mix, modulation depth, width) change progressively when the effect is heard,
// at once otherwise. An effect whose mix reached 0 outputs its input: it is cleared and not computed anymore.
// Each effect has its own sample rate. The delays are the ones of the effects at 48 kHz, scaled at the other rates so
// that they last the same.
// The computation is the one of the effects of STK (the Synthesis ToolKit in C++ by Perry R. Cook and Gary P. Scavone,
// version 4.6) used before, in double precision: same output at 48 kHz.

// Chorus of STK (its first channel, the second one having another delay and modulation), processing two channels with
// the same delay
class SynthChorus
{
public:
    SynthChorus();
    SynthChorus(const SynthChorus &) = delete;
    SynthChorus &operator=(const SynthChorus &) = delete;

    // Clear the effect if the sample rate changes
    void setSampleRate(uint32_t sampleRate);

    // Settings, the mix and the depth being reached progressively
    void setTargetMix(double mix);
    void setTargetModDepth(double depth);
    void setModFrequency(double frequency); // Hz

    // False when the mix is 0: nothing to compute
    bool isOn() const { return _mix > 0.0 || _targetMix > 0.0; }

    void process(float * dataL, float * dataR, uint32_t len);

private:
    void clear();
    void advanceModulation(uint32_t len); // The modulation goes on, without computing its values
    template<bool RAMPS> void compute(float * dataL, float * dataR, uint32_t len);

    const double * _sineTable;
    uint32_t _sampleRate;
    std::vector<double> _delayLineL, _delayLineR;
    uint32_t _size; // Length of the delay lines (maximum delay + 1)
    uint32_t _inPoint; // Position written next in the delay lines
    double _delayFactor; // Delay without modulation, multiplied by 0.707 as in STK
    double _modFrequency, _modTime, _modRate; // Position and increment in the sine table
    double _mix, _targetMix, _mixStep;
    double _modDepth, _targetModDepth, _modDepthStep;
    uint32_t _silentLength; // Consecutive silent input frames
    uint32_t _filledLength; // Frames of the input in the delay lines since they have been emptied (up to their length)
};

// FreeVerb by Jezar at Dreampoint (8 low-pass feedback comb filters in parallel and 4 allpass filters in series for
// each channel), as ported in STK by Gregory Burlet. Its state is cleared, and it is not computed anymore, when its
// input has been silent and its output under REVERB_IDLE_LEVEL during REVERB_IDLE_LENGTH frames.
class SynthReverb
{
public:
    SynthReverb();
    SynthReverb(const SynthReverb &) = delete;
    SynthReverb &operator=(const SynthReverb &) = delete;

    // Clear the effect if the sample rate changes
    void setSampleRate(uint32_t sampleRate);

    // Mix reached progressively, the other settings being set directly (the width changes over a chunk)
    void setTargetMix(double mix);
    void setRoomSize(double value);
    void setDamping(double value);
    void setWidth(double value);

    // False when the mix is 0: nothing to compute
    bool isOn() const { return _mix > 0.0 || _targetMix > 0.0; }

    void process(float * dataL, float * dataR, uint32_t len);

private:
    static const int COMB_COUNT = 8;
    static const int ALLPASS_COUNT = 4;

    struct DelayLine
    {
        double * data;
        uint32_t size;
        uint32_t position;
    };
    struct CombState;

    void setMix(double mix);
    void updateCoefficients();
    void clear();
    void processChunk(float * dataL, float * dataR, uint32_t len); // len <= COMPUTATION_CHUNK
    void computeWetSignal(const float * dataL, const float * dataR, uint32_t len); // In _wetL and _wetR

    uint32_t _sampleRate;
    std::vector<double> _memory; // All the delay lines
    DelayLine _combL[COMB_COUNT], _combR[COMB_COUNT], _allpassL[ALLPASS_COUNT], _allpassR[ALLPASS_COUNT];
    double _combFilterL[COMB_COUNT], _combFilterR[COMB_COUNT]; // Last outputs of the low-pass filters of the combs
    double _wetL[COMPUTATION_CHUNK], _wetR[COMPUTATION_CHUNK]; // Wet signal of a chunk

    // Settings and coefficients
    double _mix, _targetMix, _mixStep;
    double _roomSize, _damping, _width;
    double _wet1, _wet2, _dry; // Mixing coefficients of the current settings
    double _lastWet1, _lastWet2, _lastDry; // Mixing coefficients at the end of the last chunk computed

    uint32_t _silentLength; // Consecutive frames with a silent input and an output under REVERB_IDLE_LEVEL
    bool _isIdle;
};

#endif // SYNTHEFFECTS_H
