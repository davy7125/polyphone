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
#include <QDebug>

SampleReaderFile::SampleReaderFile(QString filename) : SampleReader(filename),
    _file(nullptr),
    _info(nullptr)
{

}

SampleReader::SampleReaderResult SampleReaderFile::getInfo(QFile &fi, InfoSound * info)
{
    // Initialize the info and keep a track of it
    info->reset();
    _info = info;

    // Access to the file
    _file = &fi;

    // Decode the file
    return launchDecoder(nullptr);
}

float * SampleReaderFile::getData(SampleReaderResult &result, QFile &fi)
{
    // Access to the file
    _file = &fi;

    // Decode the file and read data
    float * data = new float[_info->dwLength];
    result = launchDecoder(data);
    return data;
}

SampleReader::SampleReaderResult SampleReaderFile::launchDecoder(float * data)
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

    // Loop / root key / fine tune
    SF_INSTRUMENT instrument;
    if (sf_command(sndFile, SFC_GET_INSTRUMENT, &instrument, sizeof(instrument)) == SF_TRUE)
    {
        _info->dwRootKey = instrument.basenote;
        _info->iFineTune = instrument.detune;
        for (int i = 0; i < instrument.loop_count; i++)
            _info->loops << QPair<quint32, quint32>(instrument.loops[i].start, instrument.loops[i].end);
    }

    if (data != nullptr)
    {
        float * interleavedData = new float[_info->wChannels * _info->dwLength];
        sf_readf_float(sndFile, interleavedData, _info->dwLength);

        // Extract a channel
        for (quint32 i = 0; i < _info->dwLength; i++)
            data[i] = interleavedData[i * _info->wChannels + _info->wChannel];
        delete [] interleavedData;
    }

    sf_close(sndFile);
    return SampleReader::FILE_OK;
}
