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

#include "mcpfunctions.h"
#include "mcputils.h"
#include "utils.h"
#include "mcpattributes.h"
#include "mcpmodulators.h"
#include "soundfontmanager.h"
#include "mcpeditor.h"
#include "soundfonts.h"
#include "soundfont.h"
#include "instprst.h"
#include "division.h"
#include "smpl.h"
#include "voiceparam.h"
#include "voicecontext.h"
#include "midiprocessor.h"
#include "offlinerenderer.h"
#include "synthcoreconfig.h"
#include <QRegularExpression>

static QJsonObject describeSample(EltID id, bool detailed)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QJsonObject result = McpUtils::brief(id);
    quint32 length = sm->get(id, champ_dwLength).dwValue;
    quint32 sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
    int rootKey = sm->get(id, champ_byOriginalPitch).bValue;
    quint32 loopStart = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 loopEnd = sm->get(id, champ_dwEndLoop).dwValue;
    result["root_key"] = rootKey;
    result["root_key_name"] = McpUtils::keyName(rootKey);
    result["duration_s"] = Utils::roundSignificant(sampleRate > 0 ? static_cast<double>(length) / sampleRate : 0);
    result["looped"] = loopEnd > loopStart;

    SFSampleLink link = sm->get(id, champ_sfSampleType).sfLinkValue;
    result["channel"] = McpUtils::sampleChannel(link);
    if (link != monoSample && link != RomMonoSample)
        result["linked_sample"] = McpUtils::idToString(EltID(elementSmpl, id.indexSf2, sm->get(id, champ_wSampleLink).wValue));

    if (detailed)
    {
        result["type"] = "sample";
        result["length"] = static_cast<double>(length);
        result["sample_rate"] = static_cast<double>(sampleRate);
        result["pitch_correction"] = sm->get(id, champ_chPitchCorrection).cValue;
        result["loop_start"] = static_cast<double>(loopStart);
        result["loop_end"] = static_cast<double>(loopEnd);
        result["bits"] = sm->get(id, champ_bpsFile).wValue;

        // Instruments using the sample
        QJsonArray usedBy;
        EltID idInst(elementInst, id.indexSf2);
        foreach (int i, sm->getSiblings(idInst))
        {
            idInst.indexElt = i;
            EltID idDiv(elementInstSmpl, id.indexSf2, i);
            foreach (int j, sm->getSiblings(idDiv))
            {
                idDiv.indexElt2 = j;
                if (sm->get(idDiv, champ_sampleID).wValue == id.indexElt)
                {
                    usedBy.append(McpUtils::brief(idInst));
                    break;
                }
            }
        }
        result["used_by"] = usedBy;
    }
    return result;
}

static QJsonObject describeDivision(EltID idDiv)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QJsonObject result;
    result["id"] = McpUtils::idToString(idDiv);
    if (idDiv.typeElement == elementInstSmpl)
    {
        EltID idSmpl(elementSmpl, idDiv.indexSf2, sm->get(idDiv, champ_sampleID).wValue);
        QJsonObject sample = McpUtils::brief(idSmpl);
        int rootKey = sm->get(idSmpl, champ_byOriginalPitch).bValue;
        sample["root_key"] = rootKey;
        sample["channel"] = McpUtils::sampleChannel(sm->get(idSmpl, champ_sfSampleType).sfLinkValue);
        result["sample"] = sample;
    }
    else
        result["instrument"] = McpUtils::brief(EltID(elementInst, idDiv.indexSf2, sm->get(idDiv, champ_instrument).wValue));

    RangesType keyRange = sm->isSet(idDiv, champ_keyRange) ? sm->get(idDiv, champ_keyRange).rValue :
                              AttributeBase::getDefaultStoredValue(champ_keyRange, false).rValue;
    RangesType velRange = sm->isSet(idDiv, champ_velRange) ? sm->get(idDiv, champ_velRange).rValue :
                              AttributeBase::getDefaultStoredValue(champ_velRange, false).rValue;
    result["key_range"] = QJsonArray({ keyRange.byLo, keyRange.byHi });
    result["key_range_names"] = McpUtils::keyName(keyRange.byLo) + ".." + McpUtils::keyName(keyRange.byHi);
    result["velocity_range"] = QJsonArray({ velRange.byLo, velRange.byHi });

    QJsonObject parameters = McpAttributes::getValues(idDiv);
    parameters.remove("key_range");
    parameters.remove("velocity_range");
    result["parameters"] = parameters;

    QJsonArray mods = McpModulators::getAll(idDiv);
    if (!mods.isEmpty())
        result["modulators"] = mods;
    if (sm->get(idDiv, champ_mute).bValue > 0)
        result["muted"] = true;
    return result;
}

