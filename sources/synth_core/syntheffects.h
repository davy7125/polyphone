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

// Chorus computing its first channel only (the one used), and only the modulation once the delay line is empty:
// same output as stk::Chorus
class SynthChorus : public stk::Chorus
{
public:
    SynthChorus() : _silentLength(0) {}

    void process(float * data, uint32_t len);

private:
    uint32_t _silentLength; // Consecutive silent input frames
};

// Reverb whose state is cleared, and which is not computed anymore, when its input has been silent and its output
// under REVERB_IDLE_LEVEL during REVERB_IDLE_LENGTH frames
class SynthReverb : public stk::FreeVerb
{
public:
    SynthReverb() : _silentLength(0), _isIdle(true) {}

    void process(float * dataL, float * dataR, uint32_t len);

private:
    uint32_t _silentLength; // Consecutive frames with a silent input and an output under REVERB_IDLE_LEVEL
    bool _isIdle;
};

#endif // SYNTHEFFECTS_H
