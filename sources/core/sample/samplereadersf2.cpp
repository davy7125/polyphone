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

SampleReaderSf2::SampleReaderResult SampleReaderSf2::getData(QFile &fi, QVector<float> &smpl)
{
    if (_info->dwLength == 0)
        return FILE_OK;

    // Size of the vector
    smpl.resize(_info->dwLength);
    float * fData = smpl.data();

    // Load the smpl part of an sf2
    fi.seek(_info->dwStart);
    quint16 * data = new quint16[_info->dwLength];
    qint64 nb = fi.read((char *)data, _info->dwLength * 2);
    if (nb == -1)
    {
        delete [] data;
        return FILE_NOT_READABLE;
    }
    if (nb != _info->dwLength * 2)
    {
        delete [] data;
        return FILE_CORRUPT;
    }

    // Possibly load the sm24 part of an sf2
    unsigned char * data24 = new unsigned char[_info->dwLength];
    if (_info->wBpsFile >= 24 && _info->dwLength > 0)
    {
        // Copy data
        fi.seek(_info->dwStart2);
        qint64 nb = fi.read((char *)data24, _info->dwLength);
        if (nb == -1)
        {
            delete [] data;
            delete [] data24;
            return FILE_NOT_READABLE;
        }
        if (nb != _info->dwLength)
        {
            delete [] data;
            delete [] data24;
            return FILE_CORRUPT;
        }
    }
    else
        memset(data24, 0, _info->dwLength);

    // Convert to float between -1 and 1
    int tmp;
    float coeff = 1.0f / (0.5f + 0x7FFFFF);
    for (quint32 i = 0; i < _info->dwLength; i++)
    {
        tmp = (data[i] << 8) | data24[i];
        if (tmp & 0x800000)
            tmp |= ~0xffffff;
        fData[i] = (0.5f + tmp) * coeff;
    }

    delete [] data;
    delete [] data24;
    return FILE_OK;
}
