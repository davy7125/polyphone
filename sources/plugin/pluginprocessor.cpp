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


#include "pluginprocessor.h"
#include <cmath>
#include <cstring>
#include "plugineditor.h"
#include "synthconstants.h"
#include "synthsoundfontfile.h"
#include "pluginsoundfont.h"
#include "BinaryData.h"

// Parameters, in the order of _parameterValues
static const char * PARAMETER_IDS[12] = {
    "gain", "reverbLevel", "reverbSize", "reverbWidth", "reverbDamping", "chorusLevel", "chorusDepth", "chorusFrequency",
    "transpose", "tuning", "temperament", "temperamentKey"
};

static const int MAX_RECENT_FILES = 10;

// Output buses: the main one, then the ones of the 16 channels and of the effects (disabled by default). Their order
// must never change once the plugin is published (the projects of the hosts use it).
static const int CHANNEL_BUS_COUNT = 16;
static const int EFFECTS_BUS = CHANNEL_BUS_COUNT + 1;

juce::AudioProcessor::BusesProperties PluginProcessor::createBuses()
{
    BusesProperties buses;
    buses = buses.withOutput("Main", juce::AudioChannelSet::stereo(), true);
    for (int channel = 1; channel <= CHANNEL_BUS_COUNT; channel++)
        buses = buses.withOutput("Channel " + juce::String(channel), juce::AudioChannelSet::stereo(), false);
    return buses.withOutput("Effects", juce::AudioChannelSet::stereo(), false);
}

static std::filesystem::path toPath(const juce::File &file)
{
#if JUCE_WINDOWS
    return std::filesystem::path(file.getFullPathName().toWideCharPointer());
#else
    return std::filesystem::path(file.getFullPathName().toStdString());
#endif
}

PluginProcessor::PluginProcessor() :
    AudioProcessor(createBuses()),
    _parameters(*this, nullptr, "Parameters", createParameters()),
    _loadingThread(1),
    _isLoading(false),
    _loadingCount(0),
    _isSoundfontStored(false),
    _polyphony(DEFAULT_POLYPHONY),
    _hasMultipleOutputs(false),
    _isPrepared(false),
    _sampleRate(48000.0),
    _maxBlockSize(512)
{
    for (int i = 0; i < 12; i++)
    {
        _parameterValues[i] = _parameters.getRawParameterValue(PARAMETER_IDS[i]);
        _appliedValues[i] = NAN; // Applied by the first block
    }

    // Settings shared by all instances
    juce::PropertiesFile::Options options;
    options.applicationName = "Polyphone plugin";
    options.filenameSuffix = ".settings";
    options.folderName = "Polyphone";
    options.osxLibrarySubFolder = "Application Support";
    _settings.setStorageParameters(options);
    _settings.getUserSettings();

    // Soundfonts replaced that are not played anymore
    startTimer(500);
}

PluginProcessor::~PluginProcessor()
{
    stopTimer();
    _loadingCount++; // A soundfont being read is not used
    _loadingThread.removeAllJobs(true, -1);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameters()
{
    // Same ranges and default values as the settings of the editor
    auto percentage = [](const char * id, const char * name) {
        return std::make_unique<juce::AudioParameterInt>(juce::ParameterID(id, 1), name, 0, 100, 0,
                                                         juce::AudioParameterIntAttributes().withLabel("%"));
    };

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("gain", 1), "Gain", juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "reverb", "Reverb", "|", percentage("reverbLevel", "Reverb level"), percentage("reverbSize", "Reverb size"),
        percentage("reverbWidth", "Reverb width"), percentage("reverbDamping", "Reverb damping")));
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "chorus", "Chorus", "|", percentage("chorusLevel", "Chorus level"), percentage("chorusDepth", "Chorus depth"),
        percentage("chorusFrequency", "Chorus frequency")));

    juce::StringArray temperaments;
    for (const Temperament &temperament : getTemperaments())
        temperaments.add(temperament.name);
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "tuning", "Tuning", "|",
        std::make_unique<juce::AudioParameterInt>(juce::ParameterID("transpose", 1), "Transposition", -24, 24, 0,
                                                  juce::AudioParameterIntAttributes().withLabel("semitones")),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("tuning", 1), "Reference pitch",
                                                    juce::NormalisableRange<float>(400.0f, 480.0f, 0.1f), 440.0f,
                                                    juce::AudioParameterFloatAttributes().withLabel("Hz")),
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("temperament", 1), "Temperament", temperaments, 0),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("temperamentKey", 1), "Temperament key",
            juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 0)));
    return layout;
}

