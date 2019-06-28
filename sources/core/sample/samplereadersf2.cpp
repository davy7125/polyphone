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

#include "samplereadersf2.h"

SampleReaderSf2::SampleReaderSf2(QString filename) : SampleReader(filename),
    _info(nullptr)
{

}

SampleReaderSf2::SampleReaderResult SampleReaderSf2::getInfo(QFile &fi, InfoSound &info)
{
    Q_UNUSED(fi)

    // Info completed outside for an sf2: we keep the pointer
    _info = &info;

    // Extra info
    info.wChannel = 0;
    info.wChannels = 1;
    info.pitchDefined = true; // So that we don't try to find the key based on the filename

    return FILE_OK;
}

SampleReaderSf2::SampleReaderResult SampleReaderSf2::getData16(QFile &fi, QByteArray &smpl)
{
    // Size of the vector
    smpl.resize(static_cast<int>(_info->dwLength) * 2);

    // Load smpl part of an sf2
    if (_info->dwLength > 0)
    {
        // Copy data
        fi.seek(_info->dwStart);
        qint64 nb = fi.read(smpl.data(), _info->dwLength * 2);
        if (nb == -1)
            return FILE_NOT_READABLE;
        if (nb != _info->dwLength * 2)
            return FILE_CORRUPT;
    }
    else
        smpl.fill(0);

    return FILE_OK;
}

SampleReaderSf2::SampleReaderResult SampleReaderSf2::getExtraData24(QFile &fi, QByteArray &sm24)
{
    // Size of the vector
    sm24.resize(static_cast<int>(_info->dwLength));

    // Load sm24 part of an sf2
    if (_info->wBpsFile >= 24 && _info->dwLength > 0)
    {
        // Copy data
        fi.seek(_info->dwStart2);
        qint64 nb = fi.read(sm24.data(), _info->dwLength);
        if (nb == -1)
            return FILE_NOT_READABLE;
        if (nb != _info->dwLength * 2)
            return FILE_CORRUPT;
    }
    else
        sm24.fill(0);

    return FILE_OK;
}
