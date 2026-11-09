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

#include "synthbuses.h"
#include <cstring>
#include <cstdint>
#include "voice.h"
#include "fastmaths.h"
#include "synthmath.h"

SynthBuses::SynthBuses() :
    dataL(nullptr),
    dataR(nullptr),
    dataRevL(nullptr),
    dataRevR(nullptr),
    dataChoL(nullptr),
    dataChoR(nullptr),
    dataChoRevL(nullptr),
    dataChoRevR(nullptr),
    _voiceData(nullptr),
    _capacity(0),
    _hasChannelBuses(false)
{
    for (int i = 0; i < CHANNEL_BUS_COUNT; i++)
    {
        channelL[i] = nullptr;
        channelR[i] = nullptr;
        _isChannelBusUsed[i] = false;
    }
}

SynthBuses::~SynthBuses()
{
    deleteBuffers();
}

void SynthBuses::deleteBuffers()
{
    FastMaths::deleteAlignedBuffer(dataL);
    FastMaths::deleteAlignedBuffer(dataR);
    FastMaths::deleteAlignedBuffer(dataRevL);
    FastMaths::deleteAlignedBuffer(dataRevR);
    FastMaths::deleteAlignedBuffer(dataChoL);
    FastMaths::deleteAlignedBuffer(dataChoR);
    FastMaths::deleteAlignedBuffer(dataChoRevL);
    FastMaths::deleteAlignedBuffer(dataChoRevR);
    FastMaths::deleteAlignedBuffer(_voiceData);
    for (int i = 0; i < CHANNEL_BUS_COUNT; i++)
    {
        FastMaths::deleteAlignedBuffer(channelL[i]);
        FastMaths::deleteAlignedBuffer(channelR[i]);
        channelL[i] = nullptr;
        channelR[i] = nullptr;
    }
}

void SynthBuses::reserve(uint32_t len, bool withChannelBuses)
{
    // The voices compute whole chunks
    len = SynthMath::roundToChunks(len);
    withChannelBuses = withChannelBuses || _hasChannelBuses;
    if (len <= _capacity && withChannelBuses == _hasChannelBuses)
        return;
    if (len < _capacity)
        len = _capacity;

    deleteBuffers();
    _capacity = len;
    _hasChannelBuses = withChannelBuses;
    dataL = FastMaths::newAlignedBuffer(len);
    dataR = FastMaths::newAlignedBuffer(len);
    dataRevL = FastMaths::newAlignedBuffer(len);
    dataRevR = FastMaths::newAlignedBuffer(len);
    dataChoL = FastMaths::newAlignedBuffer(len);
    dataChoR = FastMaths::newAlignedBuffer(len);
    dataChoRevL = FastMaths::newAlignedBuffer(len);
    dataChoRevR = FastMaths::newAlignedBuffer(len);
    _voiceData = FastMaths::newAlignedBuffer(len);
    if (_hasChannelBuses)
    {
        for (int i = 0; i < CHANNEL_BUS_COUNT; i++)
        {
            channelL[i] = FastMaths::newAlignedBuffer(len);
            channelR[i] = FastMaths::newAlignedBuffer(len);
            memset(channelL[i], 0, len * sizeof(float));
            memset(channelR[i], 0, len * sizeof(float));
        }
    }
    clear(len);
    memset(_voiceData, 0, len * sizeof(float));
}

void SynthBuses::clear(uint32_t len)
{
    memset(dataL, 0, len * sizeof(float));
    memset(dataR, 0, len * sizeof(float));
    memset(dataRevL, 0, len * sizeof(float));
    memset(dataRevR, 0, len * sizeof(float));
    memset(dataChoL, 0, len * sizeof(float));
    memset(dataChoR, 0, len * sizeof(float));
    memset(dataChoRevL, 0, len * sizeof(float));
    memset(dataChoRevR, 0, len * sizeof(float));
    for (int i = 0; i < CHANNEL_BUS_COUNT; i++)
    {
        if (_isChannelBusUsed[i])
        {
            memset(channelL[i], 0, len * sizeof(float));
            memset(channelR[i], 0, len * sizeof(float));
        }
    }
}

void SynthBuses::setChannelBus(int channel, bool hasOwnBus)
{
    if (channel >= 0 && channel < CHANNEL_BUS_COUNT)
        _isChannelBusUsed[channel] = hasOwnBus && _hasChannelBuses;
}

