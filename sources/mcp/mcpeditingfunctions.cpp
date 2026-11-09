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
#include "mcpattributes.h"
#include "mcpmodulators.h"
#include "soundfontmanager.h"
#include "mcpeditor.h"
#include "inputfactory.h"
#include "outputfactory.h"
#include "abstractoutput.h"
#include "sampleloader.h"
#include "duplicator.h"
#include "contextmanager.h"
#include "auto_distribution/toolautodistribution.h"
#include <QFileInfo>

static QString checkName(QString name)
{
    name = name.trimmed();
    if (name.isEmpty())
        throw McpException("the name cannot be empty");
    return name.left(20);
}

static QJsonArray idsToJson(IdList ids)
{
    QJsonArray result;
    foreach (EltID id, ids)
        result.append(McpUtils::brief(id));
    return result;
}

static EltID createInstrument(int indexSf2, QString name, IdList samples)
{
    // Same as the toolbar of the editor
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementInst, indexSf2);
    id.indexElt = sm->add(id);
    sm->set(id, champ_name, name);

    EltID idLink = id;
    idLink.typeElement = elementInstSmpl;
    AttributeValue val;
    bool sampleLooped = true;
    bool samePitch = false;
    QMap<unsigned char, QPair<bool, bool> > pitches;
    foreach (EltID idSrc, samples)
    {
        idLink.indexElt2 = sm->add(idLink);
        val.wValue = static_cast<quint16>(idSrc.indexElt);
        sm->set(idLink, champ_sampleID, val);

        // Pan of the stereo samples
        SFSampleLink sampleType = sm->get(idSrc, champ_sfSampleType).sfLinkValue;
        if (sampleType == rightSample || sampleType == RomRightSample)
            val.shValue = 500;
        else if (sampleType == leftSample || sampleType == RomLeftSample)
            val.shValue = -500;
        else
            val.shValue = 0;
        sm->set(idLink, champ_pan, val);

        if (sm->get(idSrc, champ_dwStartLoop).dwValue == sm->get(idSrc, champ_dwEndLoop).dwValue)
            sampleLooped = false;

        unsigned char pitch = sm->get(idSrc, champ_byOriginalPitch).bValue;
        if (!pitches.contains(pitch))
            pitches[pitch] = QPair<bool, bool>(false, false);
        if (sampleType != rightSample && sampleType != RomRightSample)
        {
            if (pitches[pitch].first)
                samePitch = true;
            pitches[pitch].first = true;
        }
        if (sampleType != leftSample && sampleType != RomLeftSample)
        {
            if (pitches[pitch].second)
                samePitch = true;
            pitches[pitch].second = true;
        }
    }

    if (!samples.empty() && sampleLooped)
    {
        val.wValue = 1;
        sm->set(id, champ_sampleModes, val);
    }

    // Distribution of the keys if each pitch is used by one sample (per side)
    if (!samePitch && pitches.count() > 1)
    {
        ToolAutoDistribution tool;
        tool.process(sm, id, nullptr);
    }
    return id;
}

static IdList addDivisions(EltID idParent, IdList elements)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool isPrst = (idParent.typeElement == elementPrst);
    EltID idDiv(isPrst ? elementPrstInst : elementInstSmpl, idParent.indexSf2, idParent.indexElt);
    IdList result;
    AttributeValue val;
    foreach (EltID idElement, elements)
    {
        if (idElement.indexSf2 != idParent.indexSf2)
            throw McpException("element " + McpUtils::idToString(idElement) + " is not in the same soundfont");
        idDiv.indexElt2 = sm->add(idDiv);
        val.wValue = static_cast<quint16>(idElement.indexElt);
        sm->set(idDiv, isPrst ? champ_instrument : champ_sampleID, val);
        result << idDiv;
    }
    return result;
}

