/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "attribute.h"
#include "utils.h"
#include <qmath.h>
#include "basetypes.h"

QList<AttributeType> Attribute::s_attributesForPrstMod = QList<AttributeType>()
        << champ_fineTune << champ_coarseTune << champ_scaleTuning
        << champ_initialAttenuation << champ_initialFilterFc << champ_initialFilterQ
        << champ_pan << champ_chorusEffectsSend << champ_reverbEffectsSend
        << champ_delayVolEnv << champ_attackVolEnv << champ_holdVolEnv << champ_decayVolEnv << champ_sustainVolEnv << champ_releaseVolEnv
        << champ_keynumToVolEnvHold << champ_keynumToVolEnvDecay
        << champ_delayModEnv << champ_attackModEnv << champ_holdModEnv << champ_decayModEnv << champ_sustainModEnv << champ_releaseModEnv
        << champ_keynumToModEnvHold << champ_keynumToModEnvDecay
        << champ_modEnvToPitch << champ_modEnvToFilterFc
        << champ_delayModLFO << champ_freqModLFO
        << champ_modLfoToPitch << champ_modLfoToVolume << champ_modLfoToFilterFc
        << champ_delayVibLFO << champ_freqVibLFO
        << champ_vibLfoToPitch;
QList<AttributeType> Attribute::s_attributesForInstMod = QList<AttributeType>()
        << champ_overridingRootKey // Extra stuff for inst
        << s_attributesForPrstMod
           // Extra stuff for inst
        << champ_exclusiveClass << champ_sampleModes
        << champ_startAddrsOffset << champ_startAddrsCoarseOffset << champ_endAddrsOffset << champ_endAddrsCoarseOffset
        << champ_startloopAddrsOffset << champ_startloopAddrsCoarseOffset << champ_endloopAddrsOffset << champ_endloopAddrsCoarseOffset;

Attribute::Attribute(AttributeType champ, bool isPrst) :
    _champ(champ),
    _realValue(0),
    _isPrst(isPrst)
{
    // Load the default value
    setStoredValue(getDefaultStoredValue(champ, isPrst));
}

Attribute::Attribute(AttributeType champ, bool isPrst, double value) : Attribute(champ, isPrst)
{
    setRealValue(value);
}

Attribute::Attribute(AttributeType champ, bool isPrst, AttributeValue storedValue) : Attribute(champ, isPrst)
{
    setStoredValue(storedValue);
}

Attribute::Attribute() : _realValue(0)
{

}

void Attribute::setType(AttributeType champ, bool isPrst)
{
    _champ = champ;
    _isPrst = isPrst;
    setStoredValue(getDefaultStoredValue(champ, isPrst));
}

