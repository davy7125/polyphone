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

enum ModType: unsigned char
{
    typeLinear = 0,
    typeConcave = 1,
    typeConvex = 2,
    typeSwitch = 3
};

enum ModDirection: bool
{
    directionAscending = 0,
    directionDescending = 1
};

enum ModPolarity: bool
{
    polarityUnipolar = 0,
    polarityBipolar = 1
};

enum GeneralController: unsigned char
{
    GC_noController = 0,
    GC_noteOnVelocity = 2,
    GC_noteOnKeyNumber = 3,
    GC_polypressure = 10,
    GC_channelPressure = 13,
    GC_pitchWheel = 14,
    GC_pitchWheelSensitivity = 16,
    GC_link = 127
};

class SFModulator
{
public:
    SFModulator(GeneralController index, ModType type, ModDirection direction, ModPolarity polarity) :
        Type(type),
        P(polarity),
        D(direction),
        CC(false),
        Index(static_cast<quint8>(index))
    {}

    SFModulator(quint8 ccNumber, ModType type, ModDirection direction, ModPolarity polarity) :
        Type(type),
        P(polarity),
        D(direction),
        CC(true),
        Index(ccNumber)
    {}

    SFModulator() :
        Type(typeLinear),
        P(polarityUnipolar),
        D(directionAscending),
        CC(false),
        Index(0)
    {}

    bool operator==(const SFModulator& other)
    {
        return this->Type == other.Type &&
                this->P == other.P &&
                this->D == other.D &&
                this->CC == other.CC &&
                this->Index == other.Index;
    }

    quint16 toWord()
    {
        //    quint16
        //        Type  : 6, // 6 bits for the type
        //        P     : 1, // Polarity
        //        D     : 1, // Direction
        //        CC    : 1,
        //        Index : 7;
        return ((Type << 10) + (P << 9) + (D << 8) + (CC << 7) + Index) & 0xFFFF;
    }

    quint8 Type : 6;
    ModPolarity P : 1;
    ModDirection D : 1;
    bool CC : 1;
    quint8 Index : 7;
};

typedef enum
{
    linear = 0,
    absolute_value = 2
} SFTransform;

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
