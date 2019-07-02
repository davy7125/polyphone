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

#include "samplereaderflac.h"
#include "stream_decoder.h"

// https://xiph.org/flac/api/group__flac__stream__decoder.html

FLAC__StreamDecoderReadStatus readCallback(const FLAC__StreamDecoder *decoder,
                                           FLAC__byte buffer[],
                                           size_t *bytes,
                                           void * userData)
{
    Q_UNUSED(decoder)

    QFile *file = static_cast<SampleReaderFlac*>(userData)->_file;
    if (*bytes > 0) {
        *bytes = file->read((char *)buffer, *bytes * sizeof(FLAC__byte));
        return (*bytes == 0) ? FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM :
                               FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
    else
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
}

FLAC__StreamDecoderSeekStatus seekCallback(const FLAC__StreamDecoder *decoder,
                                           FLAC__uint64 absolute_byte_offset,
                                           void * userData)
{
    Q_UNUSED(decoder)

    QFile *file = static_cast<SampleReaderFlac*>(userData)->_file;
    if (!file->seek(static_cast<unsigned long>(absolute_byte_offset)))
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus tellCallback(const FLAC__StreamDecoder *decoder,
                                           FLAC__uint64 *absolute_byte_offset,
                                           void * userData)
{
    Q_UNUSED(decoder)

    QFile *file = static_cast<SampleReaderFlac*>(userData)->_file;
    *absolute_byte_offset = (FLAC__uint64)file->pos();
    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus lengthCallback(const FLAC__StreamDecoder *decoder,
                                               FLAC__uint64 *stream_length,
                                               void * userData)
{
    Q_UNUSED(decoder)

    QFile *file = static_cast<SampleReaderFlac*>(userData)->_file;
    *stream_length = static_cast<FLAC__uint64>(file->size());
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool eofCallback(const FLAC__StreamDecoder *decoder,
                       void * userData)
{
    Q_UNUSED(decoder)

    QFile *file = static_cast<SampleReaderFlac*>(userData)->_file;
    return file->atEnd();
}

FLAC__StreamDecoderWriteStatus writeCallback(const FLAC__StreamDecoder * decoder,
                                             const FLAC__Frame *frame,
                                             const FLAC__int32 * const buffer[],
                                             void * userData)
{
    Q_UNUSED(decoder)

    // Initialize variables
    InfoSound * info = static_cast<SampleReaderFlac*>(userData)->_info;
    QByteArray * data = static_cast<SampleReaderFlac*>(userData)->_data;
    quint32 minPosition = static_cast<SampleReaderFlac*>(userData)->_pos;
    quint32 maxPosition = minPosition + frame->header.blocksize;
    bool readExtra8 = static_cast<SampleReaderFlac*>(userData)->_readExtra8;
    bool channel = info->wChannel;

    // Possibly return if nothing is to be stored
    if (data == nullptr)
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

    // Extract data depending on bps
    switch (info->wBpsFile / 8)
    {
    case 1:
        // 8-bit samples
        if (readExtra8)
            for (quint32 i = minPosition; i < maxPosition; i++)
                (*data)[i] = 0;
        else
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[2 * i + 1] = static_cast<char>(value & 0xff);
                (*data)[2 * i] = 0;
            }
        }
        break;
    case 2:
        // 16-bit samples
        if (readExtra8)
            for (quint32 i = minPosition; i < maxPosition; i++)
                (*data)[i] = 0;
        else
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[2 * i + 1] = static_cast<char>((value >> 8) & 0xff);
                (*data)[2 * i] = static_cast<char>(value & 0xff);
            }
        }
        break;
    case 3:
        // 24-bit samples
        if (readExtra8)
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[i] = static_cast<char>(value & 0xff);
            }
        }
        else
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[2 * i + 1] = static_cast<char>((value >> 16) & 0xff);
                (*data)[2 * i] = static_cast<char>((value >> 8) & 0xff);
            }
        }
        break;
    case 4:
        // 32-bit samples
        if (readExtra8)
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[i] = static_cast<char>((value >> 8) & 0xff);
            }
        }
        else
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                qint32 value = buffer[channel][i - minPosition];
                (*data)[2 * i + 1] = static_cast<char>((value >> 24) & 0xff);
                (*data)[2 * i] = static_cast<char>((value >> 16) & 0xff);
            }
        }
        break;
    default:
        // Exotic samples?
        if (readExtra8)
            for (quint32 i = minPosition; i < maxPosition; i++)
                (*data)[i] = 0;
        else
        {
            for (quint32 i = minPosition; i < maxPosition; i++)
            {
                (*data)[2 * i + 1] = 0;
                (*data)[2 * i] = 0;
            }
        }
        break;
    }

    // Update the position
    static_cast<SampleReaderFlac*>(userData)->_pos = maxPosition;

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void metadataCallback(const FLAC__StreamDecoder * decoder, const FLAC__StreamMetadata *metadata, void * userData)
{
    Q_UNUSED(decoder)

    InfoSound * info = static_cast<SampleReaderFlac*>(userData)->_info;

    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
        info->wChannels = static_cast<quint16>(metadata->data.stream_info.channels);
        info->dwSampleRate = metadata->data.stream_info.sample_rate;
        info->dwLength = static_cast<quint32>(metadata->data.stream_info.total_samples);
        info->wBpsFile = static_cast<quint16>(metadata->data.stream_info.bits_per_sample);
    }
    else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
    {
        for (unsigned int i = 0; i < metadata->data.vorbis_comment.num_comments; i++)
        {
            FLAC__byte * comment = metadata->data.vorbis_comment.comments[i].entry;
            unsigned int length = metadata->data.vorbis_comment.comments[i].length;
            QString str(reinterpret_cast<char *>(comment));
            qDebug() << "Flac comment:" << str << "size:" << length;

            QStringList split = str.split("=");
            if (split.size() != 2)
                continue;

            QString key = split[0];
            QString value = split[1];

            if (key == "LOOP_START")
            {
                bool ok = false;
                unsigned int num = value.toUInt(&ok);
                if (ok)
                    info->dwStartLoop = num;
            }
            else if (key == "LOOP_END")
            {
                bool ok = false;
                unsigned int num = value.toUInt(&ok);
                if (ok)
                    info->dwEndLoop = num;
            }

            // Fine tune? root key? pitch defined?
        }
    }
}