static QJsonObject describeInstPrst(EltID id, bool detailed)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool isPrst = (id.typeElement == elementPrst);
    QJsonObject result = McpUtils::brief(id);
    if (isPrst)
    {
        result["bank"] = sm->get(id, champ_wBank).wValue;
        result["preset"] = sm->get(id, champ_wPreset).wValue;
    }

    EltID idDiv(isPrst ? elementPrstInst : elementInstSmpl, id.indexSf2, id.indexElt);
    QList<int> divisionIndexes = sm->getSiblings(idDiv);
    if (!detailed)
    {
        // Summary: number of divisions, key span and elements used
        int minKey = 127, maxKey = 0;
        QJsonArray elements;
        QList<int> elementIndexes;
        foreach (int i, divisionIndexes)
        {
            idDiv.indexElt2 = i;
            RangesType range = sm->isSet(idDiv, champ_keyRange) ? sm->get(idDiv, champ_keyRange).rValue :
                                   AttributeBase::getDefaultStoredValue(champ_keyRange, false).rValue;
            minKey = qMin(minKey, static_cast<int>(range.byLo));
            maxKey = qMax(maxKey, static_cast<int>(range.byHi));
            int index = sm->get(idDiv, isPrst ? champ_instrument : champ_sampleID).wValue;
            if (isPrst && !elementIndexes.contains(index))
            {
                elementIndexes << index;
                elements.append(McpUtils::idToString(EltID(elementInst, id.indexSf2, index)));
            }
        }
        result["division_count"] = divisionIndexes.count();
        if (!divisionIndexes.isEmpty())
            result["key_span"] = McpUtils::keyName(minKey) + ".." + McpUtils::keyName(maxKey);
        if (isPrst)
            result["instruments"] = elements;
        return result;
    }

    result["type"] = isPrst ? "preset" : "instrument";
    result["global_parameters"] = McpAttributes::getValues(id);
    QJsonArray mods = McpModulators::getAll(id);
    if (!mods.isEmpty())
        result["global_modulators"] = mods;

    QJsonArray divisions;
    foreach (int i, divisionIndexes)
    {
        idDiv.indexElt2 = i;
        divisions.append(describeDivision(idDiv));
    }
    result["divisions"] = divisions;

    if (!isPrst)
    {
        // Presets using the instrument
        QJsonArray usedBy;
        EltID idPrst(elementPrst, id.indexSf2);
        foreach (int i, sm->getSiblings(idPrst))
        {
            idPrst.indexElt = i;
            EltID idPrstDiv(elementPrstInst, id.indexSf2, i);
            foreach (int j, sm->getSiblings(idPrstDiv))
            {
                idPrstDiv.indexElt2 = j;
                if (sm->get(idPrstDiv, champ_instrument).wValue == id.indexElt)
                {
                    usedBy.append(McpUtils::brief(idPrst));
                    break;
                }
            }
        }
        result["used_by"] = usedBy;
    }
    return result;
}

