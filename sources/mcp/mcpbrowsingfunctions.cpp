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
#include <QRegularExpression>

static QString sampleType(SFSampleLink link)
{
    switch (link)
    {
    case monoSample: case RomMonoSample: return "mono";
    case leftSample: case RomLeftSample: return "left";
    case rightSample: case RomRightSample: return "right";
    case linkedSample: case RomLinkedSample: return "linked";
    default: return "unknown";
    }
}

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
    result["channel"] = sampleType(link);
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
        sample["channel"] = sampleType(sm->get(idSmpl, champ_sfSampleType).sfLinkValue);
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