void McpFunctions::addEditingFunctions(QList<McpFunction *> &functions)
{
    functions << new McpFunction(
        "new_soundfont", McpFunction::CATEGORY_EDITING, false,
        "Create a new empty soundfont in a new tab of Polyphone.",
        R"JSON({"properties":{
            "name":{"type":"string","description":"Name of the soundfont"}}})JSON",
        [](const McpArguments &args) -> McpResult {
            if (McpEditor::getInstance() == nullptr)
                throw McpException("the editor is not available");
            int index = McpEditor::getInstance()->openNewSoundfont();
            if (index < 0)
                throw McpException("cannot create a soundfont");
            QString name = args.getString("name", "");
            if (!name.trimmed().isEmpty())
                SoundfontManager::getInstance()->set(EltID(elementSf2, index), champ_name, name.trimmed());
            return McpUtils::brief(EltID(elementSf2, index));
        });

    functions << new McpFunction(
        "open_soundfont", McpFunction::CATEGORY_EDITING, false,
        "Open a file in a new tab of Polyphone (sf2, sf3, sfz, dls, sfArk, GrandOrgue organ). If the file is already "
        "open, its identifier is returned.",
        R"JSON({"properties":{
            "file":{"type":"string","description":"Absolute path of the file"}},
            "required":["file"]})JSON",
        [](const McpArguments &args) -> McpResult {
            QString file = QFileInfo(args.getString("file")).absoluteFilePath();
            if (!QFileInfo(file).isFile())
                throw McpException("file \"" + file + "\" doesn't exist");
            if (!InputFactory::isSuffixSupported(QFileInfo(file).suffix()))
                throw McpException("this kind of file is not supported");
            if (McpEditor::getInstance() == nullptr)
                throw McpException("the editor is not available");

            SoundfontManager * sm = SoundfontManager::getInstance();
            auto findOpenFile = [&]() -> int {
                foreach (int i, McpUtils::getSoundfontIndexes())
                    if (QFileInfo(sm->getQstr(EltID(elementSf2, i), champ_filenameInitial)) == QFileInfo(file))
                        return i;
                return -1;
            };

            int index = findOpenFile();
            if (index < 0)
            {
                McpEditor::getInstance()->openSoundfont(file);
                index = findOpenFile();
                if (index < 0)
                    throw McpException("cannot open the file (see the error displayed in Polyphone)");
            }
            else
                McpEditor::getInstance()->showElement(EltID(elementSf2, index));
            return McpUtils::brief(EltID(elementSf2, index));
        });

    functions << new McpFunction(
        "save_soundfont", McpFunction::CATEGORY_EDITING, false,
        "Save a soundfont on the disk, in its own file or in a new sf2 / sf3 file (sf3: samples compressed with "
        "Ogg Vorbis). Only save when the user asks for it. Saving cannot be undone.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "file":{"type":"string","description":"Absolute path of a .sf2 or .sf3 file. Required if the soundfont is new or was imported from another format"},
            "sf3_quality":{"type":"string","enum":["low","medium","high"],"description":"Compression quality for a sf3 file (default medium)"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            SoundfontManager * sm = SoundfontManager::getInstance();
            QString file = args.getString("file", "");
            if (file.isEmpty())
            {
                // Same file, if it is a sf2 / sf3 file
                file = sm->getQstr(idSf2, champ_filenameInitial);
                bool isSf3 = sm->get(idSf2, champ_IFIL).sfVerValue.wMajor == 3;
                if (file.isEmpty() || sm->getQstr(idSf2, champ_filenameForData) != file ||
                    !file.toLower().endsWith(isSf3 ? ".sf3" : ".sf2"))
                    throw McpException("a file path is required (\"file\")");
            }
            else
                file = QFileInfo(file).absoluteFilePath();

            QString suffix = QFileInfo(file).suffix().toLower();
            if (suffix != "sf2" && suffix != "sf3")
                throw McpException("the file must have the extension .sf2 or .sf3");
            if (!QFileInfo(QFileInfo(file).absolutePath()).isDir())
                throw McpException("the directory of \"" + file + "\" doesn't exist");
            sm->setSoundfontVersion(idSf2.indexSf2, suffix == "sf3");

            AbstractOutput * output = OutputFactory::getOutput(file);
            QString quality = args.getString("sf3_quality", "medium");
            output->setOption("quality", quality == "low" ? 0 : (quality == "high" ? 2 : 1));
            output->process(idSf2.indexSf2, false);
            bool success = output->isSuccess();
            QString error = output->getError();
            delete output;
            if (!success)
                throw McpException("the file couldn't be saved: " + error);
            ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SOUNDFONT, file);
            return QString("Saved in \"" + file + "\".");
        });

    functions << new McpFunction(
        "set_soundfont_info", McpFunction::CATEGORY_EDITING, false,
        "Change the information fields of a soundfont. Only the fields given are changed.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "name":{"type":"string"},"author":{"type":"string"},"copyright":{"type":"string"},
            "comment":{"type":"string"},"product":{"type":"string"},"date":{"type":"string"},
            "sound_engine":{"type":"string"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            QList<QPair<QString, AttributeType> > fields = {
                { "name", champ_name }, { "author", champ_IENG }, { "copyright", champ_ICOP }, { "comment", champ_ICMT },
                { "product", champ_IPRD }, { "date", champ_ICRD }, { "sound_engine", champ_ISNG } };
            SoundfontManager * sm = SoundfontManager::getInstance();
            int count = 0;
            for (int i = 0; i < fields.count(); i++)
            {
                if (args.contains(fields[i].first))
                {
                    sm->set(idSf2, fields[i].second, args.getString(fields[i].first));
                    count++;
                }
            }
            if (count == 0)
                throw McpException("no fields to change");
            return QString("Information changed.");
        });

    functions << new McpFunction(
        "import_samples", McpFunction::CATEGORY_EDITING, false,
        "Import audio files as samples (wav, flac, ogg, mp3...). A stereo file gives a linked pair of samples (left "
        "and right). The root key comes from the file metadata or its name (\"flute_C4.wav\" or \"flute_60.wav\"), "
        "check it with analyze_sample. Returns the new samples.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "files":{"type":"array","items":{"type":"string"},"description":"Absolute paths of the audio files"}},
            "required":["soundfont","files"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            QJsonArray files = args.getArray("files");
            if (files.isEmpty())
                throw McpException("no files to import");

            // Check the files first (the loader displays the errors)
            QStringList paths;
            for (const QJsonValue value : files)
            {
                QString path = QFileInfo(value.toString()).absoluteFilePath();
                Sound sound;
                if (!QFileInfo(path).isFile())
                    throw McpException("file \"" + path + "\" doesn't exist");
                if (!sound.setFileName(path, false))
                    throw McpException("cannot read \"" + path + "\": " + sound.getError());
                paths << path;
            }

            // As in the editor: processing of the configuration, recent files
            SampleLoader loader;
            loader.setProcessing(
                ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_auto_loop", false).toBool(),
                ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "wav_remove_blank", false).toBool());
            int replace = -1; // Duplicate if a name already exists, no questions
            IdList ids;
            foreach (QString path, paths)
            {
                ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, path);
                ids << loader.load(path, idSf2.indexSf2, &replace);
            }
            QJsonObject result;
            result["samples"] = idsToJson(ids);
            return result;
        });

    functions << new McpFunction(
        "create_instrument", McpFunction::CATEGORY_EDITING, false,
        "Create an instrument, possibly with one division per sample. As in Polyphone: stereo samples are panned, the "
        "loop is enabled if all samples are looped, and the key ranges are distributed according to the root keys of "
        "the samples if they are all different.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "name":{"type":"string","description":"Name (20 characters max)"},
            "samples":{"type":"array","items":{"type":"string"},"description":"Samples to use"}},
            "required":["soundfont","name"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            IdList samples;
            if (args.contains("samples") && !args.getArray("samples").isEmpty())
                samples = args.getIds("samples", { elementSmpl });
            foreach (EltID id, samples)
                if (id.indexSf2 != idSf2.indexSf2)
                    throw McpException("sample " + McpUtils::idToString(id) + " is not in this soundfont");
            EltID id = createInstrument(idSf2.indexSf2, checkName(args.getString("name")), samples);
            QJsonObject result = McpUtils::brief(id);
            result["division_count"] = samples.count();
            return result;
        });

    functions << new McpFunction(
        "create_preset", McpFunction::CATEGORY_EDITING, false,
        "Create a preset, possibly with one division per instrument. Without bank and preset numbers, the first "
        "available ones are used.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"},
            "name":{"type":"string","description":"Name (20 characters max)"},
            "instruments":{"type":"array","items":{"type":"string"},"description":"Instruments to use"},
            "bank":{"type":"integer","minimum":0,"maximum":128,"description":"Bank (128 = percussions)"},
            "preset":{"type":"integer","minimum":0,"maximum":127,"description":"Preset number"}},
            "required":["soundfont","name"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            SoundfontManager * sm = SoundfontManager::getInstance();
            int bank = args.getInt("bank", 0, 128, -1);
            int preset = args.getInt("preset", 0, 127, -1);
            if (bank >= 0 && preset >= 0)
            {
                if (!sm->isAvailable(idSf2, static_cast<quint16>(bank), static_cast<quint16>(preset)))
                    throw McpException(QString("preset %1:%2 is already used").arg(bank).arg(preset));
            }
            else
            {
                int nBank = bank, nPreset = preset;
                sm->firstAvailablePresetBank(idSf2, nBank, nPreset);
                if (nBank < 0 || nPreset < 0)
                    throw McpException("no preset numbers available");
                bank = nBank;
                preset = nPreset;
            }
            IdList instruments;
            if (args.contains("instruments") && !args.getArray("instruments").isEmpty())
                instruments = args.getIds("instruments", { elementInst });

            EltID id(elementPrst, idSf2.indexSf2);
            id.indexElt = sm->add(id);
            sm->set(id, champ_name, checkName(args.getString("name")));
            AttributeValue val;
            val.wValue = static_cast<quint16>(preset);
            sm->set(id, champ_wPreset, val);
            val.wValue = static_cast<quint16>(bank);
            sm->set(id, champ_wBank, val);
            addDivisions(id, instruments);

            QJsonObject result = McpUtils::brief(id);
            result["bank"] = bank;
            result["preset"] = preset;
            return result;
        });

    functions << new McpFunction(
        "add_divisions", McpFunction::CATEGORY_EDITING, false,
        "Add divisions to an instrument (one per sample) or to a preset (one per instrument), optionally with key / "
        "velocity ranges and parameters. Returns the new divisions.",
        R"JSON({"properties":{
            "element":{"type":"string","description":"Instrument or preset receiving the divisions"},
            "items":{"type":"array","items":{"type":"string"},"description":"Samples (for an instrument) or instruments (for a preset)"},
            "key_range":{"type":"array","description":"[min, max] keys (numbers or note names) for all new divisions"},
            "velocity_range":{"type":"array","description":"[min, max] velocities for all new divisions"},
            "parameters":{"type":"object","description":"Parameters for all new divisions, see list_parameters"}},
            "required":["element","items"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idParent = args.getId("element", { elementInst, elementPrst });
            IdList items = args.getIds("items", { idParent.typeElement == elementInst ? elementSmpl : elementInst });
            IdList divisions = addDivisions(idParent, items);
            QJsonObject parameters = args.contains("parameters") ? args.getObject("parameters") : QJsonObject();
            if (args.contains("key_range"))
                parameters["key_range"] = args.value("key_range");
            if (args.contains("velocity_range"))
                parameters["velocity_range"] = args.value("velocity_range");
            foreach (EltID idDiv, divisions)
                for (QJsonObject::const_iterator it = parameters.constBegin(); it != parameters.constEnd(); ++it)
                    McpAttributes::setValue(idDiv, it.key(), it.value());

            QJsonArray result;
            foreach (EltID idDiv, divisions)
                result.append(McpUtils::idToString(idDiv));
            QJsonObject response;
            response["divisions"] = result;
            return response;
        });

    functions << new McpFunction(
        "set_parameters", McpFunction::CATEGORY_EDITING, false,
        "Set parameters of instruments, presets (global parameters) or divisions, in real units (see "
        "list_parameters). A null value removes the parameter (the division then inherits the global value). A global "
        "value only applies to the divisions that don't define them (see get_parameter_map). Either the same "
        "parameters for several elements (\"elements\" + \"parameters\"), or different values per element "
        "(\"changes\"), all in one undoable step. Examples: {\"elements\":[\"sf0/inst1\"],\"parameters\":"
        "{\"vol_env_release\":0.8,\"filter_cutoff\":4000}}, {\"changes\":{\"sf0/inst1/div0\":{\"attenuation\":3.5},"
        "\"sf0/inst1/div1\":{\"attenuation\":2.8,\"fine_tune\":-3}}}.",
        R"JSON({"properties":{
            "elements":{"type":["string","array"],"items":{"type":"string"},"description":"Instruments, presets or divisions"},
            "parameters":{"type":"object","description":"Parameter names and values, for all elements"},
            "changes":{"type":"object","description":"Element identifiers and, for each, an object of parameter names and values"}}})JSON",
        [](const McpArguments &args) -> McpResult {
            QList<ElementType> allowed = { elementInst, elementPrst, elementInstSmpl, elementPrstInst };
            QList<QPair<EltID, QJsonObject> > changes;
            if (args.contains("elements") || args.contains("parameters"))
            {
                IdList ids = args.getIds("elements", allowed);
                QJsonObject parameters = args.getObject("parameters");
                if (parameters.isEmpty())
                    throw McpException("no parameters to change");
                foreach (EltID id, ids)
                    changes << QPair<EltID, QJsonObject>(id, parameters);
            }
            if (args.contains("changes"))
            {
                QJsonObject perElement = args.getObject("changes");
                for (QJsonObject::const_iterator it = perElement.constBegin(); it != perElement.constEnd(); ++it)
                {
                    if (!it.value().isObject() || it.value().toObject().isEmpty())
                        throw McpException("\"changes\": parameters expected for \"" + it.key() + "\"");
                    changes << QPair<EltID, QJsonObject>(McpUtils::readId(it.key(), allowed), it.value().toObject());
                }
            }
            if (changes.isEmpty())
                throw McpException("give \"elements\" and \"parameters\", or \"changes\"");

            for (int i = 0; i < changes.count(); i++)
                for (QJsonObject::const_iterator it = changes[i].second.constBegin(); it != changes[i].second.constEnd(); ++it)
                    McpAttributes::setValue(changes[i].first, it.key(), it.value());

            // New values
            QJsonObject result;
            for (int i = 0; i < changes.count(); i++)
            {
                QString key = McpUtils::idToString(changes[i].first);
                QJsonObject values = result.value(key).toObject();
                for (QJsonObject::const_iterator it = changes[i].second.constBegin(); it != changes[i].second.constEnd(); ++it)
                    values[it.key()] = McpAttributes::getValue(changes[i].first, it.key());
                result[key] = values;
            }
            return result;
        });

    functions << new McpFunction(
        "set_sample_properties", McpFunction::CATEGORY_EDITING, false,
        "Change the properties of samples: root key, pitch correction, loop points. By default the linked sample of a "
        "stereo pair receives the same root key, correction and loop points. Either one sample (\"sample\" and the "
        "properties), or several samples with their own values (\"changes\": array of objects with the same fields), "
        "all in one undoable step.",
        R"JSON({"properties":{
            "sample":{"type":"string","description":"Sample identifier"},
            "root_key":{"type":["integer","string"],"description":"Key at which the sample plays at its original pitch"},
            "pitch_correction":{"type":"integer","minimum":-99,"maximum":99,"description":"Correction in cents applied when the sample is played"},
            "loop_start":{"type":"integer","minimum":0,"description":"First frame of the loop"},
            "loop_end":{"type":"integer","minimum":0,"description":"Frame after the last frame of the loop (loop_start = loop_end = 0: no loop)"},
            "include_linked":{"type":"boolean","description":"Also change the linked sample (default true)"},
            "changes":{"type":"array","items":{"type":"object"},"description":"Several samples: [{\"sample\":\"sf0/smpl3\",\"pitch_correction\":-4}, ...]"}}})JSON",
        [](const McpArguments &args) -> McpResult {
            SoundfontManager * sm = SoundfontManager::getInstance();
            auto apply = [sm](const McpArguments &change) -> IdList {
                EltID id = change.getId("sample", { elementSmpl });
                IdList ids(id);
                SFSampleLink link = sm->get(id, champ_sfSampleType).sfLinkValue;
                if (change.getBool("include_linked", true) && link != monoSample && link != RomMonoSample)
                {
                    EltID idLinked(elementSmpl, id.indexSf2, sm->get(id, champ_wSampleLink).wValue);
                    if (sm->isValid(idLinked))
                        ids << idLinked;
                }

                AttributeValue val;
                foreach (EltID idSmpl, ids)
                {
                    if (change.contains("root_key"))
                    {
                        val.bValue = static_cast<quint8>(change.getKey("root_key"));
                        sm->set(idSmpl, champ_byOriginalPitch, val);
                    }
                    if (change.contains("pitch_correction"))
                    {
                        val.cValue = static_cast<qint8>(change.getInt("pitch_correction", -99, 99));
                        sm->set(idSmpl, champ_chPitchCorrection, val);
                    }
                    if (change.contains("loop_start") || change.contains("loop_end"))
                    {
                        quint32 length = sm->get(idSmpl, champ_dwLength).dwValue;
                        int start = change.getInt("loop_start", 0, static_cast<int>(length),
                                                  static_cast<int>(sm->get(idSmpl, champ_dwStartLoop).dwValue));
                        int end = change.getInt("loop_end", 0, static_cast<int>(length),
                                                static_cast<int>(sm->get(idSmpl, champ_dwEndLoop).dwValue));
                        if (start > end)
                            throw McpException("loop_start must be lower than loop_end");
                        val.dwValue = static_cast<quint32>(start);
                        sm->set(idSmpl, champ_dwStartLoop, val);
                        val.dwValue = static_cast<quint32>(end);
                        sm->set(idSmpl, champ_dwEndLoop, val);
                    }
                }
                return ids;
            };

            IdList changed;
            if (args.contains("sample"))
                changed << apply(args);
            if (args.contains("changes"))
            {
                for (const QJsonValue value : args.getArray("changes"))
                {
                    if (!value.isObject())
                        throw McpException("\"changes\" must contain objects");
                    changed << apply(McpArguments(value.toObject()));
                }
            }
            if (changed.isEmpty())
                throw McpException("give \"sample\" or \"changes\"");

            QJsonObject result;
            result["changed"] = idsToJson(changed);
            return result;
        });

    functions << new McpFunction(
        "rename_element", McpFunction::CATEGORY_EDITING, false,
        "Rename a sample, an instrument or a preset (20 characters max). Use set_soundfont_info for a soundfont.",
        R"JSON({"properties":{
            "id":{"type":"string","description":"Element identifier"},
            "name":{"type":"string","description":"New name"}},
            "required":["id","name"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("id", { elementSmpl, elementInst, elementPrst });
            SoundfontManager::getInstance()->set(id, champ_name, checkName(args.getString("name")));
            return McpUtils::brief(id);
        });

    functions << new McpFunction(
        "set_preset_number", McpFunction::CATEGORY_EDITING, false,
        "Change the bank and the number of a preset (MIDI program).",
        R"JSON({"properties":{
            "preset":{"type":"string","description":"Preset identifier"},
            "bank":{"type":"integer","minimum":0,"maximum":128},
            "number":{"type":"integer","minimum":0,"maximum":127}},
            "required":["preset","bank","number"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("preset", { elementPrst });
            int bank = args.getInt("bank", 0, 128);
            int number = args.getInt("number", 0, 127);
            SoundfontManager * sm = SoundfontManager::getInstance();
            if ((sm->get(id, champ_wBank).wValue != bank || sm->get(id, champ_wPreset).wValue != number) &&
                !sm->isAvailable(id, static_cast<quint16>(bank), static_cast<quint16>(number)))
                throw McpException(QString("preset %1:%2 is already used").arg(bank).arg(number));
            AttributeValue val;
            val.wValue = static_cast<quint16>(bank);
            sm->set(id, champ_wBank, val);
            val.wValue = static_cast<quint16>(number);
            sm->set(id, champ_wPreset, val);
            return QString("Preset number changed.");
        });

    functions << new McpFunction(
        "remove_elements", McpFunction::CATEGORY_EDITING, false,
        "Remove samples, instruments, presets, divisions or modulators. A sample used by an instrument or an "
        "instrument used by a preset cannot be removed: remove the divisions using it first.",
        R"JSON({"properties":{
            "elements":{"type":["string","array"],"items":{"type":"string"},"description":"Elements to remove"}},
            "required":["elements"]})JSON",
        [](const McpArguments &args) -> McpResult {
            IdList ids = args.getIds("elements", { elementSmpl, elementInst, elementPrst, elementInstSmpl, elementPrstInst,
                                                  elementInstMod, elementPrstMod, elementInstSmplMod, elementPrstInstMod });
            SoundfontManager * sm = SoundfontManager::getInstance();
            foreach (EltID id, ids)
            {
                if (!sm->isValid(id))
                    continue; // Already removed with its parent
                int message = 1;
                sm->remove(id, &message);
                if (sm->isValid(id))
                {
                    QString reason = (message % 2 == 0) ? "the sample is used by an instrument" :
                                         ((message % 3 == 0) ? "the instrument is used by a preset" : "unknown reason");
                    throw McpException("cannot remove " + McpUtils::idToString(id) + ": " + reason + " (nothing removed)");
                }
            }
            return QString("%1 element(s) removed.").arg(ids.count());
        });

    functions << new McpFunction(
        "duplicate_element", McpFunction::CATEGORY_EDITING, false,
        "Duplicate a sample, an instrument, a preset or a division in the same soundfont. Returns the copy.",
        R"JSON({"properties":{
            "id":{"type":"string","description":"Element to duplicate"}},
            "required":["id"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID id = args.getId("id", { elementSmpl, elementInst, elementPrst, elementInstSmpl, elementPrstInst });
            Duplicator duplicator;
            EltID copy = duplicator.duplicate(id);
            if (!SoundfontManager::getInstance()->isValid(copy))
                throw McpException("the element couldn't be duplicated");
            return McpUtils::brief(copy);
        });

    functions << new McpFunction(
        "copy_elements", McpFunction::CATEGORY_EDITING, false,
        "Copy samples, instruments or presets into another soundfont (with the elements they use), or link elements "
        "of the same soundfont (a sample into an instrument, an instrument into a preset). Elements having the same "
        "name as existing ones are duplicated.",
        R"JSON({"properties":{
            "elements":{"type":["string","array"],"items":{"type":"string"},"description":"Elements to copy"},
            "destination":{"type":"string","description":"Destination soundfont, instrument or preset"}},
            "required":["elements","destination"]})JSON",
        [](const McpArguments &args) -> McpResult {
            IdList ids = args.getIds("elements", { elementSmpl, elementInst, elementPrst, elementInstSmpl, elementPrstInst });
            EltID idDest = args.getId("destination", { elementSf2, elementInst, elementPrst });
            Duplicator duplicator;
            duplicator.setNonInteractive();
            SoundfontManager * sm = SoundfontManager::getInstance();
            QJsonArray copies;
            foreach (EltID id, ids)
            {
                EltID copy = duplicator.copy(id, idDest);
                if (!sm->isValid(copy))
                    throw McpException("cannot copy " + McpUtils::idToString(id) + " to " + McpUtils::idToString(idDest));
                copies.append(McpUtils::brief(copy));
            }
            QJsonObject result;
            result["copies"] = copies;
            return result;
        });

    functions << new McpFunction(
        "add_modulator", McpFunction::CATEGORY_EDITING, false,
        QString("Add a modulator to an instrument, a preset (global) or a division. ") + McpModulators::FORMAT,
        R"JSON({"properties":{
            "element":{"type":"string","description":"Instrument, preset or division"},
            "modulator":{"type":"object","description":"Fields source, destination, amount and optionally amount_source, transform"}},
            "required":["element","modulator"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idParent = args.getId("element", { elementInst, elementPrst, elementInstSmpl, elementPrstInst });
            return McpModulators::toJson(McpModulators::add(idParent, args.getObject("modulator")));
        });

    functions << new McpFunction(
        "update_modulator", McpFunction::CATEGORY_EDITING, false,
        "Change fields of an existing modulator (same format as add_modulator). Remove it with remove_elements.",
        R"JSON({"properties":{
            "id":{"type":"string","description":"Modulator identifier, for instance \"sf0/inst2/mod0\""},
            "changes":{"type":"object","description":"Fields to change"}},
            "required":["id","changes"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idMod = args.getId("id", { elementInstMod, elementPrstMod, elementInstSmplMod, elementPrstInstMod });
            McpModulators::update(idMod, args.getObject("changes"));
            return McpModulators::toJson(idMod);
        });

    functions << new McpFunction(
        "undo", McpFunction::CATEGORY_EDITING, false,
        "Undo the last change of a soundfont (made by the AI or by the user).",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            SoundfontManager * sm = SoundfontManager::getInstance();
            if (!sm->isUndoable(idSf2.indexSf2))
                throw McpException("nothing to undo");
            sm->undo(idSf2.indexSf2);
            return QString("Undone.");
        });

    functions << new McpFunction(
        "redo", McpFunction::CATEGORY_EDITING, false,
        "Redo the last change undone in a soundfont.",
        R"JSON({"properties":{
            "soundfont":{"type":"string","description":"Soundfont identifier"}},
            "required":["soundfont"]})JSON",
        [](const McpArguments &args) -> McpResult {
            EltID idSf2 = args.getId("soundfont", { elementSf2 });
            SoundfontManager * sm = SoundfontManager::getInstance();
            if (!sm->isRedoable(idSf2.indexSf2))
                throw McpException("nothing to redo");
            sm->redo(idSf2.indexSf2);
            return QString("Redone.");
        });
}
