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


// Threads of a host using the plugin, the sources of the plugin being compiled here: an audio thread computes the sound
// while the message thread does what a host and the interface can do (see README.md). Built with ThreadSanitizer, it
// shows the data races.
//   plugin_threads <seconds> <soundfont> <other soundfont>

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <random>
#include <thread>
#include "pluginprocessor.h"

static juce::MemoryBlock createState(const juce::String &soundfont, int mode, bool isStored)
{
    juce::ValueTree state("PolyphonePlugin");
    state.setProperty("version", 1, nullptr);
    state.setProperty("mode", mode, nullptr);
    state.setProperty("program", 0, nullptr);
    state.setProperty("polyphony", 512, nullptr);
    state.setProperty("channelPrograms", "-1,0,1,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1", nullptr);
    juce::ValueTree soundfontTree("Soundfont");
    soundfontTree.setProperty("path", soundfont, nullptr);
    soundfontTree.setProperty("stored", isStored, nullptr);
    state.appendChild(soundfontTree, nullptr);
    juce::MemoryBlock data;
    juce::MemoryOutputStream stream(data, false);
    state.writeToStream(stream);
    return data;
}

int main(int argc, char ** argv)
{
    juce::ScopedJuceInitialiser_GUI juce;
    setvbuf(stdout, nullptr, _IOLBF, 0);
    if (argc != 4)
    {
        printf("Usage: %s <seconds> <soundfont> <other soundfont>\n", argv[0]);
        return 2;
    }
    double duration = atof(argv[1]);
    juce::String soundfonts[2] = { juce::String::fromUTF8(argv[2]), juce::String::fromUTF8(argv[3]) };

    // All outputs enabled (main, channels and effects)
    std::unique_ptr<PluginProcessor> processor(new PluginProcessor());
    processor->enableAllBuses();
    const int channelCount = processor->getTotalNumOutputChannels();
    juce::MemoryBlock state = createState(soundfonts[0], 1, false);
    processor->setStateInformation(state.getData(), static_cast<int>(state.getSize()));
    processor->prepareToPlay(48000, 256);

    // Audio thread: MIDI messages at their position, about four times faster than real time
    std::atomic<bool> stop(false);
    std::atomic<int> blockCount(0);
    std::atomic<bool> hasInvalidValue(false);
    std::thread audio([&] {
        std::mt19937 random(1);
        juce::AudioBuffer<float> buffer(channelCount, 256);
        juce::MidiBuffer midi;
        while (!stop)
        {
            midi.clear();
            for (int i = 0; i < 4; i++)
            {
                int channel = 1 + static_cast<int>(random() % 16);
                int key = 30 + static_cast<int>(random() % 60);
                int position = static_cast<int>(random() % 256);
                int choice = static_cast<int>(random() % 100);
                if (choice < 45)
                    midi.addEvent(juce::MidiMessage::noteOn(channel, key, static_cast<juce::uint8>(1 + random() % 127)), position);
                else if (choice < 85)
                    midi.addEvent(juce::MidiMessage::noteOff(channel, key), position);
                else if (choice < 90)
                    midi.addEvent(juce::MidiMessage::pitchWheel(channel, static_cast<int>(random() % 16384)), position);
                else if (choice < 95)
                    midi.addEvent(juce::MidiMessage::programChange(channel, static_cast<int>(random() % 128)), position);
                else
                    midi.addEvent(juce::MidiMessage::controllerEvent(channel, static_cast<int>(random() % 128), static_cast<int>(random() % 128)), position);
            }
            buffer.clear();
            {
                // As the wrappers of the plugin formats: the callback lock is held, suspendProcessing being respected
                const juce::ScopedLock lock(processor->getCallbackLock());
                if (!processor->isSuspended())
                    processor->processBlock(buffer, midi);
            }
            for (int channel = 0; channel < channelCount; channel++)
                for (int i = 0; i < 256; i++)
                    if (!std::isfinite(buffer.getSample(channel, i)))
                        hasInvalidValue = true;
            blockCount++;
            std::this_thread::sleep_for(std::chrono::microseconds(1300));
        }
    });

    // Message thread: interface and host
    std::mt19937 random(2);
    int actionCount = 0;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() < duration)
    {
        PluginEngine &engine = processor->getEngine();
        switch (random() % 10)
        {
        case 0:
            processor->loadSoundfont(juce::File(soundfonts[random() % 2]));
            break;
        case 1: {
            juce::MemoryBlock newState = createState(soundfonts[random() % 2], static_cast<int>(random() % 2), random() % 2 == 0);
            processor->setStateInformation(newState.getData(), static_cast<int>(newState.getSize()));
        } break;
        case 2: {
            juce::MemoryBlock savedState;
            processor->getStateInformation(savedState);
        } break;
        case 3:
            engine.setMode(static_cast<PluginEngine::Mode>(random() % 3));
            engine.setProgram(static_cast<int>(random() % 128));
            engine.setChannelProgram(static_cast<int>(random() % 16), static_cast<int>(random() % 130) - 2);
            break;
        case 4:
            for (juce::AudioProcessorParameter * parameter : processor->getParameters())
                parameter->setValueNotifyingHost(static_cast<float>(random() % 1000) / 1000.0f);
            break;
        case 5:
            processor->setPolyphony(random() % 2 == 0 ? 256 : 1024);
            break;
        case 6:
            processor->getKeyboardState().noteOn(1, 40 + static_cast<int>(random() % 40), 0.8f);
            break;
        case 7:
            engine.panic();
            break;
        case 8:
            engine.deleteUnusedSoundfonts();
            processor->setSoundfontStored(random() % 2 == 0);
            processor->setMultipleOutputs(random() % 2 == 0);
            break;
        default: {
            // What the interface shows
            std::vector<PluginEngine::Preset> presets = engine.getPresets();
            int sum = engine.getVoiceCount() + engine.getPlayedProgram(0) + static_cast<int>(engine.getChannelActivity(0));
            juce::String text = processor->getSoundfontFile().getFullPathName() + processor->getError() +
                                juce::String(engine.getSoundfontInfo("INAM")) + juce::String(sum + static_cast<int>(presets.size()));
        } break;
        }
        actionCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    stop = true;
    audio.join();
    processor->releaseResources();
    processor.reset();
    printf("Blocks computed: %d, actions of the message thread: %d\n", blockCount.load(), actionCount);
    printf("%s\n", hasInvalidValue ? "FAILED (invalid values computed)" : "OK");
    return hasInvalidValue ? 1 : 0;
}
