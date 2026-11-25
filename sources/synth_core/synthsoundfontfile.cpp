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

#include "synthsoundfontfile.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>
#include "midiprocessor.h"
#include "soundfontbuilder.h"
#include "sfzreader.h"
#include "dlsreader.h"
#include "samplefilereader.h"

/// Elements of the soundfont

class SynthSoundfontFile::Sample: public ISynthSample
{
public:
    // Where the data is: in the file read (positions of the data of an sf2 or sf3 file, or of the wave of a dls file with
    // its channel), or in another file (sfz) with its channel
    struct Position
    {
        bool isCompressed = false;
        uint32_t start16 = 0;
        uint32_t start24 = 0;
        uint32_t length = 0; // Number of values, or of bytes of compressed data
        uint16_t channel = 0;
        std::string fileName;
    };

    Sample(const Sf2Reader::Sample &sample, const Position &position, std::atomic<uint32_t> &requestCount) :
        _length(sample.length),
        _sampleRate(sample.sampleRate),
        _loopStart(sample.loopStart),
        _loopEnd(sample.loopEnd),
        _rootKey(sample.originalPitch),
        _correction(sample.correction),
        _type(sample.sampleType),
        _link(sample.sampleLink),
        _position(position),
        _state(STATE_NOT_READ),
        _isRequested(false),
        _requestCount(requestCount),
        _linkedSample(nullptr),
        _dataLength(0),
        _peak(0)
    {}

    const Position &getPosition() const { return _position; }

    // Size of the data to read, for the progression (at least 1)
    uint64_t getReadSize() const
    {
        uint64_t size = _position.isCompressed ? _position.length : 2 * static_cast<uint64_t>(_position.length);
        return size > 0 ? size : 1;
    }

    // Other sample of a stereo pair, played only when both are read
    Sample * getLinkedSample() const { return _linkedSample; }
    void setLinkedSample(Sample * sample) { _linkedSample = sample; }

    // Data read (by any thread, the data being complete for the threads reading the state STATE_READ)
    bool isRead() const { return _state.load(std::memory_order_acquire) == STATE_READ; }
    bool isReady() const { return isRead() && (_linkedSample == nullptr || _linkedSample->isRead()); }
    bool isRequested() const { return _isRequested.load(std::memory_order_relaxed); }
    bool take() // With the mutex of the soundfont choosing the samples to read
    {
        if (_state.load(std::memory_order_relaxed) != STATE_NOT_READ)
            return false;
        _state.store(STATE_READING, std::memory_order_relaxed);
        return true;
    }
    void setRead() { _state.store(STATE_READ, std::memory_order_release); }

    // Data (owned from now on) of the length given, which is also the length of the sample if there is data
    void setData(int16_t * data16, uint8_t * data24, uint32_t length)
    {
        _data16.reset(data16);
        _data24.reset(data24);
        _dataLength = (data16 == nullptr) ? 0 : length;
        if (data16 != nullptr)
            _length = length;

        // Peak as Sound::getPeak
        int32_t max = 0;
        for (uint32_t i = 0; i < _dataLength; i++)
        {
            int32_t value = static_cast<int32_t>(_data16[i]) * 256 + (_data24 == nullptr ? 0 : _data24[i]);
            if (value < 0)
                value = -value;
            if (value > max)
                max = value;
        }
        _peak = static_cast<float>(max) / 8388608.0f;
    }

    // ISynthSample implementation
    bool loadData() override
    {
        // Not read yet: read first (audio thread, without waiting)
        if (isReady())
            return true;
        if (!_isRequested.exchange(true, std::memory_order_relaxed))
            _requestCount.fetch_add(1, std::memory_order_release);
        return false;
    }
    void getData(uint32_t &dataLength, int16_t * &data16, uint8_t * &data24, bool getCopy) override
    {
        dataLength = _dataLength;
        data16 = nullptr;
        data24 = nullptr;
        if (_dataLength == 0)
            return;

        if (getCopy)
        {
            data16 = new int16_t[_dataLength];
            memcpy(data16, _data16.get(), _dataLength * sizeof(int16_t));
            if (_data24 != nullptr)
            {
                data24 = new uint8_t[_dataLength];
                memcpy(data24, _data24.get(), _dataLength);
            }
        }
        else
        {
            data16 = _data16.get();
            data24 = _data24.get();
        }
    }
    float getPeak() override { return _peak; }
    uint32_t getSampleRate() override { return _sampleRate; }
    uint32_t getLength() override { return _length; }
    uint32_t getLoopStart() override { return _loopStart; }
    uint32_t getLoopEnd() override { return _loopEnd; }
    uint32_t getRootKey() override { return _rootKey; }
    int32_t getPitchCorrection() override { return _correction; }
    SFSampleLink getSampleType() override { return _type; }
    int getLinkedSampleIndex() override { return _link; }

private:
    enum State
    {
        STATE_NOT_READ,
        STATE_READING,
        STATE_READ
    };

