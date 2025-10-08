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

#include "samplereadersf2.h"
#include "utils.h"

SampleReaderSf2::SampleReaderSf2(QString filename) :
    SampleReader(filename),
    _info(nullptr)
{

}

SampleReader::SampleReaderResult SampleReaderSf2::getInfo(QFile &fi, InfoSound * info)
{
    Q_UNUSED(fi)

    // Info completed outside for an sf2: we keep the pointer
    _info = info;

    // Extra info
    info->wChannel = 0;
    info->wChannels = 1;
    info->pitchDefined = true; // So that we don't try to find the key based on the filename

    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderSf2::getData(QFile &fi, qint16 *& data16, quint8 *& data24)
{
    data16 = nullptr;
    data24 = nullptr;
    if (_info->dwLength == 0)
        return FILE_OK;

    // Load the smpl part of an sf2
    fi.seek(_info->dwStart);
    data16 = new qint16[_info->dwLength];
    qint64 nb = fi.read((char *)data16, _info->dwLength * sizeof(qint16));
    if (nb != (qint64)(_info->dwLength * sizeof(qint16)))
    {
        delete [] data16;
        data16 = nullptr;
        return nb == -1 ? FILE_NOT_READABLE : FILE_CORRUPT;
    }

    // Possibly load the sm24 part of an sf2
    if (_info->wBpsFile >= 24)
    {
        data24 = new quint8[_info->dwLength];
        fi.seek(_info->dwStart2);
        qint64 nb = fi.read((char *)data24, _info->dwLength);
        if (nb != (qint64)_info->dwLength)
        {
            delete [] data16;
            delete [] data24;
            data16 = nullptr;
            data24 = nullptr;
            return nb == -1 ? FILE_NOT_READABLE : FILE_CORRUPT;
        }
    }

    return FILE_OK;
}
