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


#include "sfzparameterregion.h"
#include <algorithm>
#include <filesystem>
#include "samplefilereader.h"
#include "synthconstants.h"
#include "textconversions.h"
using namespace TextConversions;

typedef ISoundfontBuilder::Id Id;

// Path of a file from a UTF-8 text
static std::filesystem::path toPath(const std::string &text)
{
#if defined(__cpp_char8_t)
    return std::filesystem::path(std::u8string(text.begin(), text.end()));
#else
    return std::filesystem::u8path(text);
#endif
}

static std::string toText(const std::filesystem::path &path)
{
#if defined(__cpp_char8_t)
    std::u8string text = path.u8string();
    return std::string(text.begin(), text.end());
#else
    return path.u8string();
#endif
}

// Name of a file without its directory and its last extension (QFileInfo::completeBaseName)
static std::string getCompleteBaseName(const std::string &filePath)
{
    size_t slash = filePath.find_last_of('/');
    std::string name = (slash == std::string::npos) ? filePath : filePath.substr(slash + 1);
    size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

// Information of a sample file (Sound::setFileName of the editor), false if it cannot be read
static bool getSampleInfo(const std::string &filePath, SampleFileReader::Info &info)
{
    std::unique_ptr<SampleFileReader> reader = SampleFileReader::open(toPath(filePath));
    if (reader == nullptr)
        return false;
    reader->getInfo(info);
    return true;
}

// Text in a list, ignoring the case of the ASCII letters
static bool containsIgnoreCase(const std::vector<std::string> &list, const std::string &text)
{
    std::string lowerText = toLower(text);
    for (const std::string &element : list)
        if (toLower(element) == lowerText)
            return true;
    return false;
}

std::vector<int> SfzParameterRegion::getSampleIndex(ISoundfontBuilder * sf2, Id idElt, const std::string &pathSfz) const
{
    std::vector<int> sampleIndex;

    int indexOpSample = -1;
    for (size_t i = 0; i < _listeParam.size(); i++)
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_sample)
            indexOpSample = static_cast<int>(i);

    if (indexOpSample == -1)
        return sampleIndex;

    // Build the file path
    std::string filePath = _listeParam.at(static_cast<size_t>(indexOpSample)).getStringValue();
    std::string fileName = pathSfz + "/" + filePath;
    std::error_code error;
    if (!std::filesystem::exists(toPath(fileName), error))
    {
        std::vector<std::string> list = getFullPath(pathSfz, split(filePath, "/", false));
        if (!list.empty())
            fileName = list.front();
        else
            return sampleIndex;
    }

    // Sample already loaded?
    idElt.type = ISoundfontBuilder::TYPE_SAMPLE;
    std::vector<std::string> names;
    for (int i : sf2->getSiblings(idElt))
    {
        idElt.element = i;
        if (sf2->getText(idElt, champ_filenameForData) == fileName)
            sampleIndex.push_back(i);
        names.push_back(sf2->getText(idElt, champ_name));
    }
    if (!sampleIndex.empty())
        return sampleIndex;

    // Gather sample information
    SampleFileReader::Info info;
    if (!getSampleInfo(fileName, info))
        return sampleIndex;
    int nChannels = info.channels;
    std::string nom = getCompleteBaseName(fileName);
    std::string nom2 = nom;

    // Possibly adapt the name
    int suffixNumber = 0;
    if (nChannels == 2)
    {
        while ((containsIgnoreCase(names, getName(nom, 20, suffixNumber, "L")) ||
                containsIgnoreCase(names, getName(nom, 20, suffixNumber, "R"))) &&
               suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom2 = getName(nom, 20, suffixNumber, "L");
        nom = getName(nom, 20, suffixNumber, "R");
    }
    else
    {
        while (containsIgnoreCase(names, getName(nom, 20, suffixNumber)) && suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom = getName(nom, 20, suffixNumber);
    }

    // Create a new sample
    AttributeValue val;
    for (int numChannel = 0; numChannel < nChannels; numChannel++)
    {
        idElt.element = sf2->add(idElt);
        sampleIndex.push_back(idElt.element);
        if (nChannels == 2)
        {
            if (numChannel == 0)
            {
                // Left
                sf2->setText(idElt, champ_name, nom2);
                val.wValue = static_cast<uint16_t>(idElt.element + 1);
                sf2->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = leftSample;
                sf2->set(idElt, champ_sfSampleType, val);
            }
            else
            {
                // Right
                sf2->setText(idElt, champ_name, nom);
                val.wValue = static_cast<uint16_t>(idElt.element - 1);
                sf2->set(idElt, champ_wSampleLink, val);
                val.sfLinkValue = rightSample;
                sf2->set(idElt, champ_sfSampleType, val);
            }
        }
        else
        {
            sf2->setText(idElt, champ_name, nom);
            val.wValue = 0;
            sf2->set(idElt, champ_wSampleLink, val);
            val.sfLinkValue = monoSample;
            sf2->set(idElt, champ_sfSampleType, val);
        }
        sf2->setText(idElt, champ_filenameForData, fileName);
        val.dwValue = 0;
        sf2->set(idElt, champ_dwStart16, val);
        val.dwValue = 0;
        sf2->set(idElt, champ_dwStart24, val);
        val.wValue = static_cast<uint16_t>(numChannel);
        sf2->set(idElt, champ_wChannel, val);
        val.dwValue = info.length;
        sf2->set(idElt, champ_dwLength, val);
        val.dwValue = info.sampleRate;
        sf2->set(idElt, champ_dwSampleRate, val);
        val.dwValue = info.loops.empty() ? 0 : info.loops[0].first;
        sf2->set(idElt, champ_dwStartLoop, val);
        val.dwValue = info.loops.empty() ? 0 : info.loops[0].second;
        sf2->set(idElt, champ_dwEndLoop, val);
        val.bValue = static_cast<uint8_t>(info.rootKey);
        sf2->set(idElt, champ_byOriginalPitch, val);
        val.cValue = static_cast<char>(-info.fineTune); // The correction is the opposite value
        sf2->set(idElt, champ_chPitchCorrection, val);
        val.wValue = 0;
        sf2->set(idElt, champ_bpsFile, val);
    }

    return sampleIndex;
}

void SfzParameterRegion::adaptLoop(int startLoop, int endLoop, int length)
{
    for (size_t i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_loop_start)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - startLoop);
        else if (_listeParam.at(i).getOpCode() == SfzParameter::op_loop_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - endLoop);
        else if (_listeParam.at(i).getOpCode() == SfzParameter::op_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - length);
    }

    if (startLoop != endLoop && !isDefined(SfzParameter::op_loop_mode) && (startLoop != 0 || endLoop != 1))
        _listeParam.push_back(SfzParameter("loop_mode", "loop_continuous"));

    if (isDefined(SfzParameter::op_trigger) && getStrValue(SfzParameter::op_trigger).rfind("release", 0) == 0)
    {
        removeOpCode(SfzParameter::op_loop_mode);
        _listeParam.push_back(SfzParameter("loop_mode", "one_shot"));
    }
}