    uint32_t _length; // Number of bytes of compressed data until the data is decompressed, as in the editor
    uint32_t _sampleRate;
    uint32_t _loopStart;
    uint32_t _loopEnd;
    uint32_t _rootKey;
    int32_t _correction;
    SFSampleLink _type;
    int _link;

    // Reading
    Position _position;
    std::atomic<int> _state;
    std::atomic<bool> _isRequested; // By a note
    std::atomic<uint32_t> &_requestCount; // Of the soundfont
    Sample * _linkedSample;

    // Data: 16 bits, and the 8 least significant bits of a 24-bit sample (otherwise null)
    std::unique_ptr<int16_t[]> _data16;
    std::unique_ptr<uint8_t[]> _data24;
    uint32_t _dataLength;
    float _peak;
};

class SynthSoundfontFile::Zone: public ISynthZone
{
public:
    explicit Zone(const Sf2Reader::Zone &zone) : _zone(zone), _isMuted(false) {}
    void setMuted(bool isMuted) { _isMuted = isMuted; }

    // ISynthZone implementation
    bool isPlayable() override { return !_isMuted; }
    bool * getAttributeSet() override { return _zone.attributeSet; }
    AttributeValue * getAttributeValues() override { return _zone.attributeValues; }
    int getModulatorCount() override { return static_cast<int>(_zone.modulators.size()); }
    ModulatorData * getModulatorData(int position) override { return &_zone.modulators[static_cast<size_t>(position)]; }

private:
    Sf2Reader::Zone _zone;
    bool _isMuted;
};

class SynthSoundfontFile::Element: public ISynthInstPrst
{
public:
    Element(const Sf2Reader::Element &element, int index, bool isPreset) :
        _name(element.name),
        _index(index),
        _presetNumber(isPreset ? element.preset : -1),
        _bankNumber(isPreset ? element.bank : -1),
        _isAlwaysPlayed(false),
        _globalZone(element.globalZone)
    {
        for (const Sf2Reader::Zone &zone : element.zones)
            _zones.emplace_back(new Zone(zone));
    }

    const std::string &getName() const { return _name; }
    Zone * getZoneObject(int position) { return _zones[static_cast<size_t>(position)].get(); }
    void setAlwaysPlayed(bool isAlwaysPlayed) { _isAlwaysPlayed = isAlwaysPlayed; }

    // ISynthInstPrst implementation
    int getIndex() override { return _index; }
    bool isPlayable() override { return true; }
    int getPresetNumber() override { return _presetNumber; }
    int getBankNumber() override { return _bankNumber; }
    bool isAlwaysPlayed() override { return _isAlwaysPlayed; }
    ISynthZone * getGlobalZone() override { return &_globalZone; }
    int getZoneCount() override { return static_cast<int>(_zones.size()); }
    ISynthZone * getZone(int position) override { return _zones[static_cast<size_t>(position)].get(); }

private:
    std::string _name;
    int _index;
    int _presetNumber; // -1 for an instrument
    int _bankNumber;
    bool _isAlwaysPlayed;
    Zone _globalZone;
    std::vector<std::unique_ptr<Zone>> _zones;
};

/// Soundfont converted from another format (sfz, dls), written by the readers as the model of the editor

class SynthSoundfontFile::Builder: public ISoundfontBuilder
{
public:
    struct Values
    {
        std::map<AttributeType, AttributeValue> values;
        std::string name;
        std::string fileName;
    };

    struct ZoneModel
    {
        std::map<AttributeType, AttributeValue> values;
        std::vector<ModulatorData> modulators;
    };

    struct ElementModel
    {
        std::string name;
        ZoneModel globalZone; // Also the properties of the element (preset and bank numbers)
        std::vector<ZoneModel> zones;
    };

    Values soundfont;
    std::map<AttributeType, std::string> texts;
    std::vector<Values> samples;
    std::vector<ElementModel> instruments, presets;
    std::vector<SFModulator> controllerSources; // In the order of the editor (signal inputModulatorChanged)

    int add(const Id &id) override
    {
        switch (id.type)
        {
        case TYPE_SF2:
            return 0;
        case TYPE_SAMPLE:
            samples.push_back(Values());
            return static_cast<int>(samples.size()) - 1;
        case TYPE_INSTRUMENT: case TYPE_PRESET: {
            std::vector<ElementModel> &elements = (id.type == TYPE_INSTRUMENT) ? instruments : presets;
            elements.push_back(ElementModel());
            return static_cast<int>(elements.size()) - 1;
        }
        case TYPE_INSTRUMENT_ZONE: case TYPE_PRESET_ZONE: {
            ElementModel * element = getElement(id);
            if (element == nullptr)
                return -1;
            element->zones.push_back(ZoneModel());
            return static_cast<int>(element->zones.size()) - 1;
        }
        default: {
            // Modulator, its index being its position as in the editor
            ZoneModel * zone = getZone(id);
            if (zone == nullptr)
                return -1;
            ModulatorData modulator;
            modulator.index = static_cast<uint16_t>(zone->modulators.size());
            zone->modulators.push_back(modulator);
            return static_cast<int>(zone->modulators.size()) - 1;
        }
        }
    }

