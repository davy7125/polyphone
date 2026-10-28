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
