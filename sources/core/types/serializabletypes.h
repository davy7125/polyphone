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

#ifndef SERIALIZABLETYPES_H
#define SERIALIZABLETYPES_H

#include "qglobal.h"
#include "attribute.h"
class QDataStream;

class quint32Reversed
{
public:
    quint32Reversed(quint32 val = 0) : value(val) {}
    quint32 value;
};
QDataStream & operator >> (QDataStream &in, quint32Reversed &val);

class quint16Reversed
{
public:
    quint16Reversed(quint16 val = 0) : value(val) {}
    quint16 value;
};
QDataStream & operator >> (QDataStream &in, quint16Reversed &val);

class qint32Reversed
{
public:
    qint32Reversed(qint32 val = 0) : value(val) {}
    qint32 value;
};
QDataStream & operator >> (QDataStream &in, qint32Reversed &val);

class qint16Reversed
{
public:
    qint16Reversed(qint16 val = 0) : value(val) {}
    qint16 value;
};
QDataStream & operator >> (QDataStream &in, qint16Reversed &val);

QDataStream & operator >> (QDataStream &in, SFModulator &mod);

#endif // SERIALIZABLETYPES_H
