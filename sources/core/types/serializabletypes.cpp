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

#include "serializabletypes.h"
#include <QDataStream>

QDataStream & operator >> (QDataStream &in, quint32Reversed &val)
{
    quint8 b0, b1, b2, b3;
    in >> b0 >> b1 >> b2 >> b3;
    val.value = static_cast<quint32>(b3) << 24 | static_cast<quint32>(b2) << 16 |
                static_cast<quint32>(b1) << 8 | static_cast<quint32>(b0);
    return in;
}

QDataStream & operator << (QDataStream &out, quint32Reversed &val)
{
    quint8 b0 = val.value & 0xFF;
    quint8 b1 = (val.value >> 8) & 0xFF;
    quint8 b2 = (val.value >> 16) & 0xFF;
    quint8 b3 = (val.value >> 24) & 0xFF;
    out << b0 << b1 << b2 << b3;
    return out;
}

QDataStream & operator >> (QDataStream &in, quint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = static_cast<quint16>(b1) << 8 | static_cast<quint16>(b0);
    return in;
}

QDataStream & operator << (QDataStream &out, quint16Reversed &val)
{
    quint8 b0 = val.value & 0xFF;
    quint8 b1 = (val.value >> 8) & 0xFF;
    out << b0 << b1;
    return out;
}

QDataStream & operator >> (QDataStream &in, qint16Reversed &val)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    val.value = static_cast<qint16>(
        (static_cast<quint16>(b1) << 8) | b0
        );
    return in;
}

QDataStream & operator << (QDataStream &out, qint16Reversed &val)
{
    quint8 b0 = static_cast<quint16>(val.value) & 0xFF;
    quint8 b1 = (static_cast<quint16>(val.value) >> 8) & 0xFF;
    out << b0 << b1;
    return out;
}

QDataStream & operator >> (QDataStream &in, SFModulator &mod)
{
    quint8 b0, b1;
    in >> b0 >> b1;
    mod.Type = static_cast<ModType>(b1 >> 2);
    mod.isBipolar = ((b1 >> 1) & 0x01) > 0;
    mod.isDescending = (b1 & 0x01) > 0;
    mod.CC = (b0 >> 7) > 0;
    mod.Index = (b0 & 0x7F);
    return in;
}

QDataStream & operator << (QDataStream &out, SFModulator &mod)
{
    quint8 b0 = ((mod.CC ? 1 : 0) << 7) + mod.Index;
    quint8 b1 = (mod.Type << 2) + ((mod.isBipolar ? 1 : 0) << 1) + (mod.isDescending ? 1 : 0);
    out << b0 << b1;
    return out;
}
