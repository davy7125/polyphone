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
#include <sndfile.hh>

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
    // Open the file and get the details
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(SF_INFO));
#if defined(Q_OS_WIN)
    // Reopen the file for preventing a bug
    SNDFILE * sndFile = sf_open(_file->fileName().toStdString().c_str(), SFM_READ, &sfInfo);
#else
    SNDFILE * sndFile = sf_open_fd(_file->handle(), SFM_READ, &sfInfo, false);
#endif
    if (sndFile == nullptr)
        return SampleReader::FILE_NOT_SUPPORTED;

    // General info
    _info->wChannels = sfInfo.channels;
    _info->dwSampleRate = sfInfo.samplerate;
    _info->dwLength = sfInfo.frames;
    _info->rawDataAvailable = false; // Maybe later it will be true if we keep OGG data as it it stored

    // Loop / root key / fine tune
    SF_INSTRUMENT instrument;
    if (sf_command(sndFile, SFC_GET_INSTRUMENT, &instrument, sizeof(instrument)) == SF_TRUE)
    {
        _info->dwRootKey = instrument.basenote;
        _info->iFineTune = instrument.detune;
        for (int i = 0; i < instrument.loop_count; i++)
            _info->loops << QPair<quint32, quint32>(instrument.loops[i].start, instrument.loops[i].end);
    }

    if (data16 != nullptr)
    {
        int * interleavedData = new int[_info->wChannels * _info->dwLength];
        sf_readf_int(sndFile, interleavedData, _info->dwLength);

        // Extract a channel
        int iTmp;
        for (quint32 i = 0; i < _info->dwLength; i++)
        {
            iTmp = interleavedData[i * _info->wChannels + _info->wChannel];
            data16[i] = iTmp >> 16;
            data24[i] = (iTmp >> 8) & 0xFF;
        }
        delete [] interleavedData;
    }

    sf_close(sndFile);
    return SampleReader::FILE_OK;
}