    void set(const Id &id, AttributeType champ, AttributeValue value) override
    {
        if (ModulatorData * modulator = getModulator(id))
        {
            switch (champ)
            {
            case champ_sfModSrcOper:
                modulator->srcOper = value.sfModValue;
                if (value.sfModValue.CC)
                    controllerSources.push_back(value.sfModValue);
                break;
            case champ_sfModDestOper:
                modulator->destOper = value.wValue;
                break;
            case champ_modAmount:
                modulator->amount = value.shValue;
                break;
            case champ_sfModAmtSrcOper:
                modulator->amtSrcOper = value.sfModValue;
                if (value.sfModValue.CC)
                    controllerSources.push_back(value.sfModValue);
                break;
            case champ_sfModTransOper:
                modulator->transOper = value.sfTransValue;
                break;
            case champ_indexMod:
                modulator->index = value.wValue;
                break;
            default:
                break;
            }
        }
        else if (std::map<AttributeType, AttributeValue> * values = getValues(id))
        {
            if (id.type == TYPE_SF2 || id.type == TYPE_SAMPLE)
                (*values)[champ] = value;
            else if (champ < champ_endOper)
            {
                // As Division::setGen: -1 for the root key, the key and the velocity is the default value
                if (value.shValue == -1 && (champ == champ_overridingRootKey || champ == champ_keynum || champ == champ_velocity))
                    values->erase(champ);
                else
                    (*values)[champ] = value;
            }
            else if (id.type == TYPE_PRESET && (champ == champ_wPreset || champ == champ_wBank))
                (*values)[champ] = value; // Not a generator of the global zone
        }
    }

    void setText(const Id &id, AttributeType champ, const std::string &text) override
    {
        if (id.type == TYPE_SF2)
            texts[champ] = text;
        else if (id.type == TYPE_SAMPLE && id.element >= 0 && id.element < static_cast<int>(samples.size()))
        {
            if (champ == champ_name)
                samples[static_cast<size_t>(id.element)].name = text;
            else if (champ == champ_filenameForData)
                samples[static_cast<size_t>(id.element)].fileName = text;
        }
        else if (ElementModel * element = (champ == champ_name ? getElement(id) : nullptr))
        {
            if (id.type == TYPE_INSTRUMENT || id.type == TYPE_PRESET)
                element->name = text;
        }
    }

    bool isSet(const Id &id, AttributeType champ) override
    {
        std::map<AttributeType, AttributeValue> * values = getValues(id);
        return values != nullptr && values->count(champ) > 0;
    }

    AttributeValue get(const Id &id, AttributeType champ) override
    {
        std::map<AttributeType, AttributeValue> * values = getValues(id);
        if (values != nullptr)
        {
            auto it = values->find(champ);
            if (it != values->end())
                return it->second;
        }
        return AttributeValue();
    }

    std::string getText(const Id &id, AttributeType champ) override
    {
        if (id.type == TYPE_SF2)
            return texts[champ];
        if (id.type == TYPE_SAMPLE && id.element >= 0 && id.element < static_cast<int>(samples.size()))
            return champ == champ_name ? samples[static_cast<size_t>(id.element)].name :
                                         (champ == champ_filenameForData ? samples[static_cast<size_t>(id.element)].fileName : "");
        ElementModel * element = (id.type == TYPE_INSTRUMENT || id.type == TYPE_PRESET) ? getElement(id) : nullptr;
        return (element != nullptr && champ == champ_name) ? element->name : "";
    }

    void reset(const Id &id, AttributeType champ) override
    {
        if (std::map<AttributeType, AttributeValue> * values = getValues(id))
            values->erase(champ);
    }

    void setSoundProperty(const Id &id, AttributeType champ, AttributeValue value) override
    {
        set(id, champ, value);
    }

