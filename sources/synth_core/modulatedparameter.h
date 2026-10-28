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

#ifndef MODULATEDPARAMETER_H
#define MODULATEDPARAMETER_H

#include "attributebase.h"

class ModulatedParameter
{
public:
    ModulatedParameter();

    // Initialize a modulated parameter
    void setType(AttributeType type);
    void resetComputation();

    // Set the values from the instrument or preset level
    void initValue(AttributeValue value, bool isPrst);

    // Clear or add modulations
    void clearModulations();
    void addInstModulation(float value);
    void addPrstModulation(float value);

    // Get the resulting value as an integer or a float (a conversion might occur)
    int32_t getIntValue();
    float getRealValue();

    // Same values, computed again only if the version of the parameters changed since the last call
    // (VoiceParam increments it after each change), for the thread computing the voice
    int32_t getIntValue(uint32_t version)
    {
        if (version != _cacheVersion)
            updateCache(version);
        return _cachedIntValue;
    }
    float getRealValue(uint32_t version)
    {
        if (version != _cacheVersion)
            updateCache(version);
        return _cachedRealValue;
    }

private:
    void computeValue();
    void updateCache(uint32_t version);

    AttributeType _type;
    AttributeBase _instValue, _prstValue;
    float _instModulation, _prstModulation;

    bool _notRealTime;
    bool _computed;
    AttributeValue _computedValue;

    // Values for a version of the parameters
    uint32_t _cacheVersion;
    int32_t _cachedIntValue;
    float _cachedRealValue;
};

#endif // MODULATEDPARAMETER_H
