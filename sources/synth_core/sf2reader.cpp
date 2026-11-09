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

#include "sf2reader.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
#include <vorbis/vorbisfile.h>

/// Text and numbers of the file

static uint16_t getU16(const uint8_t * data)
{
    return static_cast<uint16_t>(data[0] | (data[1] << 8));
}

static uint32_t getU32(const uint8_t * data)
{
    return static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24);
}

static std::string latin1ToUtf8(const std::string &text)
{
    std::string result;
    result.reserve(text.size());
    for (unsigned char c : text)
    {
        if (c < 0x80)
            result += static_cast<char>(c);
        else
        {
            result += static_cast<char>(0xC0 | (c >> 6));
            result += static_cast<char>(0x80 | (c & 0x3F));
        }
    }
    return result;
}

// Name of 20 characters in Latin-1, until the first '\0' and without the spaces around (as QString::trimmed)
static std::string readName(const uint8_t * data)
{
    std::string name(data, std::find(data, data + 20, 0));
    auto isSpace = [](char c) {
        unsigned char u = static_cast<unsigned char>(c);
        return (u >= 0x09 && u <= 0x0D) || u == 0x20 || u == 0x85 || u == 0xA0;
    };
    size_t first = 0;
    while (first < name.size() && isSpace(name[first]))
        first++;
    size_t last = name.size();
    while (last > first && isSpace(name[last - 1]))
        last--;
    return latin1ToUtf8(name.substr(first, last - first));
}

/// Sources

Sf2Reader::FileSource::FileSource(const std::filesystem::path &filePath) :
    _length(0),
    _streamPosition(UINT64_MAX)
{
    _stream.open(filePath, std::ios::binary);
    if (_stream.is_open())
    {
        _stream.seekg(0, std::ios::end);
        std::streamoff length = _stream.tellg();
        _length = length > 0 ? static_cast<uint64_t>(length) : 0;
    }
}

bool Sf2Reader::FileSource::readAt(uint64_t position, void * data, uint64_t size)
{
    if (position > _length || size > _length - position)
        return false;
    if (size == 0)
        return true;

    if (position != _streamPosition)
    {
        _stream.clear();
        _stream.seekg(static_cast<std::streamoff>(position));
    }
    _stream.read(static_cast<char *>(data), static_cast<std::streamsize>(size));
    bool ok = (_stream.gcount() == static_cast<std::streamsize>(size));
    _streamPosition = ok ? position + size : UINT64_MAX;
    return ok;
}

bool Sf2Reader::MemorySource::readAt(uint64_t position, void * data, uint64_t size)
{
    if (position > _length || size > _length - position)
        return false;
    if (size > 0)
        memcpy(data, _data + position, static_cast<size_t>(size));
    return true;
}

static bool fits(Sf2Reader::Source &source, uint64_t position, uint64_t size)
{
    uint64_t length = source.getLength();
    return position <= length && size <= length - position;
}

/// Sequential reading

// Parts of the file read one after the other. As with the QDataStream that the editor used, a reading that failed makes
// the next ones fail.
class Sf2Reader::Cursor
{
public:
    explicit Cursor(Source &source) :
        _source(source),
        _position(0),
        _failed(false)
    {}

    bool read(void * data, uint64_t size)
    {
        if (_failed || !_source.readAt(_position, data, size))
        {
            _failed = true;
            return false;
        }
        _position += size;
        return true;
    }

    // Data allocated only if the file is long enough
    bool read(std::vector<uint8_t> &data, uint64_t size)
    {
        if (_failed || !fits(_source, _position, size))
        {
            _failed = true;
            return false;
        }
        data.resize(static_cast<size_t>(size));
        return read(data.data(), size);
    }

    // Bytes until the end of the file at most, the missing ones being 0 (as for QDataStream reading numbers after the
    // end), false if more than maxMissing bytes are missing
    bool readUntilEnd(std::vector<uint8_t> &data, uint64_t size, uint64_t maxMissing)
    {
        uint64_t length = _source.getLength();
        uint64_t available = (_failed || _position > length) ? 0 : std::min(size, length - _position);
        if (size - available > maxMissing)
        {
            _failed = true;
            return false;
        }
        data.assign(static_cast<size_t>(size), 0);
        if (available > 0 && !_source.readAt(_position, data.data(), available))
        {
            _failed = true;
            return false;
        }
        _position += size;
        _failed = (available < size);
        return true;
    }

