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

#ifndef SFZPARAMLIST_H
#define SFZPARAMLIST_H

#include "basetypes.h"
class SoundfontManager;

class SfzParamList
{
public:
    // Constructors
    SfzParamList(SoundfontManager * sf2, EltID id);
    SfzParamList(SoundfontManager * sf2, SfzParamList * paramPrst, EltID idInst);

    // Attributes
    AttributeType getAttribute(int num) { return _attributes.at(num); }
    double getValue(int num) { return _attributeValues.at(num); }
    int attributeCount() { return _attributes.count(); }
    int findAttribute(AttributeType champ) { return _attributes.indexOf(champ); }

    // Modulators
    int modulatorCount() { return _modulators.count(); }
    ModulatorData getModulator(int num) { return _modulators.at(num); }

private:
    QList<AttributeType> _attributes;
    QList<double> _attributeValues;
    QList<ModulatorData> _modulators;

    double limit(double val, AttributeType champ);
    void loadAttributes(SoundfontManager *sf2, EltID id);
    void loadModulators(SoundfontManager *sf2, EltID id);
    void addModulator(ModulatorData &modData);
    void getGlobalValue(SoundfontManager * sf2, EltID id, AttributeType champ);
    void mix(AttributeType champCoarse, AttributeType champFine, int addValue = 0);
    void merge(AttributeType attributeType, double value);
    void prepend(AttributeType attributeType);
    void adaptKeynum2();
    void adaptKeynum2(int minKey, int maxKey, AttributeType champBase, AttributeType champKeynum);
    void adaptLfo(SoundfontManager *sf2, EltID idInstSmpl);
    double getValKeynum(double valBase, int key, double keynum);
    void processDescendingVelocityModulators();
};

#endif // SFZPARAMLIST_H