    std::vector<int> getSiblings(const Id &id) override
    {
        size_t count = 0;
        switch (id.type)
        {
        case TYPE_SAMPLE:
            count = samples.size();
            break;
        case TYPE_INSTRUMENT:
            count = instruments.size();
            break;
        case TYPE_PRESET:
            count = presets.size();
            break;
        case TYPE_INSTRUMENT_ZONE: case TYPE_PRESET_ZONE:
            if (ElementModel * element = getElement(id))
                count = element->zones.size();
            break;
        default:
            break;
        }
        std::vector<int> siblings(count);
        for (size_t i = 0; i < count; i++)
            siblings[i] = static_cast<int>(i);
        return siblings;
    }

private:
    ElementModel * getElement(const Id &id)
    {
        bool isPreset = (id.type == TYPE_PRESET || id.type == TYPE_PRESET_ZONE || id.type == TYPE_PRESET_MOD ||
                         id.type == TYPE_PRESET_ZONE_MOD);
        std::vector<ElementModel> &elements = isPreset ? presets : instruments;
        return (id.element >= 0 && id.element < static_cast<int>(elements.size())) ? &elements[static_cast<size_t>(id.element)] : nullptr;
    }

    ZoneModel * getZone(const Id &id)
    {
        ElementModel * element = getElement(id);
        if (element == nullptr)
            return nullptr;
        if (id.type == TYPE_INSTRUMENT || id.type == TYPE_PRESET || id.type == TYPE_INSTRUMENT_MOD || id.type == TYPE_PRESET_MOD)
            return &element->globalZone;
        return (id.zone >= 0 && id.zone < static_cast<int>(element->zones.size())) ? &element->zones[static_cast<size_t>(id.zone)] : nullptr;
    }

    ModulatorData * getModulator(const Id &id)
    {
        if (id.type != TYPE_INSTRUMENT_MOD && id.type != TYPE_PRESET_MOD && id.type != TYPE_INSTRUMENT_ZONE_MOD &&
            id.type != TYPE_PRESET_ZONE_MOD)
            return nullptr;
        ZoneModel * zone = getZone(id);
        return (zone != nullptr && id.modulator >= 0 && id.modulator < static_cast<int>(zone->modulators.size())) ?
                   &zone->modulators[static_cast<size_t>(id.modulator)] : nullptr;
    }

    std::map<AttributeType, AttributeValue> * getValues(const Id &id)
    {
        switch (id.type)
        {
        case TYPE_SF2:
            return &soundfont.values;
        case TYPE_SAMPLE:
            return (id.element >= 0 && id.element < static_cast<int>(samples.size())) ? &samples[static_cast<size_t>(id.element)].values : nullptr;
        case TYPE_INSTRUMENT: case TYPE_PRESET: case TYPE_INSTRUMENT_ZONE: case TYPE_PRESET_ZONE: {
            ZoneModel * zone = getZone(id);
            return zone == nullptr ? nullptr : &zone->values;
        }
        default:
            return nullptr;
        }
    }
};

/// Soundfont

SynthSoundfontFile::SynthSoundfontFile() :
    _sourceType(SOURCE_SF2),
    _hasCompressedSamples(false),
    _hasMemoryError(false),
    _requestCount(0),
    _checkedRequestCount(0),
    _nextInFile(0),
    _readCount(0),
    _totalSize(0),
    _readSize(0)
{}

SynthSoundfontFile::~SynthSoundfontFile() {}

// Extension of a file in lower case, with the dot
static std::string getExtension(const std::filesystem::path &filePath)
{
    std::string extension = filePath.extension().string();
    for (char &c : extension)
        if (c >= 'A' && c <= 'Z')
            c = static_cast<char>(c - 'A' + 'a');
    return extension;
}

// UTF-8 text of a path, '/' being the separator
static std::string toText(const std::filesystem::path &path)
{
#if defined(__cpp_char8_t)
    std::u8string text = path.generic_u8string();
    return std::string(text.begin(), text.end());
#else
    return path.generic_u8string();
#endif
}

static std::filesystem::path toPath(const std::string &text)
{
#if defined(__cpp_char8_t)
    return std::filesystem::path(std::u8string(text.begin(), text.end()));
#else
    return std::filesystem::u8path(text);
#endif
}

// Key or velocity range of a zone, defaultRange if it is not set (as the synth)
static RangesType getRange(ISynthZone * zone, AttributeType champ, RangesType defaultRange)
{
    return zone->getAttributeSet()[champ] ? zone->getAttributeValues()[champ].rValue : defaultRange;
}

bool SynthSoundfontFile::isSupported(const std::filesystem::path &filePath)
{
    std::string extension = getExtension(filePath);
    return extension == ".sf2" || extension == ".sf3" || extension == ".sfz" || extension == ".dls";
}

SynthSoundfontFile * SynthSoundfontFile::open(const std::filesystem::path &filePath, std::string &error, int velToFilterType)
{
    std::string extension = getExtension(filePath);
    if (extension == ".sfz")
        return openSfz(filePath, error, velToFilterType);

    std::unique_ptr<Sf2Reader::FileSource> source(new Sf2Reader::FileSource(filePath));
    if (!source->isOpen())
    {
        error = "cannot open the file";
        return nullptr;
    }
    return extension == ".dls" ? openDls(std::move(source), toText(filePath), error) : openSf2(std::move(source), error);
}

