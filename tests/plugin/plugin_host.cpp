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


// Host loading the plugin built (VST3) as a DAW does, for testing it without a DAW (see README.md):
//   plugin_host case <plugin> <soundfont directory> <event file> <output wav>   (VST3, LV2 or CLAP)
//   plugin_host checks <plugin> <soundfont> <other soundfont>                 (VST3 or LV2)
//   plugin_host clapchecks <plugin> <soundfont>                               (CLAP)

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "clap_instance.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <thread>
#ifdef __linux__
#include <unistd.h>
#endif

static const int CHUNK = 64; // COMPUTATION_CHUNK of the synth core

/// Plugin

static std::unique_ptr<juce::AudioPluginInstance> loadPlugin(const juce::String &pluginPath, double sampleRate, int blockSize)
{
    // Absolute path: the LV2 format of JUCE only loads a bundle given by an absolute path ("No plugin" otherwise)
    juce::String path = juce::File::getCurrentWorkingDirectory().getChildFile(pluginPath).getFullPathName();
    juce::AudioPluginFormatManager formats;
    juce::addDefaultFormatsToManager(formats);
    juce::OwnedArray<juce::PluginDescription> descriptions;
    for (juce::AudioPluginFormat * format : formats.getFormats())
        if (path.endsWithIgnoreCase(format->getName() == "LV2" ? ".lv2" : ".vst3"))
            format->findAllTypesForFile(descriptions, path);
    if (descriptions.isEmpty())
    {
        printf("No plugin in %s\n", path.toRawUTF8());
        return nullptr;
    }
    juce::String error;
    std::unique_ptr<juce::AudioPluginInstance> instance = formats.createPluginInstance(*descriptions[0], sampleRate,
                                                                                      blockSize, error);
    if (instance == nullptr)
        printf("Cannot load the plugin: %s\n", error.toRawUTF8());
    else
        instance->enableAllBuses();
    return instance;
}

// State of the plugin: soundfont, presets, parameters
struct PluginState
{
    juce::String soundfont;
    bool isStored = false;
    int mode = 1; // 16 channels
    int program = 0;
    int polyphony = 1024;
    bool hasMultipleOutputs = false;
    std::map<juce::String, float> parameters;

    juce::MemoryBlock write() const
    {
        juce::ValueTree state("PolyphonePlugin");
        state.setProperty("version", 1, nullptr);
        juce::ValueTree parameterTree("Parameters");
        for (const auto &parameter : parameters)
        {
            juce::ValueTree child("PARAM");
            child.setProperty("id", parameter.first, nullptr);
            child.setProperty("value", parameter.second, nullptr);
            parameterTree.appendChild(child, nullptr);
        }
        state.appendChild(parameterTree, nullptr);
        state.setProperty("polyphony", polyphony, nullptr);
        state.setProperty("multipleOutputs", hasMultipleOutputs, nullptr);
        state.setProperty("mode", mode, nullptr);
        state.setProperty("program", program, nullptr);
        state.setProperty("channelPrograms", "-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1", nullptr);
        juce::ValueTree soundfontTree("Soundfont");
        soundfontTree.setProperty("path", soundfont, nullptr);
        soundfontTree.setProperty("stored", isStored, nullptr);
        state.appendChild(soundfontTree, nullptr);
        juce::MemoryBlock data;
        juce::MemoryOutputStream stream(data, false);
        state.writeToStream(stream);
        return data;
    }
};

// State of the plugin given through the host of its format, as its getStateInformation gives it: in an XML element for
// VST3, in the value StateString of a state in Turtle for LV2
static void setState(juce::AudioPluginInstance &instance, const juce::MemoryBlock &state)
{
    if (instance.getPluginDescription().pluginFormatName == "LV2")
    {
        juce::MemoryBlock current;
        instance.getStateInformation(current);
        juce::String turtle = current.toString();
        int key = turtle.indexOf(":StateString>");
        int start = turtle.indexOfChar(key, '"');
        int end = turtle.indexOfChar(start + 1, '"');
        turtle = turtle.replaceSection(start + 1, end - start - 1, state.toBase64Encoding());
        instance.setStateInformation(turtle.toRawUTF8(), static_cast<int>(turtle.getNumBytesAsUTF8()));
        return;
    }

    juce::XmlElement xml("VST3PluginState");
    xml.createNewChildElement("IComponent")->addTextElement(state.toBase64Encoding());
    juce::MemoryBlock data;
    juce::AudioProcessor::copyXmlToBinary(xml, data);
    instance.setStateInformation(data.getData(), static_cast<int>(data.getSize()));
}

// State given by getStateInformation of the host
static void setHostState(juce::AudioPluginInstance &instance, const juce::MemoryBlock &state)
{
    instance.setStateInformation(state.getData(), static_cast<int>(state.getSize()));
}

// Interleaved stereo 32-bit float wav file, written as OfflineRenderer does
static bool writeWav(const juce::String &path, const std::vector<float> &data, uint32_t frameCount, uint32_t sampleRate)
{
    std::ofstream file(path.toStdString(), std::ios::binary);
    auto u32 = [&file](uint32_t value) { file.write(reinterpret_cast<const char *>(&value), 4); };
    auto u16 = [&file](uint16_t value) { file.write(reinterpret_cast<const char *>(&value), 2); };
    uint32_t dataSize = frameCount * 8;
    file.write("RIFF", 4);
    u32(4 + 26 + 12 + 8 + dataSize);
    file.write("WAVEfmt ", 8);
    u32(18);
    u16(3);
    u16(2);
    u32(sampleRate);
    u32(sampleRate * 8);
    u16(8);
    u16(32);
    u16(0);
    file.write("fact", 4);
    u32(4);
    u32(frameCount);
    file.write("data", 4);
    u32(dataSize);
    file.write(reinterpret_cast<const char *>(data.data()), dataSize);
    return file.good();
}

/// Event files of tests/render

