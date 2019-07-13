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

#include "eltid.h"

EltID::EltID(ElementType elementType, int indexSf2, int indexElt, int indexElt2, int indexMod) :
    typeElement(elementType),
    indexSf2(indexSf2),
    indexElt(indexElt),
    indexElt2(indexElt2),
    indexMod(indexMod)
{}
EltID::EltID(EltID * other) :
    typeElement(other->typeElement),
    indexSf2(other->indexSf2),
    indexElt(other->indexElt),
    indexElt2(other->indexElt2),
    indexMod(other->indexMod)
{}
EltID::EltID() :
    typeElement(elementUnknown),
    indexSf2(0),
    indexElt(0),
    indexElt2(0),
    indexMod(0)
{}

EltID EltID::parent(bool includeRoot)
{
    EltID idParent(this);

    switch (typeElement) {
    case elementSmpl:
        idParent.typeElement = (includeRoot ? elementRootSmpl : elementSf2);
        break;
    case elementInst:
        idParent.typeElement = (includeRoot ? elementRootInst : elementSf2);
        break;
    case elementPrst:
        idParent.typeElement = (includeRoot ? elementRootPrst : elementSf2);
        break;
    case elementInstSmpl: case elementInstMod: case elementInstGen:
        idParent.typeElement = elementInst;
        break;
    case elementPrstInst: case elementPrstMod: case elementPrstGen:
        idParent.typeElement = elementPrst;
        break;
    case elementRootSmpl:
    case elementRootInst:
    case elementRootPrst:
        idParent.typeElement = elementSf2;
        break;
    case elementInstSmplMod: case elementInstSmplGen:
        idParent.typeElement = elementInstSmpl;
        break;
    case elementPrstInstMod: case elementPrstInstGen:
        idParent.typeElement = elementPrstInst;
        break;
    case elementSf2:
    case elementUnknown:
        idParent.typeElement = elementUnknown;
        break;
    }

    return idParent;
}

bool EltID::operator==(const EltID &other)
{
    // Check sf2 and element type
    if (this->typeElement != other.typeElement || this->indexSf2 != other.indexSf2 || typeElement == elementUnknown)
        return false;
    if (typeElement == elementSf2 || typeElement == elementRootSmpl || typeElement == elementRootInst || typeElement == elementRootPrst)
        return true;

    // Check smpl / inst / prst
    if (this->indexElt != other.indexElt)
        return false;
    if (typeElement == elementSmpl || typeElement == elementInst || typeElement == elementPrst)
        return true;

    if (typeElement == elementInstMod || typeElement == elementInstGen ||
            typeElement == elementPrstMod || typeElement == elementPrstGen)
    {
        // No need to check indexElt2
        return (this->indexMod == other.indexMod);
    }

    // Check the division
    if (this->indexElt2 != other.indexElt2)
        return false;
    if (typeElement == elementInstSmpl || typeElement == elementPrstInst)
        return true;

    // Check the last index
    return this->indexMod == other.indexMod;
}

bool EltID::operator !=(const EltID &other)
{
    return !((*this) == other);
}

QString EltID::toString()
{
    QString type;
    switch (typeElement)
    {
    case elementSf2: type = "SF2"; break;
    case elementRootSmpl: type = "root SMPL"; break;
    case elementRootInst: type = "root INST"; break;
    case elementRootPrst: type = "root PRST";  break;
    case elementSmpl: type = "SMPL"; break;
    case elementInst: type = "INST"; break;
    case elementPrst: type = "PRST"; break;
    case elementInstSmpl: type = "InstSmpl"; break;
    case elementPrstInst: type = "PrstInst"; break;
    case elementInstMod: type = "InstMod"; break;
    case elementPrstMod: type = "PrstMod"; break;
    case elementInstSmplMod: type = "InstSmplMod"; break;
    case elementPrstInstMod: type = "PrstInstMod"; break;
    case elementInstGen: type = "InstGen"; break;
    case elementPrstGen: type = "PrstGen"; break;
    case elementInstSmplGen: type = "InstSmplGen"; break;
    case elementPrstInstGen: type = "PrstInstGen"; break;
    case elementUnknown: type = "Unknown"; break;
    }
    return "ID [type=" + type + ", sf2=" + QString::number(indexSf2) +
            ", Elt=" + QString::number(indexElt) + ", Elt2=" + QString::number(indexElt2) +
            ", Mod=" + QString::number(indexMod) + "]";
}

bool EltID::isPrst()
{
    return typeElement == elementPrst || typeElement == elementPrstGen || typeElement == elementPrstMod ||
            typeElement == elementPrstInst || typeElement == elementPrstInstGen || typeElement == elementPrstInstMod ||
            typeElement == elementRootPrst;
}