void SfzParameterRegion::adjustCorrection(const std::string &path, int defaultCorrection)
{
    std::string sample = getStrValue(SfzParameter::op_sample);
    if (!sample.empty())
    {
        SampleFileReader::Info info;
        getSampleInfo(path + "/" + sample, info);
        int correctionSample = -info.fineTune; // The correction is the opposite value
        if (correctionSample != 0)
            adjustCorrection(correctionSample, defaultCorrection);
    }
}

bool SfzParameterRegion::sampleValid(const std::string &path)
{
    bool bRet = false;
    if (this->isDefined(SfzParameter::op_sample))
    {
        std::string sample = getStrValue(SfzParameter::op_sample);
        std::error_code error;
        bRet = std::filesystem::exists(toPath(path + "/" + sample), error);
    }
    return bRet;
}

void SfzParameterRegion::checkFilter()
{
    bool removeFilter = false;
    if (isDefined(SfzParameter::op_filterType))
    {
        std::string type = getStrValue(SfzParameter::op_filterType);
        if (type != "lpf_2p" && type != "lpf_1p") // 1 pôle : acceptable
            removeFilter = true;
    }

    if (removeFilter)
    {
        for (int i = static_cast<int>(_listeParam.size()) - 1; i >= 0; i--)
        {
            SfzParameter::OpCode opCode = _listeParam.at(static_cast<size_t>(i)).getOpCode();
            if (opCode == SfzParameter::op_filterFreq ||
                opCode == SfzParameter::op_filterType ||
                opCode == SfzParameter::op_filterQ ||
                opCode == SfzParameter::op_fil_veltrack)
                _listeParam.erase(_listeParam.begin() + i);
        }
    }
}