struct Case
{
    juce::String soundfont;
    int sampleRate = 48000;
    int blockSize = 256;
    double duration = 0;
    float gain = 0;
    float tuning = 440;
    int reverb[4] = {};
    int chorus[3] = {};
    int polyphony = 1024;
    juce::String unsupported; // Setting or event that only the editor has
    int64_t firstInexactFrame = -1; // Bend that MIDI cannot carry exactly
    std::vector<std::pair<int64_t, juce::MidiMessage>> events;
};

static bool readCase(const juce::File &file, Case &result)
{
    juce::StringArray lines;
    file.readLines(lines);
    for (const juce::String &fullLine : lines)
    {
        if (fullLine.startsWith("# soundfont:"))
            result.soundfont = fullLine.fromFirstOccurrenceOf(":", false, false).trim();
        juce::StringArray words = juce::StringArray::fromTokens(fullLine.upToFirstOccurrenceOf("#", false, false), true);
        words.removeEmptyStrings(); // Several spaces between the words
        if (words.isEmpty())
            continue;

        if (!words[0].containsOnly("0123456789."))
        {
            // Settings
            const juce::String &name = words[0];
            if (name == "samplerate")
                result.sampleRate = words[1].getIntValue();
            else if (name == "block")
                result.blockSize = words[1].getIntValue();
            else if (name == "duration")
                result.duration = static_cast<double>(words[1].getFloatValue());
            else if (name == "gain")
                result.gain = words[1].getFloatValue();
            else if (name == "tuning")
                result.tuning = words[1].getFloatValue();
            else if (name == "reverb")
                for (int i = 0; i < 4; i++)
                    result.reverb[i] = words[i + 1].getIntValue();
            else if (name == "chorus")
                for (int i = 0; i < 3; i++)
                    result.chorus[i] = words[i + 1].getIntValue();
            else if (name == "polyphony")
                result.polyphony = words[1].getIntValue();
            else if (result.unsupported.isEmpty())
                result.unsupported = name;
            continue;
        }

        // Events, converted to MIDI messages as OfflineRenderer::processEvents gives them to MidiProcessor
        int64_t frame = static_cast<int64_t>(std::llround(words[0].getDoubleValue() * result.sampleRate));
        const juce::String &name = words[1];
        int channel = words[2].getIntValue() + 1;
        auto add = [&](const juce::MidiMessage &message) { result.events.push_back({ frame, message }); };
        if (name == "program")
        {
            if (channel != 10)
                add(juce::MidiMessage::controllerEvent(channel, 0, words[3].getIntValue()));
            add(juce::MidiMessage::programChange(channel, words[4].getIntValue()));
        }
        else if (name == "on" && words[4].getIntValue() > 0)
            add(juce::MidiMessage::noteOn(channel, words[3].getIntValue(), static_cast<juce::uint8>(words[4].getIntValue())));
        else if (name == "on" || name == "off")
            add(juce::MidiMessage::noteOff(channel, words[3].getIntValue(), static_cast<juce::uint8>(0)));
        else if (name == "cc")
            add(juce::MidiMessage::controllerEvent(channel, words[3].getIntValue(), words[4].getIntValue()));
        else if (name == "bend")
        {
            double value = words[3].getDoubleValue() * 8192.0 + 8192.0;
            int midiValue = juce::jlimit(0, 16383, static_cast<int>(std::lround(value)));
            if (std::fabs(midiValue - value) > 1e-9 && result.firstInexactFrame < 0)
                result.firstInexactFrame = frame;
            add(juce::MidiMessage::pitchWheel(channel, midiValue));
        }
        else if (name == "bendrange")
        {
            float semitones = words[3].getFloatValue();
            int whole = static_cast<int>(semitones);
            add(juce::MidiMessage::controllerEvent(channel, 101, 0));
            add(juce::MidiMessage::controllerEvent(channel, 100, 0));
            add(juce::MidiMessage::controllerEvent(channel, 6, whole));
            add(juce::MidiMessage::controllerEvent(channel, 38, juce::roundToInt(100.0f * (semitones - static_cast<float>(whole)))));
        }
        else if (name == "pressure")
            add(juce::MidiMessage::channelPressureChange(channel, words[3].getIntValue()));
        else if (name == "polypressure")
            add(juce::MidiMessage::aftertouchChange(channel, words[3].getIntValue(), words[4].getIntValue()));
        else if (result.unsupported.isEmpty())
            result.unsupported = name;
    }

    std::stable_sort(result.events.begin(), result.events.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });
    return result.duration > 0;
}

