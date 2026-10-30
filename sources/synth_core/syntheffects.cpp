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
#include <cmath>

// Frames for a progressive change of a setting, whatever its amplitude (43 ms at 48 kHz)
static const double s_rampLength = 2048.0;

// Value moved toward the target by step at most
static inline stk::StkFloat approach(stk::StkFloat value, stk::StkFloat target, stk::StkFloat step)
{
    if (target - value > step)
        return value + step;
    if (value - target > step)
        return value - step;
    return target;
}

SynthChorus::SynthChorus() :
    _targetMix(0.0),
    _mixStep(0.0),
    _targetModDepth(0.0),
    _modDepthStep(0.0)
{
    // Off, with an empty delay line (the input is not known before the chorus is computed)
    effectMix_ = 0.0;
    modDepth_ = 0.0;
    _silentLength = delayLine_[0].getMaximumDelay() + 1;
    _filledLength = 0;
}

void SynthChorus::setTargetMix(stk::StkFloat mix)
{
    if (mix == _targetMix)
        return;
    _targetMix = mix;
    _mixStep = std::abs(mix - effectMix_) / s_rampLength;
}

void SynthChorus::setTargetModDepth(stk::StkFloat depth)
{
    if (depth == _targetModDepth)
        return;
    _targetModDepth = depth;
    _modDepthStep = std::abs(depth - modDepth_) / s_rampLength;
}

void SynthChorus::process(float * data, uint32_t len)
{
    if (!isOn())
        return;

    // After a silent input as long as the delay line, it contains only zeros: the output is silent (and the delay line
    // does not change), only the modulation goes on and the settings can change at once
    bool isSilent = FastMaths::isZero(data, len);
    if (isSilent && _silentLength > delayLine_[0].getMaximumDelay())
    {
        effectMix_ = _targetMix;
        modDepth_ = _targetModDepth;
        _filledLength = _silentLength;
        for (uint32_t i = 0; i < len; i++)
            mods_[0].tick();
        return;
    }
    _silentLength = isSilent ? _silentLength + len : 0;

    stk::StkFloat input;
    if (effectMix_ == _targetMix && modDepth_ == _targetModDepth)
    {
        // Same computation as stk::Chorus::tick for the first channel
        for (uint32_t i = 0; i < len; i++)
        {
            input = static_cast<stk::StkFloat>(data[i]);
            delayLine_[0].setDelay(baseLength_ * 0.707 * (1.0 + modDepth_ * mods_[0].tick()));
            data[i] = static_cast<float>(effectMix_ * (delayLine_[0].tick(input) - input) + input);
        }
    }
    else
    {
        // The same, with the mix and the depth getting closer to their targets at each frame
        // A chorus turned on while its input is not silent fades in once its delay line is filled, otherwise the
        // delayed input would start abruptly
        uint32_t maximumDelay = static_cast<uint32_t>(delayLine_[0].getMaximumDelay());
        for (uint32_t i = 0; i < len; i++)
        {
            if (effectMix_ > 0.0 || _filledLength > maximumDelay)
                effectMix_ = approach(effectMix_, _targetMix, _mixStep);
            else
                _filledLength++;
            modDepth_ = approach(modDepth_, _targetModDepth, _modDepthStep);
            input = static_cast<stk::StkFloat>(data[i]);
            delayLine_[0].setDelay(baseLength_ * 0.707 * (1.0 + modDepth_ * mods_[0].tick()));
            data[i] = static_cast<float>(effectMix_ * (delayLine_[0].tick(input) - input) + input);
        }

        // Faded out: the output is the input from now on, the delay line is emptied for the next time it is used
        if (effectMix_ == 0.0 && _targetMix == 0.0)
        {
            this->clear();
            _silentLength = maximumDelay + 1;
            _filledLength = 0;
        }
    }
}

SynthReverb::SynthReverb() :
    _silentLength(0),
    _isIdle(true),
    _targetMix(0.0),
    _mixStep(0.0)
{
    // Off
    FreeVerb::setEffectMix(0.0);
    _wet1 = wet1_;
    _wet2 = wet2_;
    _dry = dry_;
}

void SynthReverb::setTargetMix(stk::StkFloat mix)
{
    if (mix == _targetMix)
        return;
    _targetMix = mix;
    _mixStep = std::abs(mix - effectMix_) / s_rampLength;
}

void SynthReverb::process(float * dataL, float * dataR, uint32_t len)
{
    if (!isOn())
        return;

    // Nothing to compute: the state is cleared and the input is silent (the mix can change at once)
    bool isSilent = FastMaths::isZero(dataL, len) && FastMaths::isZero(dataR, len);
    if (isSilent && _isIdle)
    {
        if (effectMix_ != _targetMix)
            FreeVerb::setEffectMix(_targetMix);
        _wet1 = wet1_;
        _wet2 = wet2_;
        _dry = dry_;
        return;
    }
    _isIdle = false;

    // Mix at the end of the chunk, closer to the target
    if (effectMix_ != _targetMix)
        FreeVerb::setEffectMix(approach(effectMix_, _targetMix, _mixStep * len));

    if (wet1_ == _wet1 && wet2_ == _wet2 && dry_ == _dry)
    {
        for (uint32_t i = 0; i < len; i++)
        {
            dataL[i] = static_cast<float>(tick(static_cast<stk::StkFloat>(dataL[i]), static_cast<stk::StkFloat>(dataR[i])));
            dataR[i] = static_cast<float>(lastOut(1));
        }
    }
    else
    {
        // The mixing coefficients (mix, width) go linearly from the ones of the previous chunk to the new ones
        stk::StkFloat wet1 = wet1_;
        stk::StkFloat wet2 = wet2_;
        stk::StkFloat dry = dry_;
        for (uint32_t i = 0; i < len; i++)
        {
            stk::StkFloat progress = static_cast<stk::StkFloat>(i + 1) / len;
            wet1_ = _wet1 + progress * (wet1 - _wet1);
            wet2_ = _wet2 + progress * (wet2 - _wet2);
            dry_ = _dry + progress * (dry - _dry);
            dataL[i] = static_cast<float>(tick(static_cast<stk::StkFloat>(dataL[i]), static_cast<stk::StkFloat>(dataR[i])));
            dataR[i] = static_cast<float>(lastOut(1));
        }
        _wet1 = wet1_ = wet1;
        _wet2 = wet2_ = wet2;
        _dry = dry_ = dry;

        // Faded out: the output is the input from now on, the state is cleared for the next time the reverb is used
        if (effectMix_ == 0.0)
        {
            clear();
            _isIdle = true;
            _silentLength = 0;
            return;
        }
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