void SfzParameterRegion::checkKeyTrackedFilter(bool remove)
{
    if (remove)
    {
        // Not the global division: remove op_fil_keytrack and op_fil_keycenter
        removeOpCode(SfzParameter::op_fil_keytrack);
        removeOpCode(SfzParameter::op_fil_keycenter);
        return;
    }

    if (isDefined(SfzParameter::op_fil_keytrack) && isDefined(SfzParameter::op_filterFreq))
    {
        // Key center specified?
        int keyCenter = 60; // Default value
        if (isDefined(SfzParameter::op_fil_keycenter))
            keyCenter = getIntValue(SfzParameter::op_fil_keycenter);

        // The value of the filter is set for key "keyCenter" but we want the value for key 64
        int diff = 64 - keyCenter;
        int cents = getIntValue(SfzParameter::op_fil_keytrack);

        for (size_t i = 0; i < _listeParam.size(); i++)
        {
            if (_listeParam.at(i).getOpCode() == SfzParameter::op_filterFreq)
            {
                int currentFreq = _listeParam.at(i).getDoubleValue();
                _listeParam[i].setDoubleValue(d1200e2(diff * cents) * currentFreq);
                break;
            }
        }
    }
}

void SfzParameterRegion::adjustVolume(double offset)
{
    bool isDefined = false;
    for (size_t i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_volume)
        {
            _listeParam[i].setDoubleValue(_listeParam.at(i).getDoubleValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam.push_back(SfzParameter(SfzParameter::op_volume, -offset));
}

void SfzParameterRegion::adjustCorrection(int offset, int defaultCorrection)
{
    bool isDefined = false;
    for (size_t i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == SfzParameter::op_tuningFine)
        {
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam.push_back(SfzParameter(SfzParameter::op_tuningFine, defaultCorrection - offset));
}

std::vector<std::string> SfzParameterRegion::getFullPath(const std::string &base, std::vector<std::string> directories)
{
    std::vector<std::string> listRet;
    if (directories.empty())
        listRet.push_back(base);
    else
    {
        // Files and directories, not the symbolic links, sorted by name ignoring the case
        std::vector<std::string> childs;
        std::error_code error;
        for (std::filesystem::directory_iterator it(toPath(base), error), end; !error && it != end; it.increment(error))
            if (!it->is_symlink(error))
                childs.push_back(toText(it->path().filename()));
        std::sort(childs.begin(), childs.end(), [](const std::string &a, const std::string &b) {
            std::string lowerA = toLower(a), lowerB = toLower(b);
            return lowerA != lowerB ? lowerA < lowerB : a < b;
        });

        std::string nextDirectory = toLower(directories.front());
        directories.erase(directories.begin());
        for (size_t i = 0; i < childs.size(); i++)
        {
            if (toLower(childs.at(i)) == nextDirectory)
            {
                std::vector<std::string> paths = getFullPath(base + "/" + childs.at(i), directories);
                listRet.insert(listRet.end(), paths.begin(), paths.end());
            }
        }
    }
    return listRet;
}

void SfzParameterRegion::decode(ISoundfontBuilder * sf2, Id idElt, int velToFilterType)
{
    AttributeValue val;
    double dTmp;
    for (size_t i = 0; i < _listeParam.size(); i++)
    {
        switch (_listeParam.at(i).getOpCode())
        {
        case SfzParameter::op_key:
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            val.rValue.byHi = val.rValue.byLo;
            sf2->set(idElt, champ_keyRange, val);
            val.wValue = _listeParam.at(i).getIntValue();
            if (!isDefined(SfzParameter::op_rootKey))
                sf2->set(idElt, champ_overridingRootKey, val);
            break;
        case SfzParameter::op_keyMin:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val);
            break;
        case SfzParameter::op_keyMax:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val);
            break;
        case SfzParameter::op_velMin:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val);
            break;
        case SfzParameter::op_velMax:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val);
            break;
        case SfzParameter::op_rootKey:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_overridingRootKey, val);
            break;
        case SfzParameter::op_exclusiveClass:
            val.wValue = _listeParam.at(i).getIntValue();
            if (isDefined(SfzParameter::op_off_by) && getIntValue(SfzParameter::op_off_by) == val.wValue)
                sf2->set(idElt, champ_exclusiveClass, val);
            break;
        case SfzParameter::op_tuningFine:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_fineTune, val);
            break;
        case SfzParameter::op_tuningCoarse:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_coarseTune, val);
            break;
        case SfzParameter::op_delay:
            dTmp = _listeParam.at(i).getDoubleValue();
            addSeconds(dTmp, champ_delayModEnv, sf2, idElt);
            addSeconds(dTmp, champ_delayModLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVibLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVolEnv, sf2, idElt);
            break;
        case SfzParameter::op_offset:
            if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT_ZONE && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_end:
            if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT_ZONE && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_start:
            if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT_ZONE && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_end:
            if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT_ZONE && _listeParam.at(i).getIntValue() != 0)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endloopAddrsOffset, val);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endloopAddrsCoarseOffset, val);
            }
            break;
        case SfzParameter::op_loop_mode:
            if (this->isDefined(SfzParameter::op_trigger) && getStrValue(SfzParameter::op_trigger).rfind("release", 0) == 0)
            {
                val.wValue = 2;
                addSeconds(100, champ_releaseVolEnv, sf2, idElt);
                addSeconds(100, champ_releaseModEnv, sf2, idElt);
            }
            else if (_listeParam.at(i).getStringValue() == "no_loop")
                val.wValue = 0;
            else if (_listeParam.at(i).getStringValue() == "one_shot")
            {
                val.wValue = 0;
                addSeconds(100, champ_releaseVolEnv, sf2, idElt);
                addSeconds(100, champ_releaseModEnv, sf2, idElt);
            }
            else if (_listeParam.at(i).getStringValue() == "loop_continuous")
                val.wValue = 1;
            else if (_listeParam.at(i).getStringValue() == "loop_sustain")
                val.wValue = 3;
            sf2->set(idElt, champ_sampleModes, val);
            break;
        case SfzParameter::op_trigger:
            // Nothing here, processed in op_loop_mode
            break;
        case SfzParameter::op_volume:
            dTmp = -_listeParam.at(i).getDoubleValue() / DB_SF2_TO_REAL_DB;
            if (sf2->isSet(idElt, champ_initialAttenuation))
                dTmp += (double)sf2->get(idElt, champ_initialAttenuation).shValue / 10.;
            val.shValue = qtRound(10. * dTmp);
            sf2->set(idElt, champ_initialAttenuation, val);
            break;
        case SfzParameter::op_tuningScale:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_scaleTuning, val);
            break;
        case SfzParameter::op_amp_veltrack: {
            // Create a modulator modifying the attenuation based on the velocity
            ModulatorData modData;
            modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeConcave, true, false);
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.amount = 9.6 * _listeParam.at(i).getIntValue();
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialAttenuation;
            addModulator(modData);
        } break;
        case SfzParameter::op_ampeg_delay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVolEnv, sf2, idElt);
            break;
        case SfzParameter::op_ampeg_attack:
            if (_listeParam.at(i).getDoubleValue() == 0.001)
                val.shValue = -12000;
            else
                val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackVolEnv, val);
            break;
        case SfzParameter::op_ampeg_hold:
            if (!isDefined(SfzParameter::op_noteToVolEnvHold))
            {
                val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdVolEnv, val);
            }
            break;
        case SfzParameter::op_ampeg_decay:
            if (!isDefined(SfzParameter::op_noteToVolEnvDecay))
            {
                val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayVolEnv, val);
            }
            break;
        case SfzParameter::op_ampeg_sustain:
            dTmp = _listeParam.at(i).getDoubleValue();
            if (dTmp >= 0.1)
                val.shValue = qtRound(-10. * gainToDB(dTmp / 100.));
            else
                val.shValue = 1440;
            sf2->set(idElt, champ_sustainVolEnv, val);
            break;
        case SfzParameter::op_ampeg_release:
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseVolEnv, val);
            break;
        case SfzParameter::op_noteToVolEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToVolEnvHold, SfzParameter::op_ampeg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvHold, val);
            val.shValue = qtRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdVolEnv, val);
        }break;
        case SfzParameter::op_noteToVolEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToVolEnvDecay, SfzParameter::op_ampeg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvDecay, val);
            val.shValue = qtRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayVolEnv, val);
        }break;
        case SfzParameter::op_reverb:
            val.wValue = qtRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_reverbEffectsSend, val);
            break;
        case SfzParameter::op_chorus:
            val.wValue = qtRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_chorusEffectsSend, val);
            break;
        case SfzParameter::op_filterFreq: {
            int modifier = this->getIntValue(SfzParameter::op_fil_veltrack);
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176) + modifier);
            sf2->set(idElt, champ_initialFilterFc, val);
        } break;
        case SfzParameter::op_filterQ:
            val.shValue = qtRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_initialFilterQ, val);
            break;
        case SfzParameter::op_fil_veltrack: {
            // Create a modulator modifying the filter frequency based on the velocity
            ModulatorData modData;
            if (this->isDefined(SfzParameter::op_filterFreq))
            {
                modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, true, false);
                modData.amount = -_listeParam.at(i).getIntValue();
            }
            else
            {
                modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, false, false);
                modData.amount = _listeParam.at(i).getIntValue();
            }
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialFilterFc;
            addModulator(modData);
        } break;
        case SfzParameter::op_fil_keycenter:
            // Nothing here, already processed for changing the value of the filter if op_fil_keytrack is set
            break;
        case SfzParameter::op_fil_keytrack: {
            // Create a modulator modifying the filter frequency based on the key (center has been changed to 64)
            ModulatorData modData;
            modData.srcOper = SFModulator(GeneralController::GC_noteOnKeyNumber, ModType::typeLinear, false, true);
            modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
            modData.amount = 64 * _listeParam.at(i).getIntValue();
            modData.transOper = SFTransform::linear;
            modData.destOper = champ_initialFilterFc;
            addModulator(modData);
        } break;
        case SfzParameter::op_vibLFOdelay:
            if (isDefined(SfzParameter::op_vibLFOtoTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVibLFO, sf2, idElt);
            break;
        case SfzParameter::op_vibLFOfreq:
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqVibLFO, val);
            break;
        case SfzParameter::op_vibLFOtoTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_vibLfoToPitch, val);
            break;
        case SfzParameter::op_pitcheg_delay:
            if (isDefined(SfzParameter::op_modEnvToTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModEnv, sf2, idElt);
            break;
        case SfzParameter::op_pitcheg_attack:
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackModEnv, val);
            break;
        case SfzParameter::op_pitcheg_hold:
            if (!isDefined(SfzParameter::op_noteToModEnvHold))
            {
                val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            break;
        case SfzParameter::op_pitcheg_decay:
            if (!isDefined(SfzParameter::op_noteToModEnvDecay))
            {
                val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayModEnv, val);
            }
            break;
        case SfzParameter::op_pitcheg_sustain:
            val.shValue = qtRound(1000. - 10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_sustainModEnv, val);
            break;
        case SfzParameter::op_pitcheg_release:
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseModEnv, val);
            break;
        case SfzParameter::op_modEnvToTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_modEnvToPitch, val);
            break;
        case SfzParameter::op_noteToModEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToModEnvHold, SfzParameter::op_pitcheg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvHold, val);
            val.shValue = qtRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdModEnv, val);
        }break;
        case SfzParameter::op_noteToModEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, SfzParameter::op_noteToModEnvDecay, SfzParameter::op_pitcheg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvDecay, val);
            val.shValue = qtRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayModEnv, val);
        }break;
        case SfzParameter::op_modLFOdelay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModLFO, sf2, idElt);
            break;
        case SfzParameter::op_modLFOfreq:
            val.shValue = qtRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqModLFO, val);
            break;
        case SfzParameter::op_modLFOtoVolume:
            val.shValue = qtRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_modLfoToVolume, val);
            break;
        default:
            break;
        }
    }

    // Fixed velocity
    if (isDefined(SfzParameter::op_amp_velcurve_1) && isDefined(SfzParameter::op_amp_velcurve_127))
    {
        int velMin = qtRound(127. * getDoubleValue(SfzParameter::op_amp_velcurve_1));
        int velMax = qtRound(127. * getDoubleValue(SfzParameter::op_amp_velcurve_127));
        if (velMin == velMax)
        {
            val.wValue = velMin;
            sf2->set(idElt, champ_velocity, val);
        }
    }

    // Filter envelop
    if (isDefined(SfzParameter::op_modEnvToFilter))
    {
        if (isDefined(SfzParameter::op_modEnvToTon))
        {
            // Same parameters?
            if (getDoubleValue(SfzParameter::op_fileg_delay) == getDoubleValue(SfzParameter::op_pitcheg_delay) &&
                getDoubleValue(SfzParameter::op_fileg_attack) == getDoubleValue(SfzParameter::op_pitcheg_attack) &&
                getDoubleValue(SfzParameter::op_fileg_hold) == getDoubleValue(SfzParameter::op_pitcheg_hold) &&
                getDoubleValue(SfzParameter::op_fileg_decay) == getDoubleValue(SfzParameter::op_pitcheg_decay) &&
                getDoubleValue(SfzParameter::op_fileg_sustain) == getDoubleValue(SfzParameter::op_pitcheg_sustain) &&
                getDoubleValue(SfzParameter::op_fileg_release) == getDoubleValue(SfzParameter::op_pitcheg_release) &&
                getDoubleValue(SfzParameter::op_fileg_holdcc133) == getDoubleValue(SfzParameter::op_noteToModEnvHold) &&
                getDoubleValue(SfzParameter::op_fileg_decaycc133) == getDoubleValue(SfzParameter::op_noteToModEnvDecay))
            {
                val.shValue = getIntValue(SfzParameter::op_modEnvToFilter);
                sf2->set(idElt, champ_modEnvToFilterFc, val);
            }
        }
        else
        {
            if (isDefined(SfzParameter::op_fileg_delay))
                addSeconds(getDoubleValue(SfzParameter::op_fileg_delay), champ_delayModEnv, sf2, idElt);
            if (isDefined(SfzParameter::op_fileg_attack))
            {
                val.shValue = qtRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_attack)));
                sf2->set(idElt, champ_attackModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_hold) && !isDefined(SfzParameter::op_fileg_holdcc133))
            {
                val.shValue = qtRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_hold)));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_decay) && !isDefined(SfzParameter::op_fileg_decaycc133))
            {
                val.shValue = qtRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_decay)));
                sf2->set(idElt, champ_decayModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_sustain))
            {
                val.shValue = qtRound(1000. - 10. * getDoubleValue(SfzParameter::op_fileg_sustain));
                sf2->set(idElt, champ_sustainModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_release))
            {
                val.shValue = qtRound(log2m1200(getDoubleValue(SfzParameter::op_fileg_release)));
                sf2->set(idElt, champ_releaseModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_holdcc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, SfzParameter::op_fileg_holdcc133, SfzParameter::op_fileg_hold);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvHold, val);
                val.shValue = qtRound(log2m1200(baseValue));
                sf2->set(idElt, champ_holdModEnv, val);
            }
            if (isDefined(SfzParameter::op_fileg_decaycc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, SfzParameter::op_fileg_decaycc133, SfzParameter::op_fileg_decay);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvDecay, val);
                val.shValue = qtRound(log2m1200(baseValue));
                sf2->set(idElt, champ_decayModEnv, val);
            }

            val.shValue = getIntValue(SfzParameter::op_modEnvToFilter);
            sf2->set(idElt, champ_modEnvToFilterFc, val);
        }
    }

    // Filter LFO
    if (isDefined(SfzParameter::op_modLFOtoFilter))
    {
        if (isDefined(SfzParameter::op_modLFOtoVolume))
        {
            // Same parameters?
            if (getDoubleValue(SfzParameter::op_filLFOdelay) == getDoubleValue(SfzParameter::op_modLFOdelay) &&
                getDoubleValue(SfzParameter::op_filLFOfreq) == getDoubleValue(SfzParameter::op_modLFOfreq))
            {
                val.shValue = getIntValue(SfzParameter::op_modLFOtoFilter);
                sf2->set(idElt, champ_modLfoToFilterFc, val);
            }
        }
        else
        {
            if (isDefined(SfzParameter::op_filLFOdelay))
                addSeconds(getDoubleValue(SfzParameter::op_filLFOdelay), champ_delayModLFO, sf2, idElt);
            if (isDefined(SfzParameter::op_filLFOfreq))
            {
                val.shValue = qtRound(log2m1200(getDoubleValue(SfzParameter::op_filLFOfreq) / 8.176));
                sf2->set(idElt, champ_freqModLFO, val);
            }
            val.shValue = getIntValue(SfzParameter::op_modLFOtoFilter);
            sf2->set(idElt, champ_modLfoToFilterFc, val);
        }
    }

    // Disable the default modulators if they are not already defined and
    if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT)
        disableDefaultModulators();

    // Save the modulator list in the soundfont
    for (ModulatorData modData : _decodedModulators)
        saveModulator(sf2, idElt, modData, velToFilterType);
}

