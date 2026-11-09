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

#include "mcpattributes.h"
#include "mcputils.h"
#include "utils.h"
#include "mcpfunction.h"
#include "soundfontmanager.h"

const QList<McpAttributes::Definition> &McpAttributes::getDefinitions()
{
    static const QList<Definition> definitions = {
        { "key_range", champ_keyRange, champ_unknown, false, "[min, max] MIDI keys", "[min, max] MIDI keys",
         "Keys playing the division (only for divisions)." },
        { "velocity_range", champ_velRange, champ_unknown, false, "[min, max] velocities", "[min, max] velocities",
         "Velocities playing the division (only for divisions)." },
        { "attenuation", champ_initialAttenuation, champ_unknown, false, "dB (0 to 57.6)", "dB added",
         "Attenuation of the volume." },
        { "pan", champ_pan, champ_unknown, false, "-50 (left) to 50 (right)", "added (-100 to 100)",
         "Stereo position." },
        { "coarse_tune", champ_coarseTune, champ_unknown, false, "semitones", "semitones added", "Transposition." },
        { "fine_tune", champ_fineTune, champ_unknown, false, "cents (-99 to 99)", "cents added", "Fine tuning." },
        { "scale_tuning", champ_scaleTuning, champ_unknown, false, "cents per key (default 100)", "cents added",
         "Pitch difference between two adjacent keys: 100 is the usual tuning, 0 plays all keys at the same pitch." },
        { "root_key", champ_overridingRootKey, champ_unknown, true, "MIDI key, -1 = root key of the sample", "",
         "Key playing the sample at its original pitch, overriding the root key of the sample." },
        { "fixed_key", champ_keynum, champ_unknown, true, "MIDI key, -1 = not forced", "",
         "Key forced for all notes (as if this key was always played)." },
        { "fixed_velocity", champ_velocity, champ_unknown, true, "velocity, -1 = not forced", "",
         "Velocity forced for all notes." },
        { "sample_modes", champ_sampleModes, champ_unknown, true,
         "0 = no loop, 1 = continuous loop, 2 = played only when the key is released (release sample, extension of "
         "Polyphone, 0 for the other synths), 3 = loop then play the end of the sample when the key is released", "",
         "Loop mode of the sample." },
        { "exclusive_class", champ_exclusiveClass, champ_unknown, true, "0 (none) to 127", "",
         "A note stops the notes of the same class in the same preset (hi-hats for instance)." },
        { "filter_cutoff", champ_initialFilterFc, champ_unknown, false, "Hz (19.9 to 19914, 19914 = no filtering)",
         "factor multiplying the instrument value", "Cutoff frequency of the low-pass filter." },
        { "filter_resonance", champ_initialFilterQ, champ_unknown, false, "dB (0 to 96)", "dB added",
         "Resonance (Q) of the low-pass filter." },
        { "vol_env_delay", champ_delayVolEnv, champ_unknown, false, "s", "factor", "Volume envelope: delay before the attack." },
        { "vol_env_attack", champ_attackVolEnv, champ_unknown, false, "s", "factor", "Volume envelope: attack duration." },
        { "vol_env_hold", champ_holdVolEnv, champ_unknown, false, "s", "factor", "Volume envelope: hold duration after the attack." },
        { "vol_env_decay", champ_decayVolEnv, champ_unknown, false, "s", "factor",
         "Volume envelope: duration of the decrease to the sustain level (time for a 100 dB decrease)." },
        { "vol_env_sustain", champ_sustainVolEnv, champ_unknown, false, "dB of attenuation (0 to 144)", "dB added",
         "Volume envelope: attenuation during the sustain." },
        { "vol_env_release", champ_releaseVolEnv, champ_unknown, false, "s", "factor",
         "Volume envelope: release duration after the key is released (time for a 100 dB decrease)." },
        { "key_to_vol_env_hold", champ_keynumToVolEnvHold, champ_unknown, false, "timecents per key", "added",
         "Volume envelope: change of the hold duration per key, relative to key 60 (positive values shorten the hold of the high keys; 100 = the duration is halved each octave)." },
        { "key_to_vol_env_decay", champ_keynumToVolEnvDecay, champ_unknown, false, "timecents per key", "added",
         "Volume envelope: change of the decay duration per key, relative to key 60 (positive values shorten the decay of the high keys; 100 = the duration is halved each octave)." },
        { "mod_env_delay", champ_delayModEnv, champ_unknown, false, "s", "factor", "Modulation envelope: delay." },
        { "mod_env_attack", champ_attackModEnv, champ_unknown, false, "s", "factor", "Modulation envelope: attack." },
        { "mod_env_hold", champ_holdModEnv, champ_unknown, false, "s", "factor", "Modulation envelope: hold." },
        { "mod_env_decay", champ_decayModEnv, champ_unknown, false, "s", "factor", "Modulation envelope: decay." },
        { "mod_env_sustain", champ_sustainModEnv, champ_unknown, false, "% of decrease (0 to 100)", "added",
         "Modulation envelope: sustain level (0 = full level)." },
        { "mod_env_release", champ_releaseModEnv, champ_unknown, false, "s", "factor", "Modulation envelope: release." },
        { "key_to_mod_env_hold", champ_keynumToModEnvHold, champ_unknown, false, "timecents per key", "added",
         "Modulation envelope: change of the hold duration per key." },
        { "key_to_mod_env_decay", champ_keynumToModEnvDecay, champ_unknown, false, "timecents per key", "added",
         "Modulation envelope: change of the decay duration per key." },
        { "mod_env_to_pitch", champ_modEnvToPitch, champ_unknown, false, "cents", "cents added",
         "Effect of the modulation envelope on the pitch." },
        { "mod_env_to_filter", champ_modEnvToFilterFc, champ_unknown, false, "cents", "cents added",
         "Effect of the modulation envelope on the filter cutoff." },
        { "mod_lfo_delay", champ_delayModLFO, champ_unknown, false, "s", "factor", "Modulation LFO: delay before it starts." },
        { "mod_lfo_freq", champ_freqModLFO, champ_unknown, false, "Hz", "factor", "Modulation LFO: frequency." },
        { "mod_lfo_to_pitch", champ_modLfoToPitch, champ_unknown, false, "cents", "cents added",
         "Modulation LFO: effect on the pitch (vibrato)." },
        { "mod_lfo_to_filter", champ_modLfoToFilterFc, champ_unknown, false, "cents", "cents added",
         "Modulation LFO: effect on the filter cutoff (wah)." },
        { "mod_lfo_to_volume", champ_modLfoToVolume, champ_unknown, false, "dB (-96 to 96)", "dB added",
         "Modulation LFO: effect on the volume (tremolo)." },
        { "vib_lfo_delay", champ_delayVibLFO, champ_unknown, false, "s", "factor", "Vibrato LFO: delay before it starts." },
        { "vib_lfo_freq", champ_freqVibLFO, champ_unknown, false, "Hz", "factor", "Vibrato LFO: frequency." },
        { "vib_lfo_to_pitch", champ_vibLfoToPitch, champ_unknown, false, "cents", "cents added",
         "Vibrato LFO: depth of the vibrato." },
        { "chorus_send", champ_chorusEffectsSend, champ_unknown, false, "% (0 to 100)", "added", "Amount sent to the chorus." },
        { "reverb_send", champ_reverbEffectsSend, champ_unknown, false, "% (0 to 100)", "added", "Amount sent to the reverb." },
        { "start_offset", champ_startAddrsOffset, champ_startAddrsCoarseOffset, true, "samples", "",
         "Offset of the beginning of the sample played." },
        { "end_offset", champ_endAddrsOffset, champ_endAddrsCoarseOffset, true, "samples", "",
         "Offset of the end of the sample played." },
        { "loop_start_offset", champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset, true, "samples", "",
         "Offset of the loop start." },
        { "loop_end_offset", champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset, true, "samples", "",
         "Offset of the loop end." }
    };
    return definitions;
}