static QJsonObject describeSoundfontInfo(int indexSf2)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, indexSf2);
    QJsonObject result = McpUtils::brief(id);
    result["file"] = sm->getQstr(id, champ_filenameInitial);
    result["unsaved_changes"] = sm->isEdited(indexSf2);
    SfVersionTag version = sm->get(id, champ_IFIL).sfVerValue;
    result["version"] = QString("%1.%2").arg(version.wMajor).arg(version.wMinor, 2, 10, QChar('0'));
    result["author"] = sm->getQstr(id, champ_IENG);
    result["copyright"] = sm->getQstr(id, champ_ICOP);
    result["comment"] = sm->getQstr(id, champ_ICMT);
    result["product"] = sm->getQstr(id, champ_IPRD);
    result["date"] = sm->getQstr(id, champ_ICRD);
    result["sound_engine"] = sm->getQstr(id, champ_ISNG);
    result["software"] = sm->getQstr(id, champ_ISFT);
    return result;
}

static QJsonObject describeElement(EltID id)
{
    switch (id.typeElement)
    {
    case elementSf2:
        return describeSoundfontInfo(id.indexSf2);
    case elementSmpl:
        return describeSample(id, true);
    case elementInst: case elementPrst:
        return describeInstPrst(id, true);
    case elementInstSmpl: case elementPrstInst:
        return describeDivision(id);
    default:
        return McpModulators::toJson(id);
    }
}

// Key or velocity range of a division, otherwise of the global division, otherwise the full range (as the synth)
static RangesType zoneRange(ISynthZone * zone, ISynthZone * globalZone, AttributeType champ)
{
    if (zone->getAttributeSet()[champ])
        return zone->getAttributeValues()[champ].rValue;
    if (globalZone != nullptr && globalZone->getAttributeSet()[champ])
        return globalZone->getAttributeValues()[champ].rValue;
    RangesType full;
    full.byLo = 0;
    full.byHi = 127;
    return full;
}

static bool inRanges(ISynthZone * zone, ISynthZone * globalZone, int key, int velocity)
{
    RangesType keyRange = zoneRange(zone, globalZone, champ_keyRange);
    RangesType velRange = zoneRange(zone, globalZone, champ_velRange);
    return keyRange.byLo <= key && key <= keyRange.byHi && velRange.byLo <= velocity && velocity <= velRange.byHi;
}