static int renderCase(const juce::String &pluginPath, const juce::File &soundfontDirectory, const juce::File &caseFile,
                      const juce::String &outputPath)
{
    Case events;
    if (!readCase(caseFile, events))
    {
        printf("Cannot read %s\n", caseFile.getFullPathName().toRawUTF8());
        return 2;
    }
    if (events.unsupported.isNotEmpty())
    {
        printf("UNSUPPORTED %s (%s: editor only)\n", caseFile.getFileName().toRawUTF8(), events.unsupported.toRawUTF8());
        return 3;
    }

    // Settings of the event file and the channels choosing their presets by program change, offline
    PluginState state;
    state.soundfont = soundfontDirectory.getChildFile(events.soundfont).getFullPathName();
    state.polyphony = events.polyphony;
    state.parameters = { { "gain", events.gain }, { "tuning", events.tuning },
                         { "reverbLevel", events.reverb[0] }, { "reverbSize", events.reverb[1] },
                         { "reverbWidth", events.reverb[2] }, { "reverbDamping", events.reverb[3] },
                         { "chorusLevel", events.chorus[0] }, { "chorusDepth", events.chorus[1] },
                         { "chorusFrequency", events.chorus[2] } };
    juce::MemoryBlock stateData = state.write();

    // A first block of silence as the renderer (whole chunks, a block never being longer than announced), then the
    // events of each block at its beginning
    int firstBlockSize = (events.blockSize + CHUNK - 1) / CHUNK * CHUNK;
    int maxBlockSize = juce::jmax(firstBlockSize, events.blockSize);
    uint32_t frameCount = static_cast<uint32_t>(std::llround(events.duration * events.sampleRate));
    uint32_t blockCount = (frameCount + static_cast<uint32_t>(events.blockSize) - 1) / static_cast<uint32_t>(events.blockSize);
    std::vector<float> data(2 * static_cast<size_t>(blockCount) * static_cast<size_t>(events.blockSize));
    size_t eventIndex = 0;
    auto store = [&](uint32_t block, const float * left, const float * right) {
        for (int i = 0; i < events.blockSize; i++)
        {
            data[2 * (block * static_cast<size_t>(events.blockSize) + static_cast<size_t>(i))] = left[i];
            data[2 * (block * static_cast<size_t>(events.blockSize) + static_cast<size_t>(i)) + 1] = right[i];
        }
    };

    if (pluginPath.endsWithIgnoreCase(".clap"))
    {
        ClapInstance plugin;
        std::string error;
        if (!plugin.load(pluginPath.toStdString(), error))
        {
            printf("Cannot load the plugin: %s\n", error.c_str());
            return 1;
        }
        plugin.setState(std::vector<uint8_t>(static_cast<const uint8_t *>(stateData.getData()),
                                             static_cast<const uint8_t *>(stateData.getData()) + stateData.getSize()));
        plugin.activate(events.sampleRate, static_cast<uint32_t>(maxBlockSize), true);
        std::vector<float> left(static_cast<size_t>(maxBlockSize)), right(static_cast<size_t>(maxBlockSize));
        plugin.process(left.data(), right.data(), static_cast<uint32_t>(firstBlockSize), {});
        for (uint32_t block = 0; block < blockCount; block++)
        {
            int64_t blockEnd = static_cast<int64_t>(block + 1) * events.blockSize;
            std::vector<ClapInstance::MidiEvent> midi;
            for (; eventIndex < events.events.size() && events.events[eventIndex].first < blockEnd; eventIndex++)
            {
                const juce::MidiMessage &message = events.events[eventIndex].second;
                ClapInstance::MidiEvent event = { 0, { 0, 0, 0 } };
                memcpy(event.data, message.getRawData(), static_cast<size_t>(juce::jmin(3, message.getRawDataSize())));
                midi.push_back(event);
            }
            plugin.process(left.data(), right.data(), static_cast<uint32_t>(events.blockSize), midi);
            store(block, left.data(), right.data());
        }
        plugin.deactivate();
    }
    else
    {
        std::unique_ptr<juce::AudioPluginInstance> plugin = loadPlugin(pluginPath, events.sampleRate, events.blockSize);
        if (plugin == nullptr)
            return 1;
        setState(*plugin, stateData);
        plugin->setNonRealtime(true);
        plugin->prepareToPlay(events.sampleRate, maxBlockSize);
        int channelCount = juce::jmax(2, plugin->getTotalNumOutputChannels()); // All outputs enabled, main first
        juce::AudioBuffer<float> buffer(channelCount, maxBlockSize);
        juce::MidiBuffer midi;
        buffer.setSize(channelCount, firstBlockSize, false, false, true);
        plugin->processBlock(buffer, midi);
        buffer.setSize(channelCount, events.blockSize, false, false, true);
        for (uint32_t block = 0; block < blockCount; block++)
        {
            int64_t blockEnd = static_cast<int64_t>(block + 1) * events.blockSize;
            midi.clear();
            for (; eventIndex < events.events.size() && events.events[eventIndex].first < blockEnd; eventIndex++)
                midi.addEvent(events.events[eventIndex].second, 0);
            buffer.clear();
            plugin->processBlock(buffer, midi);
            store(block, buffer.getReadPointer(0), buffer.getReadPointer(1));
        }
        plugin->releaseResources();
    }

    if (!writeWav(outputPath, data, frameCount, static_cast<uint32_t>(events.sampleRate)))
    {
        printf("Cannot write %s\n", outputPath.toRawUTF8());
        return 4;
    }

    // The frames from the block of an inexact bend can differ from the reference
    int64_t exactFrames = events.firstInexactFrame < 0 ? frameCount :
                              events.firstInexactFrame / events.blockSize * events.blockSize;
    printf("RENDERED %s exact_frames %lld\n", caseFile.getFileName().toRawUTF8(), static_cast<long long>(exactFrames));
    return 0;
}

/// Checks

static int g_failures = 0;

static void check(bool condition, const juce::String &description)
{
    printf("%s %s\n", condition ? "PASS" : "FAIL", description.toRawUTF8());
    if (!condition)
        g_failures++;
}

// Random MIDI messages over a duration, at their position in blocks of any size (same seed: same messages)
static std::vector<std::pair<int64_t, juce::MidiMessage>> randomEvents(int sampleRate, double seconds, unsigned seed)
{
    std::mt19937 random(seed);
    std::vector<std::pair<int64_t, juce::MidiMessage>> events;
    int64_t frameCount = static_cast<int64_t>(seconds * sampleRate);
    for (int64_t frame = 0; frame < frameCount; frame += 1 + static_cast<int64_t>(random() % static_cast<unsigned>(sampleRate / 20)))
    {
        int channel = 1 + static_cast<int>(random() % 16);
        int choice = static_cast<int>(random() % 100);
        int key = 30 + static_cast<int>(random() % 60);
        if (choice < 45)
            events.push_back({ frame, juce::MidiMessage::noteOn(channel, key, static_cast<juce::uint8>(1 + random() % 127)) });
        else if (choice < 85)
            events.push_back({ frame, juce::MidiMessage::noteOff(channel, key) });
        else if (choice < 90)
            events.push_back({ frame, juce::MidiMessage::programChange(channel, static_cast<int>(random() % 128)) });
        else if (choice < 95)
            events.push_back({ frame, juce::MidiMessage::controllerEvent(channel, static_cast<int>(random() % 128), static_cast<int>(random() % 128)) });
        else
            events.push_back({ frame, juce::MidiMessage::pitchWheel(channel, static_cast<int>(random() % 16384)) });
    }
    return events;
}

