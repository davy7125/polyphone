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


#include "dlsreader.h"
#include <algorithm>
#include <cstring>
#include "attributebase.h"
#include "dlsarticulation.h"
#include "samplefilereader.h"
#include "synthconstants.h"
#include "textconversions.h"
using namespace TextConversions;

typedef ISoundfontBuilder::Id Id;

static uint16_t fromLittleEndian16(const char * data)
{
    const unsigned char * bytes = reinterpret_cast<const unsigned char *>(data);
    return static_cast<uint16_t>(bytes[0] | (bytes[1] << 8));
}

static uint32_t fromLittleEndian32(const char * data)
{
    const unsigned char * bytes = reinterpret_cast<const unsigned char *>(data);
    return static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) |
           (static_cast<uint32_t>(bytes[2]) << 16) | (static_cast<uint32_t>(bytes[3]) << 24);
}

static uint16_t readUInt16(const std::string &data, int pos)
{
    return pos >= 0 && pos + 2 <= static_cast<int>(data.size()) ? fromLittleEndian16(data.data() + pos) : 0;
}

static uint32_t readUInt32(const std::string &data, int pos)
{
    return pos >= 0 && pos + 4 <= static_cast<int>(data.size()) ? fromLittleEndian32(data.data() + pos) : 0;
}

static void appendUInt32(std::string &array, uint32_t value)
{
    char bytes[4] = {
        static_cast<char>(value & 0xFF), static_cast<char>((value >> 8) & 0xFF),
        static_cast<char>((value >> 16) & 0xFF), static_cast<char>((value >> 24) & 0xFF)
    };
    array.append(bytes, 4);
}

// Bytes available from a position, up to a size
static std::string readAvailable(Sf2Reader::Source &source, int64_t position, uint32_t size)
{
    uint64_t length = source.getLength();
    if (position < 0 || static_cast<uint64_t>(position) >= length)
        return std::string();
    uint64_t count = std::min<uint64_t>(size, length - static_cast<uint64_t>(position));
    std::string data(static_cast<size_t>(count), '\0');
    if (count > 0 && !source.readAt(static_cast<uint64_t>(position), &data[0], count))
        return std::string();
    return data;
}

// Text in UTF-8 if it is valid, otherwise in Latin-1
static std::string toUtf8(const std::string &data)
{
    if (Sf2Reader::isValidUtf8(data.data(), data.size()))
        return data;
    std::string text;
    for (char c : data)
    {
        unsigned char byte = static_cast<unsigned char>(c);
        if (byte < 0x80)
            text += c;
        else
        {
            text += static_cast<char>(0xC0 | (byte >> 6));
            text += static_cast<char>(0x80 | (byte & 0x3F));
        }
    }
    return text;
}

// Name of a file without its directory and its last extension (QFileInfo::completeBaseName)
static std::string getCompleteBaseName(const std::string &filePath)
{
    size_t slash = filePath.find_last_of('/');
    std::string name = (slash == std::string::npos) ? filePath : filePath.substr(slash + 1);
    size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

template <typename T> static T bound(T minimum, T value, T maximum)
{
    return std::max(minimum, std::min(maximum, value));
}

bool DlsReader::read(Sf2Reader::Source &source, const std::string &filePath, ISoundfontBuilder &builder)
{
    _file = &source;
    _fileName = filePath;
    _sm = &builder;
    _info.clear();
    _poolTable.clear();
    _waves.clear();
    _instruments.clear();

    // Parse the file
    if (!parseFile())
        return false;

    // Create a soundfont
    Id idSf2(ISoundfontBuilder::TYPE_SF2);
    _sm->add(idSf2);
    std::string name = _info["INAM"];
    if (name.empty())
        name = getCompleteBaseName(filePath);
    _sm->setText(idSf2, champ_name, name);
    _sm->setText(idSf2, champ_ICOP, _info["ICOP"]);
    _sm->setText(idSf2, champ_IENG, _info.count("IENG") ? _info["IENG"] : _info["IART"]);
    _sm->setText(idSf2, champ_ICRD, _info["ICRD"]);
    _sm->setText(idSf2, champ_IPRD, _info["IPRD"]);
    _sm->setText(idSf2, champ_ISFT, _info["ISFT"]);
    _sm->setText(idSf2, champ_ICMT, _info.count("ICMT") ? _info["ICMT"] : std::string("Sf2 imported from dls by Polyphone"));

    // Samples
    createSamples();
    linkStereoSamples();

    // Instruments and presets
    for (const Instrument &instrument : _instruments)
    {
        std::vector<Zone> zones;
        for (const Region &region : instrument.regions)
        {
            std::vector<Zone> regionZones = getZones(instrument, region);
            zones.insert(zones.end(), regionZones.begin(), regionZones.end());
        }
        createInstrument(instrument, zones);
    }

    // Possible 24-bit mode
    for (const Wave &wave : _waves)
    {
        if (wave.bitsPerSample > 16 && !wave.sampleIds.empty())
        {
            AttributeValue value;
            value.wValue = 24;
            _sm->set(idSf2, champ_wBpsSave, value);
            break;
        }
    }

    _file = nullptr;
    return true;
}

bool DlsReader::parseFile()
{
    char header[12];
    if (!_file->readAt(0, header, 12) || memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "DLS ", 4) != 0)
        return false;
    int64_t end = std::min(static_cast<int64_t>(fromLittleEndian32(header + 4)) + 8, static_cast<int64_t>(_file->getLength()));

    // A conditional chunk may exist at the top level: an editor loads the file anyway
    for (const Chunk &chunk : readChunks(12, end))
    {
        if (chunk.id == "ptbl")
        {
            std::string data = readData(chunk);
            uint32_t headerSize = readUInt32(data, 0);
            uint32_t count = readUInt32(data, 4);
            for (uint32_t i = 0; i < count && headerSize + 4 * i + 4 <= static_cast<uint32_t>(data.size()); i++)
                _poolTable.push_back(readUInt32(data, static_cast<int>(headerSize + 4 * i)));
        }
        else if (chunk.id == "LIST")
        {
            if (chunk.listType == "lins")
            {
                for (const Chunk &subChunk : readSubChunks(chunk))
                    if (subChunk.id == "LIST" && subChunk.listType == "ins ")
                        parseInstrument(subChunk);
            }
            else if (chunk.listType == "wvpl")
                parseWavePool(chunk);
            else if (chunk.listType == "INFO")
                parseInfo(chunk, _info);
        }
    }

    return true;
}

