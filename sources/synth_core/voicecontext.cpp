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


#include "voicecontext.h"
#include "modulatordata.h"

VoiceContext::VoiceContext() :
    _midiValues(&_defaultMidiValues),
    _referencePitch(4400),
    _temperamentRelativeKey(0),
    _temperamentReferenceKey(9),
    _temperamentUsedKeys(0xFFF),
    _modulatorVelToFilterType(1)
{
    for (int i = 0; i < 12; i++)
        _temperament[i].store(0.0f, std::memory_order_relaxed);
    for (int i = 0; i < ENVELOPE_STAGE_COUNT; i++)
    {
        _envelopeChanges[i].store(ENVELOPE_UNCHANGED, std::memory_order_relaxed);
        _envelopeValues[i].store(0, std::memory_order_relaxed);
    }
}

void VoiceContext::setMidiValues(IMidiValues * midiValues)
{
    _midiValues.store(midiValues == nullptr ? &_defaultMidiValues : midiValues, std::memory_order_relaxed);
}

void VoiceContext::setTuning(int referencePitch, const float temperament[12], const bool used[12], int relativeKey,
                             int referenceKey)
{
    _referencePitch.store(referencePitch, std::memory_order_relaxed);
    int usedKeys = 0;
    for (int i = 0; i < 12; i++)
    {
        _temperament[i].store(temperament[i], std::memory_order_relaxed);
        if (used[i])
            usedKeys |= 1 << i;
    }
    _temperamentUsedKeys.store(usedKeys, std::memory_order_relaxed);
    _temperamentRelativeKey.store((relativeKey % 12 + 12) % 12, std::memory_order_relaxed);
    _temperamentReferenceKey.store((referenceKey % 12 + 12) % 12, std::memory_order_relaxed);
}

float VoiceContext::getTemperamentFineTune(int key) const
{
    // Correction so that the reference key (A at the reference pitch for instance) is accurate
    int relativeKey = _temperamentRelativeKey.load(std::memory_order_relaxed);
    int referenceKey = _temperamentReferenceKey.load(std::memory_order_relaxed);
    return _temperament[(key - relativeKey + 12) % 12].load(std::memory_order_relaxed) -
           _temperament[(referenceKey - relativeKey + 12) % 12].load(std::memory_order_relaxed);
}

bool VoiceContext::isKeyUsed(int key) const
{
    int relativeKey = _temperamentRelativeKey.load(std::memory_order_relaxed);
    return (_temperamentUsedKeys.load(std::memory_order_relaxed) >> ((key % 12 - relativeKey + 12) % 12)) & 1;
}

void VoiceContext::setModulatorVelToFilterType(int type)
{
    _modulatorVelToFilterType.store(type, std::memory_order_relaxed);
}

bool VoiceContext::setEnvelope(const int changes[ENVELOPE_STAGE_COUNT], const int values[ENVELOPE_STAGE_COUNT])
{
    bool hasChanged = false;
    for (int i = 0; i < ENVELOPE_STAGE_COUNT; i++)
    {
        // No value without a change
        int value = (changes[i] == ENVELOPE_UNCHANGED) ? 0 : values[i];
        if (_envelopeChanges[i].exchange(changes[i], std::memory_order_relaxed) != changes[i])
            hasChanged = true;
        if (_envelopeValues[i].exchange(value, std::memory_order_relaxed) != value)
            hasChanged = true;
    }
    return hasChanged;
}

const ModulatorData * VoiceContext::getDefaultModulators(int &number) const
{
    return ModulatorData::getDefaultModulators(_modulatorVelToFilterType.load(std::memory_order_relaxed), number);
}