void SfzParameterRegion::getKeynumValues(double &baseValue, int &keynum,
                                        SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase) const
{
    // Division range
    int noteMin = 0;
    int noteMax = 127;
    if (isDefined(SfzParameter::op_key))
        noteMin = noteMax = getIntValue(SfzParameter::op_key);
    if (isDefined(SfzParameter::op_keyMin))
        noteMin = getIntValue(SfzParameter::op_keyMin);
    if (isDefined(SfzParameter::op_keyMax))
        noteMax = getIntValue(SfzParameter::op_keyMax);

    // Base value of the parameter
    double valBase = 0.001;
    if (isDefined(opCodeBase))
        valBase = getDoubleValue(opCodeBase);

    // Value at min and max positions
    double valKeynum = getDoubleValue(opCodeKeynum);
    double valMin = valBase + (double)noteMin / 127. * valKeynum;
    double valMax = valBase + (double)noteMax / 127. * valKeynum;

    if (noteMin == noteMax || valMax <= 0 || valMin <= 0)
    {
        baseValue = valMin;
        keynum = 0;
    }
    else
    {
        keynum = qtRound(1200. * log2(valMin / valMax) / (noteMax - noteMin));
        baseValue = valMin * std::pow(valMin / valMax, (double)(noteMin - 60) / (noteMax - noteMin));
    }
}