// Mixing coefficients of a voice, for the left then the right side: without effect, with the reverb, with the chorus,
// with both. A side is not mixed if its coefficient of pan is 0.
static void getCoefficients(Voice * voice, float coefficients[8], bool &withLeft, bool &withRight)
{
    float tmp = 0.005f * (voice->getParam()->getFloat(champ_pan) + 50.f); // Between 0 and 1/2 for [0; PI/2]
    float coefL = FastMaths::fastCos(tmp);
    float coefR = FastMaths::fastSin(tmp);

    float coefRev = 0.01f * voice->getParam()->getFloat(champ_reverbEffectsSend);
    float coefNonRev = 1.f - coefRev;

    float coefCho = 0.01f * voice->getParam()->getFloat(champ_chorusEffectsSend);
    float coefNonCho = 1.f - coefCho;

    coefficients[0] = coefL * coefNonRev * coefNonCho; // Left, no reverb, no chorus
    coefficients[1] = coefL * coefRev * coefNonCho; // Left, reverb, no chorus
    coefficients[2] = coefL * coefNonRev * coefCho; // Left, no reverb, chorus
    coefficients[3] = coefL * coefRev * coefCho; // Left, reverb, chorus
    coefficients[4] = coefR * coefNonRev * coefNonCho; // Right, no reverb, no chorus
    coefficients[5] = coefR * coefRev * coefNonCho; // Right, reverb, no chorus
    coefficients[6] = coefR * coefNonRev * coefCho; // Right, no reverb, chorus
    coefficients[7] = coefR * coefRev * coefCho; // Right, reverb, chorus
    withLeft = coefL > 0.0f;
    withRight = coefR > 0.0f;
}

// Mix the data of a voice in a bus, the first voice of a side overwriting it
// Nothing is done with a coefficient of 0: the buses being cleared before, a bus not written stays at 0
static void mixVoice(float * bus, const float * voiceData, uint32_t len, float coeff, bool initialized)
{
    if (coeff == 0.0f)
        return;
    if (initialized)
        FastMaths::multiplyAdd(bus, voiceData, len, coeff);
    else
        FastMaths::multiply(bus, voiceData, len, coeff);
}

// Same at any position of a bus, from data possibly not aligned
static void mixVoiceAt(float * bus, uint32_t position, const float * voiceData, uint32_t len, float coeff, bool overwrite)
{
    if (coeff == 0.0f)
        return;
    if (position == 0 && reinterpret_cast<uintptr_t>(voiceData) % BUFFER_ALIGNMENT == 0)
        mixVoice(bus, voiceData, len, coeff, !overwrite);
    else if (overwrite)
    {
        float * __restrict data = bus + position;
        for (uint32_t i = 0; i < len; i++)
            data[i] = coeff * voiceData[i];
    }
    else
    {
        float * __restrict data = bus + position;
        for (uint32_t i = 0; i < len; i++)
            data[i] += coeff * voiceData[i];
    }
}

void SynthBuses::addVoice(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized)
{
    // Get data, nothing to mix if the voice is silent
    if (!voice->generateData(_voiceData, len))
        return;

    float coefficients[8];
    bool withLeft, withRight;
    getCoefficients(voice, coefficients, withLeft, withRight);

    // Left
    if (withLeft)
    {
        mixVoice(dataL, _voiceData, len, coefficients[0], leftInitialized);
        mixVoice(dataRevL, _voiceData, len, coefficients[1], leftInitialized);
        mixVoice(dataChoL, _voiceData, len, coefficients[2], leftInitialized);
        mixVoice(dataChoRevL, _voiceData, len, coefficients[3], leftInitialized);
        leftInitialized = true;
    }

    // Right
    if (withRight)
    {
        mixVoice(dataR, _voiceData, len, coefficients[4], rightInitialized);
        mixVoice(dataRevR, _voiceData, len, coefficients[5], rightInitialized);
        mixVoice(dataChoR, _voiceData, len, coefficients[6], rightInitialized);
        mixVoice(dataChoRevR, _voiceData, len, coefficients[7], rightInitialized);
        rightInitialized = true;
    }
}

void SynthBuses::mixFrames(const float * voiceData, uint32_t position, uint32_t len, const float coefficients[8],
                           int channel, bool overwriteLeft, bool overwriteRight, bool withLeft, bool withRight)
{
    // The part without effect possibly goes to the buffers of the channel, cleared before
    bool hasOwnBus = hasChannelBus(channel);
    if (withLeft)
    {
        if (hasOwnBus)
            mixVoiceAt(channelL[channel], position, voiceData, len, coefficients[0], false);
        else
            mixVoiceAt(dataL, position, voiceData, len, coefficients[0], overwriteLeft);
        mixVoiceAt(dataRevL, position, voiceData, len, coefficients[1], overwriteLeft);
        mixVoiceAt(dataChoL, position, voiceData, len, coefficients[2], overwriteLeft);
        mixVoiceAt(dataChoRevL, position, voiceData, len, coefficients[3], overwriteLeft);
    }
    if (withRight)
    {
        if (hasOwnBus)
            mixVoiceAt(channelR[channel], position, voiceData, len, coefficients[4], false);
        else
            mixVoiceAt(dataR, position, voiceData, len, coefficients[4], overwriteRight);
        mixVoiceAt(dataRevR, position, voiceData, len, coefficients[5], overwriteRight);
        mixVoiceAt(dataChoR, position, voiceData, len, coefficients[6], overwriteRight);
        mixVoiceAt(dataChoRevR, position, voiceData, len, coefficients[7], overwriteRight);
    }
}

