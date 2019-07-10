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

#ifndef MODULATEDPARAMETER_H
#define MODULATEDPARAMETER_H

#include "basetypes.h"

class ModulatedParameter
{
public:
    // Initialize a modulated parameter
    ModulatedParameter(AttributeType type);

    // Set the values from the instrument or preset level
    void initValue(AttributeValue value, bool isPrst);

    // Get the resulting value as an integer or a double (a conversion might occur)
    qint32 getIntValue();
    double getRealValue();

private:
    AttributeType _type;
    Attribute _instValue, _prstValue;

    // Limit some parameters
    double limit(double value);
};

#endif // MODULATEDPARAMETER_H