std::vector<DlsReader::Chunk> DlsReader::readChunks(int64_t start, int64_t end)
{
    std::vector<Chunk> chunks;
    int64_t pos = start;
    while (pos + 8 <= end)
    {
        char header[8];
        if (!_file->readAt(static_cast<uint64_t>(pos), header, 8))
            break;

        Chunk chunk;
        chunk.id = std::string(header, 4);
        chunk.size = fromLittleEndian32(header + 4);
        chunk.dataPosition = pos + 8;
        if (chunk.dataPosition + chunk.size > end)
            chunk.size = static_cast<uint32_t>(end - chunk.dataPosition); // Truncated chunk
        if (chunk.id == "LIST" || chunk.id == "RIFF")
        {
            if (chunk.size < 4 || !_file->readAt(static_cast<uint64_t>(pos + 8), header, 4))
                break;
            chunk.listType = std::string(header, 4);
        }
        chunks.push_back(chunk);

        pos = chunk.dataPosition + chunk.size + (chunk.size % 2);
    }
    return chunks;
}

std::vector<DlsReader::Chunk> DlsReader::readSubChunks(const Chunk &list)
{
    return readChunks(list.dataPosition + 4, list.dataPosition + list.size);
}

std::string DlsReader::readData(const Chunk &chunk)
{
    return readAvailable(*_file, chunk.dataPosition, chunk.size);
}

void DlsReader::parseInfo(const Chunk &list, std::map<std::string, std::string> &info)
{
    for (const Chunk &chunk : readSubChunks(list))
    {
        if (chunk.id == "LIST")
            continue;

        // Text ending with one or more '\0'
        std::string data = readData(chunk);
        size_t nullIndex = data.find('\0');
        if (nullIndex != std::string::npos)
            data.resize(nullIndex);
        info[chunk.id] = trimmed(toUtf8(data));
    }
}

void DlsReader::parseWavePool(const Chunk &list)
{
    // Offsets of the pool table are relative to the data following "wvpl"
    int64_t base = list.dataPosition + 4;
    for (const Chunk &chunk : readSubChunks(list))
        if (chunk.id == "LIST" && chunk.listType == "wave")
            parseWave(chunk, static_cast<uint32_t>(chunk.dataPosition - 8 - base));
}

void DlsReader::parseWave(const Chunk &list, uint32_t poolOffset)
{
    Wave wave;
    wave.listPosition = static_cast<uint32_t>(list.dataPosition - 8);
    wave.poolOffset = poolOffset;

    for (const Chunk &chunk : readSubChunks(list))
    {
        if (chunk.id == "fmt ")
        {
            std::string data = readData(chunk);
            wave.formatTag = readUInt16(data, 0);
            wave.channels = readUInt16(data, 2);
            wave.sampleRate = readUInt32(data, 4);
            wave.blockAlign = readUInt16(data, 12);
            wave.bitsPerSample = readUInt16(data, 14);
        }
        else if (chunk.id == "data")
            wave.dataSize = chunk.size;
        else if (chunk.id == "fact")
        {
            wave.hasFact = true;
            wave.factLength = readUInt32(readData(chunk), 0);
        }
        else if (chunk.id == "wsmp")
            wave.waveSample = parseWaveSample(readData(chunk));
        else if (chunk.id == "LIST" && chunk.listType == "INFO")
        {
            std::map<std::string, std::string> info;
            parseInfo(chunk, info);
            wave.name = info["INAM"];
        }
    }

    // Number of frames
    if ((wave.formatTag == 0x0001 || wave.formatTag == 0x0003 || wave.formatTag == 0x0006 ||
         wave.formatTag == 0x0007 || wave.formatTag == 0xFFFE) && wave.blockAlign > 0)
    {
        // PCM, float, A-law, µ-law or extensible
        wave.length = wave.dataSize / wave.blockAlign;
    }
    else if (wave.hasFact)
        wave.length = wave.factLength;
    else
    {
        // Compressed format
        uint16_t channels;
        uint32_t sampleRate;
        if (readWaveInfo(*_file, wave.listPosition, wave.length, channels, sampleRate) != WAVE_OK)
            wave.channels = 0; // Not supported
    }

    _waves.push_back(wave);
}