    bool readTag(char tag[4])
    {
        return read(tag, 4);
    }

    bool readU16(uint16_t &value)
    {
        uint8_t data[2];
        if (!read(data, 2))
            return false;
        value = getU16(data);
        return true;
    }

    bool readU32(uint32_t &value)
    {
        uint8_t data[4];
        if (!read(data, 4))
            return false;
        value = getU32(data);
        return true;
    }

    // Move in the sequence, possibly after the end (the next reading then fails)
    void skip(uint64_t size) { _position += size; }
    void moveBack(uint64_t size) { _position -= size; }

private:
    Source &_source;
    uint64_t _position;
    bool _failed;
};

/// Records of the pdta part

struct Sf2Reader::Parts
{
    struct ElementHeader // Instrument or preset
    {
        std::string name;
        uint16_t preset;
        uint16_t bank;
        uint16_t bagIndex;
        uint32_t library;
        uint32_t genre;
        uint32_t morphology;
    };

    struct Bag
    {
        uint16_t genIndex;
        uint16_t modIndex;
    };

    struct Generator
    {
        uint16_t oper;
        uint16_t amount;
    };

    struct Modulator
    {
        uint8_t source[2];
        uint16_t destination;
        int16_t amount;
        uint8_t amountSource[2];
        uint16_t transform;
    };

    struct SampleHeader
    {
        std::string name;
        uint32_t start;
        uint32_t end;
        uint32_t startLoop;
        uint32_t endLoop;
        uint32_t sampleRate;
        uint8_t originalPitch;
        int8_t correction;
        uint16_t sampleLink;
        uint16_t sampleType;
    };

    std::vector<ElementHeader> presetHeaders, instrumentHeaders;
    std::vector<Bag> presetBags, instrumentBags;
    std::vector<Modulator> presetModulators, instrumentModulators;
    std::vector<Generator> presetGenerators, instrumentGenerators;
    std::vector<SampleHeader> sampleHeaders;

    static bool readChunk(Cursor &cursor, uint32_t recordSize, std::vector<uint8_t> &data, bool isLast = false);
    static void readBags(const std::vector<uint8_t> &data, std::vector<Bag> &bags);
    static void readModulators(const std::vector<uint8_t> &data, std::vector<Modulator> &modulators);
    static void readGenerators(const std::vector<uint8_t> &data, std::vector<Generator> &generators);
};

bool Sf2Reader::Parts::readChunk(Cursor &cursor, uint32_t recordSize, std::vector<uint8_t> &data, bool isLast)
{
    // Name (not checked), size multiple of the size of a record, records that cannot go beyond the end of the file (a
    // size read in corrupted data would make millions of records be read)
    char tag[4];
    uint32_t size;
    if (!cursor.readTag(tag) || !cursor.readU32(size) || size % recordSize != 0)
        return false;
    if (!isLast)
        return cursor.read(data, size);

    // The last chunk is read until the end of the file, the missing records being 0 (some files do not have their
    // terminal record): up to 65536 records, as many samples as a zone can use
    return cursor.readUntilEnd(data, size, 65536ULL * recordSize);
}

void Sf2Reader::Parts::readBags(const std::vector<uint8_t> &data, std::vector<Bag> &bags)
{
    for (size_t i = 0; i < data.size(); i += 4)
        bags.push_back({ getU16(&data[i]), getU16(&data[i + 2]) });
}

void Sf2Reader::Parts::readModulators(const std::vector<uint8_t> &data, std::vector<Modulator> &modulators)
{
    for (size_t i = 0; i < data.size(); i += 10)
    {
        Modulator modulator;
        modulator.source[0] = data[i];
        modulator.source[1] = data[i + 1];
        modulator.destination = getU16(&data[i + 2]);
        modulator.amount = static_cast<int16_t>(getU16(&data[i + 4]));
        modulator.amountSource[0] = data[i + 6];
        modulator.amountSource[1] = data[i + 7];
        modulator.transform = getU16(&data[i + 8]);
        modulators.push_back(modulator);
    }
}

