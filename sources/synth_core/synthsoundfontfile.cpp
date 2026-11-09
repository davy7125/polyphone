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
    explicit Sample(const Sf2Reader::Sample &sample) :
        _length(sample.length),
        _sampleRate(sample.sampleRate),
        _loopStart(sample.loopStart),
        _loopEnd(sample.loopEnd),
        _rootKey(sample.originalPitch),
        _correction(sample.correction),
        _type(sample.sampleType),
        _link(sample.sampleLink),
        _dataLength(0),
        _peak(0)
    {}

    // Data read as the editor does when it needs it: nothing (silence) if it cannot be read
    void readData(Sf2Reader::Source &source, const Sf2Reader::Sample &sample)
    {
        int16_t * data16 = nullptr;
        uint8_t * data24 = nullptr;
        if (sample.isCompressed)
        {
            std::unique_ptr<char[]> compressedData(Sf2Reader::readCompressedData(source, sample.start16, sample.length));
            uint32_t length;
            if (compressedData != nullptr)
                data16 = Sf2Reader::decompress(compressedData.get(), sample.length, length);
            if (data16 != nullptr)
                _length = _dataLength = length;
        }
        else
        {
            Sf2Reader::readData(source, sample.start16, sample.start24, sample.length, data16, data24);
            if (data16 != nullptr)
                _dataLength = sample.length;
        }
        setData(data16, data24, _dataLength);
    }

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
    void loadData() override {} // Already read
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
    uint32_t _length; // Number of bytes of compressed data until the data is decompressed, as in the editor
    uint32_t _sampleRate;
    uint32_t _loopStart;
    uint32_t _loopEnd;
    uint32_t _rootKey;
    int32_t _correction;
    SFSampleLink _type;
    int _link;

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

SynthSoundfontFile::SynthSoundfontFile() {}

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

bool SynthSoundfontFile::isSupported(const std::filesystem::path &filePath)
{
    std::string extension = getExtension(filePath);
    return extension == ".sf2" || extension == ".sf3" || extension == ".sfz" || extension == ".dls";
}

SynthSoundfontFile * SynthSoundfontFile::load(const std::filesystem::path &filePath, std::string &error, int velToFilterType)
{
    std::string extension = getExtension(filePath);
    if (extension == ".sfz")
        return loadSfz(filePath, error, velToFilterType);

    Sf2Reader::FileSource source(filePath);
    if (!source.isOpen())
    {
        error = "cannot open the file";
        return nullptr;
    }
    return extension == ".dls" ? loadDls(source, toText(filePath), error) : load(source, error);
}

SynthSoundfontFile * SynthSoundfontFile::load(const char * data, size_t length, std::string &error)
{
    Sf2Reader::MemorySource source(data, length);
    if (length >= 12 && memcmp(data, "RIFF", 4) == 0 && memcmp(data + 8, "DLS ", 4) == 0)
        return loadDls(source, "", error);
    return load(source, error);
}

SynthSoundfontFile * SynthSoundfontFile::loadSfz(const std::filesystem::path &filePath, std::string &error, int velToFilterType)
{
    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
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
        soundfont->build(builder, nullptr);
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    return soundfont.release();
}

SynthSoundfontFile * SynthSoundfontFile::loadDls(Sf2Reader::Source &source, const std::string &filePath, std::string &error)
{
    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
    try
    {
        Builder builder;
        DlsReader reader;
        if (!reader.read(source, filePath, builder))
        {
            error = "this is not a DLS file";
            return nullptr;
        }
        soundfont->build(builder, &source);
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    return soundfont.release();
}

void SynthSoundfontFile::build(Builder &builder, Sf2Reader::Source * dlsSource)
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

    // Samples, their data being read as the editor does when it plays them
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
        Sample * sample = new Sample(properties);
        _samples.emplace_back(sample);

        uint16_t channel = value(champ_wChannel).wValue;
        int16_t * data16 = nullptr;
        uint8_t * data24 = nullptr;
        if (dlsSource != nullptr)
        {
            // Wave of the DLS file
            uint32_t length;
            if (DlsReader::readWaveData(*dlsSource, value(champ_dwStart16).dwValue, channel, data16, data24, length) ==
                DlsReader::WAVE_OK)
                sample->setData(data16, data24, length);
            else
            {
                delete [] data16;
                delete [] data24;
            }
        }
        else if (std::unique_ptr<SampleFileReader> reader = SampleFileReader::open(toPath(model.fileName)))
        {
            // File of an sfz file
            SampleFileReader::Info info;
            reader->getInfo(info);
            data16 = new int16_t[info.length];
            data24 = new uint8_t[info.length];
            reader->readChannel(channel, info.length, data16, data24);
            sample->setData(data16, data24, info.length);
        }
    }
}

SynthSoundfontFile * SynthSoundfontFile::load(Sf2Reader::Source &source, std::string &error)
{
    Sf2Reader reader;
    if (!reader.read(source))
    {
        error = reader.getError();
        return nullptr;
    }

    std::unique_ptr<SynthSoundfontFile> soundfont(new SynthSoundfontFile());
    try
    {
        soundfont->_infos = reader.getInfos();
        soundfont->addElements(reader.getInstruments(), false);
        soundfont->addElements(reader.getPresets(), true);

        // All sample data at once
        for (const Sf2Reader::Sample &sample : reader.getSamples())
        {
            Sample * synthSample = new Sample(sample);
            soundfont->_samples.emplace_back(synthSample);
            synthSample->readData(source, sample);
        }
    }
    catch (const std::bad_alloc &)
    {
        error = "not enough memory";
        return nullptr;
    }
    return soundfont.release();
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
        {
            const Sf2Reader::Zone &zone = z < 0 ? elements[i].globalZone : zones[static_cast<size_t>(z)];
            for (const ModulatorData &modulator : zone.modulators)
            {
                if (modulator.srcOper.CC)
                    _controllerSources.push_back(modulator.srcOper);
                if (modulator.amtSrcOper.CC)
                    _controllerSources.push_back(modulator.amtSrcOper);
            }
        }
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
