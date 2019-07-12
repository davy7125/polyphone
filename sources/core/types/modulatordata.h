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

#ifndef MODULATORDATA_H
#define MODULATORDATA_H

#include <QtCore>

typedef enum
{
    linear = 0,
    absolute_value = 2
} SFTransform;

typedef struct
{
    quint16
        Type  : 6, // 6 bits for the type
        P     : 1, // Polarity
        D     : 1, // Direction
        CC    : 1,
        Index : 7;
} SFModulator;

static bool operator==(const SFModulator& a1, const SFModulator& a2)
{
    return a1.Type == a2.Type &&
            a1.P == a2.P &&
            a1.D == a2.D &&
            a1.CC == a2.CC &&
            a1.Index == a2.Index;
}

typedef struct
{
    SFModulator srcOper;
    quint16 destOper; // One of the AttributeType if < 32768, otherwise the index of another modulator
    SFModulator amtSrcOper;
    SFTransform transOper;
    qint16 amount;
    quint16 index; // Global index of the modulator that may be referenced by others
} ModulatorData;

#endif // MODULATORDATA_H
