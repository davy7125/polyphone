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


#ifndef SAMPLEFILEREADER_H
#define SAMPLEFILEREADER_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
struct SF_INFO;
struct sf_private_tag; // SNDFILE of libsndfile

// Sample read with libsndfile (wav, flac, ogg, aiff...), the same way by the editor and by the plugin: information and
// data of a channel. The file can be on the disk or in memory (wave of a DLS file).
class SampleFileReader
{
public:
    struct Info
    {
        uint16_t channels = 0;
        uint32_t sampleRate = 0;
        uint32_t length = 0; // Frames
        bool hasInstrument = false; // Root key, fine tune and loops given by the file
        uint32_t rootKey = 60;
        int32_t fineTune = 0; // Cents
        std::vector<std::pair<uint32_t, uint32_t>> loops;
    };

    // A file opened by libsndfile, from a path, a file descriptor (not closed) or a content in memory (not copied):
    // null if libsndfile cannot read it
    static std::unique_ptr<SampleFileReader> open(const std::filesystem::path &path);
    static std::unique_ptr<SampleFileReader> openDescriptor(int fileDescriptor);
    static std::unique_ptr<SampleFileReader> openMemory(const char * data, size_t length);
    ~SampleFileReader();

    // Number of channels, sample rate and frames, root key, fine tune and loops of the instrument chunk (if any)
    void getInfo(Info &info) const;

    // Data of a channel on 24 bits (16 + 8 bits), length frames at most: the integer values are read on 32 bits, the
    // floating point ones are converted
    void readChannel(uint16_t channel, uint32_t length, int16_t * data16, uint8_t * data24);

    // Same, all values being read as integers (not scaled if they are floating point values), the arrays being allocated
    // (new []) for the frames read, whose number is returned
    uint32_t readChannelAsIntegers(uint16_t channel, int16_t * &data16, uint8_t * &data24);

    // Floating point value from -1 to 1 converted into a 24-bit value
    static int32_t floatToInt24(float value);

    SampleFileReader(const SampleFileReader &) = delete;
    SampleFileReader &operator=(const SampleFileReader &) = delete;

private:
    SampleFileReader();

    // Access to a content in memory (sf_count_t being int64_t)
    static int64_t getMemoryLength(void * userData);
    static int64_t seekMemory(int64_t offset, int whence, void * userData);
    static int64_t readMemory(void * ptr, int64_t count, void * userData);
    static int64_t writeMemory(const void * ptr, int64_t count, void * userData);
    static int64_t tellMemory(void * userData);

    sf_private_tag * _file;
    std::unique_ptr<SF_INFO> _info;
    const char * _memoryData;
    int64_t _memoryLength;
    int64_t _memoryPosition;
};

#endif // SAMPLEFILEREADER_H
