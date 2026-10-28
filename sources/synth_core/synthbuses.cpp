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
    _capacity(0)
{}

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
}

void SynthBuses::reserve(uint32_t len)
{
    // The voices compute whole chunks
    len = SynthMath::roundToChunks(len);
    if (len <= _capacity)
        return;

    deleteBuffers();
    _capacity = len;
    dataL = FastMaths::newAlignedBuffer(len);
    dataR = FastMaths::newAlignedBuffer(len);
    dataRevL = FastMaths::newAlignedBuffer(len);
    dataRevR = FastMaths::newAlignedBuffer(len);
    dataChoL = FastMaths::newAlignedBuffer(len);
    dataChoR = FastMaths::newAlignedBuffer(len);
    dataChoRevL = FastMaths::newAlignedBuffer(len);
    dataChoRevR = FastMaths::newAlignedBuffer(len);
    _voiceData = FastMaths::newAlignedBuffer(len);
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

void SynthBuses::addVoice(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized)
{
    // Get data, nothing to mix if the voice is silent
    if (!voice->generateData(_voiceData, len))
        return;

    // Coeffs
    float tmp = 0.005f * (voice->getParam()->getFloat(champ_pan) + 50.f); // Between 0 and 1/2 for [0; PI/2]
    float coefL = FastMaths::fastCos(tmp);
    float coefR = FastMaths::fastSin(tmp);

    float coefRev = 0.01f * voice->getParam()->getFloat(champ_reverbEffectsSend);
    float coefNonRev = 1.f - coefRev;

    float coefCho = 0.01f * voice->getParam()->getFloat(champ_chorusEffectsSend);
    float coefNonCho = 1.f - coefCho;

    // Left
    if (coefL > 0.0f)
    {
        mixVoice(dataL, _voiceData, len, coefL * coefNonRev * coefNonCho, leftInitialized); // Left, no reverb, no chorus
        mixVoice(dataRevL, _voiceData, len, coefL * coefRev * coefNonCho, leftInitialized); // Left, reverb, no chorus
        mixVoice(dataChoL, _voiceData, len, coefL * coefNonRev * coefCho, leftInitialized); // Left, no reverb, chorus
        mixVoice(dataChoRevL, _voiceData, len, coefL * coefRev * coefCho, leftInitialized); // Left, reverb, chorus
        leftInitialized = true;
    }

    // Right
    if (coefR > 0.0f)
    {
        mixVoice(dataR, _voiceData, len, coefR * coefNonRev * coefNonCho, rightInitialized); // Right, no reverb, no chorus
        mixVoice(dataRevR, _voiceData, len, coefR * coefRev * coefNonCho, rightInitialized); // Right, reverb, no chorus
        mixVoice(dataChoR, _voiceData, len, coefR * coefNonRev * coefCho, rightInitialized); // Right, no reverb, chorus
        mixVoice(dataChoRevR, _voiceData, len, coefR * coefRev * coefCho, rightInitialized); // Right, reverb, chorus
        rightInitialized = true;
    }
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
