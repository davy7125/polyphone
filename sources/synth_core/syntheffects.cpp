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

#include "syntheffects.h"
#include "fastmaths.h"
#include "synthconstants.h"

void SynthChorus::process(float * data, uint32_t len)
{
    // After a silent input as long as the delay line, it contains only zeros: the output is silent (and the delay line
    // does not change), only the modulation goes on
    bool isSilent = FastMaths::isZero(data, len);
    if (isSilent && _silentLength > delayLine_[0].getMaximumDelay())
    {
        for (uint32_t i = 0; i < len; i++)
            mods_[0].tick();
        return;
    }
    _silentLength = isSilent ? _silentLength + len : 0;

    // Same computation as stk::Chorus::tick for the first channel
    stk::StkFloat input;
    for (uint32_t i = 0; i < len; i++)
    {
        input = static_cast<stk::StkFloat>(data[i]);
        delayLine_[0].setDelay(baseLength_ * 0.707 * (1.0 + modDepth_ * mods_[0].tick()));
        data[i] = static_cast<float>(effectMix_ * (delayLine_[0].tick(input) - input) + input);
    }
}

void SynthReverb::process(float * dataL, float * dataR, uint32_t len)
{
    // Nothing to compute: the state is cleared and the input is silent
    bool isSilent = FastMaths::isZero(dataL, len) && FastMaths::isZero(dataR, len);
    if (isSilent && _isIdle)
        return;
    _isIdle = false;

    for (uint32_t i = 0; i < len; i++)
    {
        dataL[i] = static_cast<float>(tick(static_cast<stk::StkFloat>(dataL[i]), static_cast<stk::StkFloat>(dataR[i])));
        dataR[i] = static_cast<float>(lastOut(1));
    }

    // Possibly nothing more to hear: the state is cleared
    if (isSilent && FastMaths::isUnder(dataL, len, REVERB_IDLE_LEVEL) && FastMaths::isUnder(dataR, len, REVERB_IDLE_LEVEL))
    {
        _silentLength += len;
        if (_silentLength >= REVERB_IDLE_LENGTH)
        {
            clear();
            _isIdle = true;
            _silentLength = 0;
        }
    }
    else
        _silentLength = 0;
}
