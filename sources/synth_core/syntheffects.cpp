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
#include <algorithm>
#include <cmath>

// Frames for a progressive change of a setting, whatever its amplitude (43 ms at 48 kHz)
static const double s_rampLength = 2048.0;

// Sample rate of the delays given in frames
static const uint32_t s_referenceSampleRate = 48000;

// Value moved toward the target by step at most
static inline double approach(double value, double target, double step)
{
    if (target - value > step)
        return value + step;
    if (value - target > step)
        return value - step;
    return target;
}

// Length in frames at a sample rate of a length in frames at 48 kHz
static inline uint32_t scaleLength(uint32_t length, uint32_t sampleRate)
{
    return std::max<uint32_t>(1, static_cast<uint32_t>(static_cast<uint64_t>(length) * sampleRate / s_referenceSampleRate));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Chorus
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Sine table of STK (stk::SineWave), the first value being repeated at the end for the linear interpolation
static const uint32_t s_sineTableSize = 2048;

static const double * getSineTable()
{
    static const std::vector<double> table = []() {
        const double twoPi = 2 * 3.14159265358979; // Value of STK
        const double step = 1.0 / s_sineTableSize;
        std::vector<double> values(s_sineTableSize + 1);
        for (uint32_t i = 0; i <= s_sineTableSize; i++)
            values[i] = std::sin(twoPi * i * step);
        return values;
    }();
    return table.data();
}

SynthChorus::SynthChorus() :
    _sineTable(getSineTable()),
    _sampleRate(0),
    _size(0),
    _inPoint(0),
    _delayFactor(0.0),
    _modFrequency(0.2),
    _modTime(0.0),
    _modRate(0.0),
    _mix(0.0), // Off
    _targetMix(0.0),
    _mixStep(0.0),
    _modDepth(0.0),
    _targetModDepth(0.0),
    _modDepthStep(0.0),
    _silentLength(0),
    _filledLength(0)
{
    setSampleRate(s_referenceSampleRate);
}

void SynthChorus::setSampleRate(uint32_t sampleRate)
{
    if (sampleRate == 0 || sampleRate == _sampleRate)
        return;
    _sampleRate = sampleRate;

    // Delay of 6000 frames at 48 kHz without modulation (default of stk::Chorus), in delay lines able to contain
    // 1.414 times this delay + 2 (the modulation multiplies it by 0.707 x [0; 2], the depth being at most 1)
    double baseDelay = 6000.0 * sampleRate / s_referenceSampleRate;
    _delayFactor = baseDelay * 0.707;
    _size = static_cast<uint32_t>(baseDelay * 1.414) + 3;
    _delayLineL.assign(_size, 0.0);
    _delayLineR.assign(_size, 0.0);
    _inPoint = 0;
    _modRate = s_sineTableSize * _modFrequency / sampleRate;
    clear();
}

void SynthChorus::setTargetMix(double mix)
{
    if (mix == _targetMix)
        return;
    _targetMix = mix;
    _mixStep = std::abs(mix - _mix) / s_rampLength;
}

void SynthChorus::setTargetModDepth(double depth)
{
    if (depth == _targetModDepth)
        return;
    _targetModDepth = depth;
    _modDepthStep = std::abs(depth - _modDepth) / s_rampLength;
}

void SynthChorus::setModFrequency(double frequency)
{
    _modFrequency = frequency;
    _modRate = s_sineTableSize * frequency / _sampleRate;
}

void SynthChorus::clear()
{
    // Empty delay lines: they contain a silence longer than the maximum delay
    std::fill(_delayLineL.begin(), _delayLineL.end(), 0.0);
    std::fill(_delayLineR.begin(), _delayLineR.end(), 0.0);
    _silentLength = _size;
    _filledLength = 0;
}

void SynthChorus::advanceModulation(uint32_t len)
{
    double time = _modTime;
    for (uint32_t i = 0; i < len; i++)
    {
        while (time < 0.0)
            time += s_sineTableSize;
        while (time >= s_sineTableSize)
            time -= s_sineTableSize;
        time += _modRate;
    }
    _modTime = time;
}

void SynthChorus::process(float * dataL, float * dataR, uint32_t len)
{
    if (!isOn())
        return;

    // After a silent input as long as the delay lines, they contain only zeros: the output is silent (and the delay
    // lines do not change), only the modulation goes on and the settings can change at once
    uint32_t maximumDelay = _size - 1;
    bool isSilent = FastMaths::isZero(dataL, len) && FastMaths::isZero(dataR, len);
    if (isSilent && _silentLength > maximumDelay)
    {
        _mix = _targetMix;
        _modDepth = _targetModDepth;
        _filledLength = _silentLength;
        advanceModulation(len);
        return;
    }
    _silentLength = isSilent ? _silentLength + len : 0;

    if (_mix == _targetMix && _modDepth == _targetModDepth)
        compute<false>(dataL, dataR, len);
    else
    {
        // The mix and the depth get closer to their targets at each frame
        compute<true>(dataL, dataR, len);

        // Faded out: the output is the input from now on, the delay lines are emptied for the next time they are used
        if (_mix == 0.0 && _targetMix == 0.0)
            clear();
    }
}

template<bool RAMPS>
void SynthChorus::compute(float * dataL, float * dataR, uint32_t len)
{
    const double * table = _sineTable;
    double * lineL = _delayLineL.data();
    double * lineR = _delayLineR.data();
    const uint32_t size = _size;
    const uint32_t maximumDelay = size - 1;
    uint32_t inPoint = _inPoint;
    double time = _modTime;
    double mix = _mix;
    double modDepth = _modDepth;

    for (uint32_t i = 0; i < len; i++)
    {
        if (RAMPS)
        {
            // A chorus turned on while its input is not silent fades in once its delay lines are filled, otherwise the
            // delayed input would start abruptly
            if (mix > 0.0 || _filledLength > maximumDelay)
                mix = approach(mix, _targetMix, _mixStep);
            else
                _filledLength++;
            modDepth = approach(modDepth, _targetModDepth, _modDepthStep);
        }

        // Modulation (stk::SineWave::tick)
        while (time < 0.0)
            time += s_sineTableSize;
        while (time >= s_sineTableSize)
            time -= s_sineTableSize;
        uint32_t index = static_cast<uint32_t>(time);
        double alpha = time - index;
        double modulation = table[index];
        modulation += alpha * (table[index + 1] - modulation);
        time += _modRate;

        // Position of the delayed input (stk::DelayL::setDelay)
        double outPointer = inPoint - _delayFactor * (1.0 + modDepth * modulation);
        while (outPointer < 0)
            outPointer += size;
        uint32_t outPoint = static_cast<uint32_t>(outPointer);
        alpha = outPointer - outPoint;
        double omAlpha = 1.0 - alpha;
        if (outPoint == size)
            outPoint = 0;
        uint32_t nextPoint = outPoint + 1 < size ? outPoint + 1 : 0;

        // New input, then delayed input with a linear interpolation (stk::DelayL::tick)
        double inputL = static_cast<double>(dataL[i]);
        double inputR = static_cast<double>(dataR[i]);
        lineL[inPoint] = inputL;
        lineR[inPoint] = inputR;
        if (++inPoint == size)
            inPoint = 0;
        double delayedL = lineL[outPoint] * omAlpha;
        delayedL += lineL[nextPoint] * alpha;
        double delayedR = lineR[outPoint] * omAlpha;
        delayedR += lineR[nextPoint] * alpha;

        dataL[i] = static_cast<float>(mix * (delayedL - inputL) + inputL);
        dataR[i] = static_cast<float>(mix * (delayedR - inputR) + inputR);
    }

    _inPoint = inPoint;
    _modTime = time;
    _mix = mix;
    _modDepth = modDepth;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reverb
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constants of FreeVerb, delays in frames at 48 kHz (the ones of the original version at 44.1 kHz, which STK did not
// scale at the sample rate of Polyphone)
static const double s_fixedGain = 0.015;
static const double s_scaleWet = 3;
static const double s_scaleDry = 2;
static const double s_scaleDamp = 0.4;
static const double s_scaleRoom = 0.28;
static const double s_offsetRoom = 0.7;
static const double s_allpassFeedback = 0.5;
static const uint32_t s_combLengths[] = { 1617, 1557, 1491, 1422, 1356, 1277, 1188, 1116 };
static const uint32_t s_allpassLengths[] = { 225, 556, 441, 341 };
static const uint32_t s_stereoSpread = 23; // Additional delay for the right channel

// Copy of the state of a comb filter, in registers during the computation of a chunk
struct SynthReverb::CombState
{
    CombState(const DelayLine &line, double filter) :
        data(line.data),
        size(line.size),
        position(line.position),
        filter(filter)
    {}

    void save(DelayLine &line, double &filter) const
    {
        line.position = this->position;
        filter = this->filter;
    }

    // Same operations as stk::OnePole::tick and stk::Delay::tick in stk::FreeVerb::tick
    inline double tick(double input, double b0, double a1, double roomSize)
    {
        filter = b0 * data[position] - a1 * filter;
        double output = input + roomSize * filter;
        data[position] = output;
        if (++position == size)
            position = 0;
        return output;
    }

    double * data;
    uint32_t size;
    uint32_t position;
    double filter;
};

SynthReverb::SynthReverb() :
    _sampleRate(0),
    _mix(0.0), // Off
    _targetMix(0.0),
    _mixStep(0.0),
    _roomSize(0.75 * s_scaleRoom + s_offsetRoom), // Defaults of STK
    _damping(0.25 * s_scaleDamp),
    _width(1.0),
    _silentLength(0),
    _isIdle(true)
{
    updateCoefficients();
    _lastWet1 = _wet1;
    _lastWet2 = _wet2;
    _lastDry = _dry;
    setSampleRate(s_referenceSampleRate);
}

void SynthReverb::setSampleRate(uint32_t sampleRate)
{
    if (sampleRate == 0 || sampleRate == _sampleRate)
        return;
    _sampleRate = sampleRate;

    // Lengths of the delay lines
    uint32_t total = 0;
    for (int i = 0; i < COMB_COUNT; i++)
    {
        _combL[i].size = scaleLength(s_combLengths[i], sampleRate);
        _combR[i].size = scaleLength(s_combLengths[i] + s_stereoSpread, sampleRate);
        total += _combL[i].size + _combR[i].size;
    }
    for (int i = 0; i < ALLPASS_COUNT; i++)
    {
        _allpassL[i].size = scaleLength(s_allpassLengths[i], sampleRate);
        _allpassR[i].size = scaleLength(s_allpassLengths[i] + s_stereoSpread, sampleRate);
        total += _allpassL[i].size + _allpassR[i].size;
    }

    // Memory shared by the delay lines, empty
    _memory.assign(total, 0.0);
    double * data = _memory.data();
    DelayLine * lines[] = { _combL, _combR, _allpassL, _allpassR };
    int counts[] = { COMB_COUNT, COMB_COUNT, ALLPASS_COUNT, ALLPASS_COUNT };
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < counts[i]; j++)
        {
            lines[i][j].data = data;
            lines[i][j].position = 0;
            data += lines[i][j].size;
        }
    }
    for (int i = 0; i < COMB_COUNT; i++)
        _combFilterL[i] = _combFilterR[i] = 0.0;
    _isIdle = true;
    _silentLength = 0;
}

void SynthReverb::setTargetMix(double mix)
{
    if (mix == _targetMix)
        return;
    _targetMix = mix;
    _mixStep = std::abs(mix - _mix) / s_rampLength;
}

void SynthReverb::setMix(double mix)
{
    _mix = mix < 0.0 ? 0.0 : (mix > 1.0 ? 1.0 : mix);
    updateCoefficients();
}

void SynthReverb::setRoomSize(double value)
{
    _roomSize = value * s_scaleRoom + s_offsetRoom;
    updateCoefficients();
}

void SynthReverb::setDamping(double value)
{
    _damping = value * s_scaleDamp;
    updateCoefficients();
}

void SynthReverb::setWidth(double value)
{
    _width = value;
    updateCoefficients();
}

void SynthReverb::updateCoefficients()
{
    // As stk::FreeVerb::update (the dry coefficient is divided by the sum including the wet one already divided)
    double wet = s_scaleWet * _mix;
    _dry = s_scaleDry * (1.0 - _mix);
    wet /= (wet + _dry);
    _dry /= (wet + _dry);
    _wet1 = wet * (_width / 2.0 + 0.5);
    _wet2 = wet * (1.0 - _width) / 2.0;
}

void SynthReverb::clear()
{
    // As stk::FreeVerb::clear, the low-pass filters of the combs keep their state
    std::fill(_memory.begin(), _memory.end(), 0.0);
}

void SynthReverb::process(float * dataL, float * dataR, uint32_t len)
{
    for (uint32_t pos = 0; pos < len; pos += COMPUTATION_CHUNK)
        processChunk(&dataL[pos], &dataR[pos], std::min<uint32_t>(COMPUTATION_CHUNK, len - pos));
}

void SynthReverb::processChunk(float * dataL, float * dataR, uint32_t len)
{
    if (!isOn())
        return;

    // Nothing to compute: the state is cleared and the input is silent (the mix can change at once)
    bool isSilent = FastMaths::isZero(dataL, len) && FastMaths::isZero(dataR, len);
    if (isSilent && _isIdle)
    {
        if (_mix != _targetMix)
            setMix(_targetMix);
        _lastWet1 = _wet1;
        _lastWet2 = _wet2;
        _lastDry = _dry;
        return;
    }
    _isIdle = false;

    // Mix at the end of the chunk, closer to the target
    if (_mix != _targetMix)
        setMix(approach(_mix, _targetMix, _mixStep * len));

    computeWetSignal(dataL, dataR, len);
    if (_wet1 == _lastWet1 && _wet2 == _lastWet2 && _dry == _lastDry)
    {
        for (uint32_t i = 0; i < len; i++)
        {
            double inputL = static_cast<double>(dataL[i]);
            double inputR = static_cast<double>(dataR[i]);
            dataL[i] = static_cast<float>(_wetL[i] * _wet1 + _wetR[i] * _wet2 + inputL * _dry);
            dataR[i] = static_cast<float>(_wetR[i] * _wet1 + _wetL[i] * _wet2 + inputR * _dry);
        }
    }
    else
    {
        // The mixing coefficients (mix, width) go linearly from the ones of the previous chunk to the new ones
        for (uint32_t i = 0; i < len; i++)
        {
            double progress = static_cast<double>(i + 1) / len;
            double wet1 = _lastWet1 + progress * (_wet1 - _lastWet1);
            double wet2 = _lastWet2 + progress * (_wet2 - _lastWet2);
            double dry = _lastDry + progress * (_dry - _lastDry);
            double inputL = static_cast<double>(dataL[i]);
            double inputR = static_cast<double>(dataR[i]);
            dataL[i] = static_cast<float>(_wetL[i] * wet1 + _wetR[i] * wet2 + inputL * dry);
            dataR[i] = static_cast<float>(_wetR[i] * wet1 + _wetL[i] * wet2 + inputR * dry);
        }
        _lastWet1 = _wet1;
        _lastWet2 = _wet2;
        _lastDry = _dry;

        // Faded out: the output is the input from now on, the state is cleared for the next time the reverb is used
        if (_mix == 0.0)
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

void SynthReverb::computeWetSignal(const float * dataL, const float * dataR, uint32_t len)
{
    // Each filter processes the whole chunk, keeping its state in registers: same operations as stk::FreeVerb::tick,
    // which processes all the filters for each frame
    double input[COMPUTATION_CHUNK];
    double * __restrict wetL = _wetL;
    double * __restrict wetR = _wetR;
    for (uint32_t i = 0; i < len; i++)
    {
        input[i] = (static_cast<double>(dataL[i]) + static_cast<double>(dataR[i])) * s_fixedGain;
        wetL[i] = 0.0;
        wetR[i] = 0.0;
    }

    // Low-pass feedback comb filters in parallel, summed in the same order as STK. Two combs are processed at once for
    // each channel: the four independent recursions of their low-pass filters are computed in parallel by the CPU.
    const double b0 = 1.0 - _damping;
    const double a1 = -_damping;
    const double roomSize = _roomSize;
    for (int c = 0; c < COMB_COUNT; c += 2)
    {
        CombState l0(_combL[c], _combFilterL[c]), l1(_combL[c + 1], _combFilterL[c + 1]);
        CombState r0(_combR[c], _combFilterR[c]), r1(_combR[c + 1], _combFilterR[c + 1]);
        for (uint32_t i = 0; i < len; i++)
        {
            double outL0 = l0.tick(input[i], b0, a1, roomSize);
            double outL1 = l1.tick(input[i], b0, a1, roomSize);
            double outR0 = r0.tick(input[i], b0, a1, roomSize);
            double outR1 = r1.tick(input[i], b0, a1, roomSize);
            wetL[i] = (wetL[i] + outL0) + outL1;
            wetR[i] = (wetR[i] + outR0) + outR1;
        }
        l0.save(_combL[c], _combFilterL[c]);
        l1.save(_combL[c + 1], _combFilterL[c + 1]);
        r0.save(_combR[c], _combFilterR[c]);
        r1.save(_combR[c + 1], _combFilterR[c + 1]);
    }

    // Allpass filters in series
    for (int a = 0; a < ALLPASS_COUNT; a++)
    {
        double * __restrict lineL = _allpassL[a].data;
        double * __restrict lineR = _allpassR[a].data;
        const uint32_t sizeL = _allpassL[a].size;
        const uint32_t sizeR = _allpassR[a].size;
        uint32_t posL = _allpassL[a].position;
        uint32_t posR = _allpassR[a].position;
        for (uint32_t i = 0; i < len; i++)
        {
            double delayedL = lineL[posL];
            double vL = wetL[i] + s_allpassFeedback * delayedL;
            lineL[posL] = vL;
            wetL[i] = -vL + (1.0 + s_allpassFeedback) * delayedL;
            if (++posL == sizeL)
                posL = 0;

            double delayedR = lineR[posR];
            double vR = wetR[i] + s_allpassFeedback * delayedR;
            lineR[posR] = vR;
            wetR[i] = -vR + (1.0 + s_allpassFeedback) * delayedR;
            if (++posR == sizeR)
                posR = 0;
        }
        _allpassL[a].position = posL;
        _allpassR[a].position = posR;
    }
}
