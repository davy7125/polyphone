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

#ifndef SF2HEADER_H
#define SF2HEADER_H

#include "basetypes.h"
#include "sf2basetypes.h"
#include <QMap>
class QDataStream;

class Sf2Header
{
public:
    Sf2Header();

    QString getInfo(QString key);
    sfVersionTag getVersion(QString key);

    bool _isValid;

    char _RIFF[4]; // Should be "RIFF"
    quint32Reversed _size; // Full size of the file - 8
    char _sfbk[4]; // Should be "sfbk"
    char _LIST[4]; // Should be "LIST"
    quint32Reversed _infoSize; // Size of the section INFO
    char _INFO[4]; // Should be "INFO"
    QMap<QString, QString> _infos;
    QMap<QString, sfVersionTag> _versions;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2Header &header);
//QDataStream & operator << (QDataStream &out, const Sf2Header &header);

#endif // SF2HEADER_H
