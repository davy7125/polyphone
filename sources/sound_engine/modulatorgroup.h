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

#ifndef MODULATORGROUP_H
#define MODULATORGROUP_H

#include "basetypes.h"
class ModulatedParameter;
class ParameterModulator;

class ModulatorGroup
{
public:
    ModulatorGroup(QMap<AttributeType, ModulatedParameter *> * parameters, bool isPrst);
    ~ModulatorGroup();

    // Initialize with keys and vel
    void initialize(int initialKey, int keyForComputation, int velForComputation);

    // Load modulators from the instrument or preset level
    void loadModulators(QList<ModulatorData> &modulators);

    // Compute the modulations and apply them on the parameters
    void process();

private:
    void loadDefaultModulators();

    QMap<AttributeType, ModulatedParameter *> * _parameters;
    bool _isPrst;
    int _initialKey, _keyForComputation, _velForComputation;
    QList<ParameterModulator *> _modulators;
};

#endif // MODULATORGROUP_H
