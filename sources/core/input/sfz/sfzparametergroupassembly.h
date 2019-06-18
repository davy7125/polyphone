/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SFZPARAMETERGROUPASSEMBLY_H
#define SFZPARAMETERGROUPASSEMBLY_H

#include "sfzparametergroup.h"

class SfzParameterGroupAssembly
{
public:
    // Chargement
    SfzParameterGroupAssembly() {}
    void newGroup() { _listeDivisions << SfzParameterGroup(); }
    void addParam(QString opcode, QString valeur)
    {
        if (_listeDivisions.size() == 0)
        {
            if (opcode == "group_label")
                _label = valeur;
            else
                _paramGlobaux << SfzParameter(opcode, valeur);
        }
        else
            _listeDivisions.last() << SfzParameter(opcode, valeur);
    }
    void moveOpcodesInGlobal(SfzParameterGroup &globalZone);
    void moveKeynumInSamples(SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase);
    void moveOpcodeInSamples(SfzParameter::OpCode opcode, QVariant::Type type);
    void moveModInSamples();
    void moveModInSamples(QList<SfzParameter::OpCode> opCodeList);
    void checkSampleValid(QString path);
    void checkFilter();
    void simplifyAttenuation();
    void adjustStereoVolumeAndCorrection(QString path);
    void adjustModulationVolume();
    bool isChannel10();
    double getAmpliMax();
    void adjustVolume(double offset);
    QString getLabel() { return _label; }

    // Décodage
    void decode(SoundfontManager * sf2, EltID idInst, QString pathSfz);

private:
    SfzParameterGroup _paramGlobaux;
    QList<SfzParameterGroup> _listeDivisions;
    QString _label;
};

#endif // SFZPARAMETERGROUPASSEMBLY_H
