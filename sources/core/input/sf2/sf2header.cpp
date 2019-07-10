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

#include "sf2header.h"
#include <QDataStream>

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
        char blocName[4];
        if (in.readRawData(blocName, 4) != 4)
            return in;
        QString bloc = QString::fromLatin1(blocName, 4);

        // Number of bytes to read
        quint32Reversed size;
        in >> size;
        if (pos + 8 + size.value > header._infoSize.value)
            return in;

        if (bloc == "ifil" || bloc == "iver")
        {
            // Read a version
            quint16Reversed valTmp;
            in >> valTmp;
            header._versions[bloc].wMajor = valTmp.value;
            in >> valTmp;
            header._versions[bloc].wMinor = valTmp.value;
        }
        else
        {
            // Read a text
            int length = size.value;
            QByteArray buffer(length, Qt::Uninitialized);
            if (in.readRawData(buffer.data(), length) != length)
                return in;
            header._infos[bloc] = QString::fromLatin1(buffer);
        }

        pos += 8 + size.value;
    }

    header._isValid = true;
    return in;
}