void Attribute::setRealValue(double realValue)
{
    _storedValue = fromRealValue(_champ, _isPrst, realValue);

    // Adapt the real value based on the new stored value
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(AttributeValue storedValue)
{
    _storedValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(qint16 storedValue)
{
    _storedValue.shValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(quint16 storedValue)
{
    _storedValue.wValue = storedValue;
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

void Attribute::setStoredValue(quint8 lower, quint8 upper)
{
    _storedValue.rValue.byLo = (lower < upper ? lower : upper);
    _storedValue.rValue.byHi = (upper > lower ? upper : lower);
    _realValue = toRealValue(_champ, _isPrst, _storedValue);
}

double Attribute::toRealValue(AttributeType champ, bool isPrst, AttributeValue storedValue)
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
        realValue = qPow(2., 0.000833333 * storedValue.shValue); // 0.000833333 = 1/1200
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
            realValue = qPow(2., 0.000833333 * storedValue.shValue); // 0.000833333 = 1/1200
        else
            realValue = qPow(2., 0.000833333 * storedValue.shValue) * 8.176;
        break;
    case champ_keyRange: case champ_velRange:
        realValue = storedValue.rValue.byLo * 1000 + storedValue.rValue.byHi;
        break;
    default:
        break;
    }

    return realValue;
}

AttributeValue Attribute::fromRealValue(AttributeType champ, bool isPrst, double realValue)
{
    AttributeValue storedValue;
    storedValue.dwValue = 0;

    switch (champ)
    {
    case champ_byOriginalPitch:
        storedValue.bValue = static_cast<quint8>(realValue + 0.5);
        break;
    case champ_chPitchCorrection:
        storedValue.cValue = Utils::round8(realValue);
        break;
    case champ_dwStartLoop: case champ_dwEndLoop:
        storedValue.dwValue = static_cast<quint32>(realValue + 0.5);
        break;
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: // can be -1 (not defined)
        storedValue.shValue = Utils::round16(realValue);
        break;
    case champ_initialAttenuation:
        // Historical error
        storedValue.shValue = Utils::round16(10. * realValue / DB_SF2_TO_REAL_DB);
        break;
    case champ_pan: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        storedValue.shValue = Utils::round16(realValue * 10.);
        break;
    case champ_sampleModes: case champ_exclusiveClass: case champ_sampleID:
        storedValue.wValue = static_cast<quint16>(realValue + 0.5); // Only positive
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        storedValue.shValue = static_cast<qint16>(qRound(realValue) / 32768);
        break;
    case champ_startloopAddrsOffset: case champ_startAddrsOffset:
    case champ_endloopAddrsOffset: case champ_endAddrsOffset:
        storedValue.shValue = static_cast<qint16>(qRound(realValue) % 32768);
        break;
    case champ_keyRange: case champ_velRange:
        storedValue.rValue.byHi = static_cast<quint8>(0.001 * realValue);
        storedValue.rValue.byLo = static_cast<quint8>(realValue - 1000 * storedValue.rValue.byHi);
        break;
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        storedValue.shValue = Utils::round16(1200. * qLn(qMax(0.001, realValue)) / 0.69314718056);
        break;
    case champ_initialFilterFc:
        if (isPrst)
            storedValue.shValue = Utils::round16(1200. * qLn(qMax(0.001, realValue)) / 0.69314718056);
        else
            storedValue.shValue = Utils::round16(1200. * qLn(qMax(0.001, realValue) / 8.176) / 0.69314718056);
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            storedValue.shValue = Utils::round16(1200. * qLn(qMax(0.001, realValue)) / 0.69314718056);
        else
            storedValue.shValue = Utils::round16(1200. * qLn(qMax(0.001, realValue) / 8.176) / 0.69314718056);
        break;
    default:
        break;
    }

    return limit(champ, storedValue, isPrst);
}

double Attribute::getDefaultRealValue(AttributeType champ, bool isPrst)
{
    // Default stored value
    AttributeValue defaultStoredValue = Attribute::getDefaultStoredValue(champ, isPrst);

    // Compute the corresponding real value
    return toRealValue(champ, isPrst, defaultStoredValue);
}

AttributeValue Attribute::getDefaultStoredValue(AttributeType champ, bool isPrst)
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

void Attribute::getLimit(AttributeType champ, bool isPrst, int &min, int &max)
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

AttributeValue Attribute::limit(AttributeType champ, AttributeValue value, bool isPrst)
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

int Attribute::limit(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

double Attribute::limit(double value, double min, double max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

QString Attribute::getDescription(AttributeType champ, bool isPrst)
{
    QString result = tr("unknown");

    switch (champ)
    {
    case champ_startAddrsOffset:
        result = tr("Sample start offset");
        break;
    case champ_startAddrsCoarseOffset:
        result = tr("Sample start offset (× 32768)");
        break;
    case champ_endAddrsOffset:
        result = tr("Sample end offset");
        break;
    case champ_endAddrsCoarseOffset:
        result = tr("Sample end offset (× 32768)");
        break;
    case champ_startloopAddrsOffset:
        result = tr("Loop start offset");
        break;
    case champ_startloopAddrsCoarseOffset:
        result = tr("Loop start offset (× 32768)");
        break;
    case champ_endloopAddrsOffset:
        result = tr("Loop end offset");
        break;
    case champ_endloopAddrsCoarseOffset:
        result = tr("Loop end offset (× 32768)");
        break;
    case champ_modLfoToPitch:
        result = tr("Mod LFO → pitch (c)");
        break;
    case champ_vibLfoToPitch:
        result = tr("Vib LFO → pitch (c)");
        break;
    case champ_modEnvToPitch:
        result = tr("Mod env → pitch (c)");
        break;
    case champ_initialFilterFc:
        result = isPrst ? tr("Filter, cutoff (×)") :
                          tr("Filter, cutoff (Hz)");
        break;
    case champ_initialFilterQ:
        result = tr("Filter, resonance (dB)");
        break;
    case champ_modLfoToFilterFc:
        result = tr("Mod LFO → filter (c)");
        break;
    case champ_modEnvToFilterFc:
        result = tr("Mod env → filter (c)");
        break;
    case champ_modLfoToVolume:
        result = tr("Mod LFO → volume (dB)");
        break;
    case champ_chorusEffectsSend:
        result = tr("Chorus (%)");
        break;
    case champ_reverbEffectsSend:
        result = tr("Reverb (%)");
        break;
    case champ_pan:
        result = isPrst ? tr("Pan [-100;100]") :
                          tr("Pan [-50;50]");
        break;
    case champ_delayModLFO:
        result = isPrst ? tr("Mod LFO delay (×)") :
                          tr("Mod LFO delay (s)");
        break;
    case champ_freqModLFO:
        result = isPrst ? tr("Mod LFO freq (×)") :
                          tr("Mod LFO freq (Hz)");
        break;
    case champ_delayVibLFO:
        result = isPrst ? tr("Vib LFO delay (×)") :
                          tr("Vib LFO delay (s)");
        break;
    case champ_freqVibLFO:
        result = isPrst ? tr("Vib LFO freq (×)") :
                          tr("Vib LFO freq (Hz)");
        break;
    case champ_delayModEnv:
        result = isPrst ? tr("Mod env delay (×)") :
                          tr("Mod env delay (s)");
        break;
    case champ_attackModEnv:
        result = isPrst ? tr("Mod env attack (×)") :
                          tr("Mod env attack (s)");
        break;
    case champ_holdModEnv:
        result = isPrst ? tr("Mod env hold (×)") :
                          tr("Mod env hold (s)");
        break;
    case champ_decayModEnv:
        result = isPrst ? tr("Mod env decay (×)") :
                          tr("Mod env decay (s)");
        break;
    case champ_sustainModEnv:
        result = tr("Mod env sustain (%)");
        break;
    case champ_releaseModEnv:
        result = isPrst ? tr("Mod env release (×)") :
                          tr("Mod env release (s)");
        break;
    case champ_keynumToModEnvHold:
        result = tr("Key → Mod env hold (c)");
        break;
    case champ_keynumToModEnvDecay:
        result = tr("Key → Mod env decay (c)");
        break;
    case champ_delayVolEnv:
        result = isPrst ? tr("Vol env delay (×)") :
                          tr("Vol env delay (s)");
        break;
    case champ_attackVolEnv:
        result = isPrst ? tr("Vol env attack (×)") :
                          tr("Vol env attack (s)");
        break;
    case champ_holdVolEnv:
        result = isPrst ? tr("Vol env hold (×)") :
                          tr("Vol env hold (s)");
        break;
    case champ_decayVolEnv:
        result = isPrst ? tr("Vol env decay (×)") :
                          tr("Vol env decay (s)");
        break;
    case champ_sustainVolEnv:
        result = tr("Vol env sustain (dB)");
        break;
    case champ_releaseVolEnv:
        result = isPrst ? tr("Vol env release (×)") :
                          tr("Vol env release (s)");
        break;
    case champ_keynumToVolEnvHold:
        result = tr("Key → Vol env hold (c)");
        break;
    case champ_keynumToVolEnvDecay:
        result = tr("Key → Vol env decay (c)");
        break;
    case champ_keyRange:
        result = tr("Key range");
        break;
    case champ_velRange:
        result = tr("Velocity range");
        break;
    case champ_keynum:
        result = tr("Fixed key");
        break;
    case champ_velocity:
        result = tr("Fixed velocity");
        break;
    case champ_initialAttenuation:
        result = tr("Attenuation (dB)");
        break;
    case champ_coarseTune:
        result = tr("Tuning (semi-tones)");
        break;
    case champ_fineTune:
        result = tr("Tuning (cents)");
        break;
    case champ_sampleModes:
        result = tr("Loop playback");
        break;
    case champ_scaleTuning:
        result = tr("Scale tuning");
        break;
    case champ_exclusiveClass:
        result = tr("Exclusive class");
        break;
    case champ_overridingRootKey:
        result = tr("Root key");
        break;
    case champ_byOriginalPitch:
        result = tr("Sample root key");
        break;
    case champ_dwLength:
        result = tr("Sample length");
        break;
    case champ_dwStartLoop:
        result = tr("Loop start");
        break;
    case champ_dwEndLoop:
        result = tr("Loop end");
        break;
    case champ_chPitchCorrection:
        result = tr("Pitch correction (cents)");
        break;
    default:
        result = "...";
        break;
    }

    return result;
}

QList<AttributeType> Attribute::getAttributeListForMod(bool isPrst)
{
    return isPrst ? s_attributesForPrstMod : s_attributesForInstMod;
}

bool Attribute::isValidAttributeForMod(AttributeType attribute, bool isPrst)
{
    return isPrst ? s_attributesForPrstMod.contains(attribute) : s_attributesForInstMod.contains(attribute);
}
