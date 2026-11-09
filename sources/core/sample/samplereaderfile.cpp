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

#include "samplereaderfile.h"
#include "samplefilereader.h"

SampleReaderFile::SampleReaderFile(QString filename) : SampleReader(filename),
    _file(nullptr),
    _info(nullptr)
{

}

SampleReader::SampleReaderResult SampleReaderFile::getInfo(QFile &fi, InfoSound *info)
{
    // Initialize the info and keep a track of it
    info->reset();
    _info = info;

    // Access to the file
    _file = &fi;

    // Decode the file
    return launchDecoder(nullptr, nullptr);
}

SampleReader::SampleReaderResult SampleReaderFile::getRawData(QFile &fi, char* &rawData, quint32 &length)
{
    Q_UNUSED(fi)
    rawData = nullptr;
    length = 0;
    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderFile::getData(QFile &fi, qint16* &data16, quint8* &data24, const char* rawData, quint32 rawDataLength)
{
    Q_UNUSED(rawData)
    Q_UNUSED(rawDataLength)

    // Access to the file
    _file = &fi;

    // Decode the file and read data
    data16 = new qint16[_info->dwLength];
    data24 = new quint8[_info->dwLength];
    return launchDecoder(data16, data24);
}

SampleReader::SampleReaderResult SampleReaderFile::launchDecoder(qint16* data16, quint8* data24)
{
    // Open the file and get the details (SampleFileReader, shared with the plugin)
#if defined(Q_OS_WIN)
    // Reopen the file for preventing a bug
    std::unique_ptr<SampleFileReader> reader = SampleFileReader::open(std::filesystem::path(_file->fileName().toStdWString()));
#else
    std::unique_ptr<SampleFileReader> reader = SampleFileReader::openDescriptor(_file->handle());
#endif
    if (reader == nullptr)
        return SampleReader::FILE_NOT_SUPPORTED;

    // General info, loop / root key / fine tune
    SampleFileReader::Info info;
    reader->getInfo(info);
    _info->wChannels = info.channels;
    _info->dwSampleRate = info.sampleRate;
    _info->dwLength = info.length;
    _info->rawDataAvailable = false; // Maybe later it will be true if we keep OGG data as it it stored
    if (info.hasInstrument)
    {
        _info->dwRootKey = info.rootKey;
        _info->iFineTune = info.fineTune;
        for (const std::pair<uint32_t, uint32_t> &loop : info.loops)
            _info->loops << QPair<quint32, quint32>(loop.first, loop.second);
    }

    // Data of a channel
    if (data16 != nullptr)
        reader->readChannel(_info->wChannel, _info->dwLength, data16, data24);

    return SampleReader::FILE_OK;
}
