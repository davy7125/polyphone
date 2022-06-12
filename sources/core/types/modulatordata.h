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
    SFModulator(GeneralController index, ModType type, bool descending, bool bipolar) :
        Type(type),
        isBipolar(bipolar),
        isDescending(descending),
        CC(false),
        Index(static_cast<quint8>(index))
    {}

    SFModulator(quint8 ccNumber, ModType type, bool descending, bool bipolar) :
        Type(type),
        isBipolar(bipolar),
        isDescending(descending),
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

    bool operator==(const SFModulator& other) const;
    quint16 toWord() const;

    // Apply a shape, input being from 0 and 127 and output being from 0 or -1 to 1
    static void prepareConversionTables(); // Call it once before applyShape(..)
    double applyShape(double value);

    quint8 Type : 6;
    bool isBipolar : 1;
    bool isDescending : 1;
    bool CC : 1;
    quint8 Index : 7;

private:
    static double concave(double value);
    static double convex(double value);

    static double s_concaveTable[128];
    static double s_convexTable[128];
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

    // Default modulators
    static void setModulatorVelToFilterType(int type); // Modified by confmanager
    static ModulatorData * getDefaultModulators(int &number);

    SFModulator srcOper;
    quint16 destOper; // One of the AttributeType if < 32768, otherwise the index of another modulator
    SFModulator amtSrcOper;
    SFTransform transOper;
    qint16 amount;
    quint16 index; // Global index of the modulator that may be referenced by others

    // Get the range of the output values, allowing a normalization
    void getRange(qint16 &min, qint16 &max);

    bool operator== (const ModulatorData &other) const
    {
        // Everything equal except "amount" and "index"
        return srcOper.toWord() == other.srcOper.toWord() &&
                amtSrcOper.toWord() == other.amtSrcOper.toWord() &&
                transOper == other.transOper &&
                destOper == other.destOper;
    }

private:
    void loadDefaultModulator(quint16 num);

    static int s_modulatorVelToFilterType;
    static ModulatorData s_defaultMods[10];
};

#endif // MODULATORDATA_H
