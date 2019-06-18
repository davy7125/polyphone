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

#include "sf2pdtapart_inst.h"
#include <QDataStream>

Sf2PdtaPart_inst::Sf2PdtaPart_inst() :
    _isValid(false),
    _iBagIndex(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_inst &inst)
{
    // Name
    int length = 20;
    QByteArray buffer(length, Qt::Uninitialized);
    if (in.readRawData(buffer.data(), length) != length)
        return in;
    inst._name = QString::fromLatin1(buffer);

    // Min index for pBag
    in >> inst._iBagIndex;

    inst._isValid = true;
    return in;
}
