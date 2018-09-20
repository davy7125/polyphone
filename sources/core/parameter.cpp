/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "parameter.h"
#include <qmath.h>

Parameter::Parameter(Champ champ, bool isPrst) :
    _champ(champ),
    _isPrst(isPrst),
    _realValue(0)
{
    _storedValue.shAmount = 0;
}

Parameter::Parameter(Champ champ, bool isPrst, double value) : Parameter(champ, isPrst)
{
    setRealValue(value);
}

Parameter::Parameter(Champ champ, bool isPrst, genAmountType storedValue) : Parameter(champ, isPrst)
{
    setStoredValue(storedValue);
}

void Parameter::setRealValue(double realValue)
{
    _storedValue = computeStoredValue(_champ, _isPrst, realValue);

    // Adapt the real value based on the new stored value
    _realValue = computeRealValue(_champ, _isPrst, _storedValue);
}

void Parameter::setStoredValue(genAmountType storedValue)
{
    _storedValue = storedValue;
    _realValue = computeRealValue(_champ, _isPrst, _storedValue);
}

void Parameter::setStoredValue(qint16 storedValue)
{
    _storedValue.shAmount = storedValue;
    _realValue = computeRealValue(_champ, _isPrst, _storedValue);
}

void Parameter::setStoredValue(quint16 storedValue)
{
    _storedValue.wAmount = storedValue;
    _realValue = computeRealValue(_champ, _isPrst, _storedValue);
}

void Parameter::setStoredValue(qint8 lower, qint8 upper)
{
    _storedValue.ranges.byLo = (lower < upper ? lower : upper);
    _storedValue.ranges.byHi = (upper > lower ? upper : lower);
    _realValue = computeRealValue(_champ, _isPrst, _storedValue); //TODO: 2 integers
}

double Parameter::computeRealValue(Champ champ, bool isPrst, genAmountType storedValue)
{
    Q_UNUSED(isPrst);
    double realValue = 0;

    switch (champ)
    {
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        realValue = qPow(2., (double)storedValue.shAmount / 1200.);
        break;
    case champ_sustainVolEnv:
        realValue = (double)storedValue.wAmount / 10.;
        break;
    case champ_sustainModEnv:
        realValue = (double)storedValue.wAmount / 10.;
        break;
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
        realValue = storedValue.shAmount;
        break;
    default:
        break;
    }

    return realValue;
}

genAmountType Parameter::computeStoredValue(Champ champ, bool isPrst, double realValue)
{
    genAmountType storedValue;
    storedValue.shAmount = 0;

    switch (champ)
    {
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
        storedValue.shAmount = (qint16)limit(1200. * qLn(realValue) / 0.69314718056,
                                             isPrst ? -21000 : -12000,
                                             isPrst ? 21000 : 5000);
        break;
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        storedValue.shAmount = (qint16)limit(1200. * qLn(realValue) / 0.69314718056,
                                             isPrst ? -21000 : -12000,
                                             isPrst ? 21000 : 8000);
        break;
    case champ_sustainVolEnv:
        storedValue.shAmount = (qint16)limit(10. * realValue,
                                             isPrst ? -1440 : 0,
                                             isPrst ? 1440 : 1440);
        break;
    case champ_sustainModEnv:
        storedValue.shAmount = (qint16)limit(10. * realValue,
                                             isPrst ? -1000 : 0,
                                             isPrst ? 1000 : 1000);
        break;
    case champ_keynumToModEnvDecay: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToVolEnvHold:
        storedValue.shAmount = (qint16)limit(realValue,
                                             isPrst ? -2400 : -1200,
                                             isPrst ? 2400 : 1200);
        break;
    default:
        break;
    }

    return storedValue;
}

double Parameter::getDefaultRealValue(Champ champ, bool isPrst)
{
    // Default stored value
    genAmountType defaultStoredValue = Parameter::getDefaultStoredValue(champ, isPrst);

    // Compute the corresponding real value
    return computeRealValue(champ, isPrst, defaultStoredValue);
}

genAmountType Parameter::getDefaultStoredValue(Champ champ, bool isPrst)
{
    Q_UNUSED(isPrst);
    genAmountType value;
    value.shAmount = 0;

    switch (champ)
    {
    case champ_initialFilterFc:
        value.wAmount = 13500;
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
        value.shAmount = -12000;
        break;
    case champ_velRange: case champ_keyRange:
        value.ranges.byLo = 0;
        value.ranges.byHi = 127;
        break;
    case champ_keynum: case champ_velocity: case champ_overridingRootKey:
        value.shAmount = -1;
        break;
    case champ_scaleTuning:
        value.shAmount = 100;
        break;
    default:
        break;
    }

    return value;
}

int Parameter::limit(double value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
