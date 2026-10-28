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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QObject>
#include "attributetypes.h"
#include "attributebase.h"

// Attribute with the descriptions and the lists of the editor
class Attribute: QObject, public AttributeBase
{
    Q_OBJECT

public:
    Attribute() : AttributeBase() {}
    Attribute(AttributeType champ, bool isPrst) : AttributeBase(champ, isPrst) {}
    Attribute(AttributeType champ, bool isPrst, double value) : AttributeBase(champ, isPrst, value) {}
    Attribute(AttributeType champ, bool isPrst, AttributeValue storedValue) : AttributeBase(champ, isPrst, storedValue) {}

    // Textual description
    static QString getDescription(AttributeType champ, bool isPrst);

    // Modulators
    static QList<AttributeType> getAttributeListForMod(bool isPrst);
    static bool isValidAttributeForMod(AttributeType attribute, bool isPrst);

private:
    static QList<AttributeType> s_attributesForPrstMod;
    static QList<AttributeType> s_attributesForInstMod;
};

#endif // ATTRIBUTE_H