const McpAttributes::Definition * McpAttributes::find(QString name)
{
    foreach (const Definition &definition, getDefinitions())
        if (name == definition.name)
            return &definition;
    return nullptr;
}

QString McpAttributes::getName(AttributeType champ)
{
    foreach (const Definition &definition, getDefinitions())
        if (definition.champ == champ || definition.coarseChamp == champ)
            return definition.name;
    return "";
}

AttributeType McpAttributes::getChamp(QString name)
{
    const Definition * definition = find(name);
    return definition == nullptr ? champ_unknown : definition->champ;
}

QStringList McpAttributes::getNames()
{
    QStringList result;
    foreach (const Definition &definition, getDefinitions())
        result << definition.name;
    return result;
}

bool McpAttributes::isOffset(QString name)
{
    const Definition * definition = find(name);
    return definition != nullptr && definition->coarseChamp != champ_unknown;
}

bool McpAttributes::isInstrumentOnly(QString name)
{
    const Definition * definition = find(name);
    return definition != nullptr && definition->instrumentOnly;
}

void McpAttributes::getRealLimits(const Definition &definition, bool isPrst, double &min, double &max)
{
    int storedMin = -32768, storedMax = 32767;
    AttributeBase::getLimit(definition.champ, isPrst, storedMin, storedMax);
    AttributeValue value;
    value.shValue = static_cast<int16_t>(storedMin);
    min = Attribute::toRealValue(definition.champ, isPrst, value);
    value.shValue = static_cast<int16_t>(storedMax);
    max = Attribute::toRealValue(definition.champ, isPrst, value);
    if (min > max)
        std::swap(min, max);
}