void SfzParameterRegion::addSeconds(double value, AttributeType champ, ISoundfontBuilder * sf2, const Id &id)
{
    double dTmp;
    AttributeValue val;
    if (sf2->isSet(id, champ))
        dTmp = d1200e2(sf2->get(id, champ).shValue);
    else
        dTmp = 0;
    val.shValue = qtRound(log2m1200(dTmp + value));
    sf2->set(id, champ, val);
}

std::string SfzParameterRegion::getName(const std::string &name, int maxCharacters, int suffixNumber, const std::string &suffix)
{
    int suffixSize = utf16Length(suffix);

    // Case when the suffix size is higher than the maximum number of characters
    if (suffixSize > maxCharacters)
        return utf16Left(name, maxCharacters);

    // Case when a number is not necessary
    if (suffixNumber == 0)
        return utf16Left(name, maxCharacters - suffixSize) + suffix;

    std::string suffixNum = std::to_string(suffixNumber);
    int suffixNumSize = static_cast<int>(suffixNum.length()) + 1;

    // Case when the numerical suffix is too high
    if (suffixNumSize > 3 || maxCharacters - suffixSize < suffixNumSize)
        return utf16Left(name, maxCharacters - suffixSize) + suffix;

    return utf16Left(name, maxCharacters - suffixNumSize - suffixSize) + suffix + "-" + suffixNum;
}

