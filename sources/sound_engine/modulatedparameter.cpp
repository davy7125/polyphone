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
    _prstModulation(0)
{

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
{qDebug() << "inst modulation" << value;
    _instModulation += value;
}

void ModulatedParameter::addPrstModulation(double value)
{
    _prstModulation += value;
}

qint32 ModulatedParameter::getIntValue()
{
    // Combine inst and prst
    qint32 result = 0;
    switch (_type)
    {
    case champ_sampleModes: case champ_exclusiveClass:
        // Only in instrument
        result = _instValue.getStoredValue().wValue + static_cast<qint32>(_instModulation);
        break;
    case champ_overridingRootKey: case champ_velocity: case champ_keynum:
        // Only in instrument, can be -1 (not defined)
        result = _instValue.getStoredValue().shValue + static_cast<qint32>(_instModulation);
        break;
    case champ_startAddrsOffset: case champ_startAddrsCoarseOffset:
    case champ_endAddrsOffset: case champ_endAddrsCoarseOffset:
    case champ_startloopAddrsOffset: case champ_startloopAddrsCoarseOffset:
    case champ_endloopAddrsOffset: case champ_endloopAddrsCoarseOffset:
    case champ_fineTune: case champ_coarseTune: case champ_scaleTuning:
    case champ_modLfoToFilterFc: case champ_modLfoToPitch: case champ_vibLfoToPitch:
    case champ_modEnvToPitch: case champ_modEnvToFilterFc:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
        result = _instValue.getStoredValue().shValue + _prstValue.getStoredValue().shValue +
                static_cast<qint32>(_instModulation) + static_cast<qint32>(_prstModulation);
        break;
    default:
        qDebug() << "ModulatedParameter: type" << _type << "is not an integer";
        return 0;
    }

    // Limit the result
    AttributeValue value;
    if (result > 32767)
        value.shValue = 32767;
    else if (result < -32768)
        value.shValue = -32768;
    else
        value.shValue = static_cast<qint16>(result);
    value = Attribute::limit(_type, value, false);

    // Return it
    return value.shValue;
}

double ModulatedParameter::getRealValue()
{
    // Combine inst and prst
    double result = 0;
    switch (_type)
    {
    case champ_reverbEffectsSend: case champ_chorusEffectsSend:
    case champ_pan: case champ_initialAttenuation:
    case champ_initialFilterQ: case champ_modLfoToVolume:
    case champ_sustainModEnv: case champ_sustainVolEnv:
        result = _instValue.getRealValue() + _prstValue.getRealValue() + _instModulation + _prstModulation;
        break;
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv:
    case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv:
    case champ_decayModEnv: case champ_releaseModEnv:
    case champ_delayModLFO: case champ_freqModLFO:
    case champ_delayVibLFO: case champ_freqVibLFO:
    case champ_initialFilterFc:
        result = (_instValue.getRealValue() + _instModulation) * (_prstValue.getRealValue() + _prstModulation);
        break;
    default:
        qDebug() << "ModulatedParameter: type" << _type << "is not a floating value";
        return 0.0;
    }

    // Limit the result
    result = this->limit(result);

    // Return it
    return result;
}

double ModulatedParameter::limit(double value)
{
    double min = 0;
    double max = 0;

    switch (_type)
    {
    case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        min = 0;
        max = 100;
        break;
    case champ_pan:
        min = -50;
        max = 50;
        break;
    case champ_initialAttenuation: case champ_sustainModEnv: case champ_sustainVolEnv:
        min = 0;
        max = 144;
        break;
    case champ_initialFilterFc:
        min = 18;
        max = 19914;
        break;
    case champ_initialFilterQ:
        min = 0;
        max = 96;
        break;
    case champ_delayVolEnv: case champ_holdVolEnv: case champ_delayModEnv: case champ_holdModEnv:
        min = 0.001;
        max = 18;
        break;
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv:
    case champ_attackModEnv: case champ_decayModEnv: case champ_releaseModEnv:
        min = 0.001;
        max = 101.6;
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
        min = 0.001;
        max = 20;
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        min = 0.001;
        max = 100;
        break;
    case champ_modLfoToVolume:
        min = -96;
        max = 96;
        break;
    default:
        qDebug() << "ModulatedParameter: type" << _type << "cannot be limited as a double";
        return value;
    }

    if (value > max)
        return max;
    if (value < min)
        return min;
    return value;
}
