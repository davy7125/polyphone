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

#include "samplereaderogg.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

SampleReaderOgg::SampleReaderOgg(QString filename) : SampleReader(filename),
    _file(nullptr),
    _info(nullptr)
{

}

size_t ogg_read(void* buffer, size_t elementSize, size_t elementCount, void* dataSource)
{
    QFile *file = static_cast<QFile*>(dataSource);
    size_t bytesToRead = elementSize * elementCount;
    return file->read((char *)buffer, bytesToRead);
}

int ogg_seek(void* dataSource, ogg_int64_t offset, int origin)
{
    QFile *file = static_cast<QFile*>(dataSource);

    unsigned long absolute_byte_offset = offset;
    if (origin == 1)
    {
        // Offset relative to the current position
        absolute_byte_offset += file->pos();
    }
    else if (origin == 2)
    {
        // Offset relative to the end of the file
        absolute_byte_offset += file->size();
    }

    if (!file->seek(static_cast<unsigned long>(absolute_byte_offset)))
        return -1;
    return 0;
}

long ogg_tell(void* dataSource)
{
    QFile *file = static_cast<QFile*>(dataSource);
    return file->pos();
}

SampleReaderOgg::SampleReaderResult SampleReaderOgg::getInfo(QFile &fi, InfoSound &info)
{
    // Initialize the info and keep a track of it
    info.reset();
    _info = &info;

    // Public access to the file, no data to store
    _file = &fi;
    _data = nullptr;

    // Decode the file
    return launchDecoder(true);
}

SampleReaderOgg::SampleReaderResult SampleReaderOgg::getData16(QFile &fi, QByteArray &smpl)
{
    // Public access to the file, read data
    _file = &fi;
    _data = &smpl;
    _readExtra8 = false;
    smpl.resize(static_cast<int>(_info->dwLength) * 2);

    // Decode the file
    return launchDecoder(false);
}

SampleReaderOgg::SampleReaderResult SampleReaderOgg::getExtraData24(QFile &fi, QByteArray &sm24)
{
    // Public access to the file, read data
    _file = &fi;
    _data = &sm24;
    _readExtra8 = true;
    sm24.resize(static_cast<int>(_info->dwLength));

    // Decode the file
    return launchDecoder(false);
}

SampleReaderOgg::SampleReaderResult SampleReaderOgg::launchDecoder(bool justMetadata)
{
    OggVorbis_File vf;

    // Callback for operations in the input file
    ov_callbacks callbacks = {
        (size_t (*)(void *, size_t, size_t, void *)) ogg_read,
        (int (*)(void *, ogg_int64_t, int)) ogg_seek,
        nullptr,
        (long (*)(void *)) ogg_tell
    };

    // Open the file
    if (ov_open_callbacks(_file, &vf, NULL, 0, callbacks) < 0)
    {
        qWarning() << "Input does not appear to be an Ogg bitstream";
        return FILE_CORRUPT;
    }

    // Load metadata
    vorbis_info *vi = ov_info(&vf, -1);
    _info->wChannels = vi->channels;
    _info->dwSampleRate = vi->rate;
    _info->dwLength = ov_pcm_total(&vf, -1);
    _info->wBpsFile = 16;

    // Comments (search tags in it?)
    /*char **ptr = ov_comment(&vf,-1)->user_comments;
    while(*ptr)
    {
        fprintf(stderr,"%s\n",*ptr);
        ++ptr;
    }
    fprintf(stderr,"Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);*/

    if (!justMetadata)
    {
        if (_readExtra8)
        {
            _data->fill(0);
        }
        else
        {
            // Load 16-bit data
            int current_section;
            char pcmOut[4096];
            qint32 pos = 0;
            while (pos < _data->size())
            {
                long ret = ov_read(&vf, pcmOut, 4096,
                                   0, // 0: little endian, 1: big endian
                                   2, // word size in bytes
                                   1, // 0: unsigned, 1: signed
                                   &current_section);

                // End of the stream?
                if (ret == 0)
                    break;

                // Error?
                if (ret < 0)
                {
                    ov_clear(&vf);
                    return FILE_CORRUPT;
                }

                // Extract data
                int maxValue = ret / _info->wChannels / 2;
                if (maxValue > (_data->size() - pos) / 2)
                    maxValue = (_data->size() - pos) / 2;
                for (int i = 0; i < maxValue; i++)
                {
                    _data->data()[pos + 2 * i] = pcmOut[2 * (i * _info->wChannels + _info->wChannel)];
                    _data->data()[pos + 2 * i + 1] = pcmOut[2 * (i * _info->wChannels + _info->wChannel) + 1];
                }
                pos += ret / _info->wChannels;
            }

            // Fill the remaining part of _data with 0
            if (pos < _data->size())
                memset(&_data->data()[pos], 0, _data->size() - pos);
        }
    }

    ov_clear(&vf);
    return FILE_OK;
}