void DlsReader::parseInstrument(const Chunk &list)
{
    std::vector<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return;

    Instrument instrument;
    std::vector<Dls::Connection> connectionsLevel1, connectionsLevel2;
    for (const Chunk &chunk : chunks)
    {
        if (chunk.id == "insh")
        {
            std::string data = readData(chunk);
            instrument.bank = readUInt32(data, 4);
            instrument.program = readUInt32(data, 8);
        }
        else if (chunk.id == "LIST")
        {
            if (chunk.listType == "lrgn")
            {
                for (const Chunk &subChunk : readSubChunks(chunk))
                    if (subChunk.id == "LIST" && (subChunk.listType == "rgn " || subChunk.listType == "rgn2"))
                        parseRegion(subChunk, instrument);
            }
            else if (chunk.listType == "lart" || chunk.listType == "lar2")
                parseArticulationList(chunk, connectionsLevel1, connectionsLevel2);
            else if (chunk.listType == "INFO")
            {
                std::map<std::string, std::string> info;
                parseInfo(chunk, info);
                instrument.name = info["INAM"];
            }
        }
    }

    // The level 2 connections take precedence over the identical level 1 connections
    instrument.connections = connectionsLevel1;
    instrument.connections.insert(instrument.connections.end(), connectionsLevel2.begin(), connectionsLevel2.end());
    _instruments.push_back(instrument);
}

void DlsReader::parseRegion(const Chunk &list, Instrument &instrument)
{
    std::vector<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return;

    Region region;
    std::vector<Dls::Connection> connectionsLevel1, connectionsLevel2;
    for (const Chunk &chunk : chunks)
    {
        if (chunk.id == "rgnh")
        {
            std::string data = readData(chunk);
            region.keyMin = readUInt16(data, 0);
            region.keyMax = readUInt16(data, 2);
            region.velMin = readUInt16(data, 4);
            region.velMax = readUInt16(data, 6);
            region.keyGroup = readUInt16(data, 10);
        }
        else if (chunk.id == "wsmp")
            region.waveSample = parseWaveSample(readData(chunk));
        else if (chunk.id == "wlnk")
        {
            std::string data = readData(chunk);
            region.hasWaveLink = true;
            region.linkOptions = readUInt16(data, 0);
            region.phaseGroup = readUInt16(data, 2);
            region.channel = readUInt32(data, 4);
            region.tableIndex = readUInt32(data, 8);
        }
        else if (chunk.id == "LIST" && (chunk.listType == "lart" || chunk.listType == "lar2"))
        {
            if (parseArticulationList(chunk, connectionsLevel1, connectionsLevel2))
                region.hasArticulation = true;
        }
    }

    if (region.hasWaveLink)
    {
        region.connections = connectionsLevel1;
        region.connections.insert(region.connections.end(), connectionsLevel2.begin(), connectionsLevel2.end());
        instrument.regions.push_back(region);
    }
}

bool DlsReader::parseArticulationList(const Chunk &list, std::vector<Dls::Connection> &connectionsLevel1,
                                      std::vector<Dls::Connection> &connectionsLevel2)
{
    std::vector<Chunk> chunks = readSubChunks(list);
    if (!isListConditionTrue(chunks))
        return false;

    bool found = false;
    for (const Chunk &chunk : chunks)
    {
        if (chunk.id == "art1")
        {
            parseArticulation(readData(chunk), true, connectionsLevel1);
            found = true;
        }
        else if (chunk.id == "art2")
        {
            parseArticulation(readData(chunk), false, connectionsLevel2);
            found = true;
        }
    }
    return found;
}

void DlsReader::parseArticulation(const std::string &data, bool isLevel1, std::vector<Dls::Connection> &connections)
{
    uint32_t headerSize = std::max(readUInt32(data, 0), 8u);
    uint32_t count = readUInt32(data, 4);
    for (uint32_t i = 0; i < count; i++)
    {
        int pos = static_cast<int>(headerSize + 12 * i);
        if (pos + 12 > static_cast<int>(data.size()))
            break;

        Dls::Connection connection;
        connection.source = readUInt16(data, pos);
        connection.control = readUInt16(data, pos + 2);
        connection.destination = readUInt16(data, pos + 4);
        connection.transform = readUInt16(data, pos + 6);
        connection.scale = static_cast<int32_t>(readUInt32(data, pos + 8));

        if (isLevel1)
        {
            // Only the output transform exists in DLS level 1, the pitch wheel and the pan being bipolar
            connection.transform &= 0x000F;
            if (connection.source == Dls::SRC_PITCHWHEEL || connection.source == Dls::SRC_CC10)
                connection.transform |= 0x4000;
        }
        connections.push_back(connection);
    }
}

DlsReader::WaveSample DlsReader::parseWaveSample(const std::string &data)
{
    WaveSample waveSample;
    waveSample.isDefined = true;
    uint32_t headerSize = std::max(readUInt32(data, 0), 20u);
    waveSample.unityNote = readUInt16(data, 4);
    waveSample.fineTune = static_cast<int16_t>(readUInt16(data, 6));
    waveSample.gain = static_cast<int32_t>(readUInt32(data, 8));
    uint32_t loopCount = readUInt32(data, 16);
    if (loopCount > 0 && headerSize + 16 <= static_cast<uint32_t>(data.size()))
    {
        // Only the first loop is used
        waveSample.loopType = readUInt32(data, static_cast<int>(headerSize + 4));
        waveSample.loopStart = readUInt32(data, static_cast<int>(headerSize + 8));
        waveSample.loopLength = readUInt32(data, static_cast<int>(headerSize + 12));
        waveSample.hasLoop = waveSample.loopLength > 0;
    }
    return waveSample;
}