void Sf2Reader::Parts::readGenerators(const std::vector<uint8_t> &data, std::vector<Generator> &generators)
{
    for (size_t i = 0; i < data.size(); i += 4)
        generators.push_back({ getU16(&data[i]), getU16(&data[i + 2]) });
}

/// Structure

Sf2Reader::Zone::Zone()
{
    for (int i = 0; i < champ_endOper; i++)
        attributeSet[i] = false;
}

void Sf2Reader::Zone::setGenerator(uint16_t oper, uint16_t amount)
{
    // As Division::setGen: -1 for the root key, the key and the velocity is the default value
    AttributeValue value;
    value.wValue = amount;
    if (value.shValue == -1 && (oper == champ_overridingRootKey || oper == champ_keynum || oper == champ_velocity))
        attributeSet[oper] = false;
    else
    {
        attributeSet[oper] = true;
        attributeValues[oper] = value;
    }
}

bool Sf2Reader::read(Source &source)
{
    _error.clear();
    _infos.clear();
    _versions.clear();
    _samples.clear();
    _instruments.clear();
    _presets.clear();
    _infoSize = _smplOffset = _sm24Offset = 0;

    try
    {
        Cursor cursor(source);
        if (!readInfo(cursor))
        {
            _error = "invalid header";
            return false;
        }
        if (!readSdta(cursor))
        {
            _error = "corrupted file (sdta)";
            return false;
        }
        Parts parts;
        if (!readPdta(cursor, parts))
        {
            _error = "corrupted file (pdta)";
            return false;
        }
        createSamples(parts);
        if (!createElements(parts, false) || !createElements(parts, true))
            return false;
    }
    catch (const std::bad_alloc &)
    {
        _error = "not enough memory";
        return false;
    }
    return true;
}

bool Sf2Reader::readInfo(Cursor &cursor)
{
    // "RIFF", size, "sfbk", then "LIST", size and "INFO" (the names are not checked)
    char tag[4];
    uint32_t size;
    if (!cursor.readTag(tag) || !cursor.readU32(size) || !cursor.readTag(tag) ||
        !cursor.readTag(tag) || !cursor.readU32(_infoSize) || !cursor.readTag(tag))
        return false;

    uint64_t position = 4;
    while (position < _infoSize)
    {
        char name[4];
        uint32_t blockSize;
        if (!cursor.readTag(name) || !cursor.readU32(blockSize) || position + 8 + blockSize > _infoSize)
            return false;

        std::string key(name, 4);
        if (key == "ifil" || key == "iver")
        {
            // A version is 4 bytes, whatever the size of its block
            uint16_t major, minor;
            if (!cursor.readU16(major) || !cursor.readU16(minor))
                return false;
            _versions[key] = { major, minor };
        }
        else
        {
            // Text until the first '\0' (at most 2 GB), in UTF-8 for the comment if it is valid, in Latin-1 otherwise
            std::vector<uint8_t> buffer;
            if (blockSize > INT32_MAX || !cursor.read(buffer, blockSize))
                return false;
            std::string text(buffer.begin(), std::find(buffer.begin(), buffer.end(), 0));
            _infos[key] = (key == "ICMT" && isValidUtf8(text.data(), text.size())) ? text : latin1ToUtf8(text);
        }

        position += 8 + static_cast<uint64_t>(blockSize);
    }

    return true;
}

