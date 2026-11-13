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

#ifndef KEYBOARDDISPLAYCONTENT_H
#define KEYBOARDDISPLAYCONTENT_H

#include <QString>
#include "basetypes.h"

// Content of the display of the virtual keyboard: element played and navigation among the elements of the current tab
// (and among the files of a soundfont explorer)
struct KeyboardDisplayContent
{
    QString fileName; // File of the element, or name of the soundfont if it has never been saved (possibly empty)
    ElementType elementType = elementUnknown; // elementSmpl, elementInst or elementPrst, elementUnknown if nothing is playable
    QString elementNumber; // "bank:preset" of a preset
    QString elementName;
    bool hasPreviousElement = false;
    bool hasNextElement = false;
    bool hasPreviousFile = false; // File of a soundfont explorer, or editor in the order of the tabs
    bool hasNextFile = false;

    bool operator==(const KeyboardDisplayContent &other) const
    {
        return fileName == other.fileName && elementType == other.elementType && elementNumber == other.elementNumber &&
               elementName == other.elementName && hasPreviousElement == other.hasPreviousElement &&
               hasNextElement == other.hasNextElement && hasPreviousFile == other.hasPreviousFile &&
               hasNextFile == other.hasNextFile;
    }
    bool operator!=(const KeyboardDisplayContent &other) const { return !(*this == other); }
};

#endif // KEYBOARDDISPLAYCONTENT_H
