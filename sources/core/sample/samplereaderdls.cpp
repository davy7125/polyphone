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

#include "samplereaderdls.h"
#include "sf2filesource.h"
#include "dlsreader.h"

SampleReaderDls::SampleReaderDls(QString filename) : SampleReader(filename),
    _info(nullptr)
{

}

SampleReader::SampleReaderResult SampleReaderDls::getInfo(QFile &fi, InfoSound * info)
{
    Q_UNUSED(fi)

    // Info completed outside for a DLS file: we keep the pointer
    _info = info;
    info->pitchDefined = true; // So that we don't try to find the key based on the filename

    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderDls::getRawData(QFile &fi, char* &rawData, quint32 &length)
{
    Q_UNUSED(fi)
    rawData = nullptr;
    length = 0;
    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderDls::getData(QFile &fi, qint16* &data16, quint8* &data24, const char* rawData, quint32 rawDataLength)
{
    Q_UNUSED(rawData)
    Q_UNUSED(rawDataLength)

    Sf2FileSource source(fi);
    int16_t * data16Read;
    uint8_t * data24Read;
    uint32_t length;
    DlsReader::WaveResult result = DlsReader::readWaveData(source, _info->dwStart, _info->wChannel,
                                                           data16Read, data24Read, length);
    data16 = data16Read;
    data24 = data24Read;
    if (result == DlsReader::WAVE_CORRUPT)
        return FILE_CORRUPT;
    if (result == DlsReader::WAVE_NOT_SUPPORTED)
        return FILE_NOT_SUPPORTED;
    _info->dwLength = length;
    return FILE_OK;
}
