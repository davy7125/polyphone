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

#ifndef SF2READER_H
#define SF2READER_H

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "attributetypes.h"

// Reading of the sf2 and sf3 files without Qt, shared by the editor (InputParserSf and SampleReaderSf, building its model)
// and by the synth of a plugin (SynthSoundfontFile), so that a soundfont is read the same everywhere. The structure is
// read at once, the sample data afterwards (on demand in the editor).
// The reading is lenient: a file is refused only if its structure cannot be read, a sample whose data cannot be read is
// silent. The sizes read in a file are checked against its length before allocating (corrupted files).
class Sf2Reader
{
public:
    /// Source of the bytes

    class Source
    {
    public:
        virtual ~Source() {}
        virtual uint64_t getLength() = 0;

        // Bytes at a position, false if they cannot be all read
        virtual bool readAt(uint64_t position, void * data, uint64_t size) = 0;
    };

    // Bytes of a file
    class FileSource: public Source
    {
    public:
        explicit FileSource(const std::filesystem::path &filePath);
        bool isOpen() const { return _stream.is_open(); }
        uint64_t getLength() override { return _length; }
        bool readAt(uint64_t position, void * data, uint64_t size) override;

    private:
        std::ifstream _stream;
        uint64_t _length;
        uint64_t _streamPosition; // After the last reading, UINT64_MAX if unknown
    };

    // Bytes of the content of a file in memory (not copied)
    class MemorySource: public Source
    {
    public:
        MemorySource(const char * data, size_t length) : _data(data), _length(data == nullptr ? 0 : length) {}
        uint64_t getLength() override { return _length; }
        bool readAt(uint64_t position, void * data, uint64_t size) override;

    private:
        const char * _data;
        uint64_t _length;
    };

    /// Content

    struct Sample
    {
        std::string name; // UTF-8
        uint32_t length; // Number of values, or of bytes of compressed data
        uint32_t loopStart; // From the start of the sample
        uint32_t loopEnd;
        uint32_t sampleRate; // Never 0: replaced by the one of another sample or 44100 Hz
        uint8_t originalPitch;
        int8_t correction;
        uint16_t sampleLink;
        SFSampleLink sampleType; // Without the bit 0x10 of the compressed samples
        bool isCompressed; // Ogg Vorbis data of an sf3 file
        uint32_t start16; // Position of the 16-bit data or of the compressed data in the file (UINT32_MAX if beyond 4 GB)
        uint32_t start24; // Position of the 8 least significant bits of 24-bit data, 0 if there are none
    };

    // Attributes as they are in a division of the editor, modulators numbered from 0 in each element (for the links)
    struct Zone
    {
        Zone();
        void setGenerator(uint16_t oper, uint16_t amount);

        bool attributeSet[champ_endOper];
        AttributeValue attributeValues[champ_endOper];
        std::vector<ModulatorData> modulators;
    };

    // Instrument or preset: the zones without the generator of the element below (sample or instrument) are merged in
    // the global zone
    struct Element
    {
        std::string name; // UTF-8
        uint16_t preset = 0; // Presets only
        uint16_t bank = 0;
        uint32_t library = 0;
        uint32_t genre = 0;
        uint32_t morphology = 0;
        Zone globalZone;
        std::vector<Zone> zones;
    };

    // Read the structure, false in case of error (see getError)
    bool read(Source &source);
    const std::string &getError() const { return _error; }

    // Texts of the INFO chunk in UTF-8 by name ("INAM", "ICMT", "isng"...), empty if missing
    std::string getInfo(const std::string &key) const;
    const std::map<std::string, std::string> &getInfos() const { return _infos; }

    // Version "ifil" or "iver", 0.0 if missing
    SfVersionTag getVersion(const std::string &key) const;

    // True if the samples have 24 bits (sm24 chunk)
    bool has24BitSamples() const { return _sm24Offset > 0; }

    const std::vector<Sample> &getSamples() const { return _samples; }
    const std::vector<Element> &getInstruments() const { return _instruments; }
    const std::vector<Element> &getPresets() const { return _presets; }

    /// Sample data, read afterwards: arrays allocated with new [], deleted by the caller

    // Data not compressed: 16 bits, and the 8 least significant bits of 24-bit data if start24 is not 0. Both are null if
    // they cannot be all read.
    static void readData(Source &source, uint32_t start16, uint32_t start24, uint32_t length, int16_t * &data16,
                         uint8_t * &data24);

    // Compressed data of an sf3 file, null if it cannot be read
    static char * readCompressedData(Source &source, uint32_t start, uint32_t length);

    // Decompressed data (16 bits) and its length, null if the data cannot be decompressed
    static int16_t * decompress(const char * compressedData, uint32_t compressedLength, uint32_t &length);

    /// Text

    // True if the bytes are valid UTF-8, that QString::fromUtf8 reads without replacing anything: no isolated or missing
    // continuation byte, no overlong encoding, no surrogate, nothing above U+10FFFF (also used by the editor for the
    // texts of the other formats, through Utils::isValidUtf8)
    static bool isValidUtf8(const char * data, size_t length);

private:
    class Cursor;
    struct Parts;

    bool readInfo(Cursor &cursor);
    bool readSdta(Cursor &cursor);
    bool readPdta(Cursor &cursor, Parts &parts);
    void createSamples(const Parts &parts);
    bool createElements(const Parts &parts, bool isPreset);

    std::string _error;
    uint32_t _infoSize = 0;
    std::map<std::string, std::string> _infos;
    std::map<std::string, SfVersionTag> _versions;
    uint32_t _smplOffset = 0; // Position of the 16-bit data from the beginning of sdta
    uint32_t _sm24Offset = 0; // Position of the 24-bit data from the beginning of sdta, 0 if there are none
    std::vector<Sample> _samples;
    std::vector<Element> _instruments;
    std::vector<Element> _presets;
};

#endif // SF2READER_H
