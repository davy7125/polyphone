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


#include "sfzreader.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "textconversions.h"
using namespace TextConversions;

typedef ISoundfontBuilder::Id Id;

// Directory of a file (QFileInfo::path), '/' being the separator
static std::string getDirectory(const std::string &filePath)
{
    size_t slash = filePath.find_last_of('/');
    if (slash == std::string::npos)
        return ".";
    return slash == 0 ? "/" : filePath.substr(0, slash);
}

// Name of a file without its directory and its last extension (QFileInfo::completeBaseName)
static std::string getCompleteBaseName(const std::string &filePath)
{
    size_t slash = filePath.find_last_of('/');
    std::string name = (slash == std::string::npos) ? filePath : filePath.substr(slash + 1);
    size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

// Name of the directory of a file (QDir::dirName)
static std::string getDirectoryName(const std::string &filePath)
{
    std::string directory = getDirectory(filePath);
    if (directory == "/")
        return "";
    size_t slash = directory.find_last_of('/');
    return slash == std::string::npos ? directory : directory.substr(slash + 1);
}

static bool startsWithThreeDigits(const std::string &text)
{
    return text.size() >= 3 && isdigit(static_cast<unsigned char>(text[0])) &&
           isdigit(static_cast<unsigned char>(text[1])) && isdigit(static_cast<unsigned char>(text[2]));
}

bool SfzReader::read(const std::string &fileName, ISoundfontBuilder &builder, bool isNewSoundfont, const Options &options)
{
    _options = options;
    _context = SfzContext();
    _context.getKeyNumber = options.getKeyNumber;
    _context.onUnsupportedOpcode = options.onUnsupportedOpcode;
    _presetList.clear();
    _globalZone = SfzParameterRegion();
    _openFilePaths.clear();
    _replacements.clear();
    _error = ERROR_NONE;
    _errorFile.clear();
    _currentBloc = BLOC_UNKNOWN;

    // Parse the file
    _rootDir = getDirectory(fileName);
    if (!_rootDir.empty() && _rootDir.back() == '/')
        _rootDir = _rootDir.substr(0, _rootDir.size() - 1);
    if (!parseFile(fileName))
        return false;

    std::string path = getDirectory(fileName);
    bool isChannel10 = true;
    double ampliMax = 0;
    for (size_t i = 0; i < _presetList.size(); i++)
    {
        // All samples must be valid
        _presetList[i].moveOpcodesInGlobal(_globalZone);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_sample, SfzParameterGroup::VALUE_STRING);
        _presetList[i].checkSampleValid(path);

        // Offsets must be defined for samples, not in global division
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_offset, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_end, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_start, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_end, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_mode, SfzParameterGroup::VALUE_STRING);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_trigger, SfzParameterGroup::VALUE_STRING);

        // Same for pan, samples, off_by et group
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_pan, SfzParameterGroup::VALUE_DOUBLE);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_width, SfzParameterGroup::VALUE_DOUBLE);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_position, SfzParameterGroup::VALUE_DOUBLE);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_off_by, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_exclusiveClass, SfzParameterGroup::VALUE_INT);
        _presetList[i].moveModInSamples();
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvDecay, SfzParameter::op_ampeg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvHold, SfzParameter::op_ampeg_hold);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvDecay, SfzParameter::op_pitcheg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvHold, SfzParameter::op_pitcheg_hold);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_fileg_decaycc133, SfzParameter::op_fileg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_fileg_holdcc133, SfzParameter::op_fileg_hold);
        _presetList[i].adjustCorrection(path);

        // Adapt the volume if a volume modulation applies
        _presetList[i].adjustModulationVolume();

        // Process / reject filters
        _presetList[i].checkFilter();
        _presetList[i].checkKeyTrackedFilter();

        // Maximum amplification
        ampliMax = std::max(ampliMax, _presetList[i].getAmpliMax());

        // Search canal 10
        isChannel10 &= _presetList[i].isChannel10();
    }

    // Adapt the volume if the max amplification is > 0
    if (ampliMax > 0)
        for (size_t i = 0; i < _presetList.size(); i++)
            _presetList[i].adjustVolume(-ampliMax);

    // Create a soundfont
    createSf2(builder, fileName, isNewSoundfont, isChannel10);
    return true;
}