void errorCallback(const FLAC__StreamDecoder * decoder, FLAC__StreamDecoderErrorStatus status, void * userData)
{
    Q_UNUSED(decoder)
    Q_UNUSED(status)
    Q_UNUSED(userData)

    //qDebug() << "errorCallback" << status;
}

SampleReaderFlac::SampleReaderFlac(QString filename) : SampleReader(filename),
    _file(nullptr),
    _info(nullptr),
    _pos(0)
{

}

SampleReaderFlac::SampleReaderResult SampleReaderFlac::getInfo(QFile &fi, InfoSound &info)
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

SampleReaderFlac::SampleReaderResult SampleReaderFlac::getData16(QFile &fi, QByteArray &smpl)
{
    // Public access to the file, read data
    _file = &fi;
    _data = &smpl;
    _readExtra8 = false;
    smpl.resize(static_cast<int>(_info->dwLength) * 2);
    _pos = 0;

    // Decode the file
    return launchDecoder(false);
}

SampleReaderFlac::SampleReaderResult SampleReaderFlac::getExtraData24(QFile &fi, QByteArray &sm24)
{
    // Public access to the file, read data
    _file = &fi;
    _data = &sm24;
    _readExtra8 = true;
    sm24.resize(static_cast<int>(_info->dwLength));
    _pos = 0;

    // Decode the file
    return launchDecoder(false);
}

SampleReaderFlac::SampleReaderResult SampleReaderFlac::launchDecoder(bool justMetadata)
{
    // Initialize a FLAC decoder
    FLAC__StreamDecoder * decoder = FLAC__stream_decoder_new();
    if (FLAC__stream_decoder_init_stream (decoder,
                                          &readCallback, &seekCallback,
                                          &tellCallback, &lengthCallback, &eofCallback,
                                          &writeCallback, &metadataCallback, &errorCallback,
                                          this) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
    {
        FLAC__stream_decoder_delete(decoder);
        return FILE_CORRUPT;
    }

    // Begin decoding the file
    bool ok = justMetadata ?
                FLAC__stream_decoder_process_until_end_of_metadata(decoder) :
                FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_finish(decoder);
    FLAC__stream_decoder_delete(decoder);
    return ok ? FILE_OK : FILE_CORRUPT;
}