// Voice started by the synth for a division of an instrument (possibly reached from a preset division): parameters
// computed by the same code as the sound engine (VoiceParam), modulators included
static QJsonObject describeVoice(int indexSf2, Soundfont * soundfont, InstPrst * prst, int prstDivIndex,
                                 InstPrst * inst, int instDivIndex, int key, int velocity)
{
    Division * prstDiv = prst != nullptr ? prst->getDivision(prstDivIndex) : nullptr;
    Division * instDiv = inst->getDivision(instDivIndex);
    int sampleIndex = instDiv->getAttributeSet()[champ_sampleID] ? instDiv->getAttributeValues()[champ_sampleID].wValue : -1;
    Smpl * smpl = sampleIndex >= 0 ? soundfont->getSample(sampleIndex) : nullptr;
    if (smpl == nullptr)
        return QJsonObject();

    // Default settings of the sound engine (as the offline rendering): A4 = 440 Hz, equal temperament, default values
    // of the controllers as in the editor (volume and expression at 127, pan at 64, the controllers read by the
    // modulators of the soundfont at 64 if bipolar, 127 if decreasing)
    MidiProcessor midiValues;
    OfflineRenderer::applyDefaultControllerValues(soundfont, midiValues);
    VoiceContext context;
    context.setMidiValues(&midiValues);
    context.setModulatorVelToFilterType(SynthCoreConfig().modulatorVelToFilterType);
    VoiceParam param(&context);
    param.initialize(indexSf2, prst, prstDiv, inst, instDiv, smpl, static_cast<int8_t>(prst != nullptr ? 0 : -1),
                     static_cast<int8_t>(key), static_cast<int8_t>(velocity), 0);

    SoundfontManager * sm = SoundfontManager::getInstance();
    QJsonObject voice;
    if (prst != nullptr)
        voice["preset_division"] = McpUtils::idToString(EltID(elementPrstInst, indexSf2, prst->getIndex(), prstDivIndex));
    voice["instrument"] = McpUtils::brief(EltID(elementInst, indexSf2, inst->getIndex()));
    voice["instrument_division"] = McpUtils::idToString(EltID(elementInstSmpl, indexSf2, inst->getIndex(), instDivIndex));
    EltID idSmpl(elementSmpl, indexSf2, sampleIndex);
    QJsonObject sample = McpUtils::brief(idSmpl);
    sample["root_key"] = sm->get(idSmpl, champ_byOriginalPitch).bValue;
    sample["pitch_correction"] = sm->get(idSmpl, champ_chPitchCorrection).cValue;
    sample["sample_rate"] = static_cast<double>(sm->get(idSmpl, champ_dwSampleRate).dwValue);
    sample["channel"] = McpUtils::sampleChannel(sm->get(idSmpl, champ_sfSampleType).sfLinkValue);
    voice["sample"] = sample;

    int playedKey = param.getCurrentInteger(champ_keynum);
    int rootKey = param.getCurrentInteger(champ_overridingRootKey);
    int scaleTuning = param.getCurrentInteger(champ_scaleTuning);
    int fineTuneWithCorrection = param.getCurrentInteger(champ_fineTune);
    int coarseTune = param.getCurrentInteger(champ_coarseTune);
    voice["sample_mode"] = param.getCurrentInteger(champ_sampleModes);
    voice["key_used"] = playedKey;
    voice["velocity_used"] = param.getCurrentInteger(champ_velocity);
    voice["root_key_used"] = rootKey;
    voice["static_pitch_shift_cents"] = (playedKey - rootKey) * scaleTuning + fineTuneWithCorrection + 100 * coarseTune;

    QJsonObject parameters;
    foreach (QString name, McpAttributes::getNames())
    {
        if (name == "key_range" || name == "velocity_range" || name == "root_key" || name == "fixed_key" ||
            name == "fixed_velocity" || name == "sample_modes" || McpAttributes::isOffset(name))
            continue;
        AttributeType champ = McpAttributes::getChamp(name);
        if (champ == champ_exclusiveClass)
            parameters[name] = param.getCurrentInteger(champ);
        else
            parameters[name] = Utils::roundSignificant(param.getCurrentFloat(champ));
    }

    // Hold and decay durations at this key
    auto atKey = [&](AttributeType duration, AttributeType keyScaling) -> double {
        return param.getCurrentFloat(duration) * std::pow(2.0, param.getCurrentInteger(keyScaling) / 1200.0 * (60 - playedKey));
    };
    parameters["vol_env_hold_at_key"] = Utils::roundSignificant(atKey(champ_holdVolEnv, champ_keynumToVolEnvHold));
    parameters["vol_env_decay_at_key"] = Utils::roundSignificant(atKey(champ_decayVolEnv, champ_keynumToVolEnvDecay));
    parameters["mod_env_hold_at_key"] = Utils::roundSignificant(atKey(champ_holdModEnv, champ_keynumToModEnvHold));
    parameters["mod_env_decay_at_key"] = Utils::roundSignificant(atKey(champ_decayModEnv, champ_keynumToModEnvDecay));
    voice["parameters"] = parameters;

    QJsonObject positions;
    positions["start"] = static_cast<double>(param.getPosition(champ_dwStart16));
    positions["end"] = static_cast<double>(param.getPosition(champ_dwLength));
    positions["loop_start"] = static_cast<double>(param.getPosition(champ_dwStartLoop));
    positions["loop_end"] = static_cast<double>(param.getPosition(champ_dwEndLoop));
    voice["sample_positions"] = positions;
    return voice;
}

