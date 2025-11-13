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

#include "sf2pdtapart_shdr.h"
#include <QDataStream>

Sf2PdtaPart_shdr::Sf2PdtaPart_shdr() :
    _isValid(false),
    _name(""),
    _start(0),
    _end(0),
    _startLoop(0),
    _endLoop(0),
    _sampleRate(0),
    _originalPitch(0),
    _correction(0)
{

}

QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_shdr &shdr)
{
    // Name
    int length = 20;
    QByteArray buffer(length + 1, Qt::Uninitialized);
    if (in.readRawData(buffer.data(), length) != length)
        return in;
    buffer[20] = 0;
    shdr._name = QString::fromLatin1(buffer.data()).trimmed();

    // Positions
    in >> shdr._start >> shdr._end >> shdr._startLoop >> shdr._endLoop;

    // Pitch
    in >> shdr._sampleRate >> shdr._originalPitch >> shdr._correction;

    // Link
    in >> shdr._wSampleLink >> shdr._sfSampleType;

    shdr._isValid = true;
    return in;
}

QDataStream & operator << (QDataStream &out, Sf2PdtaPart_shdr &shdr)
{
    // Name
    QByteArray data = shdr._name.toLatin1().left(20);
    if (out.writeRawData(data.constData(), data.length()) != data.length())
        return out;
    for (int i = 0; i < 20 - data.length(); i++)
        out.writeRawData("\0", 1);

    // Positions
    out << shdr._start << shdr._end << shdr._startLoop << shdr._endLoop;

    // Pitch
    out << shdr._sampleRate << shdr._originalPitch << shdr._correction;

    // Link
    out << shdr._wSampleLink << shdr._sfSampleType;

    shdr._isValid = true;
    return out;
}