bool Sf2Reader::readSdta(Cursor &cursor)
{
    // "LIST", size, "sdta", then possibly "smpl" and "sm24" whose data are skipped
    char tag[4];
    uint32_t sdtaSize;
    if (!cursor.readTag(tag) || !cursor.readU32(sdtaSize) || !cursor.readTag(tag) || !cursor.readTag(tag))
        return false;

    // A block having an odd size is followed by a pad byte, skipped only if the size of sdta includes it
    auto skipPadByte = [&cursor, sdtaSize](uint32_t blockSize, uint64_t readSize) -> uint64_t {
        if (blockSize % 2 == 0 || readSize >= sdtaSize)
            return 0;
        cursor.skip(1);
        return 1;
    };

    if (memcmp(tag, "smpl", 4) == 0)
    {
        uint32_t smplSize;
        if (!cursor.readU32(smplSize))
            return false;
        _smplOffset = 20;
        cursor.skip(smplSize);
        uint64_t readSize = 12 + static_cast<uint64_t>(smplSize);
        readSize += skipPadByte(smplSize, readSize);

        if (!cursor.readTag(tag))
            return false;
        if (memcmp(tag, "sm24", 4) == 0)
        {
            uint32_t sm24Size;
            if (!cursor.readU32(sm24Size))
                return false;
            _sm24Offset = static_cast<uint32_t>(8 + readSize + 8);
            cursor.skip(sm24Size);
            readSize += 8 + static_cast<uint64_t>(sm24Size);
            skipPadByte(sm24Size, readSize);
        }
        else
        {
            // Not "sm24": the next part
            cursor.moveBack(4);
            _sm24Offset = 0;
        }
    }
    else
    {
        // The samples are directly after "sdta"
        _smplOffset = 12;
        _sm24Offset = 0;
        cursor.skip(static_cast<uint32_t>(sdtaSize - 8));
    }

    return true;
}

bool Sf2Reader::readPdta(Cursor &cursor, Parts &parts)
{
    // "LIST", size, "pdta", then the chunks in the order of the format
    char tag[4];
    uint32_t size;
    if (!cursor.readTag(tag) || !cursor.readU32(size) || !cursor.readTag(tag))
        return false;

    std::vector<uint8_t> data;
    if (!Parts::readChunk(cursor, 38, data))
        return false;
    for (size_t i = 0; i < data.size(); i += 38)
        parts.presetHeaders.push_back({ readName(&data[i]), getU16(&data[i + 20]), getU16(&data[i + 22]),
                                        getU16(&data[i + 24]), getU32(&data[i + 26]), getU32(&data[i + 30]),
                                        getU32(&data[i + 34]) });

    if (!Parts::readChunk(cursor, 4, data))
        return false;
    Parts::readBags(data, parts.presetBags);

    if (!Parts::readChunk(cursor, 10, data))
        return false;
    Parts::readModulators(data, parts.presetModulators);

    if (!Parts::readChunk(cursor, 4, data))
        return false;
    Parts::readGenerators(data, parts.presetGenerators);

    if (!Parts::readChunk(cursor, 22, data))
        return false;
    for (size_t i = 0; i < data.size(); i += 22)
        parts.instrumentHeaders.push_back({ readName(&data[i]), 0, 0, getU16(&data[i + 20]), 0, 0, 0 });

    if (!Parts::readChunk(cursor, 4, data))
        return false;
    Parts::readBags(data, parts.instrumentBags);

    if (!Parts::readChunk(cursor, 10, data))
        return false;
    Parts::readModulators(data, parts.instrumentModulators);

    if (!Parts::readChunk(cursor, 4, data))
        return false;
    Parts::readGenerators(data, parts.instrumentGenerators);

    if (!Parts::readChunk(cursor, 46, data, true))
        return false;
    for (size_t i = 0; i < data.size(); i += 46)
    {
        Parts::SampleHeader header;
        header.name = readName(&data[i]);
        header.start = getU32(&data[i + 20]);
        header.end = getU32(&data[i + 24]);
        header.startLoop = getU32(&data[i + 28]);
        header.endLoop = getU32(&data[i + 32]);
        header.sampleRate = getU32(&data[i + 36]);
        header.originalPitch = data[i + 40];
        header.correction = static_cast<int8_t>(data[i + 41]);
        header.sampleLink = getU16(&data[i + 42]);
        header.sampleType = getU16(&data[i + 44]);
        parts.sampleHeaders.push_back(header);
    }

    return true;
}