const std::vector<PluginProcessor::Temperament> &PluginProcessor::getTemperaments()
{
    // Equal temperament, then the ones of the editor (name and 12 fine tunes on each line)
    static const std::vector<Temperament> temperaments = [] {
        std::vector<Temperament> list;
        Temperament equal;
        equal.name = "Equal";
        for (float &fineTune : equal.fineTunes)
            fineTune = 0.0f;
        list.push_back(equal);

        juce::StringArray lines = juce::StringArray::fromLines(
            juce::String::fromUTF8(BinaryData::temperaments_csv, BinaryData::temperaments_csvSize));
        for (const juce::String &line : lines)
        {
            juce::StringArray parts = juce::StringArray::fromTokens(line, ",", "");
            if (parts.size() != 13)
                continue;
            Temperament temperament;
            temperament.name = parts[0];
            for (int i = 0; i < 12; i++)
                temperament.fineTunes[i] = parts[i + 1].getFloatValue();
            list.push_back(temperament);
        }
        return list;
    }();
    return temperaments;
}

/// Soundfont

void PluginProcessor::loadSoundfont(const juce::File &file)
{
    int loading = ++_loadingCount;
    _isLoading = true;
    sendChangeMessage();

    _loadingThread.addJob([this, file, loading] {
        // Possibly already played by another instance
        std::string error;
        std::shared_ptr<PluginSoundfont> soundfont = PluginSoundfont::load(toPath(file), error);
        if (loading != _loadingCount.load())
            return; // Another soundfont has been asked since
        setSoundfont(soundfont, file, soundfont == nullptr ? juce::String(error) : juce::String());
        _isLoading = false;
        sendChangeMessage();
    });
}

void PluginProcessor::setSoundfont(std::shared_ptr<PluginSoundfont> soundfont, const juce::File &file,
                                   const juce::String &error)
{
    // A soundfont that cannot be read does not replace the current one
    juce::String message = error;
    if (soundfont != nullptr && !_engine.setSoundfont(soundfont->getSynthSoundfont()))
        message = "too many soundfonts are still playing, try again in a moment";
    else if (soundfont != nullptr)
    {
        const juce::ScopedLock lock(_soundfontLock);
        _soundfontFile = file;
        _soundfont = soundfont;
    }

    {
        const juce::ScopedLock lock(_soundfontLock);
        _error = message;
    }

    // Recent files
    if (message.isEmpty() && file.existsAsFile())
    {
        juce::PropertiesFile * settings = _settings.getUserSettings();
        settings->reload();
        juce::RecentlyOpenedFilesList recentFiles;
        recentFiles.setMaxNumberOfItems(MAX_RECENT_FILES);
        recentFiles.restoreFromString(settings->getValue("recentFiles"));
        recentFiles.addFile(file);
        settings->setValue("recentFiles", recentFiles.toString());
        settings->saveIfNeeded();
    }
}

juce::File PluginProcessor::getSoundfontFile() const
{
    const juce::ScopedLock lock(_soundfontLock);
    return _soundfontFile;
}

juce::String PluginProcessor::getError() const
{
    const juce::ScopedLock lock(_soundfontLock);
    return _error;
}

