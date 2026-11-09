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


#ifndef DLSREADER_H
#define DLSREADER_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "dlsdefinitions.h"
#include "modulatordata.h"
#include "sf2reader.h"
#include "soundfontbuilder.h"

// Reader of a DLS file (levels 1 and 2), converted into a soundfont by the editor and by the plugin. The samples keep
// the position of their wave in the file (dwStart16), whose data are read with readWaveData.
class DlsReader
{
public:
    enum WaveResult
    {
        WAVE_OK,
        WAVE_CORRUPT,
        WAVE_NOT_SUPPORTED
    };

    // Read a file into a new soundfont (the builder receiving first the addition of the soundfont): false if it is not
    // a DLS file. The path (UTF-8) is the one given to the samples (champ_filenameForData) and the name of the
    // soundfont if the file has none.
    bool read(Sf2Reader::Source &source, const std::string &filePath, ISoundfontBuilder &builder);

    // Data of a channel of a wave (list "wave" at a position), all values being read as integers: arrays allocated
    // (new []) and number of frames read
    static WaveResult readWaveData(Sf2Reader::Source &source, uint32_t waveListPosition, uint16_t channel,
                                   int16_t * &data16, uint8_t * &data24, uint32_t &length);

    // Number of frames, channels and sample rate of a wave
    static WaveResult readWaveInfo(Sf2Reader::Source &source, uint32_t waveListPosition, uint32_t &length,
                                   uint16_t &channels, uint32_t &sampleRate);

private:
    struct Chunk
    {
        std::string id;
        std::string listType; // For the chunks "RIFF" and "LIST"
        int64_t dataPosition;
        uint32_t size;
    };

    struct WaveSample
    {
        bool isDefined = false;
        uint16_t unityNote = 60;
        int16_t fineTune = 0;
        int32_t gain = 0;
        bool hasLoop = false;
        uint32_t loopType = 0;
        uint32_t loopStart = 0;
        uint32_t loopLength = 0;
    };

    struct Wave
    {
        uint32_t listPosition = 0; // Position of the chunk "LIST wave" in the file
        uint32_t poolOffset = 0; // Offset referenced by the pool table
        std::string name;
        uint16_t formatTag = 0;
        uint16_t channels = 0;
        uint32_t sampleRate = 0;
        uint16_t bitsPerSample = 0;
        uint16_t blockAlign = 0;
        uint32_t dataSize = 0;
        bool hasFact = false;
        uint32_t factLength = 0;
        WaveSample waveSample;

        // Created samples (one per channel) and their properties
        std::vector<int> sampleIds;
        uint32_t length = 0;
        int rootKey = 60;
        int correction = 0;
        uint32_t loopStart = 0;
        uint32_t loopEnd = 0;
    };

    struct Region
    {
        uint16_t keyMin = 0;
        uint16_t keyMax = 127;
        uint16_t velMin = 0;
        uint16_t velMax = 127;
        uint16_t keyGroup = 0;
        WaveSample waveSample;
        bool hasWaveLink = false;
        uint16_t linkOptions = 0;
        uint16_t phaseGroup = 0;
        uint32_t channel = Dls::WAVELINK_CHANNEL_LEFT;
        uint32_t tableIndex = 0;
        bool hasArticulation = false;
        std::vector<Dls::Connection> connections;
    };

    struct Instrument
    {
        std::string name;
        uint32_t bank = 0;
        uint32_t program = 0;
        std::vector<Dls::Connection> connections;
        std::vector<Region> regions;
    };

    struct Zone
    {
        int sampleId;
        std::map<AttributeType, AttributeValue> generators;
        double attenuation; // Not limited yet
        std::vector<ModulatorData> modulators;
    };

    // Parse the file
    bool parseFile();
    std::vector<Chunk> readChunks(int64_t start, int64_t end);
    std::vector<Chunk> readSubChunks(const Chunk &list);
    std::string readData(const Chunk &chunk);
    void parseInfo(const Chunk &list, std::map<std::string, std::string> &info);
    void parseWavePool(const Chunk &list);
    void parseWave(const Chunk &list, uint32_t poolOffset);
    void parseInstrument(const Chunk &list);
    void parseRegion(const Chunk &list, Instrument &instrument);
    bool parseArticulationList(const Chunk &list, std::vector<Dls::Connection> &connectionsLevel1,
                               std::vector<Dls::Connection> &connectionsLevel2);
    void parseArticulation(const std::string &data, bool isLevel1, std::vector<Dls::Connection> &connections);
    WaveSample parseWaveSample(const std::string &data);
    bool isConditionTrue(const std::string &data);
    bool isListConditionTrue(const std::vector<Chunk> &subChunks);
    static uint32_t queryDevice(uint32_t queryId, bool &isSupported);
    static bool extractWaveFile(Sf2Reader::Source &source, uint32_t waveListPosition, std::string &waveFile);

    // Create the soundfont
    void createSamples();
    void linkStereoSamples();
    std::vector<Zone> getZones(const Instrument &instrument, const Region &region);
    void createInstrument(const Instrument &instrument, std::vector<Zone> &zones);
    int getWaveIndex(uint32_t tableIndex);
    static void setOffset(Zone &zone, AttributeType fineChamp, AttributeType coarseChamp, int64_t offset);
    static void writeModulators(ISoundfontBuilder * sm, ISoundfontBuilder::Id id,
                                const std::vector<ModulatorData> &modulators, int &index);

    Sf2Reader::Source * _file;
    std::string _fileName;
    ISoundfontBuilder * _sm;
    std::map<std::string, std::string> _info;
    std::vector<uint32_t> _poolTable;
    std::vector<Wave> _waves;
    std::vector<Instrument> _instruments;
};

#endif // DLSREADER_H
