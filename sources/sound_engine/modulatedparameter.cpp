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

#include "modulatedparameter.h"

ModulatedParameter::ModulatedParameter(AttributeType type) :
    _type(type),
    _instValue(type, false),
    _prstValue(type, true),
    _instModulation(0),
    _prstModulation(0),
    _computed(false)
{
    // Some parameters are computed only once
    _notRealTime = (_type == champ_keynum || _type == champ_velocity || _type == champ_sampleModes ||
                    _type == champ_scaleTuning || _type == champ_exclusiveClass || _type == champ_overridingRootKey ||
                    _type == champ_keyRange || _type == champ_velRange);
}

void ModulatedParameter::initValue(AttributeValue value, bool isPrst)
{
    if (isPrst)
        _prstValue.setStoredValue(value);
    else
        _instValue.setStoredValue(value);
}

void ModulatedParameter::clearModulations()
{
    _instModulation = 0;
    _prstModulation = 0;
}

void ModulatedParameter::addInstModulation(double value)
{
    _instModulation += value;
}

void ModulatedParameter::addPrstModulation(double value)
{
    // Some attributes cannot be modulated at the preset level
    if (_type != champ_overridingRootKey && _type != champ_velocity && _type != champ_keynum)
        _prstModulation += value;
}

qint32 ModulatedParameter::getIntValue()
{
    // Compute the value
    computeValue();

    // Return a modulated value without conversion
    return _computedValue.shValue;
}

double ModulatedParameter::getRealValue()
{
    // Compute the value
    computeValue();

    // Return a possibly converted value
    return Attribute(_type, false, _computedValue).getRealValue();
}

void ModulatedParameter::computeValue()
{
    if (_computed && _notRealTime)
        return;

    // Add all values (before any conversion)
    qint32 addition = static_cast<qint32>(_instModulation + _prstModulation) +
            _instValue.getStoredValue().shValue +
            _prstValue.getStoredValue().shValue;

    // Limit the result
    if (addition > 32767)
        _computedValue.shValue = 32767;
    else if (addition < -32768)
        _computedValue.shValue = -32768;
    else
        _computedValue.shValue = static_cast<qint16>(addition);

    if (_type != champ_fineTune) // This parameter can be out of its original range (pitch wheel)
        _computedValue = Attribute::limit(_type, _computedValue, false);

    _computed = true;
}