// Render events with blocks of a size, the messages being at their position in the blocks. The plugin is prepared by
// the caller (on the message thread, as the host of the VST3 format requires it). The main output is returned, all the
// outputs being possibly given in outputs (one vector for each channel).
static std::vector<float> process(juce::AudioPluginInstance &plugin, int sampleRate, int blockSize,
                                  const std::vector<std::pair<int64_t, juce::MidiMessage>> &events, double seconds,
                                  std::function<void(int block)> beforeBlock = nullptr,
                                  std::vector<std::vector<float>> * outputs = nullptr)
{
    int64_t frameCount = static_cast<int64_t>(seconds * sampleRate);
    std::vector<float> data;
    data.reserve(2 * static_cast<size_t>(frameCount));
    juce::AudioBuffer<float> buffer(juce::jmax(2, plugin.getTotalNumOutputChannels()), blockSize);
    if (outputs != nullptr)
        outputs->assign(static_cast<size_t>(plugin.getTotalNumOutputChannels()), std::vector<float>());
    juce::MidiBuffer midi;
    size_t eventIndex = 0;
    int block = 0;
    for (int64_t start = 0; start < frameCount; start += blockSize, block++)
    {
        if (beforeBlock)
            beforeBlock(block);
        midi.clear();
        for (; eventIndex < events.size() && events[eventIndex].first < start + blockSize; eventIndex++)
            midi.addEvent(events[eventIndex].second, static_cast<int>(events[eventIndex].first - start));
        buffer.clear();
        plugin.processBlock(buffer, midi);
        for (int i = 0; i < blockSize; i++)
        {
            data.push_back(buffer.getSample(0, i));
            data.push_back(buffer.getSample(1, i));
        }
        if (outputs != nullptr)
            for (size_t channel = 0; channel < outputs->size(); channel++)
                (*outputs)[channel].insert((*outputs)[channel].end(), buffer.getReadPointer(static_cast<int>(channel)),
                                           buffer.getReadPointer(static_cast<int>(channel)) + blockSize);
    }
    return data;
}

static std::vector<float> render(juce::AudioPluginInstance &plugin, int sampleRate, int blockSize,
                                 const std::vector<std::pair<int64_t, juce::MidiMessage>> &events, double seconds,
                                 std::function<void(int block)> beforeBlock = nullptr,
                                 std::vector<std::vector<float>> * outputs = nullptr)
{
    plugin.prepareToPlay(sampleRate, blockSize);
    std::vector<float> data = process(plugin, sampleRate, blockSize, events, seconds, beforeBlock, outputs);
    plugin.releaseResources();
    return data;
}

// Memory used by the process (resident), 0 if unknown
static double getMemoryMB()
{
#ifdef __linux__
    std::ifstream statm("/proc/self/statm");
    long long pages = 0, residentPages = 0;
    if (statm >> pages >> residentPages)
        return static_cast<double>(residentPages) * static_cast<double>(sysconf(_SC_PAGESIZE)) / 1048576.0;
#endif
    return 0;
}

// Largest difference between two renderings on their common length, in dBFS (-1000 if identical)
static double maxDifference(const std::vector<float> &a, const std::vector<float> &b)
{
    double difference = 0;
    for (size_t i = 0; i < std::min(a.size(), b.size()); i++)
        difference = std::max(difference, std::fabs(static_cast<double>(a[i]) - static_cast<double>(b[i])));
    return difference == 0 ? -1000.0 : 20.0 * std::log10(difference);
}

static juce::String toText(double level)
{
    return level <= -1000.0 ? juce::String("identical") : juce::String(level, 1) + " dBFS";
}

// RPN 0 (pitch bend range) or 6 (MPE configuration) of a channel (from 1), at a frame
static void addRpn(std::vector<std::pair<int64_t, juce::MidiMessage>> &events, int64_t frame, int channel, int parameter,
                   int msb, int lsb)
{
    events.push_back({ frame, juce::MidiMessage::controllerEvent(channel, 101, 0) });
    events.push_back({ frame, juce::MidiMessage::controllerEvent(channel, 100, parameter) });
    events.push_back({ frame, juce::MidiMessage::controllerEvent(channel, 6, msb) });
    if (lsb >= 0)
        events.push_back({ frame, juce::MidiMessage::controllerEvent(channel, 38, lsb) });
}

static bool isValid(const std::vector<float> &data, float &peak)
{
    peak = 0;
    for (float value : data)
    {
        if (!std::isfinite(value))
            return false;
        peak = std::max(peak, std::fabs(value));
    }
    return true;
}

// New instance with a state (of the plugin, or given by the host if isHostState), offline
static std::unique_ptr<juce::AudioPluginInstance> createInstance(const juce::String &pluginPath, const juce::MemoryBlock &state,
                                                                 bool isHostState = false)
{
    std::unique_ptr<juce::AudioPluginInstance> plugin = loadPlugin(pluginPath, 48000, 512);
    if (plugin != nullptr)
    {
        if (isHostState)
            setHostState(*plugin, state);
        else
            setState(*plugin, state);
        plugin->setNonRealtime(true);
    }
    return plugin;
}

