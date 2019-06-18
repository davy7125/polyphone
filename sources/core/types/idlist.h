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

#ifndef IDLIST_H
#define IDLIST_H

#include <QList>
#include <QMap>
#include "eltid.h"

class IdList : public QList<EltID>
{
public:
    // Constructors
    IdList() : QList<EltID>() {}
    IdList(EltID id) : QList<EltID>() { this->append(id); }

    // Call this function if an element has been added or removed after one of the methods below is used
    void resetComputedLists();

    // Get all possible ids of a specific type based on the current selection
    IdList getSelectedIds(ElementType type);

    // Get the first id of a specific type. Can return unknown if nothing is found
    EltID getFirstId(ElementType type);

    // Return true if the selection is related to only one smpl / inst / prst / sf2
    bool isElementUnique(ElementType type);

    // Return true if the selection is only related to smpl / inst / prst / sf2
    bool areAllWithType(ElementType type);

    // For instruments and presets only, return true if all elements comprises divisions (InstSmpl or PrstInst)
    bool areAllWithDivisions(ElementType type);

    // For instrument and preset only, return true if all their divisions have a range specified
    bool areAllWithRange(ElementType type);

    // True if all elements have the same type
    bool sameType();

private:
    QMap<ElementType, IdList > _computedLists;
};

#endif // IDLIST_H