QJsonArray McpAttributes::getCatalog()
{
    QJsonArray result;
    foreach (const Definition &definition, getDefinitions())
    {
        QJsonObject item;
        item["name"] = definition.name;
        item["description"] = definition.description;
        item["unit"] = definition.unit;
        if (definition.instrumentOnly)
            item["instrument_only"] = true;
        else
            item["preset_unit"] = definition.presetUnit;

        if (definition.champ != champ_keyRange && definition.champ != champ_velRange && definition.coarseChamp == champ_unknown)
        {
            double min, max;
            getRealLimits(definition, false, min, max);
            item["instrument_range"] = QJsonArray({ Utils::roundSignificant(min), Utils::roundSignificant(max) });
            item["default"] = Utils::roundSignificant(AttributeBase::getDefaultRealValue(definition.champ, false));
        }
        result.append(item);
    }
    return result;
}

QJsonValue McpAttributes::getValue(EltID id, QString name)
{
    const Definition * definition = find(name);
    if (definition == nullptr)
        throw McpException("unknown parameter \"" + name + "\" (see list_parameters)");

    SoundfontManager * sm = SoundfontManager::getInstance();
    bool isSet = sm->isSet(id, definition->champ) ||
                 (definition->coarseChamp != champ_unknown && sm->isSet(id, definition->coarseChamp));
    if (!isSet)
        return QJsonValue();

    AttributeValue value = sm->get(id, definition->champ);
    if (definition->champ == champ_keyRange || definition->champ == champ_velRange)
        return QJsonArray({ value.rValue.byLo, value.rValue.byHi });
    if (definition->coarseChamp != champ_unknown)
        return static_cast<int>(value.shValue) + 32768 * static_cast<int>(sm->get(id, definition->coarseChamp).shValue);
    return Utils::roundSignificant(Attribute::toRealValue(definition->champ, id.isPrst(), value));
}

QJsonObject McpAttributes::getValues(EltID id)
{
    QJsonObject result;
    foreach (const Definition &definition, getDefinitions())
    {
        QJsonValue value = getValue(id, definition.name);
        if (!value.isNull())
            result[definition.name] = value;
    }
    return result;
}