static int runChecks(const juce::String &pluginPath, const juce::String &soundfont, const juce::String &otherSoundfont)
{
    PluginState state;
    state.soundfont = soundfont;
    juce::MemoryBlock stateData = state.write();
    std::vector<std::pair<int64_t, juce::MidiMessage>> events = randomEvents(48000, 3.0, 1);
    float peak;

    // Creations and deletions
    for (int i = 0; i < 5; i++)
        if (createInstance(pluginPath, stateData) == nullptr)
            return 1;
    check(true, "5 instances created and deleted");

    // Instances playing the same soundfont share it (same process): 7 instances more do not take the memory of 7 copies
    // of a large soundfont (only their voices, about 45 MB each with 1024 voices)
    {
        std::vector<std::unique_ptr<juce::AudioPluginInstance>> instances;
        instances.push_back(createInstance(pluginPath, stateData));
        double memory = getMemoryMB();
        for (int i = 0; i < 7; i++)
            instances.push_back(createInstance(pluginPath, stateData));
        double added = getMemoryMB() - memory;
        double fileSize = static_cast<double>(juce::File(soundfont).getSize()) / 1048576.0;
        check(memory == 0 || fileSize < 50 || added < 0.5 * 7 * fileSize,
              "7 instances more playing the same soundfont (" + juce::String(fileSize, 0) + " MB): " +
                  juce::String(added, 0) + " MB" + (fileSize < 50 ? " (soundfont too small to check)" : ""));
    }

    // Reference: offline, blocks of 512 frames, the same with another instance
    std::unique_ptr<juce::AudioPluginInstance> plugin = createInstance(pluginPath, stateData);
    // All outputs enabled by the host: main, 16 channels and effects
    check(plugin->getBusCount(false) == 18 && plugin->getTotalNumOutputChannels() == 36 &&
              plugin->getChannelCountOfBus(false, 0) == 2 && plugin->getTotalNumInputChannels() == 0 && plugin->acceptsMidi(),
          "18 stereo outputs, no input, MIDI input");
    std::vector<float> reference = render(*plugin, 48000, 512, events, 3.0);
    bool isReferenceValid = isValid(reference, peak);
    check(isReferenceValid && peak > 0.001f, "random MIDI messages played (peak " + juce::String(peak, 3) + ")");
    check(render(*createInstance(pluginPath, stateData), 48000, 512, events, 3.0) == reference,
          "same rendering by another instance");

    // Any block size: the same rendering, the notes starting at the frame of their message (except rounding errors,
    // the voices being summed in another order)
    for (int blockSize : { 1, 7, 64, 333, 4096 })
    {
        std::vector<float> data = render(*createInstance(pluginPath, stateData), 48000, blockSize, events, 3.0);
        double difference = maxDifference(data, reference);
        check(isValid(data, peak) && peak > 0.001f && difference < -100.0,
              "blocks of " + juce::String(blockSize) + " frames: same rendering (" + toText(difference) + ")");
    }

    // A note starts at the frame of its message: played later, it is the same rendering shifted
    PluginState noteState;
    noteState.soundfont = soundfont;
    noteState.mode = 0;
    juce::MemoryBlock noteData = noteState.write();
    auto noteAt = [](int64_t frame) {
        return std::vector<std::pair<int64_t, juce::MidiMessage>> {
            { frame, juce::MidiMessage::noteOn(1, 60, static_cast<juce::uint8>(100)) },
            { frame + 24000, juce::MidiMessage::noteOff(1, 60) } };
    };
    std::vector<float> early = render(*createInstance(pluginPath, noteData), 48000, 512, noteAt(1000), 1.0);
    for (std::pair<int, int> blockAndShift : { std::make_pair(1, 37), std::make_pair(100, 1), std::make_pair(512, 4001),
                                               std::make_pair(64, 63) })
    {
        int shift = blockAndShift.second;
        std::vector<float> later = render(*createInstance(pluginPath, noteData), 48000, blockAndShift.first,
                                          noteAt(1000 + shift), 1.0);
        bool isShifted = isValid(later, peak) && peak > 0.001f;
        for (size_t i = 0; i < 2 * 48000 && isShifted; i++)
            isShifted = (i < 2 * static_cast<size_t>(1000 + shift)) ? later[i] == 0.0f : later[i] == early[i - 2 * static_cast<size_t>(shift)];
        check(isShifted, "note " + juce::String(shift) + " frames later, blocks of " + juce::String(blockAndShift.first) +
                             " frames: same rendering shifted");
    }

    // Multiple outputs, with effects: each channel playing has its own output, the others are silent, and with the one
    // of the effects their sum is the main output without multiple outputs
    {
        PluginState multipleState;
        multipleState.soundfont = soundfont;
        multipleState.parameters = { { "gain", -12.0f }, { "reverbLevel", 40.0f }, { "chorusLevel", 40.0f } };
        // Notes on the channels 1 to 4 with their default preset (3 key ranges), sent to the effects by the controllers 91
        // and 93 (default modulators). Not the random messages: they may select a missing preset for a channel (channel 4
        // silent with polyphone-test.sf2), and the soundfont may have no effect send (effects silent).
        std::vector<std::pair<int64_t, juce::MidiMessage>> fourChannels;
        for (int channel = 1; channel <= 4; channel++)
        {
            fourChannels.push_back({ 0, juce::MidiMessage::controllerEvent(channel, 91, 127) });
            fourChannels.push_back({ 0, juce::MidiMessage::controllerEvent(channel, 93, 127) });
            for (int key : { 40, 64, 76 })
            {
                int64_t frame = 1000 * channel + 37 * key;
                fourChannels.push_back({ frame, juce::MidiMessage::noteOn(channel, key + channel, static_cast<juce::uint8>(100)) });
                fourChannels.push_back({ frame + 72000, juce::MidiMessage::noteOff(channel, key + channel) });
            }
        }
        std::stable_sort(fourChannels.begin(), fourChannels.end(),
                         [](const auto &a, const auto &b) { return a.first < b.first; });
        std::vector<float> mainOnly = render(*createInstance(pluginPath, multipleState.write()), 48000, 512, fourChannels, 3.0);
        multipleState.hasMultipleOutputs = true;
        std::unique_ptr<juce::AudioPluginInstance> multiple = createInstance(pluginPath, multipleState.write());
        std::vector<std::vector<float>> outputs;
        render(*multiple, 48000, 512, fourChannels, 3.0, nullptr, &outputs);
        bool isLayoutOk = outputs.size() == 36;
        std::vector<float> sum;
        juce::String silentOutputs, playingOutputs;
        for (size_t channel = 0; channel < outputs.size(); channel++)
        {
            float outputPeak = 0;
            for (float value : outputs[channel])
                outputPeak = std::max(outputPeak, std::fabs(value));
            if (channel >= 2 && channel % 2 == 0)
            {
                int bus = static_cast<int>(channel / 2); // 1 to 16 for the channels, 17 for the effects
                ((bus >= 1 && bus <= 4) || bus == 17 ? playingOutputs : silentOutputs) += outputPeak > 0.0f ? "1" : "0";
            }
            if (sum.empty())
                sum.assign(2 * outputs[channel].size(), 0.0f);
            for (size_t i = 0; i < outputs[channel].size(); i++)
                sum[2 * i + channel % 2] += outputs[channel][i];
        }
        double difference = maxDifference(sum, mainOnly);
        check(isLayoutOk && playingOutputs == "11111" && silentOutputs == "000000000000" && difference < -100.0,
              "multiple outputs: channels 1 to 4 and effects playing (" + playingOutputs + "), channels 5 to 16 silent (" +
                  silentOutputs + "), sum of the outputs = main output (" + toText(difference) + ")");
    }

    // MPE: pitch bend of a note (member channel) and of its zone (master channel), configuration of the zones,
    // controllers of the master channel; compared with the same pitches and controllers in the mode "One preset"
    {
        PluginState mpeState;
        mpeState.soundfont = soundfont;
        mpeState.mode = 2;
        juce::MemoryBlock mpeData = mpeState.write();
        auto note = [](int channel) {
            return std::vector<std::pair<int64_t, juce::MidiMessage>> {
                { 100, juce::MidiMessage::noteOn(channel, 60, static_cast<juce::uint8>(100)) },
                { 30000, juce::MidiMessage::noteOff(channel, 60) } };
        };
        auto compare = [&](const juce::String &description, std::vector<std::pair<int64_t, juce::MidiMessage>> mpeEvents,
                           std::vector<std::pair<int64_t, juce::MidiMessage>> otherEvents, int channel) {
            for (const auto &event : note(channel))
            {
                mpeEvents.push_back(event);
                otherEvents.push_back(event);
            }
            std::vector<float> mpe = render(*createInstance(pluginPath, mpeData), 48000, 256, mpeEvents, 1.0);
            std::vector<float> other = render(*createInstance(pluginPath, noteData), 48000, 256, otherEvents, 1.0);
            double difference = maxDifference(mpe, other);
            check(isValid(mpe, peak) && peak > 0.001f && difference < -100.0,
                  "MPE: " + description + " (" + toText(difference) + ")");
        };

        // +12 semitones with the range of 48 semitones of the member channels
        std::vector<std::pair<int64_t, juce::MidiMessage>> other;
        addRpn(other, 0, 2, 0, 48, 0);
        other.push_back({ 0, juce::MidiMessage::pitchWheel(2, 10240) });
        compare("pitch bend of a note", { { 0, juce::MidiMessage::pitchWheel(2, 10240) } }, other, 2);

        // +1 semitone from the master channel (range of 2 semitones) and +12 semitones from the member channel
        other.clear();
        addRpn(other, 0, 3, 0, 26, 0);
        other.push_back({ 0, juce::MidiMessage::pitchWheel(3, 12288) });
        compare("pitch bend of the zone added to the one of a note",
                { { 0, juce::MidiMessage::pitchWheel(1, 12288) }, { 0, juce::MidiMessage::pitchWheel(3, 10240) } }, other, 3);

        // Lower zone of 3 member channels: the channel 6 is not in a zone anymore (range of 2 semitones)
        std::vector<std::pair<int64_t, juce::MidiMessage>> configuration;
        addRpn(configuration, 0, 1, 6, 3, -1);
        configuration.push_back({ 0, juce::MidiMessage::pitchWheel(6, 12288) });
        compare("configuration of the zones", configuration, { { 0, juce::MidiMessage::pitchWheel(6, 12288) } }, 6);

        // Volume of the zone
        compare("controller of the master channel", { { 0, juce::MidiMessage::controllerEvent(1, 7, 64) } },
                { { 0, juce::MidiMessage::controllerEvent(4, 7, 64) } }, 4);
    }

    // Any sample rate
    for (int sampleRate : { 22050, 44100, 96000, 192000 })
    {
        std::vector<float> data = render(*createInstance(pluginPath, stateData), sampleRate, 512,
                                         randomEvents(sampleRate, 1.0, 2), 1.0);
        check(isValid(data, peak) && peak > 0.001f, "sample rate " + juce::String(sampleRate) + " Hz");
    }

    // State: the parameters changed by the host are restored by another instance, which then sounds the same as a
    // third one
    juce::Random random(3);
    for (juce::AudioProcessorParameter * parameter : plugin->getParameters())
        parameter->setValueNotifyingHost(random.nextFloat());
    render(*plugin, 48000, 512, {}, 0.02); // The host of the LV2 format gives the values to the plugin with the audio
    juce::MemoryBlock changedState;
    plugin->getStateInformation(changedState);
    std::unique_ptr<juce::AudioPluginInstance> restored = createInstance(pluginPath, changedState, true);
    render(*restored, 48000, 512, {}, 0.02); // The host of the LV2 format learns the values with the audio
    juce::String differences;
    int parameterCount = 0;
    for (int i = 0; i < plugin->getParameters().size(); i++)
    {
        // The parameters "MIDI CC" of the VST3 format send controllers, they are not in the state
        juce::AudioProcessorParameter * parameter = plugin->getParameters()[i];
        juce::AudioProcessorParameter * restoredParameter = restored->getParameters()[i];
        if (parameter->getName(30).startsWith("MIDI CC"))
            continue;
        parameterCount++;
        // The plugin rounds the values to its steps, the host of a format may still show the value it gave
        if (parameter->getCurrentValueAsText() != restoredParameter->getCurrentValueAsText() &&
            std::fabs(parameter->getValue() - restoredParameter->getValue()) > 0.01f)
            differences += " " + parameter->getName(30) + " (" + parameter->getCurrentValueAsText() + " / " +
                           restoredParameter->getCurrentValueAsText() + ")";
    }
    check(differences.isEmpty(), juce::String(parameterCount) + " parameters restored" + differences);
    std::vector<float> restoredRendering = render(*restored, 48000, 512, events, 3.0);
    std::unique_ptr<juce::AudioPluginInstance> restoredAgain = createInstance(pluginPath, changedState, true);
    render(*restoredAgain, 48000, 512, {}, 0.02);
    check(restoredRendering == render(*restoredAgain, 48000, 512, events, 3.0) && restoredRendering != reference,
          "state with other parameters: same rendering by two instances");

    // Soundfont stored in the state: played without the file
    juce::File copy = juce::File::createTempFile(".sf2");
    juce::File(soundfont).copyFileTo(copy);
    PluginState storedState;
    storedState.soundfont = copy.getFullPathName();
    storedState.isStored = true;
    std::unique_ptr<juce::AudioPluginInstance> withFile = createInstance(pluginPath, storedState.write());
    juce::MemoryBlock withContent;
    withFile->getStateInformation(withContent);
    std::vector<float> fromFile = render(*withFile, 48000, 512, events, 3.0);
    copy.deleteFile();
    check(static_cast<juce::int64>(withContent.getSize()) > juce::File(soundfont).getSize() &&
              render(*createInstance(pluginPath, withContent, true), 48000, 512, events, 3.0) == fromFile,
          "soundfont stored in the state (" + juce::String(withContent.getSize() / 1024) + " kB) played without the file");

    // Missing file without content: silence
    PluginState missingState;
    missingState.soundfont = "/nonexistent/soundfont.sf2";
    std::vector<float> silence = render(*createInstance(pluginPath, missingState.write()), 48000, 512, events, 1.0);
    check(isValid(silence, peak) && peak < 1e-30f, "missing file: silence");

    // Automation of all parameters at each block
    std::vector<float> automated = render(*plugin, 48000, 256, events, 3.0, [&](int) {
        for (juce::AudioProcessorParameter * parameter : plugin->getParameters())
            parameter->setValue(random.nextFloat());
    });
    check(isValid(automated, peak), "automation of all parameters at each block");

    // Instances computed at the same time by several threads (no state shared between instances)
    std::vector<std::unique_ptr<juce::AudioPluginInstance>> instances;
    std::vector<std::vector<float>> results(4);
    for (int i = 0; i < 4; i++)
    {
        instances.push_back(createInstance(pluginPath, stateData));
        instances.back()->prepareToPlay(48000, 512);
    }
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; i++)
        threads.emplace_back([&, i] {
            results[static_cast<size_t>(i)] = process(*instances[static_cast<size_t>(i)], 48000, 512, events, 3.0);
        });
    for (std::thread &thread : threads)
        thread.join();
    bool same = true;
    for (size_t i = 0; i < results.size(); i++)
    {
        instances[i]->releaseResources();
        same = same && results[i] == reference;
    }
    check(same, "4 instances computed by 4 threads give the reference");

    // Soundfonts set by the message thread while another thread computes the audio
    PluginState otherState;
    otherState.soundfont = otherSoundfont;
    juce::MemoryBlock otherData = otherState.write();
    std::unique_ptr<juce::AudioPluginInstance> swapped = createInstance(pluginPath, stateData);
    swapped->setNonRealtime(false);
    swapped->prepareToPlay(48000, 128);
    std::atomic<bool> isRendering(true);
    std::vector<float> swappedData;
    std::thread audio([&] {
        // About the pace of an audio device (blocks of 2.7 ms)
        std::vector<std::pair<int64_t, juce::MidiMessage>> longEvents = randomEvents(48000, 10.0, 4);
        swappedData = process(*swapped, 48000, 128, longEvents, 10.0, [](int) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        });
        isRendering = false;
    });
    int swapCount = 0;
    while (isRendering)
    {
        setState(*swapped, swapCount % 2 == 0 ? otherData : stateData);
        swapCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    audio.join();
    swapped->releaseResources();
    check(isValid(swappedData, peak) && peak > 0.001f && swapCount > 10,
          juce::String(swapCount) + " soundfonts set by the message thread while another thread computes");

    printf("%s: %d failure(s)\n", g_failures == 0 ? "OK" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}

// Soundfonts in the formats sfz and dls (converted as the editor imports them)
static int runFormatChecks(const juce::String &pluginPath, const juce::String &sfzFile, const juce::String &dlsFile)
{
    std::vector<std::pair<int64_t, juce::MidiMessage>> events = randomEvents(48000, 2.0, 5);
    float peak;
    for (const juce::String &file : { sfzFile, dlsFile })
    {
        PluginState state;
        state.soundfont = file;
        state.mode = 0;
        std::unique_ptr<juce::AudioPluginInstance> plugin = createInstance(pluginPath, state.write());
        std::vector<float> data = render(*plugin, 48000, 512, events, 2.0);
        bool isPlayed = isValid(data, peak) && peak > 0.001f;
        check(isPlayed, juce::File(file).getFileName() + " played (peak " + juce::String(peak, 3) + ")");
    }

    // A DLS file stored in the state is played without the file, an sfz file is not stored (its samples are other files)
    juce::File copy = juce::File::createTempFile(".dls");
    juce::File(dlsFile).copyFileTo(copy);
    PluginState storedState;
    storedState.soundfont = copy.getFullPathName();
    storedState.isStored = true;
    storedState.mode = 0;
    std::unique_ptr<juce::AudioPluginInstance> withFile = createInstance(pluginPath, storedState.write());
    juce::MemoryBlock withContent;
    withFile->getStateInformation(withContent);
    std::vector<float> fromFile = render(*withFile, 48000, 512, events, 2.0);
    copy.deleteFile();
    check(static_cast<juce::int64>(withContent.getSize()) > juce::File(dlsFile).getSize() &&
              render(*createInstance(pluginPath, withContent, true), 48000, 512, events, 2.0) == fromFile,
          "dls file stored in the state played without the file");

    storedState.soundfont = sfzFile;
    juce::MemoryBlock sfzState;
    createInstance(pluginPath, storedState.write())->getStateInformation(sfzState);
    check(sfzState.getSize() < 100000, "sfz file not stored in the state (" + juce::String(sfzState.getSize()) + " bytes)");

    printf("%s: %d failure(s)\n", g_failures == 0 ? "OK" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}

/// Checks of the CLAP format

static std::vector<float> renderClap(ClapInstance &plugin, int sampleRate, int blockSize,
                                     const std::vector<std::pair<int64_t, juce::MidiMessage>> &events, double seconds)
{
    plugin.activate(sampleRate, static_cast<uint32_t>(blockSize), true);
    int64_t frameCount = static_cast<int64_t>(seconds * sampleRate);
    std::vector<float> left(static_cast<size_t>(blockSize)), right(static_cast<size_t>(blockSize)), data;
    size_t eventIndex = 0;
    for (int64_t start = 0; start < frameCount; start += blockSize)
    {
        std::vector<ClapInstance::MidiEvent> midi;
        for (; eventIndex < events.size() && events[eventIndex].first < start + blockSize; eventIndex++)
        {
            const juce::MidiMessage &message = events[eventIndex].second;
            ClapInstance::MidiEvent event = { static_cast<uint32_t>(events[eventIndex].first - start), { 0, 0, 0 } };
            memcpy(event.data, message.getRawData(), static_cast<size_t>(juce::jmin(3, message.getRawDataSize())));
            midi.push_back(event);
        }
        plugin.process(left.data(), right.data(), static_cast<uint32_t>(blockSize), midi);
        for (int i = 0; i < blockSize; i++)
        {
            data.push_back(left[static_cast<size_t>(i)]);
            data.push_back(right[static_cast<size_t>(i)]);
        }
    }
    plugin.deactivate();
    return data;
}

static int runClapChecks(const juce::String &pluginPath, const juce::String &soundfont)
{
    auto toVector = [](const juce::MemoryBlock &block) {
        return std::vector<uint8_t>(static_cast<const uint8_t *>(block.getData()),
                                    static_cast<const uint8_t *>(block.getData()) + block.getSize());
    };
    auto create = [&](const std::vector<uint8_t> &state) {
        std::unique_ptr<ClapInstance> plugin(new ClapInstance());
        std::string error;
        if (!plugin->load(pluginPath.toStdString(), error))
        {
            printf("Cannot load the plugin: %s\n", error.c_str());
            return std::unique_ptr<ClapInstance>();
        }
        plugin->setState(state);
        return plugin;
    };

    PluginState state;
    state.soundfont = soundfont;
    std::vector<uint8_t> stateData = toVector(state.write());
    std::vector<std::pair<int64_t, juce::MidiMessage>> events = randomEvents(48000, 3.0, 1);
    float peak;

    std::unique_ptr<ClapInstance> plugin = create(stateData);
    if (plugin == nullptr)
        return 1;
    std::vector<float> reference = renderClap(*plugin, 48000, 512, events, 3.0);
    bool isReferenceValid = isValid(reference, peak);
    check(isReferenceValid && peak > 0.001f, "random MIDI messages played (peak " + juce::String(peak, 3) + ")");
    check(renderClap(*create(stateData), 48000, 512, events, 3.0) == reference, "same rendering by another instance");

    for (int blockSize : { 1, 7, 333, 4096 })
    {
        std::vector<float> data = renderClap(*create(stateData), 48000, blockSize, events, 3.0);
        double difference = maxDifference(data, reference);
        check(isValid(data, peak) && peak > 0.001f && difference < -100.0,
              "blocks of " + juce::String(blockSize) + " frames: same rendering (" + toText(difference) + ")");
    }
    for (int sampleRate : { 22050, 96000, 192000 })
    {
        std::vector<float> data = renderClap(*create(stateData), sampleRate, 512, randomEvents(sampleRate, 1.0, 2), 1.0);
        check(isValid(data, peak) && peak > 0.001f, "sample rate " + juce::String(sampleRate) + " Hz");
    }

    // State saved by an instance and restored by another one
    std::vector<uint8_t> saved;
    check(plugin->getState(saved) && renderClap(*create(saved), 48000, 512, events, 3.0) == reference,
          "state saved and restored by another instance");

    // Soundfont stored in the state
    juce::File copy = juce::File::createTempFile(".sf2");
    juce::File(soundfont).copyFileTo(copy);
    PluginState storedState;
    storedState.soundfont = copy.getFullPathName();
    storedState.isStored = true;
    std::unique_ptr<ClapInstance> withFile = create(toVector(storedState.write()));
    std::vector<uint8_t> withContent;
    withFile->getState(withContent);
    std::vector<float> fromFile = renderClap(*withFile, 48000, 512, events, 3.0);
    copy.deleteFile();
    check(static_cast<juce::int64>(withContent.size()) > juce::File(soundfont).getSize() &&
              renderClap(*create(withContent), 48000, 512, events, 3.0) == fromFile,
          "soundfont stored in the state played without the file");

    printf("%s: %d failure(s)\n", g_failures == 0 ? "OK" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}

int main(int argc, char ** argv)
{
    juce::ScopedJuceInitialiser_GUI juce;
    setvbuf(stdout, nullptr, _IOLBF, 0);
    juce::StringArray args;
    for (int i = 1; i < argc; i++)
        args.add(juce::String::fromUTF8(argv[i]));

    int result = 2;
    if (args.size() == 2 && args[0] == "state")
    {
        // State of a new instance, as the host of its format gives it
        std::unique_ptr<juce::AudioPluginInstance> plugin = loadPlugin(args[1], 48000, 512);
        if (plugin == nullptr)
            return 1;
        juce::MemoryBlock state;
        plugin->getStateInformation(state);
        fwrite(state.getData(), 1, state.getSize(), stdout);
        return 0;
    }
    else if (args.size() == 5 && args[0] == "case")
        result = renderCase(args[1], juce::File(args[2]), juce::File(args[3]), args[4]);
    else if (args.size() == 4 && args[0] == "checks")
        result = runChecks(args[1], args[2], args[3]);
    else if (args.size() == 3 && args[0] == "clapchecks")
        result = runClapChecks(args[1], args[2]);
    else if (args.size() == 4 && args[0] == "formatchecks")
        result = runFormatChecks(args[1], args[2], args[3]);
    else
        printf("Usage:\n  %s case <plugin> <soundfont directory> <event file> <output wav>\n"
               "  %s checks <plugin> <soundfont> <other soundfont>\n"
               "  %s clapchecks <plugin> <soundfont>\n"
               "  %s formatchecks <plugin> <sfz file> <dls file>\n", argv[0], argv[0], argv[0], argv[0]);
    juce::DeletedAtShutdown::deleteAll();
    return result;
}
