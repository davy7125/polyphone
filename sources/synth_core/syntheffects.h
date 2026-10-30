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
#include "stk/FreeVerb.h"
#include "stk/Chorus.h"

// Effects of STK processing chunks in place, which stop computing when their input is silent and nothing more can
// be heard. The decision is made for each chunk (COMPUTATION_CHUNK frames), whatever the number of chunks computed
// at once.
// The settings that would make a click (mix, modulation depth, width) change progressively when the effect is heard,
// at once otherwise. An effect whose mix reached 0 outputs its input: it is cleared and not computed anymore.

// Chorus computing its first channel only (the one used), and only the modulation once the delay line is empty:
// same output as stk::Chorus
class SynthChorus : public stk::Chorus
{
public:
    SynthChorus();

    // Settings reached progressively
    void setTargetMix(stk::StkFloat mix);
    void setTargetModDepth(stk::StkFloat depth);

    // False when the mix is 0: nothing to compute
    bool isOn() { return effectMix_ > 0.0 || _targetMix > 0.0; }

    void process(float * data, uint32_t len);

private:
    uint32_t _silentLength; // Consecutive silent input frames
    uint32_t _filledLength; // Frames of the input in the delay line since it has been emptied (up to its length)
    stk::StkFloat _targetMix, _mixStep;
    stk::StkFloat _targetModDepth, _modDepthStep;
};

// Reverb whose state is cleared, and which is not computed anymore, when its input has been silent and its output
// under REVERB_IDLE_LEVEL during REVERB_IDLE_LENGTH frames
class SynthReverb : public stk::FreeVerb
{
public:
    SynthReverb();

    // Mix reached progressively, the other settings being set directly (the width changes over a chunk)
    void setTargetMix(stk::StkFloat mix);

    // False when the mix is 0: nothing to compute
    bool isOn() { return effectMix_ > 0.0 || _targetMix > 0.0; }

    void process(float * dataL, float * dataR, uint32_t len);

private:
    uint32_t _silentLength; // Consecutive frames with a silent input and an output under REVERB_IDLE_LEVEL
    bool _isIdle;
    stk::StkFloat _targetMix, _mixStep;
    stk::StkFloat _wet1, _wet2, _dry; // Mixing coefficients at the end of the last chunk computed
};

#endif // SYNTHEFFECTS_H
