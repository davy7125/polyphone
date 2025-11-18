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
#include "utils.h"
#include "sf2pdtapart.h"
#include "sf2sdtapart.h"

Sf2Header::Sf2Header() :
    _isValid(false),
    _size(0),
    _infoSize(0)
{

}

QString Sf2Header::getInfo(QString key)
{
    if (_infos.contains(key))
        return _infos[key];
    return "";
}

SfVersionTag Sf2Header::getVersion(QString key)
{
    if (_versions.contains(key))
        return _versions[key];
    SfVersionTag version;
    version.wMajor = 0;
    version.wMinor = 0;
    return version;
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

QDataStream & operator >> (QDataStream &in, Sf2Header &header)
{
    // 4 char, should be "RIFF"
    if (in.readRawData(header._RIFF, 4) != 4)
        return in;

    // Full size - 8
    in >> header._size;

    // 4 char, should be "sfbk"
    if (in.readRawData(header._sfbk, 4) != 4)
        return in;

    // 4 char, should be "LIST"
    if (in.readRawData(header._LIST, 4) != 4)
        return in;

    // Size of the section "INFO"
    in >> header._infoSize;

    // 4 char, should be "INFO"
    if (in.readRawData(header._INFO, 4) != 4)
        return in;

    uint pos = 4;
    while (pos < header._infoSize.value)
    {
        // Bloc name
        char blockName[4];
        if (in.readRawData(blockName, 4) != 4)
            return in;
        QString block = QString::fromLatin1(blockName, 4);

        // Number of bytes to read
        quint32Reversed size;
        in >> size;
        if (pos + 8 + size.value > header._infoSize.value)
            return in;

        if (block == "ifil" || block == "iver")
        {
            // Read a version
            quint16Reversed valTmp;
            in >> valTmp;
            header._versions[block].wMajor = valTmp.value;
            in >> valTmp;
            header._versions[block].wMinor = valTmp.value;
        }
        else
        {
            // Read a text
            int length = size.value;
            QByteArray buffer(length, Qt::Uninitialized);
            if (in.readRawData(buffer.data(), length) != length)
                return in;

            // Keep only one '\0' at the end
            int count = 0;
            while (count < length && buffer[length - 1 - count] == '\0')
                count++;
            if (count > 1)
                buffer.resize(length - count + 1);

            // Truncate if '\0' is found
            int nullIndex = buffer.indexOf('\0');
            if (nullIndex != -1)
                buffer.truncate(nullIndex);

            if (block == "ICMT")
            {
                if (Utils::isValidUtf8(buffer))
                    header._infos[block] = QString::fromUtf8(buffer);
                else
                    header._infos[block] = QString::fromLatin1(buffer);
            }
            else
                header._infos[block] = QString::fromLatin1(buffer);
        }

        pos += 8 + size.value;
    }

    header._isValid = true;
    return in;
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