juce::StringArray PluginProcessor::getRecentFiles()
{
    juce::PropertiesFile * settings = _settings.getUserSettings();
    settings->reload();
    juce::RecentlyOpenedFilesList recentFiles;
    recentFiles.setMaxNumberOfItems(MAX_RECENT_FILES);
    recentFiles.restoreFromString(settings->getValue("recentFiles"));
    recentFiles.removeNonExistentFiles();
    juce::StringArray files;
    for (int i = 0; i < recentFiles.getNumFiles(); i++)
        files.add(recentFiles.getFile(i).getFullPathName());
    return files;
}

void PluginProcessor::setSoundfontStored(bool isStored)
{
    _isSoundfontStored = isStored;
    sendChangeMessage();
}

/// Settings

void PluginProcessor::setPolyphony(int polyphony)
{
    if (polyphony == _polyphony)
        return;
    _polyphony = polyphony;

    // The voices are allocated again while no audio is computed
    if (_isPrepared)
    {
        suspendProcessing(true);
        _engine.prepare(static_cast<uint32_t>(_sampleRate + 0.5), static_cast<uint32_t>(_maxBlockSize), _polyphony);
        suspendProcessing(false);
    }
    sendChangeMessage();
}

void PluginProcessor::setMultipleOutputs(bool hasMultipleOutputs)
{
    _hasMultipleOutputs = hasMultipleOutputs;
    sendChangeMessage();
}

void PluginProcessor::timerCallback()
{
    _engine.deleteUnusedSoundfonts();
}

/// Audio

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    _sampleRate = sampleRate;
    _maxBlockSize = juce::jmax(samplesPerBlock, 64);
    _engine.prepare(static_cast<uint32_t>(sampleRate + 0.5), static_cast<uint32_t>(_maxBlockSize), _polyphony);
    _isPrepared = true;

    // The settings are applied again by the next block
    for (float &value : _appliedValues)
        value = NAN;
}

void PluginProcessor::releaseResources()
{
    _keyboardState.allNotesOff(0);
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    // Stereo main output, the others being stereo or disabled
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo() ||
        !layouts.getMainInputChannelSet().isDisabled())
        return false;
    for (int bus = 1; bus < layouts.outputBuses.size(); bus++)
        if (!layouts.outputBuses[bus].isDisabled() && layouts.outputBuses[bus] != juce::AudioChannelSet::stereo())
            return false;
    return true;
}

void PluginProcessor::updateConfiguration()
{
    // Applied only if a parameter changed
    float values[12];
    bool hasChanged = false;
    for (int i = 0; i < 12; i++)
    {
        values[i] = _parameterValues[i]->load(std::memory_order_relaxed);
        if (std::memcmp(&values[i], &_appliedValues[i], sizeof(float)) != 0)
            hasChanged = true;
    }
    if (!hasChanged)
        return;

    SynthCoreConfig configuration;
    configuration.masterGain = values[0];
    configuration.revLevel = juce::roundToInt(values[1]);
    configuration.revSize = juce::roundToInt(values[2]);
    configuration.revWidth = juce::roundToInt(values[3]);
    configuration.revDamping = juce::roundToInt(values[4]);
    configuration.choLevel = juce::roundToInt(values[5]);
    configuration.choDepth = juce::roundToInt(values[6]);
    configuration.choFrequency = juce::roundToInt(values[7]);
    configuration.referencePitch = juce::roundToInt(values[9] * 10.0f);
    const std::vector<Temperament> &temperaments = getTemperaments();
    const Temperament &temperament = temperaments[static_cast<size_t>(
        juce::jlimit(0, static_cast<int>(temperaments.size()) - 1, juce::roundToInt(values[10])))];
    for (int i = 0; i < 12; i++)
        configuration.temperament[i] = temperament.fineTunes[i];
    configuration.temperamentRelativeKey = juce::jlimit(0, 11, juce::roundToInt(values[11]));
    _engine.setConfiguration(configuration, juce::roundToInt(values[8]));

    for (int i = 0; i < 12; i++)
        _appliedValues[i] = values[i];
}