bool SfzReader::parseFile(const std::string &filename)
{
    if (std::find(_openFilePaths.begin(), _openFilePaths.end(), filename) != _openFilePaths.end())
    {
        // Recursion!
        _error = ERROR_RECURSION;
        return false;
    }

    std::error_code error;
    std::filesystem::path filePath = std::filesystem::u8path(filename);
    std::ifstream inputFile;
    if (!filename.empty() && !std::filesystem::is_directory(filePath, error))
        inputFile.open(filePath, std::ios::binary);
    if (!inputFile.is_open())
    {
        _error = ERROR_CANNOT_OPEN;
        _errorFile = filename;
        return false;
    }

    _openFilePaths.push_back(filename);
    std::string line;
    bool isFirstLine = true;
    while (std::getline(inputFile, line))
    {
        // Line without its end ("\n" or "\r\n") and without the byte order mark of UTF-8
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (isFirstLine && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line = line.substr(3);
        isFirstLine = false;
        if (line.empty())
            continue;

        // Remove comment
        size_t comment = line.find("//");
        if (comment != std::string::npos)
            line = line.substr(0, comment);

        // Cut the line
        std::vector<std::string> list = split(line, " ", true);
        int length = static_cast<int>(list.size());
        for (int i = length - 1; i >= 1; i--)
        {
            if (list.at(static_cast<size_t>(i)).find_first_of("<>=") == std::string::npos)
            {
                list[static_cast<size_t>(i - 1)] += " " + list[static_cast<size_t>(i)];
                list.erase(list.begin() + i);
            }
        }

        // Trim spaces and remove empty fields
        length = static_cast<int>(list.size());
        for (int i = length - 1; i >= 0; i--)
        {
            std::string strTmp = trimmed(list.at(static_cast<size_t>(i)));
            if (strTmp.empty())
                list.erase(list.begin() + i);
            else
                list[static_cast<size_t>(i)] = strTmp;
        }

        // Store elements
        for (const std::string &str : list)
        {
            // Valid?
            if (utf16Length(str) <= 2)
                continue;

            if (str.rfind("#include", 0) == 0)
            {
                // Read another file, returning immediately in case of error
                std::string otherFile = getFilePathFromInclude(str);
                if (!this->parseFile(otherFile))
                    return false;
            }
            else if (str.rfind("#define", 0) == 0)
            {
                std::vector<std::string> splitTmp = split(str, " ", false);
                if (splitTmp.size() == 3)
                    _replacements[splitTmp[1]] = splitTmp[2];
            }
            else if (str.front() == '<' && str.back() == '>')
                changeBloc(str.substr(1, str.size() - 2));
            else if (str.find('=') != std::string::npos)
            {
                size_t index = str.find('=');
                std::string opcode = toLower(str.substr(0, index));
                std::string value = this->applyReplacements(str.substr(index + 1));
                if (!opcode.empty() && !value.empty())
                    addOpcode(opcode, value);
            }
        }
    }
    _openFilePaths.erase(std::remove(_openFilePaths.begin(), _openFilePaths.end(), filename), _openFilePaths.end());

    return true;
}

std::string SfzReader::getFilePathFromInclude(std::string str)
{
    // Skip the "#include", remove quotes and extra spaces
    str = trimmed(replaceAll(replaceAll(str.substr(8), "\"", ""), "'", ""));
    if (str.empty())
        return "";

    // Make an absolute path
    str = replaceAll(str, "\\", "/");
    if (str[0] == '/')
        str = _rootDir + str;
    else
        str = _rootDir + "/" + str;

    return str;
}

std::string SfzReader::applyReplacements(std::string opcodeValue)
{
    for (const auto &replacement : _replacements)
        opcodeValue = replaceAll(opcodeValue, replacement.first, replacement.second);
    return opcodeValue;
}

void SfzReader::changeBloc(const std::string &bloc)
{
    if (bloc == "control")
        _currentBloc = BLOC_CONTROL;
    else if (bloc == "global")
        _currentBloc = BLOC_GLOBAL;
    else if (bloc == "group")
    {
        _currentBloc = BLOC_GROUP;
        _presetList.push_back(SfzParameterGroup());
    }
    else if (bloc == "region")
    {
        _currentBloc = BLOC_REGION;
        if (_presetList.empty())
            _presetList.push_back(SfzParameterGroup());
        _presetList.back().newGroup();
    }
    else
    {
        if (_options.onUnknownBlock)
            _options.onUnknownBlock(bloc);
        _currentBloc = BLOC_UNKNOWN;
    }
}

void SfzReader::addOpcode(const std::string &opcode, std::string value)
{
    switch (_currentBloc)
    {
    case BLOC_GROUP: case BLOC_REGION:
        _presetList.back().addParam(opcode, value, _context);
        break;
    case BLOC_CONTROL:
        if (opcode == "default_path")
        {
            std::string defaultPath = replaceAll(value, "\\", "/");
            if (!defaultPath.empty() && defaultPath[0] == '/')
                defaultPath = defaultPath.substr(1);
            if (!defaultPath.empty() && defaultPath.back() == '/')
                defaultPath = defaultPath.substr(0, defaultPath.size() - 1);
            _context.defaultPath = defaultPath;
        }
        break;
    case BLOC_GLOBAL:
        _globalZone << SfzParameter(opcode, value, &_context);
        break;
    case BLOC_UNKNOWN:
        break;
    }
}

void SfzReader::createSf2(ISoundfontBuilder &sm, const std::string &filename, bool isNewSoundfont, bool isChannel10)
{
    int numBank = 0;
    int numPreset = 0;
    std::string nom = getInstrumentName(filename, numBank, numPreset);
    if (isChannel10)
        numBank = 128;
    Id idSf2(ISoundfontBuilder::TYPE_SF2);
    if (isNewSoundfont)
    {
        sm.add(idSf2);
        sm.setText(idSf2, champ_name, nom);
    }
    else
        sm.setText(idSf2, champ_name, _options.importName);
    sm.setText(idSf2, champ_ICMT, "Sf2 imported from sfz by Polyphone");
    sm.closestAvailablePreset(numBank, numPreset);

    // Create a preset
    Id idPrst(ISoundfontBuilder::TYPE_PRESET);
    idPrst.element = sm.add(idPrst);
    sm.setText(idPrst, champ_name, nom);
    AttributeValue val;
    val.wValue = static_cast<uint16_t>(numBank);
    sm.set(idPrst, champ_wBank, val);
    val.wValue = static_cast<uint16_t>(numPreset);
    sm.set(idPrst, champ_wPreset, val);

    // Create instruments
    Id idInst(ISoundfontBuilder::TYPE_INSTRUMENT);
    std::string path = getDirectory(filename);
    for (size_t i = 0; i < _presetList.size(); i++)
    {
        idInst.element = sm.add(idInst);
        std::string nomInst = _presetList[i].getLabel();
        if (nomInst == "")
        {
            if (_presetList.size() > 9)
                nomInst = utf16Left(nom, 17) + "-" + std::to_string(i + 1);
            else if (_presetList.size() > 1)
                nomInst = utf16Left(nom, 18) + "-" + std::to_string(i + 1);
            else
                nomInst = nom;
        }
        sm.setText(idInst, champ_name, nomInst);

        Id idInstSmpl = idInst;
        idInstSmpl.type = ISoundfontBuilder::TYPE_INSTRUMENT_ZONE;

        // Link to the preset
        Id idPrstInst = idPrst;
        idPrstInst.type = ISoundfontBuilder::TYPE_PRESET_ZONE;
        idPrstInst.zone = sm.add(idPrstInst);
        val.dwValue = static_cast<uint32_t>(idInst.element);
        sm.set(idPrstInst, champ_instrument, val);

        // Fill the instrument and create samples
        _presetList[i].decode(&sm, idInst, path, _options.velToFilterType);

        // Find the preset keyRange and velRange
        int keyMin = 127;
        int keyMax = 0;
        int velMin = 127;
        int velMax = 0;
        for (int j : sm.getSiblings(idInstSmpl))
        {
            idInstSmpl.zone = j;
            if (sm.isSet(idInstSmpl, champ_keyRange))
            {
                RangesType range = sm.get(idInstSmpl, champ_keyRange).rValue;
                keyMin = std::min(keyMin, static_cast<int>(range.byLo));
                keyMax = std::max(keyMax, static_cast<int>(range.byHi));
            }
            if (sm.isSet(idInstSmpl, champ_velRange))
            {
                RangesType range = sm.get(idInstSmpl, champ_velRange).rValue;
                velMin = std::min(velMin, static_cast<int>(range.byLo));
                velMax = std::max(velMax, static_cast<int>(range.byHi));
            }
        }
        if (sm.isSet(idInst, champ_keyRange))
        {
            RangesType range = sm.get(idInst, champ_keyRange).rValue;
            keyMin = std::min(keyMin, static_cast<int>(range.byLo));
            keyMax = std::max(keyMax, static_cast<int>(range.byHi));
        }
        if (sm.isSet(idInst, champ_velRange))
        {
            RangesType range = sm.get(idInst, champ_velRange).rValue;
            velMin = std::min(velMin, static_cast<int>(range.byLo));
            velMax = std::max(velMax, static_cast<int>(range.byHi));
        }
        if (keyMin > keyMax)
        {
            keyMin = 0;
            keyMax = 127;
        }
        val.rValue.byLo = static_cast<uint8_t>(keyMin);
        val.rValue.byHi = static_cast<uint8_t>(keyMax);
        sm.set(idPrstInst, champ_keyRange, val);
        if (velMin <= velMax)
        {
            val.rValue.byLo = static_cast<uint8_t>(velMin);
            val.rValue.byHi = static_cast<uint8_t>(velMax);
            sm.set(idPrstInst, champ_velRange, val);
        }

        // Simplify parameters in instruments
        sm.simplify(idInst, champ_initialAttenuation);
        sm.simplify(idInst, champ_initialFilterFc);
        sm.simplify(idInst, champ_fineTune);
        sm.simplify(idInst, champ_sampleModes);
        sm.simplify(idInst, champ_exclusiveClass);
        sm.simplify(idInst, champ_delayVolEnv);
        sm.simplify(idInst, champ_attackVolEnv);
        sm.simplify(idInst, champ_decayVolEnv);
        sm.simplify(idInst, champ_holdVolEnv);
        sm.simplify(idInst, champ_sustainVolEnv);
        sm.simplify(idInst, champ_releaseVolEnv);

        // Delete keyrange and velocity range of the global division in the instrument
        sm.reset(idInst, champ_velRange);
        sm.reset(idInst, champ_keyRange);
    }

    // Possible 24-bit mode
    Id idSmpl(ISoundfontBuilder::TYPE_SAMPLE);
    for (int i : sm.getSiblings(idSmpl))
    {
        idSmpl.element = i;
        if (sm.get(idSmpl, champ_bpsFile).wValue >= 24)
        {
            AttributeValue value;
            value.wValue = 24;
            sm.set(idSf2, champ_wBpsSave, value);
            break;
        }
    }
}

std::string SfzReader::getInstrumentName(const std::string &filePath, int &numBank, int &numPreset)
{
    std::string nomFichier = getCompleteBaseName(filePath);
    std::string nomDir = getDirectoryName(filePath);

    // Preset number
    if (startsWithThreeDigits(nomFichier))
    {
        numPreset = toInt(nomFichier.substr(0, 3));
        if (numPreset < 0 || numPreset > 127)
            numPreset = 0;
        nomFichier = nomFichier.substr(3);
        if (!nomFichier.empty())
        {
            char ch = nomFichier[0];
            if (ch == '-' || ch == '_' || ch == '.' || ch == ' ')
                nomFichier = nomFichier.substr(1);
        }
    }

    // Bank number
    if (startsWithThreeDigits(nomDir))
    {
        numBank = toInt(nomDir.substr(0, 3));
        if (numBank < 0 || numBank > 127)
            numBank = 0;
    }

    // Instrument name
    if (nomFichier.empty())
        nomFichier = _options.untitledName;
    return utf16Left(nomFichier, 20);
}
