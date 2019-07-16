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


#include "serializabletypes.h"
#include <QDataStream>

QDataStream & operator >> (QDataStream &in, quint32Reversed &val)
{
    quint8 b0, b1, b2, b3;
    in >> b0 >> b1 >> b2 >> b3;
    val.value = b3 << 24 | b2 << 16 | b1 << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, quint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = b1 << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, qint32Reversed &val)
{
    quint8 b0, b1, b2, b3;
    in >> b0 >> b1 >> b2 >> b3;
    val.value = ((short) b3) << 24 | b2 << 16 | b1 << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, qint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = ((short) b1) << 8 | b0;
    return in;
}

QDataStream & operator >> (QDataStream &in, SFModulator &mod)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    mod.Type = static_cast<ModType>(b1 >> 2);
    mod.isBipolar = ((b1 >> 1) & 1);
    mod.isDescending = (b1 & 1);
    mod.CC = bool(b0 >> 7);
    mod.Index = quint16(b0 & 0x7F);
    return in;
}
