/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#ifndef SF2FILESOURCE_H
#define SF2FILESOURCE_H

#include <QFile>
#include "sf2reader.h"

// Bytes of a file opened by the editor, read by Sf2Reader
class Sf2FileSource: public Sf2Reader::Source
{
public:
    explicit Sf2FileSource(QFile &file) : _file(file) {}

    uint64_t getLength() override
    {
        qint64 size = _file.size();
        return size > 0 ? static_cast<uint64_t>(size) : 0;
    }

    bool readAt(uint64_t position, void * data, uint64_t size) override
    {
        uint64_t length = getLength();
        if (position > length || size > length - position)
            return false;
        if (size == 0)
            return true;
        if (_file.pos() != static_cast<qint64>(position) && !_file.seek(static_cast<qint64>(position)))
            return false;
        return _file.read(static_cast<char *>(data), static_cast<qint64>(size)) == static_cast<qint64>(size);
    }

private:
    QFile &_file;
};

#endif // SF2FILESOURCE_H
