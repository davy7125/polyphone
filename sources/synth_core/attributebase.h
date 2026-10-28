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

#ifndef ATTRIBUTEBASE_H
#define ATTRIBUTEBASE_H

#include "attributetypes.h"

// Value of an attribute, stored as in a soundfont and converted in real units
// (Attribute adds the descriptions for the editor)
class AttributeBase
{
public:
    // Creation of a parameter
    // Champ: type of the parameter
    // isPrst: true if this parameter is for a preset
    // value: real value
    AttributeBase();
    AttributeBase(AttributeType champ, bool isPrst);
    AttributeBase(AttributeType champ, bool isPrst, double value);
    AttributeBase(AttributeType champ, bool isPrst, AttributeValue storedValue);

    void setType(AttributeType champ, bool isPrst);

    // Get / Set the real value
    // The stored value will be adapted accordingly
    void setRealValue(double realValue);
    double getRealValue() { return _realValue; }

    // Get / Set the stored value (from the binary file)
    // The real value will be adapted accordingly
    void setStoredValue(AttributeValue storedValue);
    void setStoredValue(int16_t storedValue);
    void setStoredValue(uint16_t storedValue);
    void setStoredValue(uint8_t lower, uint8_t upper);
    AttributeValue getStoredValue() { return _storedValue; }

    // Default values
    static double getDefaultRealValue(AttributeType champ, bool isPrst);
    static AttributeValue getDefaultStoredValue(AttributeType champ, bool isPrst);

    // Conversions
    static double toRealValue(AttributeType champ, bool isPrst, AttributeValue storedValue);
    static AttributeValue fromRealValue(AttributeType champ, bool isPrst, double realValue);

    // Limit
    static void getLimit(AttributeType champ, bool isPrst, int &min, int &max);
    static AttributeValue limit(AttributeType champ, AttributeValue value, bool isPrst);

private:
    static int limit(int value, int min, int max);
    static double limit(double value, double min, double max);

    AttributeType _champ;
    AttributeValue _storedValue;
    double _realValue;
    bool _isPrst;
};

#endif // ATTRIBUTEBASE_H
