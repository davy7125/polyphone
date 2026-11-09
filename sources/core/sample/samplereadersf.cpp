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


#include "samplereadersf.h"
#include "sf2filesource.h"

SampleReaderSf::SampleReaderSf(QString filename) :
    SampleReader(filename),
    _info(nullptr)
{

}

SampleReader::SampleReaderResult SampleReaderSf::getInfo(QFile &fi, InfoSound* info)
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

SampleReader::SampleReaderResult SampleReaderSf::getRawData(QFile &fi, char* &rawData, quint32 &length)
{
    // Read a compressed sample in a sf3 soundfont
    rawData = nullptr;
    length = 0;
    if (_info->dwLength == 0)
        return FILE_OK;

    Sf2FileSource source(fi);
    rawData = Sf2Reader::readCompressedData(source, _info->dwStart, _info->dwLength);
    if (rawData == nullptr)
        return FILE_CORRUPT;
    length = _info->dwLength;

    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderSf::getData(QFile &fi, qint16* &data16, quint8* &data24, const char* rawData, quint32 rawDataLength)
{
    data16 = nullptr;
    data24 = nullptr;
    if (_info->dwLength == 0)
        return FILE_OK;

    if (rawData == nullptr)
    {
        // 16-bit data, and the 24-bit part of an sf2
        Sf2FileSource source(fi);
        Sf2Reader::readData(source, _info->dwStart, _info->wBpsFile >= 24 ? _info->dwStart2 : 0, _info->dwLength,
                            data16, data24);
        return data16 == nullptr ? FILE_CORRUPT : FILE_OK;
    }

    // Data of a compressed sample, whose length is then known
    quint32 length;
    data16 = Sf2Reader::decompress(rawData, rawDataLength, length);
    if (data16 == nullptr)
        return FILE_CORRUPT;
    _info->dwLength = length;

    return FILE_OK;
}
