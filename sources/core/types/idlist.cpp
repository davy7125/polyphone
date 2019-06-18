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

#include "idlist.h"
#include "soundfontmanager.h"

void IdList::resetComputedLists()
{
    _computedLists.clear();
}

IdList IdList::getSelectedIds(ElementType type)
{
    // Already computed?
    if (!_computedLists.keys().contains(type))
    {
        _computedLists[type] = IdList();

        switch (type) {
        case elementSf2: {
            // Get all possible sf2
            QList<int> indexList;
            for (int i = 0; i < this->count(); i++)
            {
                const EltID id = this->at(i);
                if (id.indexSf2 != -1 && !indexList.contains(id.indexSf2))
                {
                    indexList.append(id.indexSf2);
                    _computedLists[type].append(EltID(elementSf2, id.indexSf2, -1, -1, -1));
                }
            }
        } break;
        case elementSmpl: {
            // Get all possible smpl
            QMap<int, QList<int> > indexList;
            for (int i = 0; i < this->count(); i++)
            {
                const EltID id = this->at(i);
                if (id.indexSf2 != -1 && id.typeElement == elementSmpl && id.indexElt != -1 &&
                        (!indexList.contains(id.indexSf2) || !indexList[id.indexSf2].contains(id.indexElt)))
                {
                    indexList[id.indexSf2].append(id.indexElt);
                    _computedLists[type].append(EltID(elementSmpl, id.indexSf2, id.indexElt, -1, -1));
                }
            }
        } break;
        case elementInst: {
            // Get all possible inst
            QMap<int, QList<int> > indexList;
            for (int i = 0; i < this->count(); i++)
            {
                const EltID id = this->at(i);
                if (id.indexSf2 != -1 && (id.typeElement == elementInst || id.typeElement == elementInstSmpl) && id.indexElt != -1 &&
                        (!indexList.contains(id.indexSf2) || !indexList[id.indexSf2].contains(id.indexElt)))
                {
                    indexList[id.indexSf2].append(id.indexElt);
                    _computedLists[type].append(EltID(elementInst, id.indexSf2, id.indexElt, -1, -1));
                }
            }
        } break;
        case elementPrst: {
            // Get all possible prst
            QMap<int, QList<int> > indexList;
            for (int i = 0; i < this->count(); i++)
            {
                const EltID id = this->at(i);
                if (id.indexSf2 != -1 && (id.typeElement == elementPrst || id.typeElement == elementPrstInst) && id.indexElt != -1 &&
                        (!indexList.contains(id.indexSf2) || !indexList[id.indexSf2].contains(id.indexElt)))
                {
                    indexList[id.indexSf2].append(id.indexElt);
                    _computedLists[type].append(EltID(elementPrst, id.indexSf2, id.indexElt, -1, -1));
                }
            }
        } break;
        case elementInstSmpl: case elementPrstInst: {
            // Get all possible InstSmpl
            QMap<int, QMap<int, QList<int> > > indexList;
            for (int i = 0; i < this->count(); i++)
            {
                const EltID id = this->at(i);
                if (id.indexSf2 != -1 && id.typeElement == type && id.indexElt != -1 && id.indexElt2 != 1 &&
                        (!indexList.contains(id.indexSf2) || !indexList[id.indexSf2].contains(id.indexElt) || !indexList[id.indexSf2][id.indexElt].contains(id.indexElt2)))
                {
                    indexList[id.indexSf2][id.indexElt].append(id.indexElt2);
                    _computedLists[type].append(EltID(type, id.indexSf2, id.indexElt, id.indexElt2, -1));
                }
            }
        } break;
        default:
            break;
        }
    }

    return _computedLists[type];
}

EltID IdList::getFirstId(ElementType type)
{
    QList<EltID> ids = this->getSelectedIds(type);
    if (ids.empty())
        return EltID();
    return ids[0];
}

bool IdList::isElementUnique(ElementType type)
{
    return this->getSelectedIds(type).count() == 1;
}

bool IdList::areAllWithType(ElementType type)
{
    switch (type)
    {
    case elementSmpl:
        for (int i = 0; i < this->count(); i++)
            if (this->at(i).typeElement != elementSmpl)
                return false;
        break;
    case elementInst:
        for (int i = 0; i < this->count(); i++)
            if (this->at(i).typeElement != elementInst && this->at(i).typeElement != elementInstSmpl)
                return false;
        break;
    case elementPrst:
        for (int i = 0; i < this->count(); i++)
            if (this->at(i).typeElement != elementPrst && this->at(i).typeElement != elementPrstInst)
                return false;
        break;
    case elementSf2:
        for (int i = 0; i < this->count(); i++)
            if (this->at(i).typeElement != elementSf2)
                return false;
        break;
    default:
        return false;
    }

    return !this->empty();
}

bool IdList::areAllWithDivisions(ElementType type)
{
    if (type != elementInst && type != elementPrst)
        return false;

    // Check all elements
    QList<EltID> ids = this->getSelectedIds(type);
    foreach (EltID id, ids)
    {
        EltID idTmp = id;
        idTmp.typeElement = (type == elementPrst) ? elementPrstInst : elementInstSmpl;
        if (SoundfontManager::getInstance()->getSiblings(idTmp).empty())
            return false;
    }

    return true;
}

bool IdList::areAllWithRange(ElementType type)
{
    if (type != elementInst && type != elementPrst)
        return false;

    QList<EltID> ids = this->getSelectedIds(type);
    foreach (EltID id, ids)
    {
        EltID idTmp = id;
        idTmp.typeElement = (type == elementPrst) ? elementPrstInst : elementInstSmpl;

        foreach (int i, SoundfontManager::getInstance()->getSiblings(idTmp))
        {
            idTmp.indexElt2 = i;
            if (!SoundfontManager::getInstance()->isSet(idTmp, champ_keyRange))
                return false;
        }
    }

    return true;
}

bool IdList::sameType()
{
    ElementType type = elementUnknown;
    for (int i = 0; i < this->count(); i++)
    {
        if (i == 0)
            type = this->at(i).typeElement;
        else if (type != this->at(i).typeElement)
            return false;
    }
    return true;
}
