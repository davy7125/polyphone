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
    delete [] dataL;
    delete [] dataR;
    delete [] dataRevL;
    delete [] dataRevR;
    delete [] dataChoL;
    delete [] dataChoR;
    delete [] dataChoRevL;
    delete [] dataChoRevR;
    delete [] _voiceData;
}

void SynthBuses::reserve(uint32_t len)
{
    if (len <= _capacity)
        return;

    deleteBuffers();
    _capacity = len;
    dataL = new float[len];
    dataR = new float[len];
    dataRevL = new float[len];
    dataRevR = new float[len];
    dataChoL = new float[len];
    dataChoR = new float[len];
    dataChoRevL = new float[len];
    dataChoRevR = new float[len];
    _voiceData = new float[len];
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

void SynthBuses::addVoice(Voice * voice, uint32_t len, bool &leftInitialized, bool &rightInitialized)
{
    // Get data
    voice->generateData(_voiceData, len);

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
        if (leftInitialized)
        {
            FastMaths::multiplyAdd(dataL, _voiceData, len, coefL * coefNonRev * coefNonCho); // Left, no reverb, no chorus
            FastMaths::multiplyAdd(dataRevL, _voiceData, len, coefL * coefRev * coefNonCho); // Left, reverb, no chorus
            FastMaths::multiplyAdd(dataChoL, _voiceData, len, coefL * coefNonRev * coefCho); // Left, no reverb, chorus
            FastMaths::multiplyAdd(dataChoRevL, _voiceData, len, coefL * coefRev * coefCho); // Left, reverb, chorus
        }
        else
        {
            leftInitialized = true;
            FastMaths::multiply(dataL, _voiceData, len, coefL * coefNonRev * coefNonCho); // Left, no reverb, no chorus
            FastMaths::multiply(dataRevL, _voiceData, len, coefL * coefRev * coefNonCho); // Left, reverb, no chorus
            FastMaths::multiply(dataChoL, _voiceData, len, coefL * coefNonRev * coefCho); // Left, no reverb, chorus
            FastMaths::multiply(dataChoRevL, _voiceData, len, coefL * coefRev * coefCho); // Left, reverb, chorus
        }
    }

    // Right
    if (coefR > 0.0f)
    {
        if (rightInitialized)
        {
            FastMaths::multiplyAdd(dataR, _voiceData, len, coefR * coefNonRev * coefNonCho); // Right, no reverb, no chorus
            FastMaths::multiplyAdd(dataRevR, _voiceData, len, coefR * coefRev * coefNonCho); // Right, reverb, no chorus
            FastMaths::multiplyAdd(dataChoR, _voiceData, len, coefR * coefNonRev * coefCho); // Right, no reverb, chorus
            FastMaths::multiplyAdd(dataChoRevR, _voiceData, len, coefR * coefRev * coefCho); // Right, reverb, chorus
        }
        else
        {
            rightInitialized = true;
            FastMaths::multiply(dataR, _voiceData, len, coefR * coefNonRev * coefNonCho); // Right, no reverb, no chorus
            FastMaths::multiply(dataRevR, _voiceData, len, coefR * coefRev * coefNonCho); // Right, reverb, no chorus
            FastMaths::multiply(dataChoR, _voiceData, len, coefR * coefNonRev * coefCho); // Right, no reverb, chorus
            FastMaths::multiply(dataChoRevR, _voiceData, len, coefR * coefRev * coefCho); // Right, reverb, chorus
        }
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
