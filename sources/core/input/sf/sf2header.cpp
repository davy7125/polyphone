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

#include "sf2header.h"
#include <QDataStream>
#include "sf2pdtapart.h"
#include "sf2sdtapart.h"

Sf2Header::Sf2Header() :
    _isValid(false),
    _size(0),
    _infoSize(0)
{

}

void Sf2Header::prepareBeforeWritingData(Sf2SdtaPart * sdtaPart, Sf2PdtaPart * pdtaPart, bool isSf3, double qualityValue)
{
    // Constant strings
    memcpy(_RIFF, "RIFF", 4);
    memcpy(_sfbk, "sfbk", 4);
    memcpy(_LIST, "LIST", 4);
    memcpy(_INFO, "INFO", 4);

    // Sizes
    _infoSize.value = 4;
    foreach (QString block, _infos.keys())
    {
        QByteArray data = block == "ICMT" ? _infos[block].toUtf8() : _infos[block].toLatin1();
        _infoSize.value += 8 + data.size() + 1; // Extra zero
        _infoSize.value += _infoSize.value % 2; // Even size: another zero
    }
    _infoSize.value += 12 * _versions.count();

    _size = 12 + _infoSize.value +
            sdtaPart->prepareBeforeWritingData(isSf3, qualityValue) +
            pdtaPart->prepareBeforeWritingData(sdtaPart, isSf3);
    sdtaPart->_position = 20 + _infoSize.value;
}

QDataStream & operator << (QDataStream &out, Sf2Header &header)
{
    // RIFF
    if (out.writeRawData(header._RIFF, 4) != 4)
        return out;

    // Full size - 8
    out << header._size;

    // "sfbk"
    if (out.writeRawData(header._sfbk, 4) != 4)
        return out;

    // "LIST"
    if (out.writeRawData(header._LIST, 4) != 4)
        return out;

    // Size of the section "INFO"
    out << header._infoSize;

    // "INFO"
    if (out.writeRawData(header._INFO, 4) != 4)
        return out;

    quint16Reversed val16Tmp;
    quint32Reversed val32Tmp;
    foreach (QString block, header._versions.keys())
    {
        out.writeRawData(block.toStdString().c_str(), 4);
        val32Tmp.value = 4;
        out << val32Tmp;
        val16Tmp.value = header._versions[block].wMajor;
        out << val16Tmp;
        val16Tmp.value = header._versions[block].wMinor;
        out << val16Tmp;
    }
    foreach (QString block, header._infos.keys())
    {
        QByteArray data = block == "ICMT" ? header._infos[block].toUtf8() : header._infos[block].toLatin1();

        out.writeRawData(block.toStdString().c_str(), 4);
        val32Tmp.value = data.size() + 1;
        if (val32Tmp.value % 2 == 1)
        {
            val32Tmp.value += 1;
            out << val32Tmp;
            if (out.writeRawData(data.constData(), data.length()) != data.length())
                return out;
            out.writeRawData("\0\0", 2);
        }
        else
        {
            out << val32Tmp;
            if (out.writeRawData(data.constData(), data.length()) != data.length())
                return out;
            out.writeRawData("\0", 1);
        }
    }

    header._isValid = true;
    return out;
}