void Sf2Reader::createSamples(const Parts &parts)
{
    // The last header (EOS) is not a sample
    const std::vector<Parts::SampleHeader> &headers = parts.sampleHeaders;
    bool isSf3 = (getVersion("ifil").wMajor >= 3);
    for (size_t i = 0; i + 1 < headers.size(); i++)
    {
        const Parts::SampleHeader &header = headers[i];
        _samples.emplace_back();
        Sample &sample = _samples.back();
        sample.name = header.name;

        // Type and link, the type of a compressed sample of an sf3 file having the bit 0x10
        uint16_t sampleType = header.sampleType;
        sample.isCompressed = (isSf3 && (sampleType & 0x10));
        if (sample.isCompressed)
            sampleType &= ~0x10;
        sample.sampleType = static_cast<SFSampleLink>(sampleType);
        sample.sampleLink = header.sampleLink;
        sample.originalPitch = header.originalPitch;
        sample.correction = header.correction;

        // A sample rate of 0 is replaced by the first one of another sample that is not 0, or 44100 Hz
        sample.sampleRate = header.sampleRate;
        for (size_t j = 0; sample.sampleRate == 0 && j + 1 < headers.size(); j++)
            sample.sampleRate = headers[j].sampleRate;
        if (sample.sampleRate == 0)
            sample.sampleRate = 44100;

        // Length, position of the data (in bytes from the sample data in an sf3 file, a position that would be beyond
        // 4 GB cannot be read) and loop relative to the start (already in an sf3 file)
        sample.length = header.end - header.start;
        if (isSf3 && !sample.isCompressed)
            sample.length /= 2;
        uint64_t start16 = 20 + static_cast<uint64_t>(_infoSize) + _smplOffset +
                           static_cast<uint64_t>(header.start) * (isSf3 ? 1 : 2);
        sample.start16 = static_cast<uint32_t>(std::min<uint64_t>(start16, UINT32_MAX));
        sample.start24 = 0;
        if (_sm24Offset > 0)
        {
            uint64_t start24 = 20 + static_cast<uint64_t>(_infoSize) + _sm24Offset + header.start;
            sample.start24 = static_cast<uint32_t>(std::min<uint64_t>(start24, UINT32_MAX));
        }
        sample.loopStart = isSf3 ? header.startLoop : header.startLoop - header.start;
        sample.loopEnd = isSf3 ? header.endLoop : header.endLoop - header.start;
    }
}

bool Sf2Reader::createElements(const Parts &parts, bool isPreset)
{
    // The zones without the generator of the element below (sample or instrument) are merged in the global zone, and
    // the modulators of an element are numbered from 0 in their order
    const std::vector<Parts::ElementHeader> &headers = isPreset ? parts.presetHeaders : parts.instrumentHeaders;
    const std::vector<Parts::Bag> &bags = isPreset ? parts.presetBags : parts.instrumentBags;
    const std::vector<Parts::Modulator> &modulators = isPreset ? parts.presetModulators : parts.instrumentModulators;
    const std::vector<Parts::Generator> &generators = isPreset ? parts.presetGenerators : parts.instrumentGenerators;
    std::vector<Element> &elements = isPreset ? _presets : _instruments;
    const uint16_t childGenerator = static_cast<uint16_t>(isPreset ? champ_instrument : champ_sampleID);
    const char * prefix = isPreset ? "p" : "i";

    for (size_t i = 0; i + 1 < headers.size(); i++) // The last header (EOP or EOI) is not an element
    {
        elements.emplace_back();
        Element &element = elements.back();
        element.name = headers[i].name;
        if (isPreset)
        {
            element.preset = headers[i].preset;
            element.bank = headers[i].bank;
            element.library = headers[i].library;
            element.genre = headers[i].genre;
            element.morphology = headers[i].morphology;
        }

        uint16_t modulatorIndex = 0;
        uint16_t bagMin = headers[i].bagIndex;
        uint16_t bagMax = headers[i + 1].bagIndex;
        for (int j = bagMin; j < bagMax; j++)
        {
            if (j >= static_cast<int>(bags.size()))
            {
                _error = std::string("invalid ") + prefix + "bag index";
                return false;
            }

            // Generators and modulators of the zone, until the ones of the next zone
            uint16_t modMin = bags[j].modIndex;
            uint16_t genMin = bags[j].genIndex;
            uint16_t modMax, genMax;
            if (j < static_cast<int>(bags.size()) - 1)
            {
                modMax = bags[j + 1].modIndex;
                genMax = bags[j + 1].genIndex;
            }
            else
            {
                modMax = static_cast<uint16_t>(modulators.size());
                genMax = static_cast<uint16_t>(generators.size());
            }

            bool isGlobal = true;
            for (int k = genMin; k < genMax; k++)
            {
                if (k >= static_cast<int>(generators.size()))
                {
                    _error = std::string("invalid ") + prefix + "gen index";
                    return false;
                }
                if (generators[k].oper == childGenerator)
                    isGlobal = false;
            }

            Zone * zone = &element.globalZone;
            if (!isGlobal)
            {
                element.zones.emplace_back();
                zone = &element.zones.back();
            }

            for (int k = genMin; k < genMax; k++)
                if (generators[k].oper < champ_endOper)
                    zone->setGenerator(generators[k].oper, generators[k].amount);

            for (int k = modMin; k < modMax; k++)
            {
                if (k >= static_cast<int>(modulators.size()))
                {
                    _error = std::string("invalid ") + prefix + "mod index";
                    return false;
                }

                // Sources: index (7 bits) and CC flag in the first byte, direction, polarity and type in the second
                const Parts::Modulator &modulator = modulators[k];
                ModulatorData data;
                SFModulator * sources[2] = { &data.srcOper, &data.amtSrcOper };
                const uint8_t * bytes[2] = { modulator.source, modulator.amountSource };
                for (int s = 0; s < 2; s++)
                {
                    sources[s]->Type = static_cast<ModType>(bytes[s][1] >> 2);
                    sources[s]->isBipolar = ((bytes[s][1] >> 1) & 0x01) > 0;
                    sources[s]->isDescending = (bytes[s][1] & 0x01) > 0;
                    sources[s]->CC = (bytes[s][0] >> 7) > 0;
                    sources[s]->Index = bytes[s][0] & 0x7F;
                }
                data.destOper = modulator.destination;
                data.amount = modulator.amount;
                data.transOper = static_cast<SFTransform>(modulator.transform);
                data.index = modulatorIndex++;
                zone->modulators.push_back(data);
            }
        }
    }

    return true;
}