// Stereo output of a bus, false if it is disabled
static bool getBusOutput(juce::AudioProcessor &processor, juce::AudioBuffer<float> &buffer, int bus, float * &left,
                         float * &right)
{
    left = right = nullptr;
    if (bus >= processor.getBusCount(false) || processor.getChannelCountOfBus(false, bus) != 2)
        return false;
    int first = processor.getChannelIndexInProcessBlockBuffer(false, bus, 0);
    if (first + 1 >= buffer.getNumChannels())
        return false;
    left = buffer.getWritePointer(first);
    right = buffer.getWritePointer(first + 1);
    return true;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int frameCount = buffer.getNumSamples();

    // Main output, and the outputs of the channels and of the effects that the host enabled
    SynthCore::Outputs outputs;
    if (!getBusOutput(*this, buffer, 0, outputs.left, outputs.right))
    {
        buffer.clear();
        return;
    }
    float * left, * right;
    bool hasMultipleOutputs = _hasMultipleOutputs.load(std::memory_order_relaxed);
    for (int channel = 0; channel < CHANNEL_BUS_COUNT; channel++)
    {
        if (getBusOutput(*this, buffer, 1 + channel, left, right))
        {
            if (hasMultipleOutputs)
            {
                outputs.channelLeft[channel] = left;
                outputs.channelRight[channel] = right;
            }
            else
            {
                // Silent, the channel being in the main output
                juce::FloatVectorOperations::clear(left, frameCount);
                juce::FloatVectorOperations::clear(right, frameCount);
            }
        }
    }
    if (getBusOutput(*this, buffer, EFFECTS_BUS, left, right))
    {
        if (hasMultipleOutputs)
        {
            outputs.effectsLeft = left;
            outputs.effectsRight = right;
        }
        else
        {
            juce::FloatVectorOperations::clear(left, frameCount);
            juce::FloatVectorOperations::clear(right, frameCount);
        }
    }

    // Notes of the virtual keyboard added to the ones of the host, the keyboard showing all of them
    _keyboardState.processNextMidiBuffer(midiMessages, 0, frameCount, true);

    // Settings, requests of the interface, then the MIDI messages at their position in the block
    updateConfiguration();
    _engine.beginBlock(!isNonRealtime());
    int position = 0;
    auto render = [this, &outputs, &position](int end) {
        SynthCore::Outputs part;
        auto offset = [&position](float * data) { return data == nullptr ? nullptr : data + position; };
        part.left = offset(outputs.left);
        part.right = offset(outputs.right);
        for (int channel = 0; channel < CHANNEL_BUS_COUNT; channel++)
        {
            part.channelLeft[channel] = offset(outputs.channelLeft[channel]);
            part.channelRight[channel] = offset(outputs.channelRight[channel]);
        }
        part.effectsLeft = offset(outputs.effectsLeft);
        part.effectsRight = offset(outputs.effectsRight);
        _engine.render(part, static_cast<uint32_t>(end - position));
        position = end;
    };
    for (const juce::MidiMessageMetadata metadata : midiMessages)
    {
        int messagePosition = juce::jlimit(0, frameCount, metadata.samplePosition);
        if (messagePosition > position)
            render(messagePosition);
        _engine.processMidi(metadata.data, static_cast<size_t>(metadata.numBytes));
    }
    if (position < frameCount)
        render(frameCount);
    _engine.endBlock();

    // Other channels given by the host, if any, are silent
    for (int channel = getTotalNumOutputChannels(); channel < buffer.getNumChannels(); channel++)
        buffer.clear(channel, 0, frameCount);
    midiMessages.clear();
}

juce::AudioProcessorEditor * PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

