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
#include <iterator>
#include "plugineditor.h"
#include "synthconstants.h"
#include "synthsoundfontfile.h"
#include "pluginsoundfont.h"
#include "BinaryData.h"

// Parameters, in the order of _parameterValues
static const char * PARAMETER_IDS[] = {
    "gain", "reverbLevel", "reverbSize", "reverbWidth", "reverbDamping", "chorusLevel", "chorusDepth", "chorusFrequency",
    "transpose", "tuning", "temperament", "temperamentKey", "velocityToFilter", "velocityCurve", "temperamentEnabled",
    "reverbEnabled", "chorusEnabled"
};

static const int MAX_RECENT_FILES = 10;

// Output buses: the main one, then the ones of the 16 channels and of the effects (disabled by default). Their order
// must never change once the plugin is published (the projects of the hosts use it).
static const int CHANNEL_BUS_COUNT = 16;
static const int EFFECTS_BUS = CHANNEL_BUS_COUNT + 1;

juce::AudioProcessor::BusesProperties PluginProcessor::createBuses()
{
    BusesProperties buses;
    buses = buses.withOutput(tr("Main"), juce::AudioChannelSet::stereo(), true);
    for (int channel = 1; channel <= CHANNEL_BUS_COUNT; channel++)
        buses = buses.withOutput(PluginTranslator::arg(tr("Channel %1"), juce::String(channel)),
                                 juce::AudioChannelSet::stereo(), false);
    return buses.withOutput(tr("Effects"), juce::AudioChannelSet::stereo(), false);
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
    _hasFixedLatency(false),
    _isClipping(true),
    _playUnusedNotes(false),
    _appliedPlayUnusedNotes(false),
    _referencePitchKey(9),
    _tuningVersion(0),
    _appliedTuningVersion(0),
    _isPrepared(false),
    _sampleRate(48000.0),
    _maxBlockSize(512)
{
    static_assert(std::size(PARAMETER_IDS) == PARAMETER_COUNT, "one identifier for each parameter");
    for (int i = 0; i < 12; i++)
    {
        _customFineTunes[i] = 0.0f;
        _customUsed[i] = true;
    }
    for (int i = 0; i < PARAMETER_COUNT; i++)
    {
        _parameterValues[i] = _parameters.getRawParameterValue(PARAMETER_IDS[i]);
        _appliedValues[i] = NAN; // Applied by the first block
    }

    // Settings shared by all instances
    _settings.setStorageParameters(getSettingsOptions());
    _settings.getUserSettings();

    // Soundfonts replaced that are not played anymore, samples of the presets played read first
    startTimer(500);
}

PluginProcessor::~PluginProcessor()
{
    stopTimer();
    _loadingCount++; // A soundfont being read is not used
    _loadingThread.removeAllJobs(true, -1);
}

juce::PropertiesFile::Options PluginProcessor::getSettingsOptions()
{
    juce::PropertiesFile::Options options;
    options.applicationName = "Polyphone plugin";
    options.filenameSuffix = ".settings";
    options.folderName = "Polyphone";
    options.osxLibrarySubFolder = "Application Support";
    return options;
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameters()
{
    // Same ranges and default values as the settings of the editor (except the gain). The names are translated in the
    // language of the moment (the parameters of an instance keep them).
    auto percentage = [](const char * id, const juce::String &name) {
        return std::make_unique<juce::AudioParameterInt>(juce::ParameterID(id, 1), name, 0, 100, 0,
                                                         juce::AudioParameterIntAttributes().withLabel("%"));
    };

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Gain limited to +/-24 dB (+/-50 dB in the editor), 0 being in the middle
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("gain", 1), tr("Gain"), juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "reverb", tr("Reverb"), "|", percentage("reverbLevel", tr("Reverb level")),
        percentage("reverbSize", tr("Reverb size")), percentage("reverbWidth", tr("Reverb width")),
        percentage("reverbDamping", tr("Reverb damping"))));
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "chorus", tr("Chorus"), "|", percentage("chorusLevel", tr("Chorus level")),
        percentage("chorusDepth", tr("Chorus depth")), percentage("chorusFrequency", tr("Chorus frequency"))));

    juce::StringArray temperaments;
    for (size_t i = 0; i <= getTemperaments().size(); i++) // Custom temperament at the end
        temperaments.add(getTemperamentName(i));
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "tuning", tr("Tuning"), "|",
        std::make_unique<juce::AudioParameterInt>(juce::ParameterID("transpose", 1), tr("Transposition"), -24, 24, 0,
                                                  juce::AudioParameterIntAttributes().withLabel(tr("semitones"))),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("tuning", 1), tr("Reference pitch"),
                                                    juce::NormalisableRange<float>(400.0f, 480.0f, 0.1f), 440.0f,
                                                    juce::AudioParameterFloatAttributes().withLabel("Hz")),
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("temperament", 1), tr("Temperament"),
                                                     temperaments, 0),
        std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("temperamentKey", 1), tr("Temperament key"),
            juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 0)));

    // Added in a later version (versions of the parameters for AU), after the others for the hosts using their order
    layout.add(std::make_unique<juce::AudioProcessorParameterGroup>(
        "velocity", tr("Velocity"), "|",
        std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("velocityToFilter", 2),
                                                     tr("Velocity to filter cutoff"), getVelocityToFilterNames(), 1),
        std::make_unique<juce::AudioParameterInt>(juce::ParameterID("velocityCurve", 2), tr("Velocity curve"), -100,
                                                  100, 0)));

    // Switches of the temperament (equal temperament otherwise) and of the effects (their settings being kept)
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("temperamentEnabled", 2),
                                                          tr("Temperament enabled"), false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("reverbEnabled", 2), tr("Reverb enabled"),
                                                          false));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("chorusEnabled", 2), tr("Chorus enabled"),
                                                          false));
    return layout;
}