SynthSoundfontFile * SynthSoundfontFile::open(const char * data, size_t length, std::string &error)
{
    std::unique_ptr<Sf2Reader::Source> source(new Sf2Reader::MemorySource(data, length));
    if (length >= 12 && memcmp(data, "RIFF", 4) == 0 && memcmp(data + 8, "DLS ", 4) == 0)
        return openDls(std::move(source), "", error);
    return openSf2(std::move(source), error);
}

SynthSoundfontFile * SynthSoundfontFile::load(const std::filesystem::path &filePath, std::string &error, int velToFilterType)
{
    return loadAll(open(filePath, error, velToFilterType), error);
}

SynthSoundfontFile * SynthSoundfontFile::load(const char * data, size_t length, std::string &error)
{
    return loadAll(open(data, length, error), error);
}

SynthSoundfontFile * SynthSoundfontFile::loadAll(SynthSoundfontFile * soundfont, std::string &error)
{
    if (soundfont != nullptr)
    {
        soundfont->loadAllSamples();
        if (soundfont->hasMemoryError())
        {
            delete soundfont;
            error = "not enough memory";
            return nullptr;
        }
    }
    return soundfont;
}

SynthSoundfontFile * SynthSoundfontFile::openSfz(const std::filesystem::path &filePath, std::string &error, int velToFilterType)
{
    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
    soundfont->_sourceType = SOURCE_FILES;
    try
    {
        Builder builder;
        SfzReader reader;
        SfzReader::Options options;
        options.velToFilterType = velToFilterType;
        if (!reader.read(toText(filePath), builder, true, options))
        {
            error = reader.getError() == SfzReader::ERROR_RECURSION ? "file recursion (#include)" :
                        "cannot open the file " + reader.getErrorFile();
            return nullptr;
        }
        soundfont->build(builder);
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    soundfont->prepareLoading();
    return soundfont.release();
}

SynthSoundfontFile * SynthSoundfontFile::openDls(std::unique_ptr<Sf2Reader::Source> source, const std::string &filePath,
                                                 std::string &error)
{
    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
    soundfont->_sourceType = SOURCE_DLS;
    try
    {
        Builder builder;
        DlsReader reader;
        if (!reader.read(*source, filePath, builder))
        {
            error = "this is not a DLS file";
            return nullptr;
        }
        soundfont->build(builder);
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    soundfont->_source = std::move(source);
    soundfont->prepareLoading();
    return soundfont.release();
}

void SynthSoundfontFile::build(Builder &builder)
{
    // Texts of the soundfont
    const std::pair<AttributeType, const char *> infos[] = {
        { champ_name, "INAM" }, { champ_ICOP, "ICOP" }, { champ_IENG, "IENG" }, { champ_ICRD, "ICRD" },
        { champ_IPRD, "IPRD" }, { champ_ISFT, "ISFT" }, { champ_ICMT, "ICMT" }
    };
    for (const auto &info : infos)
        if (builder.texts.count(info.first) && !builder.texts[info.first].empty())
            _infos[info.second] = builder.texts[info.first];

    // Instruments and presets, the controllers used by the modulators being in the order of the editor
    auto toZone = [](const Builder::ZoneModel &model) {
        Sf2Reader::Zone zone;
        for (const auto &value : model.values)
        {
            if (value.first < champ_endOper)
            {
                zone.attributeSet[value.first] = true;
                zone.attributeValues[value.first] = value.second;
            }
        }
        zone.modulators = model.modulators;
        return zone;
    };
    for (int type = 0; type < 2; type++)
    {
        std::vector<Sf2Reader::Element> elements;
        for (const Builder::ElementModel &model : type == 0 ? builder.instruments : builder.presets)
        {
            Sf2Reader::Element element;
            element.name = model.name;
            auto bank = model.globalZone.values.find(champ_wBank);
            auto preset = model.globalZone.values.find(champ_wPreset);
            element.bank = bank == model.globalZone.values.end() ? 0 : bank->second.wValue;
            element.preset = preset == model.globalZone.values.end() ? 0 : preset->second.wValue;
            element.globalZone = toZone(model.globalZone);
            for (const Builder::ZoneModel &zone : model.zones)
                element.zones.push_back(toZone(zone));
            elements.push_back(element);
        }
        addElements(elements, type == 1, false);
    }
    _controllerSources = builder.controllerSources;

    // Samples, their data being read afterwards: wave of the dls file, or file of the sfz file
    for (const Builder::Values &model : builder.samples)
    {
        auto value = [&model](AttributeType champ) {
            auto it = model.values.find(champ);
            return it == model.values.end() ? AttributeValue() : it->second;
        };
        Sf2Reader::Sample properties;
        properties.name = model.name;
        properties.length = value(champ_dwLength).dwValue;
        properties.loopStart = value(champ_dwStartLoop).dwValue;
        properties.loopEnd = value(champ_dwEndLoop).dwValue;
        properties.sampleRate = value(champ_dwSampleRate).dwValue;
        properties.originalPitch = value(champ_byOriginalPitch).bValue;
        properties.correction = value(champ_chPitchCorrection).cValue;
        properties.sampleLink = value(champ_wSampleLink).wValue;
        properties.sampleType = model.values.count(champ_sfSampleType) ? value(champ_sfSampleType).sfLinkValue : monoSample;
        Sample::Position position;
        position.start16 = value(champ_dwStart16).dwValue;
        position.length = properties.length;
        position.channel = value(champ_wChannel).wValue;
        position.fileName = model.fileName;
        _samples.emplace_back(new Sample(properties, position, _requestCount));
    }
}

SynthSoundfontFile * SynthSoundfontFile::openSf2(std::unique_ptr<Sf2Reader::Source> source, std::string &error)
{
    Sf2Reader reader;
    if (!reader.read(*source))
    {
        error = reader.getError();
        return nullptr;
    }

    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
    try
    {
        soundfont->_infos = reader.getInfos();
        if (reader.hasDefaultModulators())
        {
            // Before the elements, as the editor reads them
            soundfont->_defaultModulators.reset(new Zone(reader.getDefaultModulators()));
            soundfont->addControllerSources(reader.getDefaultModulators());
        }
        soundfont->addElements(reader.getInstruments(), false);
        soundfont->addElements(reader.getPresets(), true);

        // Samples, their data being read afterwards
        for (const Sf2Reader::Sample &sample : reader.getSamples())
        {
            Sample::Position position;
            position.isCompressed = sample.isCompressed;
            position.start16 = sample.start16;
            position.start24 = sample.start24;
            position.length = sample.length;
            soundfont->_samples.emplace_back(new Sample(sample, position, soundfont->_requestCount));
            soundfont->_hasCompressedSamples = soundfont->_hasCompressedSamples || sample.isCompressed;
        }
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    soundfont->_source = std::move(source);
    soundfont->prepareLoading();
    return soundfont.release();
}

/// Sample data

void SynthSoundfontFile::prepareLoading()
{
    // Stereo pairs (as the synth plays them), size of the data
    int count = static_cast<int>(_samples.size());
    for (int i = 0; i < count; i++)
    {
        Sample * sample = _samples[static_cast<size_t>(i)].get();
        int link = sample->getLinkedSampleIndex();
        if (sample->getSampleType() != monoSample && sample->getSampleType() != RomMonoSample &&
            link >= 0 && link < count && link != i)
            sample->setLinkedSample(_samples[static_cast<size_t>(link)].get());
        _totalSize += sample->getReadSize();
    }
    if (_samples.empty())
        _source.reset();
}

bool SynthSoundfontFile::loadNextSample()
{
    Sample * samples[2];
    int count;
    {
        std::lock_guard<std::mutex> lock(_loadingMutex);
        count = takeNextSamples(samples);
    }
    if (count == 0)
        return false;

    // Read, then played together
    for (int i = 0; i < count; i++)
        readData(*samples[i]);
    for (int i = 0; i < count; i++)
    {
        _readSize.fetch_add(samples[i]->getReadSize(), std::memory_order_relaxed);
        samples[i]->setRead();
    }

    // Everything read: the source is released (file closed)
    std::lock_guard<std::mutex> lock(_loadingMutex);
    _readCount.store(_readCount.load(std::memory_order_relaxed) + static_cast<size_t>(count), std::memory_order_release);
    if (_readCount.load(std::memory_order_relaxed) == _samples.size())
    {
        {
            std::lock_guard<std::mutex> sourceLock(_sourceMutex);
            _source.reset();
        }
        _loadedCondition.notify_all();
    }
    return true;
}

int SynthSoundfontFile::takeNextSamples(Sample * samples[2])
{
    // Samples asked by the notes since the last time
    uint32_t requestCount = _requestCount.load(std::memory_order_acquire);
    if (requestCount != _checkedRequestCount)
    {
        _checkedRequestCount = requestCount;
        for (size_t i = 0; i < _samples.size(); i++)
            if (_samples[i]->isRequested() && !_samples[i]->isReady())
                _requested.push_back(static_cast<int>(i));
    }

    for (;;)
    {
        // Samples asked by the notes, priorities, then the order of the file
        int index;
        if (!_requested.empty())
        {
            index = _requested.front();
            _requested.pop_front();
        }
        else if (!_priorities.empty())
        {
            index = _priorities.front();
            _priorities.pop_front();
        }
        else if (_nextInFile < _samples.size())
            index = static_cast<int>(_nextInFile++);
        else
            return 0;

        // The sample and the one linked to it, if they are not read or being read
        Sample * sample = _samples[static_cast<size_t>(index)].get();
        int count = 0;
        for (Sample * candidate : { sample, sample->getLinkedSample() })
            if (candidate != nullptr && candidate->take())
                samples[count++] = candidate;
        if (count > 0)
            return count;
    }
}

void SynthSoundfontFile::readData(Sample &sample)
{
    // Data read as the editor does when it needs it: nothing (silence) if it cannot be read. The source is read by one
    // thread at a time, the data of the sf3 files being decompressed outside (by several threads at the same time).
    const Sample::Position &position = sample.getPosition();
    int16_t * data16 = nullptr;
    uint8_t * data24 = nullptr;
    uint32_t length = 0;
    try
    {
        switch (_sourceType)
        {
        case SOURCE_SF2:
            if (position.isCompressed)
            {
                std::unique_ptr<char[]> compressedData;
                {
                    std::lock_guard<std::mutex> lock(_sourceMutex);
                    compressedData.reset(Sf2Reader::readCompressedData(*_source, position.start16, position.length));
                }
                if (compressedData != nullptr)
                    data16 = Sf2Reader::decompress(compressedData.get(), position.length, length);
            }
            else
            {
                std::lock_guard<std::mutex> lock(_sourceMutex);
                Sf2Reader::readData(*_source, position.start16, position.start24, position.length, data16, data24);
                length = position.length;
            }
            break;
        case SOURCE_DLS: {
            // Wave of the DLS file
            std::lock_guard<std::mutex> lock(_sourceMutex);
            if (DlsReader::readWaveData(*_source, position.start16, position.channel, data16, data24, length) !=
                DlsReader::WAVE_OK)
            {
                delete [] data16;
                delete [] data24;
                data16 = nullptr;
                data24 = nullptr;
            }
            break;
        }
        case SOURCE_FILES: {
            // File of an sfz file, one at a time
            std::lock_guard<std::mutex> lock(_sourceMutex);
            if (std::unique_ptr<SampleFileReader> reader = SampleFileReader::open(toPath(position.fileName)))
            {
                SampleFileReader::Info info;
                reader->getInfo(info);
                data16 = new int16_t[info.length];
                data24 = new uint8_t[info.length];
                length = info.length;
                reader->readChannel(position.channel, info.length, data16, data24);
            }
            break;
        }
        }
    }
    catch (const std::bad_alloc &)
    {
        delete [] data16;
        delete [] data24;
        data16 = nullptr;
        data24 = nullptr;
        _hasMemoryError = true;
    }
    sample.setData(data16, data24, length);
}

void SynthSoundfontFile::loadAllSamples()
{
    while (loadNextSample()) {}

    // Samples read by other threads
    std::unique_lock<std::mutex> lock(_loadingMutex);
    _loadedCondition.wait(lock, [this] { return _readCount.load(std::memory_order_relaxed) == _samples.size(); });
}

void SynthSoundfontFile::prioritizePreset(int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= static_cast<int>(_presets.size()) || isLoaded())
        return;

    // Samples not read of the zones of the preset, with the distance of their keys to the middle of the keyboard (C4)
    std::vector<std::pair<int, int>> samples; // Distance, index
    RangesType fullRange;
    fullRange.byLo = 0;
    fullRange.byHi = 127;
    Element * preset = _presets[static_cast<size_t>(presetIndex)].get();
    RangesType presetDefaultRange = getRange(preset->getGlobalZone(), champ_keyRange, fullRange);
    for (int i = 0; i < preset->getZoneCount(); i++)
    {
        ISynthZone * presetZone = preset->getZone(i);
        if (!presetZone->isPlayable() || !presetZone->getAttributeSet()[champ_instrument])
            continue;
        ISynthInstPrst * instrument = getSynthInstrument(presetZone->getAttributeValues()[champ_instrument].wValue);
        if (instrument == nullptr)
            continue;
        RangesType presetRange = getRange(presetZone, champ_keyRange, presetDefaultRange);
        RangesType instrumentDefaultRange = getRange(instrument->getGlobalZone(), champ_keyRange, fullRange);
        for (int j = 0; j < instrument->getZoneCount(); j++)
        {
            ISynthZone * zone = instrument->getZone(j);
            if (!zone->isPlayable() || !zone->getAttributeSet()[champ_sampleID])
                continue;
            int index = zone->getAttributeValues()[champ_sampleID].wValue;
            if (index >= static_cast<int>(_samples.size()) || _samples[static_cast<size_t>(index)]->isReady())
                continue;
            RangesType range = getRange(zone, champ_keyRange, instrumentDefaultRange);
            int low = std::max<int>(range.byLo, presetRange.byLo);
            int high = std::min<int>(range.byHi, presetRange.byHi);
            if (low > high)
                continue; // Not played
            int distance = (low <= 60 && 60 <= high) ? 0 : std::min(std::abs(low - 60), std::abs(high - 60));
            samples.emplace_back(distance, index);
        }
    }
    std::stable_sort(samples.begin(), samples.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
        return a.first < b.first;
    });

    // Before the previous priorities
    std::lock_guard<std::mutex> lock(_loadingMutex);
    for (auto it = samples.rbegin(); it != samples.rend(); ++it)
        _priorities.push_front(it->second);
}

float SynthSoundfontFile::getLoadedRatio() const
{
    if (isLoaded() || _totalSize == 0)
        return 1.0f;
    return std::min(0.999f, static_cast<float>(static_cast<double>(_readSize.load(std::memory_order_relaxed)) /
                                               static_cast<double>(_totalSize)));
}

bool SynthSoundfontFile::isLoaded() const
{
    return _readCount.load(std::memory_order_acquire) == _samples.size();
}

void SynthSoundfontFile::addElements(const std::vector<Sf2Reader::Element> &elements, bool isPreset, bool withControllers)
{
    // The controllers used by the modulators are in the order the editor adds them (InputParserSf): global zone, then the
    // other zones, source then amount source
    std::vector<std::unique_ptr<Element>> &list = isPreset ? _presets : _instruments;
    for (size_t i = 0; i < elements.size(); i++)
    {
        list.emplace_back(new Element(elements[i], static_cast<int>(i), isPreset));
        if (!withControllers)
            continue;
        const std::vector<Sf2Reader::Zone> &zones = elements[i].zones;
        for (int z = -1; z < static_cast<int>(zones.size()); z++)
            addControllerSources(z < 0 ? elements[i].globalZone : zones[static_cast<size_t>(z)]);
    }
}

void SynthSoundfontFile::addControllerSources(const Sf2Reader::Zone &zone)
{
    for (const ModulatorData &modulator : zone.modulators)
    {
        if (modulator.srcOper.CC)
            _controllerSources.push_back(modulator.srcOper);
        if (modulator.amtSrcOper.CC)
            _controllerSources.push_back(modulator.amtSrcOper);
    }
}

std::string SynthSoundfontFile::getInfo(const std::string &key) const
{
    auto it = _infos.find(key);
    return it == _infos.end() ? "" : it->second;
}

std::string SynthSoundfontFile::getPresetName(int index) const
{
    return (index >= 0 && index < static_cast<int>(_presets.size())) ? _presets[index]->getName() : "";
}

std::string SynthSoundfontFile::getInstrumentName(int index) const
{
    return (index >= 0 && index < static_cast<int>(_instruments.size())) ? _instruments[index]->getName() : "";
}

void SynthSoundfontFile::applyDefaultControllerValues(MidiProcessor &processor) const
{
    for (const SFModulator &source : _controllerSources)
        processor.setDefaultControllerValue(source.Index, source.isBipolar, source.isDescending);
}

bool SynthSoundfontFile::setZoneMuted(bool isPreset, int index, int zonePosition, bool isMuted)
{
    std::vector<std::unique_ptr<Element>> &elements = isPreset ? _presets : _instruments;
    if (index < 0 || index >= static_cast<int>(elements.size()) ||
        zonePosition < 0 || zonePosition >= elements[index]->getZoneCount())
        return false;
    elements[index]->getZoneObject(zonePosition)->setMuted(isMuted);
    return true;
}

bool SynthSoundfontFile::setAlwaysPlayed(bool isPreset, int index, bool isAlwaysPlayed)
{
    std::vector<std::unique_ptr<Element>> &elements = isPreset ? _presets : _instruments;
    if (index < 0 || index >= static_cast<int>(elements.size()))
        return false;
    elements[index]->setAlwaysPlayed(isAlwaysPlayed);
    return true;
}

ISynthSample * SynthSoundfontFile::getSynthSample(int index)
{
    return (index >= 0 && index < static_cast<int>(_samples.size())) ? _samples[index].get() : nullptr;
}

ISynthInstPrst * SynthSoundfontFile::getSynthInstrument(int index)
{
    return (index >= 0 && index < static_cast<int>(_instruments.size())) ? _instruments[index].get() : nullptr;
}

ISynthInstPrst * SynthSoundfontFile::getSynthPreset(int index)
{
    return (index >= 0 && index < static_cast<int>(_presets.size())) ? _presets[index].get() : nullptr;
}

int SynthSoundfontFile::getInstrumentCount()
{
    return static_cast<int>(_instruments.size());
}

ISynthInstPrst * SynthSoundfontFile::getInstrumentAt(int position)
{
    return getSynthInstrument(position); // Same order
}

int SynthSoundfontFile::getPresetCount()
{
    return static_cast<int>(_presets.size());
}

ISynthInstPrst * SynthSoundfontFile::getPresetAt(int position)
{
    return getSynthPreset(position); // Same order
}

ISynthZone * SynthSoundfontFile::getDefaultModulators()
{
    return _defaultModulators.get();
}
