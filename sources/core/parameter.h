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

#ifndef PARAMETER_H
#define PARAMETER_H

#include "sf2_types.h"

class Parameter
{
public:
    // Creation of a parameter
    // Champ: type of the parameter
    // isPrst: true if this parameter is for a preset
    // value: real value
    Parameter(Champ champ, bool isPrst);
    Parameter(Champ champ, bool isPrst, double value);
    Parameter(Champ champ, bool isPrst, genAmountType storedValue);

    // Get / Set the real value
    // The stored value will be adapted accordingly
    void setRealValue(double realValue);
    double getRealValue() { return _realValue; }

    // Get / Set the stored value
    // The real value will be adapted accordingly
    void setStoredValue(genAmountType storedValue);
    void setStoredValue(qint16 storedValue);
    void setStoredValue(quint16 storedValue);
    void setStoredValue(qint8 lower, qint8 upper);
    genAmountType getStoredValue() { return _storedValue; }

    // Default values
    static double getDefaultRealValue(Champ champ, bool isPrst);
    static genAmountType getDefaultStoredValue(Champ champ, bool isPrst);

    static double computeRealValue(Champ champ, bool isPrst, genAmountType storedValue);
    static genAmountType computeStoredValue(Champ champ, bool isPrst, double realValue);

private:

    static int limit(double value, int min, int max);

    Champ _champ;
    bool _isPrst;
    double _realValue;
    genAmountType _storedValue;
};

#endif // PARAMETER_H