bool DlsReader::isListConditionTrue(const std::vector<Chunk> &subChunks)
{
    for (const Chunk &chunk : subChunks)
        if (chunk.id == "cdl ")
            return isConditionTrue(readData(chunk));
    return true;
}

bool DlsReader::isConditionTrue(const std::string &data)
{
    // Expression in reverse polish notation, evaluated as a DLS level 2 device
    std::vector<uint32_t> stack;
    int pos = 0;
    while (pos + 2 <= static_cast<int>(data.size()))
    {
        uint16_t opcode = readUInt16(data, pos);
        pos += 2;

        if (opcode == 0x0010) // Constant
        {
            stack.push_back(readUInt32(data, pos));
            pos += 4;
        }
        else if (opcode == 0x0011 || opcode == 0x0012) // Query (value or support)
        {
            bool isSupported;
            uint32_t value = queryDevice(readUInt32(data, pos), isSupported);
            pos += 16;
            if (opcode == 0x0011)
                stack.push_back(isSupported ? value : 0);
            else
                stack.push_back(isSupported ? 0xFFFFFFFF : 0);
        }
        else if (opcode == 0x000F) // Not
        {
            if (stack.empty())
                return true;
            uint32_t value = stack.back();
            stack.pop_back();
            stack.push_back(value ? 0 : 0xFFFFFFFF);
        }
        else if (opcode >= 0x0001 && opcode <= 0x000E) // Binary operators
        {
            if (stack.size() < 2)
                return true;
            uint32_t x = stack.back();
            stack.pop_back();
            uint32_t y = stack.back();
            stack.pop_back();
            uint32_t result = 0;
            switch (opcode)
            {
            case 0x0001: result = x & y; break;
            case 0x0002: result = x | y; break;
            case 0x0003: result = x ^ y; break;
            case 0x0004: result = x + y; break;
            case 0x0005: result = x - y; break;
            case 0x0006: result = x * y; break;
            case 0x0007: result = y == 0 ? 0 : x / y; break;
            case 0x0008: result = (x && y) ? 0xFFFFFFFF : 0; break;
            case 0x0009: result = (x || y) ? 0xFFFFFFFF : 0; break;
            case 0x000A: result = x < y ? 0xFFFFFFFF : 0; break;
            case 0x000B: result = x <= y ? 0xFFFFFFFF : 0; break;
            case 0x000C: result = x > y ? 0xFFFFFFFF : 0; break;
            case 0x000D: result = x >= y ? 0xFFFFFFFF : 0; break;
            case 0x000E: result = x == y ? 0xFFFFFFFF : 0; break;
            default: break;
            }
            stack.push_back(result);
        }
        else
            return true; // Unknown opcode: the list is used
    }

    return stack.empty() || stack.back() != 0;
}

uint32_t DlsReader::queryDevice(uint32_t queryId, bool &isSupported)
{
    // Queries are identified by the first field of their GUID
    isSupported = true;
    switch (queryId)
    {
    case 0x178f2f27: // Supports DLS level 1
    case 0xf14599e5: // Supports DLS level 2
    case 0x178f2f28: // Sample memory size
        return 0xFFFFFFFF;
    case 0x178f2f24: // GM in hardware
    case 0x178f2f25: // GS in hardware
    case 0x178f2f26: // XG in hardware
    case 0xb03e1181: // Manufacturer ID
    case 0xb03e1182: // Product ID
        return 0;
    case 0x2a91f713: // Sample playback rate
        return 44100;
    default:
        break;
    }
    isSupported = false;
    return 0;
}

int DlsReader::getWaveIndex(uint32_t tableIndex)
{
    if (tableIndex < static_cast<uint32_t>(_poolTable.size()))
    {
        uint32_t offset = _poolTable[tableIndex];
        for (size_t i = 0; i < _waves.size(); i++)
            if (_waves[i].poolOffset == offset)
                return static_cast<int>(i);
    }

    // No pool table or wrong offset: the index is the position of the wave
    return tableIndex < static_cast<uint32_t>(_waves.size()) ? static_cast<int>(tableIndex) : -1;
}

