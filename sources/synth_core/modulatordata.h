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

#ifndef MODULATORDATA_H
#define MODULATORDATA_H

#include <cstdint>

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
        Index(static_cast<uint8_t>(index))
    {}

    SFModulator(uint8_t ccNumber, ModType type, bool descending, bool bipolar) :
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
    uint16_t toWord() const;

    // Apply a shape, input being from 0 and 127 and output being from 0 or -1 to 1
    static void prepareConversionTables(); // Call it once before applyShape(..)
    double applyShape(double value);

    uint8_t Type : 6;
    bool isBipolar : 1;
    bool isDescending : 1;
    bool CC : 1;
    uint8_t Index : 7;

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

    // Default modulators, the velocity to filter cutoff being the one of the version 2.01 (0), 2.04 (1) or none (2)
    static const ModulatorData * getDefaultModulators(int velToFilterType, int &number);

    SFModulator srcOper;
    uint16_t destOper; // One of the AttributeType if < 32768, otherwise the index of another modulator
    SFModulator amtSrcOper;
    SFTransform transOper;
    int16_t amount;
    uint16_t index; // Global index of the modulator that may be referenced by others

    // Get the range of the output values, allowing a normalization
    void getRange(int16_t &min, int16_t &max);

    bool operator== (const ModulatorData &other) const
    {
        // Everything equal except "amount" and "index"
        return srcOper.toWord() == other.srcOper.toWord() &&
                amtSrcOper.toWord() == other.amtSrcOper.toWord() &&
                transOper == other.transOper &&
                destOper == other.destOper;
    }

private:
    void loadDefaultModulator(uint16_t num, int velToFilterType);
};

#endif // MODULATORDATA_H