void SfzParameterRegion::mergeIfNotDefined(SfzParameterRegion &groupToMerge)
{
    for (const SfzParameter &param : groupToMerge._listeParam)
        if (!this->isDefined(param.getOpCode()))
            _listeParam.push_back(param);
}

void SfzParameterRegion::addModulator(ModulatorData modData)
{
    // Store the modulator only if it's not already defined
    for (const ModulatorData &existingModData : _decodedModulators)
        if (existingModData == modData)
            return;
    _decodedModulators.push_back(modData);
}

void SfzParameterRegion::saveModulator(ISoundfontBuilder * sf2, const Id &idElt, ModulatorData &modData, int velToFilterType)
{
    // If we are about to add a modulator for the global division, first check that it's not the definition of a default modulator
    if (idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT)
    {
        int defaultModulatorCount;
        const ModulatorData * defaultModulators = ModulatorData::getDefaultModulators(velToFilterType, defaultModulatorCount);
        for (int i = 0; i < defaultModulatorCount; i++)
        {
            if (defaultModulators[i] == modData && defaultModulators[i].amount == modData.amount)
                return;
        }
    }

    // Add a new modulator
    Id idMod(idElt.type == ISoundfontBuilder::TYPE_INSTRUMENT ? ISoundfontBuilder::TYPE_INSTRUMENT_MOD :
                                                                ISoundfontBuilder::TYPE_INSTRUMENT_ZONE_MOD,
             idElt.element, idElt.zone);
    idMod.modulator = sf2->add(idMod);

    // And configure it
    AttributeValue val;
    val.sfModValue = modData.srcOper;
    sf2->set(idMod, champ_sfModSrcOper, val);
    val.wValue = modData.destOper;
    sf2->set(idMod, champ_sfModDestOper, val);
    val.wValue = modData.amount;
    sf2->set(idMod, champ_modAmount, val);
    val.sfModValue = modData.amtSrcOper;
    sf2->set(idMod, champ_sfModAmtSrcOper, val);
    val.sfTransValue = modData.transOper;
    sf2->set(idMod, champ_sfModTransOper, val);
}

void SfzParameterRegion::disableDefaultModulators()
{
    // Disable "MIDI Note-On Velocity to Initial Attenuation"
    ModulatorData modData;
    modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeConcave, true, false);
    modData.amtSrcOper = SFModulator(GeneralController::GC_noController, ModType::typeLinear, false, false);
    modData.amount = 0;
    modData.transOper = SFTransform::linear;
    modData.destOper = champ_initialAttenuation;
    addModulator(modData);

    // Disable "MIDI Note-On Velocity to Filter Cutoff"
    modData.srcOper = SFModulator(GeneralController::GC_noteOnVelocity, ModType::typeLinear, true, false);
    modData.destOper = champ_initialFilterFc;
    addModulator(modData);
}