juce::StringArray PluginProcessor::getVelocityToFilterNames()
{
    return { "SoundFont 2.01", "SoundFont 2.04", tr("Disabled", "velocity to filter") };
}

const std::vector<TemperamentData> &PluginProcessor::getTemperaments()
{
    // Equal temperament, then the ones of the editor (same order, the index being saved)
    static const std::vector<TemperamentData> temperaments = [] {
        std::vector<TemperamentData> list;
        TemperamentData equal;
        equal.name = "Equal";
        list.push_back(equal);
        for (const TemperamentData &temperament : Temperaments::parse(
                 std::string(BinaryData::temperaments_csv, static_cast<size_t>(BinaryData::temperaments_csvSize))))
            list.push_back(temperament);
        return list;
    }();
    return temperaments;
}

juce::String PluginProcessor::getTemperamentName(size_t index)
{
    // Generic names translated, proper names unchanged
    if (index == 0)
        return tr("Equal", "Equal musical temperament");
    if (index == getTemperaments().size())
        return tr("Custom", "custom temperament");
    if (index > getTemperaments().size())
        return juce::String();
    return PluginTranslator::translate(Temperaments::TRANSLATION_CONTEXT, getTemperaments()[index].name.c_str());
}

juce::StringArray PluginProcessor::getTemperamentCategory(size_t index)
{
    juce::StringArray levels;
    if (index >= getTemperaments().size())
        return levels;
    for (const juce::String &level : juce::StringArray::fromTokens(
             juce::String::fromUTF8(getTemperaments()[index].category.c_str()), "/", ""))
        if (level.isNotEmpty())
            levels.add(PluginTranslator::translate(Temperaments::TRANSLATION_CONTEXT, level.toRawUTF8()));
    return levels;
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
        message = QT_TRANSLATE_NOOP("PluginEditor", "too many soundfonts are still playing, try again in a moment");
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

void PluginProcessor::setFixedLatency(bool hasFixedLatency)
{
    _hasFixedLatency = hasFixedLatency;
    sendChangeMessage();
}

void PluginProcessor::setClipping(bool isClipping)
{
    _isClipping = isClipping;
    sendChangeMessage();
}

int PluginProcessor::getCustomTemperamentIndex()
{
    return static_cast<int>(getTemperaments().size());
}

double PluginProcessor::getRatioToA4(int key)
{
    // Notes from A#3 to A4, so that the step of 0.1 Hz of A4 is not larger for the other notes
    return std::pow(2.0, ((key <= 9 ? key : key - 12) - 9) / 12.0);
}

void PluginProcessor::setReferencePitchKey(int key)
{
    _referencePitchKey = (key % 12 + 12) % 12;
    _tuningVersion++; // Applied by the next block
    sendChangeMessage();
}

void PluginProcessor::getCustomTemperament(float fineTunes[12], bool used[12]) const
{
    for (int i = 0; i < 12; i++)
    {
        fineTunes[i] = _customFineTunes[i].load();
        used[i] = _customUsed[i].load();
    }
}

void PluginProcessor::setCustomTemperament(const float fineTunes[12], const bool used[12])
{
    for (int i = 0; i < 12; i++)
    {
        _customFineTunes[i] = fineTunes[i];
        _customUsed[i] = used[i];
    }
    _tuningVersion++; // Applied by the next block
}

void PluginProcessor::getTemperament(int index, int tonality, float fineTunes[12], bool used[12]) const
{
    // From the tonic, then transposed
    float tonicFineTunes[12];
    bool tonicUsed[12];
    const std::vector<TemperamentData> &temperaments = getTemperaments();
    index = juce::jlimit(0, getCustomTemperamentIndex(), index);
    if (index == getCustomTemperamentIndex())
        getCustomTemperament(tonicFineTunes, tonicUsed);
    else
    {
        for (int i = 0; i < 12; i++)
        {
            tonicFineTunes[i] = temperaments[static_cast<size_t>(index)].fineTunes[i];
            tonicUsed[i] = temperaments[static_cast<size_t>(index)].used[i];
        }
    }
    tonality = (tonality % 12 + 12) % 12;
    for (int key = 0; key < 12; key++)
    {
        fineTunes[key] = tonicFineTunes[(key - tonality + 12) % 12];
        used[key] = tonicUsed[(key - tonality + 12) % 12];
    }
}

void PluginProcessor::setPlayUnusedNotes(bool playUnusedNotes)
{
    _playUnusedNotes = playUnusedNotes; // Applied by the next block
    sendChangeMessage();
}

void PluginProcessor::timerCallback()
{
    _engine.deleteUnusedSoundfonts();
    _engine.prioritizePlayedPresets();
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
    // Applied only if a parameter, the option playing the unused notes, the reference pitch key or the custom
    // temperament changed
    float values[PARAMETER_COUNT];
    bool playUnusedNotes = _playUnusedNotes.load(std::memory_order_relaxed);
    int tuningVersion = _tuningVersion.load();
    bool hasChanged = (playUnusedNotes != _appliedPlayUnusedNotes) || (tuningVersion != _appliedTuningVersion);
    for (int i = 0; i < PARAMETER_COUNT; i++)
    {
        values[i] = _parameterValues[i]->load(std::memory_order_relaxed);
        if (std::memcmp(&values[i], &_appliedValues[i], sizeof(float)) != 0)
            hasChanged = true;
    }
    if (!hasChanged)
        return;

    // Effects switched off: level 0, temperament switched off: equal temperament, all the notes being played
    bool isTemperamentEnabled = values[14] > 0.5f;
    bool isReverbEnabled = values[15] > 0.5f;
    bool isChorusEnabled = values[16] > 0.5f;
    SynthCoreConfig configuration;
    configuration.masterGain = values[0];
    configuration.revLevel = isReverbEnabled ? juce::roundToInt(values[1]) : 0;
    configuration.revSize = juce::roundToInt(values[2]);
    configuration.revWidth = juce::roundToInt(values[3]);
    configuration.revDamping = juce::roundToInt(values[4]);
    configuration.choLevel = isChorusEnabled ? juce::roundToInt(values[5]) : 0;
    configuration.choDepth = juce::roundToInt(values[6]);
    configuration.choFrequency = juce::roundToInt(values[7]);
    configuration.referencePitch = juce::roundToInt(values[9] * 10.0f);
    float fineTunes[12];
    bool used[12];
    getTemperament(isTemperamentEnabled ? juce::roundToInt(values[10]) : 0, 0, fineTunes, used); // From the tonic
    for (int i = 0; i < 12; i++)
    {
        configuration.temperament[i] = fineTunes[i];
        configuration.temperamentUsed[i] = used[i] || playUnusedNotes;
    }
    configuration.temperamentRelativeKey = juce::jlimit(0, 11, juce::roundToInt(values[11]));
    configuration.temperamentReferenceKey = _referencePitchKey.load(); // Keeps its pitch
    configuration.modulatorVelToFilterType = juce::jlimit(0, 2, juce::roundToInt(values[12])); // Next notes
    _engine.setConfiguration(configuration, juce::roundToInt(values[8]));
    _engine.setVelocityCurve(juce::roundToInt(values[13]));

    for (int i = 0; i < PARAMETER_COUNT; i++)
        _appliedValues[i] = values[i];
    _appliedPlayUnusedNotes = playUnusedNotes;
    _appliedTuningVersion = tuningVersion;
}

// Position of the first note started in a block, or of the first message if no note starts (0 if there is none)
static int getFirstNotePosition(const juce::MidiBuffer &midiMessages)
{
    int firstPosition = -1;
    for (const juce::MidiMessageMetadata metadata : midiMessages)
    {
        if (firstPosition < 0)
            firstPosition = metadata.samplePosition;
        if (metadata.numBytes >= 3 && (metadata.data[0] & 0xF0) == 0x90 && metadata.data[2] > 0)
            return juce::jmax(0, metadata.samplePosition);
    }
    return juce::jmax(0, firstPosition);
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
    _engine.setClipping(_isClipping.load(std::memory_order_relaxed));
    bool isRealTime = !isNonRealtime();
    _engine.beginBlock(isRealTime);

    // In real time and without a fixed latency, the messages are moved together so that the first note starts at the
    // beginning of the block: the notes played live (received during the previous block) start sooner on average, and the
    // durations between the messages of a block are kept (a short note is not lost)
    int shift = (isRealTime && !_hasFixedLatency.load(std::memory_order_relaxed)) ? getFirstNotePosition(midiMessages) : 0;
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
        int messagePosition = juce::jlimit(0, frameCount, metadata.samplePosition - shift);
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
    state.setProperty("fixedLatency", _hasFixedLatency.load(), nullptr);
    state.setProperty("clipping", _isClipping.load(), nullptr);
    state.setProperty("playUnusedNotes", _playUnusedNotes.load(), nullptr);

    // Tuning: note of the reference pitch, custom temperament ("-" for a note not used)
    state.setProperty("referencePitchKey", _referencePitchKey.load(), nullptr);
    juce::StringArray customTemperament;
    for (int i = 0; i < 12; i++)
        customTemperament.add(_customUsed[i].load() ? juce::String(_customFineTunes[i].load(), 1) : juce::String("-"));
    state.setProperty("customTemperament", customTemperament.joinIntoString(","), nullptr);

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
    {
        // States saved before the switches: the effects heard and a temperament other than the equal one are enabled
        // (the switches missing are found before the state is replaced, which adds them with their default values)
        const char * const switchIds[] = { "temperamentEnabled", "reverbEnabled", "chorusEnabled" };
        bool isSwitchMissing[3];
        for (int i = 0; i < 3; i++)
            isSwitchMissing[i] = !parameters.getChildWithProperty("id", switchIds[i]).isValid();
        _parameters.replaceState(parameters);
        bool isEnabled[3] = { juce::roundToInt(_parameters.getRawParameterValue("temperament")->load()) != 0,
                              _parameters.getRawParameterValue("reverbLevel")->load() > 0.0f,
                              _parameters.getRawParameterValue("chorusLevel")->load() > 0.0f };
        for (int i = 0; i < 3; i++)
            if (isSwitchMissing[i])
                _parameters.getParameter(switchIds[i])->setValueNotifyingHost(isEnabled[i] ? 1.0f : 0.0f);
    }
    setPolyphony(state.getProperty("polyphony", DEFAULT_POLYPHONY));
    _hasMultipleOutputs = static_cast<bool>(state.getProperty("multipleOutputs", false));
    _hasFixedLatency = static_cast<bool>(state.getProperty("fixedLatency", false));
    _isClipping = static_cast<bool>(state.getProperty("clipping", true));
    _playUnusedNotes = static_cast<bool>(state.getProperty("playUnusedNotes", false));
    _referencePitchKey = (static_cast<int>(state.getProperty("referencePitchKey", 9)) % 12 + 12) % 12;
    juce::StringArray customTemperament = juce::StringArray::fromTokens(
        state.getProperty("customTemperament", "").toString(), ",", "");
    for (int i = 0; i < 12; i++)
    {
        juce::String value = i < customTemperament.size() ? customTemperament[i].trim() : juce::String("0");
        _customUsed[i] = (value != "-");
        _customFineTunes[i] = _customUsed[i] ? juce::jlimit(-50.0f, 50.0f, value.getFloatValue()) : 0.0f;
    }
    _tuningVersion++; // Applied by the next block

    // Structure of the soundfont read now, the host possibly computing the audio just after (a loading in progress is
    // not used): its samples are read by other threads, an offline rendering waiting for them
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
        _error = path.isEmpty() ? juce::String() : (file.existsAsFile() ? juce::String(error) :
                                                            QT_TRANSLATE_NOOP("PluginEditor", "file not found"));
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
