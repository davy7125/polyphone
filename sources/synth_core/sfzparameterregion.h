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


#ifndef SFZPARAMETERREGION_H
#define SFZPARAMETERREGION_H

#include <cmath>
#include <string>
#include <vector>
#include "sfzparameter.h"
#include "soundfontbuilder.h"
#include "modulatordata.h"

// Opcodes of a region (or of the global part of a group) of an sfz file, converted into the parameters of a division
class SfzParameterRegion
{
public:
    // Load
    SfzParameterRegion() {}
    void operator << (const SfzParameter &param) { _listeParam.push_back(param); }
    void mergeIfNotDefined(SfzParameterRegion &groupToMerge);

    // Decode (velToFilterType: default modulator of the velocity to the filter, see ModulatorData)
    void decode(ISoundfontBuilder * sf2, ISoundfontBuilder::Id idElt, int velToFilterType);
    std::vector<int> getSampleIndex(ISoundfontBuilder * sf2, ISoundfontBuilder::Id idElt, const std::string &pathSfz) const;
    void adaptLoop(int startLoop, int endLoop, int length);
    void adjustCorrection(const std::string &path, int defaultCorrection);
    bool sampleValid(const std::string &path);
    void checkFilter();
    void checkKeyTrackedFilter(bool remove);
    void adjustVolume(double offset);
    void adjustCorrection(int offset, int defaultCorrection);
    void removeOpCode(SfzParameter::OpCode opcode)
    {
        for (int i = static_cast<int>(_listeParam.size()) - 1; i >= 0; i--)
            if (_listeParam.at(i).getOpCode() == opcode)
                _listeParam.erase(_listeParam.begin() + i);
    }

    // Lecture
    bool isDefined(SfzParameter::OpCode opcode) const
    {
        bool bRet = false;
        for (size_t i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                bRet = true;
        return bRet;
    }
    int getIntValue(SfzParameter::OpCode opcode) const
    {
        int iRet = 0;
        for (size_t i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                iRet = _listeParam.at(i).getIntValue();
        return iRet;
    }
    double getDoubleValue(SfzParameter::OpCode opcode) const
    {
        double dRet = 0;
        for (size_t i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                dRet = _listeParam.at(i).getDoubleValue();
        return dRet;
    }
    std::string getStrValue(SfzParameter::OpCode opcode) const
    {
        std::string strRet = "";
        for (size_t i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                strRet = _listeParam.at(i).getStringValue();
        return strRet;
    }
    static double gainToDB(double gain) { return log10(gain) * 20.; }

private:
    std::vector<SfzParameter> _listeParam;
    std::vector<ModulatorData> _decodedModulators;

    void addModulator(ModulatorData modData);
    void disableDefaultModulators();
    static void saveModulator(ISoundfontBuilder * sf2, const ISoundfontBuilder::Id &idElt, ModulatorData &modData,
                              int velToFilterType);

    static std::vector<std::string> getFullPath(const std::string &base, std::vector<std::string> directories);
    static double log2m1200(double value) { return 1200. * std::log(std::max(0.001, value)) / 0.69314718056 /* ln(2) */; }
    static double d1200e2(int value) { return std::pow(2., static_cast<double>(value) / 1200.); }
    static void addSeconds(double value, AttributeType champ, ISoundfontBuilder * sf2, const ISoundfontBuilder::Id &id);
    void getKeynumValues(double &baseValue, int &keynum, SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase) const;
    static std::string getName(const std::string &name, int maxCharacters, int suffixNumber, const std::string &suffix = "");
};

#endif // SFZPARAMETERREGION_H
