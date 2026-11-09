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


#ifndef SFZPARAMETERGROUP_H
#define SFZPARAMETERGROUP_H

#include <string>
#include <vector>
#include "sfzparameterregion.h"

// Group of an sfz file: its global part and its regions, converted into an instrument

class SfzParameterGroup
{
public:
    SfzParameterGroup() {}
    void newGroup() { _regionList.push_back(SfzParameterRegion()); }
    void addParam(const std::string &opcode, const std::string &valeur, const SfzContext &context)
    {
        if (_regionList.size() == 0)
        {
            if (opcode == "group_label")
                _label = valeur;
            else
                _paramGlobaux << SfzParameter(opcode, valeur, &context);
        }
        else
            _regionList.back() << SfzParameter(opcode, valeur, &context);
    }
    void moveOpcodesInGlobal(SfzParameterRegion &globalZone);
    void moveKeynumInSamples(SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase);
    enum ValueType
    {
        VALUE_DOUBLE,
        VALUE_INT,
        VALUE_STRING
    };

    void moveOpcodeInSamples(SfzParameter::OpCode opcode, ValueType type);
    void moveModInSamples();
    void moveModInSamples(const std::vector<SfzParameter::OpCode> &opCodeList);
    void checkSampleValid(const std::string &path);
    void checkFilter();
    void checkKeyTrackedFilter();
    void adjustCorrection(const std::string &path);
    void adjustModulationVolume();
    bool isChannel10();
    double getAmpliMax();
    void adjustVolume(double offset);
    std::string getLabel() { return _label; }

    // Decode (velToFilterType: default modulator of the velocity to the filter, see ModulatorData)
    void decode(ISoundfontBuilder * sf2, ISoundfontBuilder::Id idInst, const std::string &pathSfz, int velToFilterType);

private:
    static double limit(double value, double min, double max);

    SfzParameterRegion _paramGlobaux;
    std::vector<SfzParameterRegion> _regionList;
    std::string _label;
};

#endif // SFZPARAMETERGROUP_H
