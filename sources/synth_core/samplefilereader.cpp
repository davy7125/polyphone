/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "samplefilereader.h"
#include <cstdio>
#include <cstring>
#include <type_traits>
#ifdef _WIN32
#include <windows.h>
#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES 1
#endif
#include <sndfile.h>

static_assert(std::is_same<sf_count_t, int64_t>::value, "sf_count_t is int64_t");

SampleFileReader::SampleFileReader() :
    _file(nullptr),
    _memoryData(nullptr),
    _memoryLength(0),
    _memoryPosition(0)
{
    _info.reset(new SF_INFO);
    memset(_info.get(), 0, sizeof(SF_INFO));
}

SampleFileReader::~SampleFileReader()
{
    if (_file != nullptr)
        sf_close(_file);
}

std::unique_ptr<SampleFileReader> SampleFileReader::open(const std::filesystem::path &path)
{
    std::unique_ptr<SampleFileReader> reader(new SampleFileReader());
#ifdef _WIN32
    reader->_file = sf_wchar_open(path.c_str(), SFM_READ, reader->_info.get());
#else
    reader->_file = sf_open(path.c_str(), SFM_READ, reader->_info.get());
#endif
    if (reader->_file == nullptr)
        reader.reset();
    return reader;
}

std::unique_ptr<SampleFileReader> SampleFileReader::openDescriptor(int fileDescriptor)
{
    std::unique_ptr<SampleFileReader> reader(new SampleFileReader());
    reader->_file = sf_open_fd(fileDescriptor, SFM_READ, reader->_info.get(), false);
    if (reader->_file == nullptr)
        reader.reset();
    return reader;
}

std::unique_ptr<SampleFileReader> SampleFileReader::openMemory(const char * data, size_t length)
{
    static SF_VIRTUAL_IO virtualIo = { getMemoryLength, seekMemory, readMemory, writeMemory, tellMemory };
    std::unique_ptr<SampleFileReader> reader(new SampleFileReader());
    reader->_memoryData = data;
    reader->_memoryLength = static_cast<sf_count_t>(length);
    reader->_file = sf_open_virtual(&virtualIo, SFM_READ, reader->_info.get(), reader.get());
    if (reader->_file == nullptr)
        reader.reset();
    return reader;
}

int64_t SampleFileReader::getMemoryLength(void * userData)
{
    return static_cast<SampleFileReader *>(userData)->_memoryLength;
}

int64_t SampleFileReader::seekMemory(int64_t offset, int whence, void * userData)
{
    SampleFileReader * reader = static_cast<SampleFileReader *>(userData);
    switch (whence)
    {
    case SEEK_SET: reader->_memoryPosition = offset; break;
    case SEEK_CUR: reader->_memoryPosition += offset; break;
    case SEEK_END: reader->_memoryPosition = reader->_memoryLength + offset; break;
    default: break;
    }
    if (reader->_memoryPosition < 0)
        reader->_memoryPosition = 0;
    if (reader->_memoryPosition > reader->_memoryLength)
        reader->_memoryPosition = reader->_memoryLength;
    return reader->_memoryPosition;
}

int64_t SampleFileReader::readMemory(void * ptr, int64_t count, void * userData)
{
    SampleFileReader * reader = static_cast<SampleFileReader *>(userData);
    if (count > reader->_memoryLength - reader->_memoryPosition)
        count = reader->_memoryLength - reader->_memoryPosition;
    if (count > 0)
    {
        memcpy(ptr, reader->_memoryData + reader->_memoryPosition, static_cast<size_t>(count));
        reader->_memoryPosition += count;
    }
    return count;
}

int64_t SampleFileReader::writeMemory(const void *, int64_t, void *)
{
    return 0;
}

int64_t SampleFileReader::tellMemory(void * userData)
{
    return static_cast<SampleFileReader *>(userData)->_memoryPosition;
}

void SampleFileReader::getInfo(Info &info) const
{
    info.channels = static_cast<uint16_t>(_info->channels);
    info.sampleRate = static_cast<uint32_t>(_info->samplerate);
    info.length = static_cast<uint32_t>(_info->frames);

    // Loop / root key / fine tune
    SF_INSTRUMENT instrument;
    if (sf_command(_file, SFC_GET_INSTRUMENT, &instrument, sizeof(instrument)) == SF_TRUE)
    {
        info.hasInstrument = true;
        info.rootKey = static_cast<uint32_t>(instrument.basenote);
        info.fineTune = instrument.detune;
        for (int i = 0; i < instrument.loop_count; i++)
            info.loops.push_back(std::make_pair(instrument.loops[i].start, instrument.loops[i].end));
    }
}

void SampleFileReader::readChannel(uint16_t channel, uint32_t length, int16_t * data16, uint8_t * data24)
{
    uint32_t channels = static_cast<uint32_t>(_info->channels);
    int subFormat = _info->format & SF_FORMAT_SUBMASK;
    if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE)
    {
        // Floating point values are not scaled when they are read as integers: they would all be rounded to 0
        float * interleavedData = new float[channels * length];
        sf_readf_float(_file, interleavedData, length);

        // Extract a channel
        int iTmp;
        for (uint32_t i = 0; i < length; i++)
        {
            iTmp = floatToInt24(interleavedData[i * channels + channel]);
            data16[i] = static_cast<int16_t>(iTmp >> 8);
            data24[i] = static_cast<uint8_t>(iTmp & 0xFF);
        }
        delete [] interleavedData;
    }
    else
    {
        int * interleavedData = new int[channels * length];
        sf_readf_int(_file, interleavedData, length);

        // Extract a channel
        int iTmp;
        for (uint32_t i = 0; i < length; i++)
        {
            iTmp = interleavedData[i * channels + channel];
            data16[i] = static_cast<int16_t>(iTmp >> 16);
            data24[i] = static_cast<uint8_t>((iTmp >> 8) & 0xFF);
        }
        delete [] interleavedData;
    }
}

uint32_t SampleFileReader::readChannelAsIntegers(uint16_t channel, int16_t * &data16, uint8_t * &data24)
{
    data16 = nullptr;
    data24 = nullptr;
    if (_info->channels <= 0 || _info->frames <= 0)
        return 0;

    uint32_t channels = static_cast<uint32_t>(_info->channels);
    uint32_t length = static_cast<uint32_t>(_info->frames);
    int * interleavedData = new int[channels * length];
    length = static_cast<uint32_t>(sf_readf_int(_file, interleavedData, length));

    // Extract a channel
    if (channel >= channels)
        channel = 0;
    data16 = new int16_t[length];
    data24 = new uint8_t[length];
    for (uint32_t i = 0; i < length; i++)
    {
        int value = interleavedData[i * channels + channel];
        data16[i] = static_cast<int16_t>(value >> 16);
        data24[i] = static_cast<uint8_t>((value >> 8) & 0xFF);
    }
    delete [] interleavedData;
    return length;
}

int32_t SampleFileReader::floatToInt24(float f)
{
    // Limit the input
    if (f > 1.0f)
        f = 1.0f;
    else if (f < -1.0f)
        f = -1.0f;

    f = f * 8388607.5f /*(.5f + 0x7FFFFF)*/ - .5f;
    int32_t result = static_cast<int32_t>(f + (f > 0 ? 0.5f : -0.5f));

    // Limit the output
    if (result > 8388607)
        return 8388607;
    else if (result < -8388608)
        return -8388608;

    return result;
}
