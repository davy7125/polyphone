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
#include <sndfile.hh>
#include <QtEndian>

// Virtual file for libsndfile, reading a wav file stored in memory
struct VirtualFile
{
    const char * data;
    sf_count_t length;
    sf_count_t pos;
};

static sf_count_t vfGetFileLength(void * userData)
{
    return static_cast<VirtualFile *>(userData)->length;
}

static sf_count_t vfSeek(sf_count_t offset, int whence, void * userData)
{
    VirtualFile * vf = static_cast<VirtualFile *>(userData);
    switch (whence)
    {
    case SEEK_SET: vf->pos = offset; break;
    case SEEK_CUR: vf->pos += offset; break;
    case SEEK_END: vf->pos = vf->length + offset; break;
    default: break;
    }
    if (vf->pos < 0)
        vf->pos = 0;
    if (vf->pos > vf->length)
        vf->pos = vf->length;
    return vf->pos;
}

static sf_count_t vfRead(void * ptr, sf_count_t count, void * userData)
{
    VirtualFile * vf = static_cast<VirtualFile *>(userData);
    if (count > vf->length - vf->pos)
        count = vf->length - vf->pos;
    if (count > 0)
    {
        memcpy(ptr, vf->data + vf->pos, count);
        vf->pos += count;
    }
    return count;
}

static sf_count_t vfWrite(const void * ptr, sf_count_t count, void * userData)
{
    Q_UNUSED(ptr)
    Q_UNUSED(count)
    Q_UNUSED(userData)
    return 0;
}

static sf_count_t vfTell(void * userData)
{
    return static_cast<VirtualFile *>(userData)->pos;
}

static SF_VIRTUAL_IO s_virtualIo = { vfGetFileLength, vfSeek, vfRead, vfWrite, vfTell };

static void appendUInt32(QByteArray &array, quint32 value)
{
    char bytes[4] = {
        static_cast<char>(value & 0xFF), static_cast<char>((value >> 8) & 0xFF),
        static_cast<char>((value >> 16) & 0xFF), static_cast<char>((value >> 24) & 0xFF)
    };
    array.append(bytes, 4);
}

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
    data16 = nullptr;
    data24 = nullptr;

    // Wav file in memory
    QByteArray waveFile;
    SampleReaderResult result = extractWaveFile(fi, _info->dwStart, waveFile);
    if (result != FILE_OK)
        return result;

    // Decode it
    VirtualFile vf;
    vf.data = waveFile.constData();
    vf.length = waveFile.size();
    vf.pos = 0;
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(SF_INFO));
    SNDFILE * sndFile = sf_open_virtual(&s_virtualIo, SFM_READ, &sfInfo, &vf);
    if (sndFile == nullptr)
        return FILE_NOT_SUPPORTED;
    if (sfInfo.channels <= 0 || sfInfo.frames <= 0)
    {
        sf_close(sndFile);
        _info->dwLength = 0;
        return FILE_OK;
    }

    quint32 length = static_cast<quint32>(sfInfo.frames);
    int * interleavedData = new int[sfInfo.channels * length];
    length = static_cast<quint32>(sf_readf_int(sndFile, interleavedData, length));
    sf_close(sndFile);

    // Extract a channel
    int channel = _info->wChannel < sfInfo.channels ? _info->wChannel : 0;
    data16 = new qint16[length];
    data24 = new quint8[length];
    for (quint32 i = 0; i < length; i++)
    {
        int value = interleavedData[i * sfInfo.channels + channel];
        data16[i] = static_cast<qint16>(value >> 16);
        data24[i] = static_cast<quint8>((value >> 8) & 0xFF);
    }
    delete [] interleavedData;
    _info->dwLength = length;

    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderDls::readWaveInfo(QFile &fi, quint32 waveListPosition, quint32 &length, quint16 &channels, quint32 &sampleRate)
{
    QByteArray waveFile;
    SampleReaderResult result = extractWaveFile(fi, waveListPosition, waveFile);
    if (result != FILE_OK)
        return result;

    VirtualFile vf;
    vf.data = waveFile.constData();
    vf.length = waveFile.size();
    vf.pos = 0;
    SF_INFO sfInfo;
    memset(&sfInfo, 0, sizeof(SF_INFO));
    SNDFILE * sndFile = sf_open_virtual(&s_virtualIo, SFM_READ, &sfInfo, &vf);
    if (sndFile == nullptr)
        return FILE_NOT_SUPPORTED;
    length = static_cast<quint32>(sfInfo.frames);
    channels = static_cast<quint16>(sfInfo.channels);
    sampleRate = static_cast<quint32>(sfInfo.samplerate);
    sf_close(sndFile);

    return FILE_OK;
}

SampleReader::SampleReaderResult SampleReaderDls::extractWaveFile(QFile &fi, quint32 waveListPosition, QByteArray &waveFile)
{
    // Header of the list "wave"
    char header[12];
    if (!fi.seek(waveListPosition) || fi.read(header, 12) != 12)
        return FILE_CORRUPT;
    if (memcmp(header, "LIST", 4) != 0 || memcmp(header + 8, "wave", 4) != 0)
        return FILE_CORRUPT;
    quint32 listSize = qFromLittleEndian<quint32>(header + 4);
    qint64 listEnd = static_cast<qint64>(waveListPosition) + 8 + listSize;

    // Copy the chunks that are understood by libsndfile
    QByteArray fmtChunk, factChunk, dataChunk;
    qint64 pos = waveListPosition + 12;
    while (pos + 8 <= listEnd)
    {
        char chunkHeader[8];
        if (!fi.seek(pos) || fi.read(chunkHeader, 8) != 8)
            return FILE_CORRUPT;
        quint32 chunkSize = qFromLittleEndian<quint32>(chunkHeader + 4);
        if (pos + 8 + chunkSize > listEnd)
            chunkSize = static_cast<quint32>(listEnd - pos - 8); // Truncated chunk

        QByteArray * target = nullptr;
        if (memcmp(chunkHeader, "fmt ", 4) == 0)
            target = &fmtChunk;
        else if (memcmp(chunkHeader, "fact", 4) == 0)
            target = &factChunk;
        else if (memcmp(chunkHeader, "data", 4) == 0)
            target = &dataChunk;

        if (target != nullptr)
        {
            *target = QByteArray(chunkHeader, 4);
            appendUInt32(*target, chunkSize);
            QByteArray content = fi.read(chunkSize);
            if (content.size() != static_cast<int>(chunkSize))
                return FILE_CORRUPT;
            target->append(content);
            if (chunkSize % 2)
                target->append('\0');
        }

        pos += 8 + chunkSize + (chunkSize % 2);
    }
    if (fmtChunk.isEmpty() || dataChunk.isEmpty())
        return FILE_CORRUPT;

    // Wav file
    waveFile = QByteArray("RIFF");
    appendUInt32(waveFile, static_cast<quint32>(4 + fmtChunk.size() + factChunk.size() + dataChunk.size()));
    waveFile.append("WAVE", 4);
    waveFile.append(fmtChunk);
    waveFile.append(factChunk);
    waveFile.append(dataChunk);

    return FILE_OK;
}