/// State

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::ValueTree state("PolyphonePlugin");
    state.setProperty("version", 1, nullptr);
    state.appendChild(_parameters.copyState(), nullptr);
    state.setProperty("polyphony", _polyphony, nullptr);
    state.setProperty("multipleOutputs", _hasMultipleOutputs.load(), nullptr);

    // Presets
    state.setProperty("mode", static_cast<int>(_engine.getMode()), nullptr);
    state.setProperty("program", _engine.getProgram(), nullptr);
    juce::StringArray channelPrograms;
    for (int channel = 0; channel < 16; channel++)
        channelPrograms.add(juce::String(_engine.getChannelProgram(channel)));
    state.setProperty("channelPrograms", channelPrograms.joinIntoString(","), nullptr);

    // Soundfont, possibly with the content of the file
    juce::ValueTree soundfont("Soundfont");
    juce::File file = getSoundfontFile();
    soundfont.setProperty("path", file.getFullPathName(), nullptr);
    soundfont.setProperty("stored", _isSoundfontStored, nullptr);
    if (_isSoundfontStored && _engine.hasSoundfont() && !file.hasFileExtension("sfz")) // Samples of an sfz in other files
    {
        juce::MemoryBlock content;
        {
            const juce::ScopedLock lock(_soundfontLock);
            if (_soundfont != nullptr && !_soundfont->getContent().empty())
                content.replaceAll(_soundfont->getContent().data(), _soundfont->getContent().size());
        }
        if (content.getSize() == 0)
            file.loadFileAsData(content);
        if (content.getSize() > 0)
            soundfont.setProperty("content", content, nullptr);
    }
    state.appendChild(soundfont, nullptr);

    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void PluginProcessor::setStateInformation(const void * data, int sizeInBytes)
{
    juce::ValueTree state = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
    if (!state.isValid() || !state.hasType("PolyphonePlugin"))
        return;

    juce::ValueTree parameters = state.getChildWithName(_parameters.state.getType());
    if (parameters.isValid())
        _parameters.replaceState(parameters);
    setPolyphony(state.getProperty("polyphony", DEFAULT_POLYPHONY));
    _hasMultipleOutputs = static_cast<bool>(state.getProperty("multipleOutputs", false));

    // Soundfont read now, the host possibly computing the audio just after (a loading in progress is not used)
    _loadingCount++;
    juce::ValueTree soundfontState = state.getChildWithName("Soundfont");
    _isSoundfontStored = soundfontState.getProperty("stored", false);
    juce::String path = soundfontState.getProperty("path", "").toString();
    juce::File file = path.isEmpty() ? juce::File() : juce::File(path);
    juce::MemoryBlock content;
    if (const juce::MemoryBlock * stored = soundfontState.getProperty("content").getBinaryData())
        content = *stored;

    // Possibly already played by another instance
    std::shared_ptr<PluginSoundfont> soundfont;
    std::string error;
    if (file.existsAsFile())
        soundfont = PluginSoundfont::load(toPath(file), error);
    if (soundfont == nullptr && content.getSize() > 0)
        soundfont = PluginSoundfont::load(static_cast<const char *>(content.getData()), content.getSize(), error);

    if (soundfont != nullptr)
        setSoundfont(soundfont, file, juce::String());
    else
    {
        // Nothing played, the file can be located
        _engine.setSoundfont(nullptr);
        const juce::ScopedLock lock(_soundfontLock);
        _soundfontFile = file;
        _soundfont.reset();
        _error = path.isEmpty() ? juce::String() : (file.existsAsFile() ? juce::String(error) : "file not found");
    }
    _isLoading = false;

    // Presets
    int mode = state.getProperty("mode", 0);
    _engine.setMode(mode == 1 ? PluginEngine::MODE_CHANNELS :
                                (mode == 2 ? PluginEngine::MODE_MPE : PluginEngine::MODE_ONE_PRESET));
    _engine.setProgram(state.getProperty("program", 0));
    juce::StringArray channelPrograms = juce::StringArray::fromTokens(state.getProperty("channelPrograms", "").toString(),
                                                                      ",", "");
    for (int channel = 0; channel < 16; channel++)
        _engine.setChannelProgram(channel, channel < channelPrograms.size() ? channelPrograms[channel].getIntValue() : -1);

    sendChangeMessage();
}

juce::AudioProcessor * JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
