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

#ifndef SFZPARAMETERGROUP_H
#define SFZPARAMETERGROUP_H

#include "sfzparameterregion.h"

class SfzParameterGroup
{
public:
    SfzParameterGroup() {}
    void newGroup() { _regionList << SfzParameterRegion(); }
    void addParam(QString opcode, QString valeur)
    {
        if (_regionList.size() == 0)
        {
            if (opcode == "group_label")
                _label = valeur;
            else
                _paramGlobaux << SfzParameter(opcode, valeur);
        }
        else
            _regionList.last() << SfzParameter(opcode, valeur);
    }
    void moveOpcodesInGlobal(SfzParameterRegion &globalZone);
    void moveKeynumInSamples(SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase);
    void moveOpcodeInSamples(SfzParameter::OpCode opcode, QMetaType::Type type);
    void moveModInSamples();
    void moveModInSamples(QList<SfzParameter::OpCode> opCodeList);
    void checkSampleValid(QString path);
    void checkFilter();
    void checkKeyTrackedFilter();
    void adjustCorrection(QString path);
    void adjustModulationVolume();
    bool isChannel10();
    double getAmpliMax();
    void adjustVolume(double offset);
    QString getLabel() { return _label; }

    // Decode
    void decode(SoundfontManager * sf2, EltID idInst, QString pathSfz);

private:
    static double limit(double value, double min, double max);

    SfzParameterRegion _paramGlobaux;
    QList<SfzParameterRegion> _regionList;
    QString _label;
};

#endif // SFZPARAMETERGROUP_H