uint32_t SynthBuses::addVoiceFrames(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized)
{
    Voice::PendingFrames &pending = voice->getPendingFrames();
    int channel = voice->getParam()->getChannel();

    // Frames of the last chunk not mixed yet, then new chunks (none if the voice is finished: silent)
    uint32_t pendingLength = SynthMath::min(pending.length, len);
    uint32_t newLength = 0;
    uint32_t computedLength = 0;
    float coefficients[8];
    bool withLeft = false, withRight = false, isAudible = false;
    if (pendingLength < len && !voice->isFinished())
    {
        newLength = len - pendingLength;
        computedLength = SynthMath::roundToChunks(newLength);
        isAudible = voice->generateData(_voiceData, computedLength);
        getCoefficients(voice, coefficients, withLeft, withRight);
    }

    // The first voice mixed on a side overwrites the buffers if it writes all their frames
    bool pendingAudible = pendingLength > 0 && pending.isAudible;
    bool newAudible = newLength > 0 && isAudible;
    bool writesAllLeft = (pendingLength == 0 || (pendingAudible && pending.hasLeft)) &&
                         (pendingLength == len || (newAudible && withLeft));
    bool writesAllRight = (pendingLength == 0 || (pendingAudible && pending.hasRight)) &&
                          (pendingLength == len || (newAudible && withRight));
    bool overwriteLeft = !leftInitialized && writesAllLeft;
    bool overwriteRight = !rightInitialized && writesAllRight;
    if (pendingAudible)
    {
        mixFrames(&pending.data[pending.position], 0, pendingLength, pending.coefficients, channel,
                  overwriteLeft, overwriteRight, pending.hasLeft, pending.hasRight);
        leftInitialized = leftInitialized || pending.hasLeft;
        rightInitialized = rightInitialized || pending.hasRight;
    }
    if (newAudible)
    {
        mixFrames(_voiceData, pendingLength, newLength, coefficients, channel,
                  overwriteLeft, overwriteRight, withLeft, withRight);
        leftInitialized = leftInitialized || withLeft;
        rightInitialized = rightInitialized || withRight;
    }

    // The frames computed and not asked are kept for the next call
    pending.position += pendingLength;
    pending.length -= pendingLength;
    if (computedLength > newLength)
    {
        memcpy(pending.data, &_voiceData[newLength], (computedLength - newLength) * sizeof(float));
        pending.position = 0;
        pending.length = computedLength - newLength;
        memcpy(pending.coefficients, coefficients, sizeof(coefficients));
        pending.hasLeft = withLeft;
        pending.hasRight = withRight;
        pending.isAudible = isAudible;
    }

    return computedLength >> COMPUTATION_CHUNK_SHIFT;
}

void SynthBuses::copyTo(SynthBuses &destination, uint32_t len)
{
    memcpy(destination.dataL, dataL, len * sizeof(float));
    memcpy(destination.dataR, dataR, len * sizeof(float));
    memcpy(destination.dataChoL, dataChoL, len * sizeof(float));
    memcpy(destination.dataChoR, dataChoR, len * sizeof(float));
    memcpy(destination.dataRevL, dataRevL, len * sizeof(float));
    memcpy(destination.dataRevR, dataRevR, len * sizeof(float));
    memcpy(destination.dataChoRevL, dataChoRevL, len * sizeof(float));
    memcpy(destination.dataChoRevR, dataChoRevR, len * sizeof(float));
}

void SynthBuses::addTo(SynthBuses &destination, uint32_t len)
{
    FastMaths::addVectors(destination.dataL, dataL, len);
    FastMaths::addVectors(destination.dataR, dataR, len);
    FastMaths::addVectors(destination.dataChoL, dataChoL, len);
    FastMaths::addVectors(destination.dataChoR, dataChoR, len);
    FastMaths::addVectors(destination.dataRevL, dataRevL, len);
    FastMaths::addVectors(destination.dataRevR, dataRevR, len);
    FastMaths::addVectors(destination.dataChoRevL, dataChoRevL, len);
    FastMaths::addVectors(destination.dataChoRevR, dataChoRevR, len);
}
