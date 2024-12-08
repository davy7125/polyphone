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

SampleReaderSf2::SampleReaderSf2(QString filename) : SampleReader(filename),
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

float * SampleReaderSf2::getData(SampleReaderResult &result, QFile &fi)
{
    if (_info->dwLength == 0)
    {
        result = FILE_OK;
        return nullptr;
    }

    // Sample data
    float * fData = new float[_info->dwLength];

    // Load the smpl part of an sf2
    fi.seek(_info->dwStart);
    qint16 * data = new qint16[_info->dwLength];
    qint64 nb = fi.read((char *)data, _info->dwLength * sizeof(qint16));
    if (nb == -1)
    {
        delete [] data;
        delete [] fData;
        result = FILE_NOT_READABLE;
        return nullptr;
    }
    if (nb != (qint64)(_info->dwLength * sizeof(qint16)))
    {
        delete [] data;
        delete [] fData;
        result = FILE_CORRUPT;
        return nullptr;
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
            delete [] fData;
            result = FILE_NOT_READABLE;
            return nullptr;
        }
        if (nb != _info->dwLength)
        {
            delete [] data;
            delete [] data24;
            delete [] fData;
            result = FILE_CORRUPT;
            return nullptr;
        }
    }
    else
        memset(data24, 0, _info->dwLength);

    // Convert to float between -1 and 1
    qint32 tmp;
    for (quint32 i = 0; i < _info->dwLength; i++)
    {
        tmp = (data[i] << 8) | data24[i];
        if (tmp & 0x800000)
            tmp |= 0xff000000;
        fData[i] = Utils::int24ToFloat(tmp);
    }

    delete [] data;
    delete [] data24;
    result = FILE_OK;
    return fData;
}