// Voices started by a key of an instrument or a preset
static QJsonObject describeKey(EltID id, int key, int velocity)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    Soundfont * soundfont = sm->getSoundfonts()->getSoundfont(id.indexSf2);
    if (soundfont == nullptr)
        throw McpException("the soundfont is not loaded");

    QJsonArray voices;
    auto addInstrumentVoices = [&](InstPrst * prst, int prstDivIndex, InstPrst * inst) {
        EltID idDiv(elementInstSmpl, id.indexSf2, inst->getIndex());
        foreach (int i, sm->getSiblings(idDiv))
        {
            Division * division = inst->getDivision(i);
            if (division != nullptr && division->isPlayable() && inRanges(division, inst->getGlobalDivision(), key, velocity))
            {
                QJsonObject voice = describeVoice(id.indexSf2, soundfont, prst, prstDivIndex, inst, i, key, velocity);
                if (!voice.isEmpty())
                    voices.append(voice);
            }
        }
    };

    if (id.typeElement == elementPrst)
    {
        InstPrst * prst = soundfont->getPreset(id.indexElt);
        EltID idDiv(elementPrstInst, id.indexSf2, id.indexElt);
        foreach (int i, sm->getSiblings(idDiv))
        {
            Division * division = prst->getDivision(i);
            if (division == nullptr || !division->isPlayable() || !inRanges(division, prst->getGlobalDivision(), key, velocity) ||
                !division->getAttributeSet()[champ_instrument])
                continue;
            InstPrst * inst = soundfont->getInstrument(division->getAttributeValues()[champ_instrument].wValue);
            if (inst != nullptr)
                addInstrumentVoices(prst, i, inst);
        }
    }
    else
    {
        InstPrst * inst = soundfont->getInstrument(id.indexElt);
        if (inst != nullptr)
            addInstrumentVoices(nullptr, -1, inst);
    }

    QJsonObject result = McpUtils::brief(id);
    result["key"] = key;
    result["key_name"] = McpUtils::keyName(key);
    result["velocity"] = velocity;
    result["voices"] = voices;
    result["note"] = "one voice per division of instrument reached (a stereo pair gives two voices); values computed by "
                     "the sound engine when the key is pressed: division value, otherwise global value, otherwise "
                     "default, preset values added or multiplied, then the modulators (default modulators included) "
                     "with the default values of the controllers as in the editor, in the units of list_parameters "
                     "(attenuation in dB, times in s, filter cutoff in Hz, pan from -50 to 50); sample_mode 2: played when the key is "
                     "released; static_pitch_shift_cents: transposition applied to the sample data = (key_used - "
                     "root_key_used) x scale_tuning + fine tune + pitch correction of the sample + 100 x coarse tune, "
                     "without what changes over time (modulation envelope and LFOs to the pitch, possibly compensated "
                     "by the static values) nor the reference pitch and the temperament of the settings: render_note "
                     "measures the pitch actually heard over time; *_at_key: hold and decay durations once scaled by the key; sample_positions: frames "
                     "of the sample data played (offsets applied)";
    return result;
}