void DlsReader::createSamples()
{
    // Pitches and loops used by the regions for each wave, so that the samples have the most frequent ones
    std::map<int, std::map<std::pair<int, int>, int>> pitchCounts;
    std::map<int, std::map<std::pair<uint32_t, uint32_t>, int>> loopCounts;
    for (const Instrument &instrument : _instruments)
    {
        for (const Region &region : instrument.regions)
        {
            int waveIndex = getWaveIndex(region.tableIndex);
            if (waveIndex == -1)
                continue;
            const WaveSample &waveSample = region.waveSample.isDefined ? region.waveSample :
                                                                         _waves[static_cast<size_t>(waveIndex)].waveSample;
            pitchCounts[waveIndex][std::pair<int, int>(waveSample.unityNote, waveSample.fineTune)]++;
            if (waveSample.hasLoop)
                loopCounts[waveIndex][std::pair<uint32_t, uint32_t>(waveSample.loopStart, waveSample.loopLength)]++;
        }
    }

    Id id(ISoundfontBuilder::TYPE_SAMPLE);
    AttributeValue value;
    for (size_t i = 0; i < _waves.size(); i++)
    {
        Wave &wave = _waves[i];
        if (wave.channels == 0)
            continue;

        // Pitch
        std::pair<int, int> pitch(wave.waveSample.unityNote, wave.waveSample.fineTune);
        int maxCount = 0;
        for (const auto &count : pitchCounts[static_cast<int>(i)])
        {
            if (count.second > maxCount)
            {
                maxCount = count.second;
                pitch = count.first;
            }
        }
        wave.rootKey = bound(0, pitch.first, 127);
        wave.correction = bound(-99, pitch.second, 99);

        // Loop
        std::pair<uint32_t, uint32_t> loop(wave.waveSample.loopStart, wave.waveSample.hasLoop ? wave.waveSample.loopLength : 0);
        maxCount = 0;
        for (const auto &count : loopCounts[static_cast<int>(i)])
        {
            if (count.second > maxCount)
            {
                maxCount = count.second;
                loop = count.first;
            }
        }
        if (loop.second > 0)
        {
            wave.loopStart = std::min(loop.first, wave.length);
            wave.loopEnd = static_cast<uint32_t>(std::min(static_cast<int64_t>(loop.first) + loop.second,
                                                          static_cast<int64_t>(wave.length)));
        }

        std::string name = wave.name.empty() ? "wave " + std::to_string(i + 1) : wave.name;
        for (int channel = 0; channel < wave.channels; channel++)
        {
            id.element = _sm->add(id);
            if (wave.channels == 2)
                _sm->setText(id, champ_name, utf16Left(name, 19) + (channel == 0 ? "L" : "R"));
            else if (wave.channels > 2)
                _sm->setText(id, champ_name, utf16Left(name, 17) + "-" + std::to_string(channel + 1));
            else
                _sm->setText(id, champ_name, name);
            _sm->setText(id, champ_filenameForData, _fileName);

            value.dwValue = wave.listPosition;
            _sm->setSoundProperty(id, champ_dwStart16, value);
            value.dwValue = 0;
            _sm->setSoundProperty(id, champ_dwStart24, value);
            value.wValue = wave.bitsPerSample > 16 ? 24 : 16;
            _sm->setSoundProperty(id, champ_bpsFile, value);
            value.wValue = wave.channels;
            _sm->setSoundProperty(id, champ_wChannels, value);
            value.wValue = static_cast<uint16_t>(channel);
            _sm->setSoundProperty(id, champ_wChannel, value);
            value.dwValue = wave.length;
            _sm->setSoundProperty(id, champ_dwLength, value);
            value.dwValue = wave.sampleRate > 0 ? wave.sampleRate : 44100;
            _sm->setSoundProperty(id, champ_dwSampleRate, value);
            value.bValue = static_cast<uint8_t>(wave.rootKey);
            _sm->setSoundProperty(id, champ_byOriginalPitch, value);
            value.cValue = static_cast<int8_t>(wave.correction);
            _sm->setSoundProperty(id, champ_chPitchCorrection, value);
            value.dwValue = wave.loopStart;
            _sm->setSoundProperty(id, champ_dwStartLoop, value);
            value.dwValue = wave.loopEnd;
            _sm->setSoundProperty(id, champ_dwEndLoop, value);
            value.cValue = 0;
            _sm->setSoundProperty(id, champ_rawDataAvailable, value);

            if (wave.channels == 2)
                value.sfLinkValue = (channel == 0 ? leftSample : rightSample);
            else
                value.sfLinkValue = monoSample;
            _sm->set(id, champ_sfSampleType, value);

            wave.sampleIds.push_back(id.element);
        }

        // Stereo wave
        if (wave.channels == 2)
        {
            value.wValue = static_cast<uint16_t>(wave.sampleIds[1]);
            _sm->set(Id(ISoundfontBuilder::TYPE_SAMPLE, wave.sampleIds[0]), champ_wSampleLink, value);
            value.wValue = static_cast<uint16_t>(wave.sampleIds[0]);
            _sm->set(Id(ISoundfontBuilder::TYPE_SAMPLE, wave.sampleIds[1]), champ_wSampleLink, value);
        }
    }
}

void DlsReader::linkStereoSamples()
{
    // Mono waves played together in the left and right channels with the same phase group form a stereo sample
    for (const Instrument &instrument : _instruments)
    {
        for (size_t i = 0; i < instrument.regions.size(); i++)
        {
            const Region &left = instrument.regions[i];
            if (left.phaseGroup == 0 || left.channel != Dls::WAVELINK_CHANNEL_LEFT)
                continue;
            int leftWave = getWaveIndex(left.tableIndex);

            for (size_t j = 0; j < instrument.regions.size(); j++)
            {
                const Region &right = instrument.regions[j];
                if (right.phaseGroup != left.phaseGroup || right.channel != Dls::WAVELINK_CHANNEL_RIGHT)
                    continue;
                int rightWave = getWaveIndex(right.tableIndex);
                if (leftWave == -1 || rightWave == -1 || leftWave == rightWave ||
                    _waves[static_cast<size_t>(leftWave)].sampleIds.size() != 1 ||
                    _waves[static_cast<size_t>(rightWave)].sampleIds.size() != 1)
                    continue;

                Id idLeft(ISoundfontBuilder::TYPE_SAMPLE, _waves[static_cast<size_t>(leftWave)].sampleIds[0]);
                Id idRight(ISoundfontBuilder::TYPE_SAMPLE, _waves[static_cast<size_t>(rightWave)].sampleIds[0]);
                if (_sm->get(idLeft, champ_sfSampleType).sfLinkValue != monoSample ||
                    _sm->get(idRight, champ_sfSampleType).sfLinkValue != monoSample)
                    continue;

                AttributeValue value;
                value.sfLinkValue = leftSample;
                _sm->set(idLeft, champ_sfSampleType, value);
                value.sfLinkValue = rightSample;
                _sm->set(idRight, champ_sfSampleType, value);
                value.wValue = static_cast<uint16_t>(idRight.element);
                _sm->set(idLeft, champ_wSampleLink, value);
                value.wValue = static_cast<uint16_t>(idLeft.element);
                _sm->set(idRight, champ_wSampleLink, value);
                break;
            }
        }
    }
}

