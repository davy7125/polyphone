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

#include "attributebase.h"
#include <cmath>
#include "synthconstants.h"
#include "synthmath.h"
#include "fastmaths.h"

AttributeBase::AttributeBase(AttributeType champ, bool isPrst) :
    _champ(champ),
    _realValue(0),
    _isPrst(isPrst)
{
    // Load the default value
    setStoredValue(getDefaultStoredValue(champ, isPrst));
}

AttributeBase::AttributeBase(AttributeType champ, bool isPrst, double value) : AttributeBase(champ, isPrst)
{
    setRealValue(value);
}

AttributeBase::AttributeBase(AttributeType champ, bool isPrst, AttributeValue storedValue) : AttributeBase(champ, isPrst)
{
    setStoredValue(storedValue);
}

AttributeBase::AttributeBase() : _realValue(0)
{

}

void AttributeBase::setType(AttributeType champ, bool isPrst)
{
    _champ = champ;
    _isPrst = isPrst;
    setStoredValue(getDefaultStoredValue(champ, isPrst));
}

void AttributeBase::setRealValue(double realValue)
{
    _storedValue = fromRealValue(_champ, _isPrst, realValue);

    // Adapt the real value based on the new stored value
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void AttributeBase::setStoredValue(AttributeValue storedValue)
{
    _storedValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void AttributeBase::setStoredValue(int16_t storedValue)
{
    _storedValue.shValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void AttributeBase::setStoredValue(uint16_t storedValue)
{
    _storedValue.wValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void AttributeBase::setStoredValue(uint8_t lower, uint8_t upper)
{
    _storedValue.rValue.byLo = (lower < upper ? lower : upper);
    _storedValue.rValue.byHi = (upper > lower ? upper : lower);
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

double AttributeBase::toRealValue(AttributeType champ, bool isPrst, AttributeValue storedValue)
{
    double realValue = 0;

    switch (champ)
    {
    case champ_byOriginalPitch:
        realValue = storedValue.bValue;
        break;
    case champ_chPitchCorrection:
        realValue = storedValue.cValue;
        break;
    case champ_dwStartLoop: case champ_dwEndLoop:
        realValue = storedValue.dwValue;
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        realValue = (double)FastMaths::fastPow2(0.000833333f * storedValue.shValue); // 0.000833333 = 1/1200
        break;
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset: case champ_endloopAddrsOffset:
    case champ_endAddrsOffset:
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: // can be -1 (not defined)
        realValue = storedValue.shValue;
        break;
    case champ_initialAttenuation:
        // Special case because of an extra coeff 0.4 (historical error)
        realValue = 0.1 * DB_SF2_TO_REAL_DB * static_cast<double>(storedValue.shValue);
        break;
    case champ_pan: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        realValue = 0.1 * static_cast<double>(storedValue.shValue);
        break;
    case champ_sampleModes:
    case champ_exclusiveClass: case champ_sampleID:
        realValue = storedValue.wValue;
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        realValue = storedValue.shValue * 32768;
        break;
    case champ_initialFilterFc: case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            realValue = (double)FastMaths::fastPow2(0.000833333f * storedValue.shValue); // 0.000833333 = 1/1200
        else
            realValue = (double)FastMaths::fastPow2(0.000833333f * storedValue.shValue) * 8.176;
        break;
    case champ_keyRange: case champ_velRange:
        realValue = storedValue.rValue.byLo * 1000 + storedValue.rValue.byHi;
        break;
    default:
        break;
    }

    return realValue;
}

AttributeValue AttributeBase::fromRealValue(AttributeType champ, bool isPrst, double realValue)
{
    AttributeValue storedValue;
    storedValue.dwValue = 0;

    switch (champ)
    {
    case champ_byOriginalPitch:
        storedValue.bValue = static_cast<uint8_t>(realValue + 0.5);
        break;
    case champ_chPitchCorrection:
        storedValue.cValue = SynthMath::round8(realValue);
        break;
    case champ_dwStartLoop: case champ_dwEndLoop:
        storedValue.dwValue = static_cast<uint32_t>(realValue + 0.5);
        break;
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: // can be -1 (not defined)
        storedValue.shValue = SynthMath::round16(realValue);
        break;
    case champ_initialAttenuation:
        // Historical error
        storedValue.shValue = SynthMath::round16(10. * realValue / DB_SF2_TO_REAL_DB);
        break;
    case champ_pan: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        storedValue.shValue = SynthMath::round16(realValue * 10.);
        break;
    case champ_sampleModes: case champ_exclusiveClass: case champ_sampleID:
        storedValue.wValue = static_cast<uint16_t>(realValue + 0.5); // Only positive
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        storedValue.shValue = static_cast<int16_t>(SynthMath::round(realValue) / 32768);
        break;
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
        storedValue.shValue = static_cast<int16_t>(SynthMath::round(realValue) % 32768);
        break;
    case champ_keyRange: case champ_velRange:
        storedValue.rValue.byHi = static_cast<uint8_t>(0.001 * realValue);
        storedValue.rValue.byLo = static_cast<uint8_t>(realValue - 1000 * storedValue.rValue.byHi);
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        storedValue.shValue = SynthMath::round16(1200. * std::log(SynthMath::max(0.001, realValue)) / 0.69314718056);
        break;
    case champ_initialFilterFc:
        if (isPrst)
            storedValue.shValue = SynthMath::round16(1200. * std::log(SynthMath::max(0.001, realValue)) / 0.69314718056);
        else
            storedValue.shValue = SynthMath::round16(1200. * std::log(SynthMath::max(0.001, realValue) / 8.176) / 0.69314718056);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            storedValue.shValue = SynthMath::round16(1200. * std::log(SynthMath::max(0.001, realValue)) / 0.69314718056);
        else
            storedValue.shValue = SynthMath::round16(1200. * std::log(SynthMath::max(0.001, realValue) / 8.176) / 0.69314718056);
        break;
    default:
        break;
    }

    return limit(champ, storedValue, isPrst);
}

double AttributeBase::getDefaultRealValue(AttributeType champ, bool isPrst)
{
    // Default stored value
    AttributeValue defaultStoredValue = AttributeBase::getDefaultStoredValue(champ, isPrst);

    // Compute the corresponding real value
    return toRealValue(champ, isPrst, defaultStoredValue);
}

AttributeValue AttributeBase::getDefaultStoredValue(AttributeType champ, bool isPrst)
{
    AttributeValue value;
    value.dwValue = 0;

    switch (champ)
    {
    case champ_byOriginalPitch: case champ_chPitchCorrection: case champ_dwStartLoop: case champ_dwEndLoop:
    case champ_fineTune: case champ_coarseTune: case champ_initialFilterQ:
    case champ_startAddrsOffset: case champ_endAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endloopAddrsOffset: case champ_initialAttenuation: case champ_pan:
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_sustainVolEnv: case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_sustainModEnv: case champ_modEnvToPitch: case champ_modEnvToFilterFc:
    case champ_modLfoToPitch: case champ_modLfoToFilterFc: case champ_modLfoToVolume:
    case champ_vibLfoToPitch: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_exclusiveClass: case champ_sampleModes:
        value.wValue = 0; // Same than value.shValue = 0
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        value.shValue = isPrst ? 0 : -12000;
        break;
    case champ_velRange: case champ_keyRange:
        value.rValue.byLo = 0;
        value.rValue.byHi = 127;
        break;
    case champ_keynum: case champ_velocity: case champ_overridingRootKey:
        value.shValue = -1;
        break;
    case champ_scaleTuning:
        value.shValue = isPrst ? 0 : 100;
        break;
    case champ_initialFilterFc:
        value.wValue = isPrst ? 0 : 13500;
        break;
    default:
        break;
    }

    return value;
}

void AttributeBase::getLimit(AttributeType champ, bool isPrst, int &min, int &max)
{
    switch (champ)
    {
    case champ_chPitchCorrection:
        min = -99;
        max = 99;
        break;
    case champ_exclusiveClass:
    case champ_byOriginalPitch:
    case champ_keyRange: case champ_velRange:
        min = 0;
        max = 127;
        break;
    case champ_coarseTune:
        min = isPrst ? -240 : -120;
        max = isPrst ? 240 : 120;
        break;
    case champ_fineTune:
        min = isPrst ? -198 : -99;
        max = isPrst ? 198 : 99;
        break;
    case champ_scaleTuning:
        min = isPrst ? -1200 : 0;
        max = 1200;
        break;
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
        min = isPrst ? -24000 : -12000;
        max = isPrst ? 24000 : 12000;
        break;
    case champ_pan:
        min = isPrst ? -1000 : -500;
        max = isPrst ? 1000 : 500;
        break;
    case champ_sampleModes:
        min = 0;
        max = 3;
        break;
    case champ_initialFilterQ:
        min = isPrst ? -960 : 0;
        max = 960;
        break;
    case champ_modLfoToVolume:
        min = isPrst ? -1920 : -960;
        max = isPrst ? 1920 : 960;
        break;
    case champ_velocity: case champ_overridingRootKey: case champ_keynum:
        min = -1; // -1 = default value which means "not set"
        max = 127;
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
        min = -32767;
        max = 32767;
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
        min = isPrst ? -21000 : -12000;
        max = isPrst ? 21000 : 5000;
        break;
    case champ_initialFilterFc:
        min = isPrst ? -21000 : 1500;
        max = isPrst ? 21000 : 13500;
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        min = isPrst ? -21000 : -16000;
        max = isPrst ? 21000 : 4500;
        break;
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        min = isPrst ? -21000 : -12000;
        max = isPrst ? 21000 : 8000;
        break;
    case champ_sustainVolEnv: case champ_initialAttenuation:
        min = isPrst ? -1440 : 0;
        max = 1440;
        break;
    case champ_sustainModEnv: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        min = isPrst ? -1000 : 0;
        max = 1000;
        break;
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
        min = isPrst ? -2400 : -1200;
        max = isPrst ? 2400 : 1200;
        break;
    default:
        break;
    }
}

AttributeValue AttributeBase::limit(AttributeType champ, AttributeValue value, bool isPrst)
{
    int min = 0, max = 0;
    getLimit(champ, isPrst, min, max);

    switch (champ)
    {
    case champ_chPitchCorrection:
        value.cValue = limit(value.cValue, min, max);
        break;
    case champ_byOriginalPitch:
        value.bValue = limit(value.bValue, min, max);
        break;
    case champ_keyRange: case champ_velRange:
        value.rValue.byLo = limit(value.rValue.byLo, min, max);
        value.rValue.byHi = limit(value.rValue.byHi, min, max);
        break;
    case champ_coarseTune:
    case champ_fineTune:
    case champ_scaleTuning:
    case champ_modEnvToPitch: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modLfoToFilterFc: case champ_modEnvToFilterFc:
    case champ_pan:
    case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_velocity: case champ_overridingRootKey: case champ_keynum:
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_initialFilterFc:
    case champ_freqModLFO: case champ_freqVibLFO:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
    case champ_sustainVolEnv: case champ_initialAttenuation:
    case champ_sustainModEnv: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
        value.shValue = limit(value.shValue, min, max);
        break;
    case champ_sampleModes:
    case champ_exclusiveClass:
        value.wValue = limit(value.wValue, min, max);
        break;
    default:
        break;
    }

    return value;
}

int AttributeBase::limit(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

double AttributeBase::limit(double value, double min, double max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