void McpFunctions::addBrowsingFunctions(QList<McpFunction *> &functions)
{
    functions << new McpFunction(
        "list_soundfonts", McpFunction::CATEGORY_BROWSING, true,
        "List the soundfonts open in Polyphone, with their identifiers, files and number of elements.",
        R"JSON({"properties":{}})JSON",
        [](const McpArguments &) -> McpResult {
            SoundfontManager * sm = SoundfontManager::getInstance();
            int current = McpEditor::getInstance() == nullptr ? -1 : McpEditor::getInstance()->getCurrentSoundfont();
            QJsonArray soundfonts;
            foreach (int i, McpUtils::getSoundfontIndexes())
            {
                QJsonObject soundfont = McpUtils::brief(EltID(elementSf2, i));
                soundfont["file"] = sm->getQstr(EltID(elementSf2, i), champ_filenameInitial);
                soundfont["unsaved_changes"] = sm->isEdited(i);
                soundfont["displayed"] = (i == current);
                EltID id(elementSmpl, i);
                soundfont["samples"] = sm->getSiblings(id).count();
                id.typeElement = elementInst;
                soundfont["instruments"] = sm->getSiblings(id).count();
                id.typeElement = elementPrst;
                soundfont["presets"] = sm->getSiblings(id).count();
                soundfonts.append(soundfont);
            }
            QJsonObject result;
            result["soundfonts"] = soundfonts;
            return result;
        });

    functions << new McpFunction(
        "get_selection", McpFunction::CATEGORY_BROWSING, true,
        "Get the soundfont displayed in Polyphone and the elements selected by the user in its tree. Use it when "
        "the user refers to \"this sample\", \"the selected instrument\", etc.",
        R"JSON({"properties":{}})JSON",
        [](const McpArguments &) -> McpResult {
            QJsonObject result;
            int current = McpEditor::getInstance() == nullptr ? -1 : McpEditor::getInstance()->getCurrentSoundfont();
            result["soundfont"] = current >= 0 ? QJsonValue(McpUtils::idToString(EltID(elementSf2, current))) : QJsonValue();
            QJsonArray selection;
            if (current >= 0)
            {
                SoundfontManager * sm = SoundfontManager::getInstance();
                foreach (EltID id, McpEditor::getInstance()->getCurrentSelection())
                {
                    if (!sm->isValid(id))
                        continue;
                    QJsonObject item = McpUtils::brief(id);
                    item["type"] = McpUtils::typeName(id.typeElement);
                    selection.append(item);
                }
            }
            result["selection"] = selection;
            return result;
        });

    functions << new McpFunction(
        "get_soundfont", McpFunction::CATEGORY_BROWSING, true,
        "Overview of a soundfont: information fields and the list of its samples (root key, duration, loop, stereo "
        "link), instruments (number of divisions, key span) and presets (bank, preset number, instruments used).",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier, for instance \"sf0\""},
            "sections":{"type":"array","items":{"type":"string","enum":["info","samples","instruments","presets"]},
                        "description":"Parts to return, all by default"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            QStringList sections;
            if (args.contains("sections"))
            {
                for (const QJsonValue value : args.getArray("sections"))
                    sections << value.toString();
            }
            else
                sections << "info" << "samples" << "instruments" << "presets";

            SoundfontManager * sm = SoundfontManager::getInstance();
            QJsonObject result = McpUtils::brief(idSf2);
            if (sections.contains("info"))
                result["info"] = describeSoundfontInfo(idSf2.indexSf2);
            if (sections.contains("samples"))
            {
                QJsonArray samples;
                EltID id(elementSmpl, idSf2.indexSf2);
                foreach (int i, sm->getSiblings(id))
                {
                    id.indexElt = i;
                    samples.append(describeSample(id, false));
                }
                result["samples"] = samples;
            }
            if (sections.contains("instruments") || sections.contains("presets"))
            {
                for (int type = 0; type < 2; type++)
                {
                    QString section = (type == 0 ? "instruments" : "presets");
                    if (!sections.contains(section))
                        continue;
                    QJsonArray elements;
                    EltID id(type == 0 ? elementInst : elementPrst, idSf2.indexSf2);
                    foreach (int i, sm->getSiblings(id))
                    {
                        id.indexElt = i;
                        elements.append(describeInstPrst(id, false));
                    }
                    result[section] = elements;
                }
            }
            return result;
        });

    functions << new McpFunction(
        "get_element", McpFunction::CATEGORY_BROWSING, true,
        "Detailed description of an element. Sample: length, sample rate, root key, pitch correction, loop points, "
        "stereo link, instruments using it. Instrument or preset: global parameters, modulators and all divisions "
        "with their sample / instrument, key and velocity ranges, parameters (only the ones explicitly set: a division "
        "inherits the global parameters it doesn't define, then the default values). Soundfont: information fields.",
        R"JSON({"properties":{
            "id":{"type":"string","description":"Element identifier, for instance \"sf0/inst2\" or \"sf0/smpl5\""}},
            "required":["id"]})JSON",
        [](const McpArguments &args) -> McpResult {
            return describeElement(args.getId("id"));
        });

    functions << new McpFunction(
        "find_elements", McpFunction::CATEGORY_BROWSING, true,
        "Find samples, instruments or presets whose name contains a text (case insensitive) or matches a regular "
        "expression.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "text":{"type":"string","description":"Text to search in the names"},
            "regex":{"type":"boolean","description":"true if the text is a regular expression (default false)"},
            "type":{"type":"string","enum":["sample","instrument","preset"],"description":"Restrict the search to a type"}},
            "required":["soundfont","text"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            QString text = args.getString("text");
            QRegularExpression regex(args.getBool("regex", false) ? text : QRegularExpression::escape(text),
                                     QRegularExpression::CaseInsensitiveOption);
            if (!regex.isValid())
                throw McpException("invalid regular expression: " + regex.errorString());
            QString type = args.getString("type", "");

            SoundfontManager * sm = SoundfontManager::getInstance();
            QJsonArray found;
            QList<QPair<ElementType, QString> > types = {
                { elementSmpl, "sample" }, { elementInst, "instrument" }, { elementPrst, "preset" } };
            for (int t = 0; t < types.count(); t++)
            {
                if (!type.isEmpty() && type != types[t].second)
                    continue;
                EltID id(types[t].first, idSf2.indexSf2);
                foreach (int i, sm->getSiblings(id))
                {
                    id.indexElt = i;
                    if (regex.match(sm->getQstr(id, champ_name)).hasMatch())
                    {
                        QJsonObject item = McpUtils::brief(id);
                        item["type"] = types[t].second;
                        found.append(item);
                    }
                }
            }
            QJsonObject result;
            result["elements"] = found;
            return result;
        });

    functions << new McpFunction(
        "list_parameters", McpFunction::CATEGORY_BROWSING, true,
        "Describe the parameters of instruments, presets and divisions (names used by set_parameters, units, ranges, "
        "default values) and the format of the modulators.",
        R"JSON({"properties":{}})JSON",
        [](const McpArguments &) -> McpResult {
            QJsonObject result;
            result["parameters"] = McpAttributes::getCatalog();
            result["rules"] = "Instrument values are absolute. Preset values are relative: added to the instrument "
                              "values, or multiplying them (\"factor\"). A division uses its own value if set, otherwise "
                              "the global value of its instrument / preset, otherwise the default value.";
            result["modulator_format"] = McpModulators::FORMAT;
            return result;
        });

    functions << new McpFunction(
        "describe_key", McpFunction::CATEGORY_BROWSING, true,
        "What a key of a preset or an instrument plays, without rendering: each voice started (preset division, "
        "instrument division, sample) with the values computed by the sound engine once all levels are combined and "
        "the modulators applied at this velocity: root key used, static transposition applied to the sample, "
        "attenuation, pan, filter, envelopes (hold and decay scaled by the key), LFOs, effects, sample positions. What "
        "evolves over time (envelopes, LFOs) is given as parameters, not as a result: render_note measures it.",
        R"JSON({"properties":{
            "element":{"type":"string","description":"Preset or instrument"},
            "key":{"type":["integer","string"],"description":"Key (number or note name)"},
            "velocity":{"type":"integer","minimum":1,"maximum":127,"description":"Default 100"}},
            "required":["element","key"]})JSON",
        [](const McpArguments &args) -> McpResult {
            return describeKey(args.getId("element", { elementPrst, elementInst }), args.getKey("key"),
                               args.getInt("velocity", 1, 127, 100));
        });

    functions << new McpFunction(
        "show_element", McpFunction::CATEGORY_BROWSING, true,
        "Display a soundfont in Polyphone and select one of its elements, so that the user sees it.",
        R"JSON({"properties":{
            "id":{"type":"string","description":"Element identifier"}},
            "required":["id"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("id");
            if (McpEditor::getInstance() == nullptr || !McpEditor::getInstance()->showElement(id))
                throw McpException("the soundfont is not displayed in an editor");
            return QString("Element displayed.");
        });
}