std::vector<DlsReader::Zone> DlsReader::getZones(const Instrument &instrument, const Region &region)
{
    std::vector<Zone> zones;
    int waveIndex = getWaveIndex(region.tableIndex);
    if (waveIndex == -1 || _waves[static_cast<size_t>(waveIndex)].sampleIds.empty())
        return zones;
    const Wave &wave = _waves[static_cast<size_t>(waveIndex)];

    // The wave sample of the region replaces the one of the wave, the local articulation replaces the global one
    const WaveSample &waveSample = region.waveSample.isDefined ? region.waveSample : wave.waveSample;
    DlsArticulation articulation(region.hasArticulation ? region.connections : instrument.connections);

    // Channels to play: a multichannel wave is completely played, unless other regions play the other channels
    std::vector<int> channels;
    bool allChannels = false;
    if (wave.sampleIds.size() == 1)
        channels.push_back(0);
    else
    {
        bool otherChannelPlayed = false;
        for (const Region &other : instrument.regions)
        {
            if (&other != &region && getWaveIndex(other.tableIndex) == waveIndex && other.channel != region.channel)
            {
                otherChannelPlayed = true;
                break;
            }
        }
        if (otherChannelPlayed)
            channels.push_back(region.channel == Dls::WAVELINK_CHANNEL_RIGHT ? 1 : 0);
        else
        {
            for (size_t i = 0; i < wave.sampleIds.size(); i++)
                channels.push_back(static_cast<int>(i));
            allChannels = true;
        }
    }

    AttributeValue value;
    for (int channel : channels)
    {
        Zone zone;
        zone.sampleId = wave.sampleIds[static_cast<size_t>(channel)];

        // Articulation
        std::map<AttributeType, int16_t> generators = articulation.getGenerators();
        for (const auto &generator : generators)
        {
            value.shValue = generator.second;
            zone.generators[generator.first] = value;
        }
        zone.modulators = articulation.getModulators();

        // Ranges, a velocity range 0-0 meaning that it is not used (DLS level 1)
        value.rValue.byLo = static_cast<uint8_t>(std::min(region.keyMin, region.keyMax) & 0x7F);
        value.rValue.byHi = static_cast<uint8_t>(std::min(std::max(region.keyMin, region.keyMax), static_cast<uint16_t>(127)));
        zone.generators[champ_keyRange] = value;
        if (region.velMax > 0)
        {
            value.rValue.byLo = static_cast<uint8_t>(std::min(region.velMin, region.velMax) & 0x7F);
            value.rValue.byHi = static_cast<uint8_t>(std::min(std::max(region.velMin, region.velMax), static_cast<uint16_t>(127)));
            zone.generators[champ_velRange] = value;
        }

        // Key group
        if (region.keyGroup > 0)
        {
            value.shValue = static_cast<int16_t>(std::min(region.keyGroup, static_cast<uint16_t>(127)));
            zone.generators[champ_exclusiveClass] = value;
        }

        // Root key and tuning
        int unityNote = bound(0, static_cast<int>(waveSample.unityNote), 127);
        if (unityNote != wave.rootKey)
        {
            value.shValue = static_cast<int16_t>(unityNote);
            zone.generators[champ_overridingRootKey] = value;
        }
        int tuning = qtRound(waveSample.fineTune + articulation.getTuning()) - wave.correction;
        int coarseTune = bound(-120, tuning / 100, 120);
        int fineTune = bound(-99, tuning - 100 * coarseTune, 99);
        if (coarseTune != 0)
        {
            value.shValue = static_cast<int16_t>(coarseTune);
            zone.generators[champ_coarseTune] = value;
        }
        if (fineTune != 0)
        {
            value.shValue = static_cast<int16_t>(fineTune);
            zone.generators[champ_fineTune] = value;
        }

        // Attenuation in cB (the sf2 attenuations are multiplied by 0.4 in the synthesizers)
        zone.attenuation = -(waveSample.gain / Dls::UNIT_SCALE + articulation.getGain()) / DB_SF2_TO_REAL_DB;

        // Loop
        if (waveSample.hasLoop)
        {
            value.shValue = waveSample.loopType == Dls::WLOOP_TYPE_RELEASE ? 3 : 1;
            zone.generators[champ_sampleModes] = value;
            setOffset(zone, champ_startloopAddrsOffset, champ_startloopAddrsCoarseOffset,
                      static_cast<int64_t>(waveSample.loopStart) - wave.loopStart);
            setOffset(zone, champ_endloopAddrsOffset, champ_endloopAddrsCoarseOffset,
                      static_cast<int64_t>(waveSample.loopStart) + waveSample.loopLength - wave.loopEnd);
        }

        // Pan of the channels
        if (allChannels && channel < 2)
        {
            value.shValue = (channel == 0 ? -500 : 500);
            zone.generators[champ_pan] = value;
        }
        else if (region.linkOptions & Dls::F_WAVELINK_MULTICHANNEL)
        {
            // The channel replaces the pan of the articulation
            if (region.channel == Dls::WAVELINK_CHANNEL_RIGHT)
            {
                value.shValue = 500;
                zone.generators[champ_pan] = value;
            }
            else if (region.channel == Dls::WAVELINK_CHANNEL_LEFT && region.phaseGroup != 0)
            {
                value.shValue = -500;
                zone.generators[champ_pan] = value;
            }
        }

        zones.push_back(zone);
    }

    return zones;
}