std::string Sf2Reader::getInfo(const std::string &key) const
{
    auto it = _infos.find(key);
    return it == _infos.end() ? "" : it->second;
}

SfVersionTag Sf2Reader::getVersion(const std::string &key) const
{
    auto it = _versions.find(key);
    if (it != _versions.end())
        return it->second;
    SfVersionTag version;
    version.wMajor = 0;
    version.wMinor = 0;
    return version;
}

/// Text

bool Sf2Reader::isValidUtf8(const char * data, size_t length)
{
    const unsigned char * bytes = reinterpret_cast<const unsigned char *>(data);
    size_t i = 0;
    while (i < length)
    {
        unsigned char c = bytes[i];
        if (c < 0x80)
        {
            i++;
            continue;
        }

        // Number of continuation bytes after the first one (0x80 to 0xC1: continuation byte alone or overlong encoding
        // of an ASCII character, 0xF5 and more: above U+10FFFF)
        size_t count;
        if (c >= 0xC2 && c <= 0xDF)
            count = 1;
        else if (c >= 0xE0 && c <= 0xEF)
            count = 2;
        else if (c >= 0xF0 && c <= 0xF4)
            count = 3;
        else
            return false;
        if (length - i - 1 < count)
            return false; // Sequence cut by the end

        // The second byte excludes the overlong encodings (0xE0, 0xF0), the surrogates (0xED) and what is above U+10FFFF
        // (0xF4)
        unsigned char second = bytes[i + 1];
        if ((c == 0xE0 && second < 0xA0) || (c == 0xED && second > 0x9F) ||
            (c == 0xF0 && second < 0x90) || (c == 0xF4 && second > 0x8F))
            return false;
        for (size_t k = 1; k <= count; k++)
            if ((bytes[i + k] & 0xC0) != 0x80)
                return false;
        i += count + 1;
    }
    return true;
}

/// Sample data