void McpAttributes::setValue(EltID id, QString name, QJsonValue value)
{
    const Definition * definition = find(name);
    if (definition == nullptr)
        throw McpException("unknown parameter \"" + name + "\" (see list_parameters)");
    if (value.isNull())
    {
        resetValue(id, name);
        return;
    }

    bool isPrst = id.isPrst();
    if (isPrst && definition->instrumentOnly)
        throw McpException("parameter \"" + name + "\" is not allowed in a preset");
    bool isDivision = (id.typeElement == elementInstSmpl || id.typeElement == elementPrstInst);
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue storedValue;

    if (definition->champ == champ_keyRange || definition->champ == champ_velRange)
    {
        if (!isDivision)
            throw McpException("parameter \"" + name + "\" can only be set in a division");
        QJsonArray range = value.toArray();
        if (!value.isArray() || range.count() != 2)
            throw McpException("parameter \"" + name + "\" must be an array [min, max]");
        int bounds[2];
        for (int i = 0; i < 2; i++)
        {
            if (definition->champ == champ_keyRange)
                bounds[i] = McpUtils::readKey(range[i], name);
            else
            {
                double number = range[i].toDouble(-1);
                if (!range[i].isDouble() || number != qRound(number) || number < 0 || number > 127)
                    throw McpException("parameter \"" + name + "\" must contain integers between 0 and 127");
                bounds[i] = qRound(number);
            }
        }
        storedValue.rValue.byLo = static_cast<uint8_t>(qMin(bounds[0], bounds[1]));
        storedValue.rValue.byHi = static_cast<uint8_t>(qMax(bounds[0], bounds[1]));
        sm->set(id, definition->champ, storedValue);
        return;
    }

    // Keys can be given with their names
    double number;
    if (definition->champ == champ_overridingRootKey || definition->champ == champ_keynum)
        number = (value.isDouble() && value.toDouble() == -1) ? -1 : McpUtils::readKey(value, name);
    else if (value.isDouble())
        number = value.toDouble();
    else
        throw McpException("parameter \"" + name + "\" must be a number");

    if (definition->coarseChamp != champ_unknown)
    {
        int offset = qRound(number);
        if (offset < -32768 * 32767 || offset > 32768 * 32767)
            throw McpException("parameter \"" + name + "\" is out of range");
        int coarse = offset / 32768; // Rounded toward 0
        int fine = offset - 32768 * coarse;
        storedValue.shValue = static_cast<int16_t>(fine);
        sm->set(id, definition->champ, storedValue);
        if (coarse == 0)
            sm->reset(id, definition->coarseChamp);
        else
        {
            storedValue.shValue = static_cast<int16_t>(coarse);
            sm->set(id, definition->coarseChamp, storedValue);
        }
        return;
    }

    double min, max;
    getRealLimits(*definition, isPrst, min, max);
    if (number < min - 1e-6 * std::fabs(min) - 1e-9 || number > max + 1e-6 * std::fabs(max) + 1e-9)
        throw McpException(QString("parameter \"%1\" must be between %2 and %3 in %4").arg(name)
                               .arg(Utils::roundSignificant(min)).arg(Utils::roundSignificant(max)).arg(isPrst ? "a preset" : "an instrument"));
    sm->set(id, definition->champ, Attribute::fromRealValue(definition->champ, isPrst, number));
}

void McpAttributes::resetValue(EltID id, QString name)
{
    const Definition * definition = find(name);
    if (definition == nullptr)
        throw McpException("unknown parameter \"" + name + "\" (see list_parameters)");
    if (definition->champ == champ_keyRange || definition->champ == champ_velRange)
        throw McpException("parameter \"" + name + "\" cannot be removed, set it to [0, 127] instead");

    SoundfontManager * sm = SoundfontManager::getInstance();
    sm->reset(id, definition->champ);
    if (definition->coarseChamp != champ_unknown)
        sm->reset(id, definition->coarseChamp);
}