void DlsReader::setOffset(Zone &zone, AttributeType fineChamp, AttributeType coarseChamp, int64_t offset)
{
    AttributeValue value;
    int64_t coarse = offset / 32768;
    int64_t fine = offset - 32768 * coarse;
    if (coarse != 0)
    {
        value.shValue = static_cast<int16_t>(bound(static_cast<int64_t>(-32767), coarse, static_cast<int64_t>(32767)));
        zone.generators[coarseChamp] = value;
    }
    if (fine != 0)
    {
        value.shValue = static_cast<int16_t>(fine);
        zone.generators[fineChamp] = value;
    }
}

void DlsReader::createInstrument(const Instrument &instrument, std::vector<Zone> &zones)
{
    // A positive gain is a negative attenuation, only possible at the preset level
    double minAttenuation = 0;
    for (const Zone &zone : zones)
        minAttenuation = std::min(minAttenuation, zone.attenuation);
    int presetAttenuation = std::max(-1440, qtRound(minAttenuation));

    // Instrument
    Id idInst(ISoundfontBuilder::TYPE_INSTRUMENT);
    int number = static_cast<int>(_sm->getSiblings(idInst).size()) + 1;
    std::string name = instrument.name.empty() ? "instrument " + std::to_string(number) : instrument.name;
    idInst.element = _sm->add(idInst);
    _sm->setText(idInst, champ_name, name);

    // Modulators common to all divisions are moved in the global division
    std::vector<ModulatorData> globalModulators;
    if (!zones.empty())
    {
        std::vector<ModulatorData> firstModulators = zones[0].modulators;
        for (const ModulatorData &modulator : firstModulators)
        {
            bool isEverywhere = true;
            for (size_t i = 1; i < zones.size() && isEverywhere; i++)
            {
                bool found = false;
                for (const ModulatorData &other : zones[i].modulators)
                {
                    if (other == modulator && other.amount == modulator.amount)
                    {
                        found = true;
                        break;
                    }
                }
                isEverywhere = found;
            }

            if (isEverywhere)
            {
                globalModulators.push_back(modulator);
                for (size_t i = 0; i < zones.size(); i++)
                {
                    for (size_t j = 0; j < zones[i].modulators.size(); j++)
                    {
                        if (zones[i].modulators[j] == modulator)
                        {
                            zones[i].modulators.erase(zones[i].modulators.begin() + static_cast<long>(j));
                            break;
                        }
                    }
                }
            }
        }
    }

    // Modulators compensating the default modulators of the sf2 format, unless they are already overridden
    for (const ModulatorData &modulator : DlsArticulation::getDefaultModulatorCompensation())
        if (std::find(globalModulators.begin(), globalModulators.end(), modulator) == globalModulators.end())
            globalModulators.push_back(modulator);

    int modulatorIndex = 0;
    writeModulators(_sm, idInst, globalModulators, modulatorIndex);

    // Divisions
    Id idInstSmpl(ISoundfontBuilder::TYPE_INSTRUMENT_ZONE, idInst.element);
    AttributeValue value;
    for (const Zone &zone : zones)
    {
        idInstSmpl.zone = _sm->add(idInstSmpl);
        value.wValue = static_cast<uint16_t>(zone.sampleId);
        _sm->set(idInstSmpl, champ_sampleID, value);

        for (const auto &generator : zone.generators)
            if (generator.second.dwValue != AttributeBase::getDefaultStoredValue(generator.first, false).dwValue)
                _sm->set(idInstSmpl, generator.first, generator.second);

        value.shValue = static_cast<int16_t>(bound(0, qtRound(zone.attenuation) - presetAttenuation, 1440));
        if (value.shValue != 0)
            _sm->set(idInstSmpl, champ_initialAttenuation, value);

        writeModulators(_sm, idInstSmpl, zone.modulators, modulatorIndex);
    }

    // Parameters common to all divisions are moved in the global division
    for (int champ = 0; champ < champ_endOper; champ++)
    {
        switch (champ)
        {
        case champ_keyRange: case champ_velRange: case champ_sampleID: case champ_instrument:
        case champ_unused1: case champ_unused2: case champ_unused3: case champ_unused4: case champ_unused5:
        case champ_reserved1: case champ_reserved2: case champ_reserved3:
            break;
        default:
            _sm->simplify(idInst, static_cast<AttributeType>(champ));
            break;
        }
    }

    // Preset, drums being in the bank 128
    int bank;
    int preset = instrument.program & 0x7F;
    if (instrument.bank & Dls::F_INSTRUMENT_DRUMS)
        bank = 128;
    else
    {
        int msb = (instrument.bank >> 8) & 0x7F;
        int lsb = instrument.bank & 0x7F;
        bank = (msb != 0 ? msb : lsb);
    }
    if (!_sm->isAvailable(bank, preset))
    {
        int availablePreset = _sm->closestAvailablePreset(bank, preset);
        if (availablePreset != -1)
            preset = availablePreset;
    }

    Id idPrst(ISoundfontBuilder::TYPE_PRESET);
    idPrst.element = _sm->add(idPrst);
    _sm->setText(idPrst, champ_name, name);
    value.wValue = static_cast<uint16_t>(bank);
    _sm->set(idPrst, champ_wBank, value);
    value.wValue = static_cast<uint16_t>(preset);
    _sm->set(idPrst, champ_wPreset, value);

    Id idPrstInst(ISoundfontBuilder::TYPE_PRESET_ZONE, idPrst.element);
    idPrstInst.zone = _sm->add(idPrstInst);
    value.wValue = static_cast<uint16_t>(idInst.element);
    _sm->set(idPrstInst, champ_instrument, value);
    if (presetAttenuation < 0)
    {
        value.shValue = static_cast<int16_t>(presetAttenuation);
        _sm->set(idPrstInst, champ_initialAttenuation, value);
    }
}