void Sf2Reader::readData(Source &source, uint32_t start16, uint32_t start24, uint32_t length, int16_t * &data16,
                         uint8_t * &data24)
{
    data16 = nullptr;
    data24 = nullptr;

    // Not allocated if the file is too short (a length read in corrupted data can be huge)
    if (length == 0 || !fits(source, start16, 2 * static_cast<uint64_t>(length)) ||
        (start24 > 0 && !fits(source, start24, length)))
        return;

    data16 = new (std::nothrow) int16_t[length];
    if (data16 == nullptr || !source.readAt(start16, data16, 2 * static_cast<uint64_t>(length)))
    {
        delete [] data16;
        data16 = nullptr;
        return;
    }

    if (start24 > 0)
    {
        data24 = new (std::nothrow) uint8_t[length];
        if (data24 == nullptr || !source.readAt(start24, data24, length))
        {
            delete [] data16;
            delete [] data24;
            data16 = nullptr;
            data24 = nullptr;
        }
    }
}

char * Sf2Reader::readCompressedData(Source &source, uint32_t start, uint32_t length)
{
    if (length == 0 || !fits(source, start, length))
        return nullptr;

    char * data = new (std::nothrow) char[length];
    if (data != nullptr && !source.readAt(start, data, length))
    {
        delete [] data;
        data = nullptr;
    }
    return data;
}

namespace
{
struct VorbisData
{
    const char * data;
    size_t length;
    size_t position;
};
}

static size_t vorbisRead(void * pointer, size_t size, size_t count, void * source)
{
    VorbisData * vorbisData = static_cast<VorbisData *>(source);
    size_t length = size * count;
    if (vorbisData->position >= vorbisData->length)
        length = 0;
    else if (length > vorbisData->length - vorbisData->position)
        length = vorbisData->length - vorbisData->position;
    if (length > 0)
    {
        memcpy(pointer, vorbisData->data + vorbisData->position, length);
        vorbisData->position += length;
    }
    return length;
}

static int vorbisSeek(void * source, ogg_int64_t offset, int whence)
{
    VorbisData * vorbisData = static_cast<VorbisData *>(source);
    ogg_int64_t position = offset;
    if (whence == SEEK_CUR)
        position += static_cast<ogg_int64_t>(vorbisData->position);
    else if (whence == SEEK_END)
        position += static_cast<ogg_int64_t>(vorbisData->length);
    if (position < 0)
        position = 0;
    else if (position > static_cast<ogg_int64_t>(vorbisData->length))
        position = static_cast<ogg_int64_t>(vorbisData->length);
    vorbisData->position = static_cast<size_t>(position);
    return 0;
}

static long vorbisTell(void * source)
{
    return static_cast<long>(static_cast<VorbisData *>(source)->position);
}

int16_t * Sf2Reader::decompress(const char * compressedData, uint32_t compressedLength, uint32_t &length)
{
    length = 0;
    VorbisData vorbisData = { compressedData, compressedData == nullptr ? 0 : compressedLength, 0 };
    ov_callbacks callbacks = { vorbisRead, vorbisSeek, nullptr, vorbisTell };
    OggVorbis_File file;
    if (ov_open_callbacks(&vorbisData, &file, nullptr, 0, callbacks) != 0)
        return nullptr;

    // The length is read in the compressed data, possibly corrupted
    ogg_int64_t pcmLength = ov_pcm_total(&file, -1);
    int16_t * data = nullptr;
    if (pcmLength > 0 && pcmLength <= UINT32_MAX)
        data = new (std::nothrow) int16_t[static_cast<size_t>(pcmLength)];
    if (data == nullptr)
    {
        ov_clear(&file);
        return nullptr;
    }

    // 16-bit little-endian signed values, read by blocks of 2048 bytes and never beyond the length
    char * buffer = reinterpret_cast<char *>(data);
    int64_t expectedLength = 2 * static_cast<int64_t>(pcmLength);
    int64_t readLength = 0;
    long count;
    do
    {
        int blockLength = static_cast<int>(std::min<int64_t>(2048, expectedLength - readLength));
        count = blockLength > 0 ? ov_read(&file, buffer + readLength, blockLength, 0, 2, 1, nullptr) : 0;
        if (count > 0)
            readLength += count;
    } while (count > 0);
    ov_clear(&file);

    if (readLength != expectedLength)
    {
        delete [] data;
        return nullptr;
    }
    length = static_cast<uint32_t>(pcmLength);
    return data;
}
