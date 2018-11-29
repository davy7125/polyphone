/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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

#ifndef ELTID_H
#define ELTID_H

#include <QMetaType>
#include <QString>

typedef enum
{
    elementUnknown = -1,
    elementSf2 = 0,
    elementSmpl = 1,
    elementInst = 2,
    elementPrst = 3,
    elementInstSmpl = 4,
    elementPrstInst = 5,
    elementRootSmpl = 6,
    elementRootInst = 7,
    elementRootPrst = 8,
    elementInstMod = 9,
    elementPrstMod = 10,
    elementInstSmplMod = 11,
    elementPrstInstMod = 12,
    elementInstGen = 13,
    elementPrstGen = 14,
    elementInstSmplGen = 15,
    elementPrstInstGen = 16
} ElementType;

class EltID
{
public:
    /// Constructors
    EltID(ElementType elementType, int indexSf2 = -1, int indexElt = -1, int indexElt2 = -1, int indexMod = -1);
    EltID(EltID * other);
    EltID();

    /// Parent of the ID, possibly including elementRootSmpl / elementRootInst / elementRootPrst (elementSf2 otherwise)
    EltID parent(bool includeRoot = false);

    /// Operator for comparison
    bool operator !=(const EltID &other);
    bool operator==(const EltID &other);

    /// Textual description of the ID (for debug purposes)
    QString toString();

    /// Public access to the id keys
    ElementType typeElement;
    int indexSf2;
    int indexElt;
    int indexElt2;
    int indexMod;
};

Q_DECLARE_METATYPE(EltID)

#endif // ELTID_H
