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
    SFModulator(GeneralController index, ModType type, bool direction, bool polarity) :
        Type(type),
        isBipolar(polarity),
        isDescending(direction),
        CC(false),
        Index(static_cast<quint8>(index))
    {}

    SFModulator(quint8 ccNumber, ModType type, bool direction, bool polarity) :
        Type(type),
        isBipolar(polarity),
        isDescending(direction),
        CC(true),
        Index(ccNumber)
    {}

    SFModulator() :
        Type(typeLinear),
        isBipolar(false),
        isDescending(false),
        CC(false),
        Index(0)
    {}

    bool operator==(const SFModulator& other);
    quint16 toWord();

    quint8 Type : 6;
    bool isBipolar : 1;
    bool isDescending : 1;
    bool CC : 1;
    quint8 Index : 7;
};

typedef enum
{
    linear = 0,
    absolute_value = 2
} SFTransform;

class ModulatorData
{
public:
    ModulatorData() :
        destOper(0),
        amount(0),
        index(0)
    {}

    // Load a default modulator (num being from 0 to 9 included)
    void loadDefaultModulator(quint16 num);

    SFModulator srcOper;
    quint16 destOper; // One of the AttributeType if < 32768, otherwise the index of another modulator
    SFModulator amtSrcOper;
    SFTransform transOper;
    qint16 amount;
    quint16 index; // Global index of the modulator that may be referenced by others

    bool operator== (ModulatorData &other)
    {
        // Everything equal except "amount" and "index"
        return srcOper.toWord() == other.srcOper.toWord() &&
                amtSrcOper.toWord() == other.amtSrcOper.toWord() &&
                transOper == other.transOper &&
                destOper == other.destOper;
    }
} ;

#endif // MODULATORDATA_H
