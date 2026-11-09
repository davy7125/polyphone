/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#ifndef SOUNDFONTBUILDER_H
#define SOUNDFONTBUILDER_H

#include <functional>
#include <string>
#include <vector>
#include "attributetypes.h"

// Destination of a soundfont converted from another format (SfzReader, DlsReader): the model of the editor, or the
// soundfont played by the plugin. The readers write and read it as the editor does with its model, the elements being
// addressed in the same way (type, index of the element, of the division and of the modulator).
class ISoundfontBuilder
{
public:
    // Same values as ElementType in the editor
    enum Type
    {
        TYPE_SF2 = 0,
        TYPE_SAMPLE = 1,
        TYPE_INSTRUMENT = 2,
        TYPE_PRESET = 3,
        TYPE_INSTRUMENT_ZONE = 4,
        TYPE_PRESET_ZONE = 5,
        TYPE_INSTRUMENT_MOD = 9,
        TYPE_PRESET_MOD = 10,
        TYPE_INSTRUMENT_ZONE_MOD = 11,
        TYPE_PRESET_ZONE_MOD = 12
    };

    struct Id
    {
        Id(Type type, int element = -1, int zone = -1, int modulator = -1) :
            type(type), element(element), zone(zone), modulator(modulator) {}
        Type type;
        int element;
        int zone;
        int modulator;
    };

    virtual ~ISoundfontBuilder() {}

    // New element (sample, instrument, preset, division of an instrument or a preset, modulator): its index
    virtual int add(const Id &id) = 0;

    // Values of the generators and of the properties of the elements
    virtual void set(const Id &id, AttributeType champ, AttributeValue value) = 0;
    virtual void setText(const Id &id, AttributeType champ, const std::string &text) = 0; // UTF-8
    virtual bool isSet(const Id &id, AttributeType champ) = 0;
    virtual AttributeValue get(const Id &id, AttributeType champ) = 0;
    virtual std::string getText(const Id &id, AttributeType champ) = 0;
    virtual void reset(const Id &id, AttributeType champ) = 0;

    // Properties of the sound of a sample, given as is (the editor does not adapt the other ones)
    virtual void setSoundProperty(const Id &id, AttributeType champ, AttributeValue value) = 0;

    // Indexes of the elements of the same kind (samples, instruments, presets, divisions of an element)
    virtual std::vector<int> getSiblings(const Id &id) = 0;

    // Bank and preset numbers that no preset uses, closest available preset number in a bank (-1 if none)
    virtual bool isAvailable(int bank, int preset);
    virtual int closestAvailablePreset(int bank, int preset);

    // Closest preset number available, from a function telling whether a number is available (also used by the editor)
    static int findClosestAvailablePreset(int preset, const std::function<bool(int)> &isAvailable);

    // The editor moves the value of a parameter in the global division of an instrument or a preset if all its
    // divisions have it: the sound does not change, a soundfont only played can ignore it
    virtual void simplify(const Id &id, AttributeType champ) { (void)id; (void)champ; }
};

#endif // SOUNDFONTBUILDER_H