void DlsReader::writeModulators(ISoundfontBuilder * sm, Id id, const std::vector<ModulatorData> &modulators, int &index)
{
    id.type = (id.type == ISoundfontBuilder::TYPE_INSTRUMENT ? ISoundfontBuilder::TYPE_INSTRUMENT_MOD :
                                                              ISoundfontBuilder::TYPE_INSTRUMENT_ZONE_MOD);
    AttributeValue value;
    for (const ModulatorData &modulator : modulators)
    {
        id.modulator = sm->add(id);
        value.sfModValue = modulator.srcOper;
        sm->set(id, champ_sfModSrcOper, value);
        value.wValue = modulator.destOper;
        sm->set(id, champ_sfModDestOper, value);
        value.shValue = modulator.amount;
        sm->set(id, champ_modAmount, value);
        value.sfModValue = modulator.amtSrcOper;
        sm->set(id, champ_sfModAmtSrcOper, value);
        value.sfTransValue = modulator.transOper;
        sm->set(id, champ_sfModTransOper, value);
        value.wValue = static_cast<uint16_t>(index++);
        sm->set(id, champ_indexMod, value);
    }
}

bool DlsReader::extractWaveFile(Sf2Reader::Source &source, uint32_t waveListPosition, std::string &waveFile)
{
    // Header of the list "wave"
    char header[12];
    if (!source.readAt(waveListPosition, header, 12))
        return false;
    if (memcmp(header, "LIST", 4) != 0 || memcmp(header + 8, "wave", 4) != 0)
        return false;
    uint32_t listSize = fromLittleEndian32(header + 4);
    int64_t listEnd = static_cast<int64_t>(waveListPosition) + 8 + listSize;

    // Copy the chunks that are understood by libsndfile
    std::string fmtChunk, factChunk, dataChunk;
    int64_t pos = waveListPosition + 12;
    while (pos + 8 <= listEnd)
    {
        char chunkHeader[8];
        if (!source.readAt(static_cast<uint64_t>(pos), chunkHeader, 8))
            return false;
        uint32_t chunkSize = fromLittleEndian32(chunkHeader + 4);
        if (pos + 8 + chunkSize > listEnd)
            chunkSize = static_cast<uint32_t>(listEnd - pos - 8); // Truncated chunk

        std::string * target = nullptr;
        if (memcmp(chunkHeader, "fmt ", 4) == 0)
            target = &fmtChunk;
        else if (memcmp(chunkHeader, "fact", 4) == 0)
            target = &factChunk;
        else if (memcmp(chunkHeader, "data", 4) == 0)
            target = &dataChunk;

        if (target != nullptr)
        {
            *target = std::string(chunkHeader, 4);
            appendUInt32(*target, chunkSize);
            std::string content = readAvailable(source, pos + 8, chunkSize);
            if (content.size() != static_cast<size_t>(chunkSize))
                return false;
            target->append(content);
            if (chunkSize % 2)
                target->push_back('\0');
        }

        pos += 8 + chunkSize + (chunkSize % 2);
    }
    if (fmtChunk.empty() || dataChunk.empty())
        return false;

    // Wav file
    waveFile = "RIFF";
    appendUInt32(waveFile, static_cast<uint32_t>(4 + fmtChunk.size() + factChunk.size() + dataChunk.size()));
    waveFile.append("WAVE", 4);
    waveFile.append(fmtChunk);
    waveFile.append(factChunk);
    waveFile.append(dataChunk);
    return true;
}

DlsReader::WaveResult DlsReader::readWaveData(Sf2Reader::Source &source, uint32_t waveListPosition, uint16_t channel,
                                              int16_t * &data16, uint8_t * &data24, uint32_t &length)
{
    data16 = nullptr;
    data24 = nullptr;
    length = 0;

    // Wav file in memory, decoded
    std::string waveFile;
    if (!extractWaveFile(source, waveListPosition, waveFile))
        return WAVE_CORRUPT;
    std::unique_ptr<SampleFileReader> reader = SampleFileReader::openMemory(waveFile.data(), waveFile.size());
    if (reader == nullptr)
        return WAVE_NOT_SUPPORTED;
    length = reader->readChannelAsIntegers(channel, data16, data24);
    return WAVE_OK;
}

DlsReader::WaveResult DlsReader::readWaveInfo(Sf2Reader::Source &source, uint32_t waveListPosition, uint32_t &length,
                                              uint16_t &channels, uint32_t &sampleRate)
{
    std::string waveFile;
    if (!extractWaveFile(source, waveListPosition, waveFile))
        return WAVE_CORRUPT;
    std::unique_ptr<SampleFileReader> reader = SampleFileReader::openMemory(waveFile.data(), waveFile.size());
    if (reader == nullptr)
        return WAVE_NOT_SUPPORTED;
    SampleFileReader::Info info;
    reader->getInfo(info);
    length = info.length;
    channels = info.channels;
    sampleRate = info.sampleRate;
    return WAVE_OK;
}
