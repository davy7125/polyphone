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

// Threads of a plugin playing soundfonts without making the audio thread wait (see README.md): an "interface" thread
// reads soundfonts and replaces the one played while the audio thread plays notes, the interface also sending notes
// through a queue. Half of the soundfonts are played while their samples are read by other threads. Built with
// ThreadSanitizer or AddressSanitizer, it shows the data races and the memory read after being freed (sample data of a
// soundfont deleted too early).
// Usage: stress_threads <seconds> <soundfont> [<soundfont>...]

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <iterator>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include "synthcore.h"
#include "synthchannels.h"
#include "midiprocessor.h"
#include "synthqueue.h"
#include "synthsoundfontfile.h"
#include "synthsoundfontset.h"

static const uint32_t BLOCK_SIZE = 256;
static const uint32_t SAMPLE_RATE = 48000;

// Sent by the interface to the audio thread
struct Command
{
    enum Type
    {
        COMMAND_NOTE,
        COMMAND_SOUNDFONT
    };

    Type type = COMMAND_NOTE;
    int channel = 0;
    int key = 0;
    int velocity = 0;
    int sf2Index = -1; // Soundfont replacing the one played...
    SynthSoundfontFile * soundfont = nullptr; // ...whose default values of the controllers are read by the audio thread
};

struct Results
{
    std::atomic<uint64_t> blockCount { 0 };
    std::atomic<uint64_t> replacementCount { 0 };
    std::atomic<int> maxVoiceCount { 0 };
    std::atomic<bool> hasInvalidValue { false };
    std::atomic<float> peak { 0 };
};

static void audioThread(SynthSoundfontSet * soundfonts, SynthQueue<Command> * queue, std::atomic<bool> * stop,
                        Results * results)
{
    // Everything that plays is used by this thread only
    SynthCore core(soundfonts);
    MidiProcessor processor;
    SynthChannels channels(&core, soundfonts);
    processor.setListener(&channels);
    core.setMidiValues(&processor);
    SynthCoreConfig configuration;
    configuration.revLevel = 40;
    configuration.revSize = 60;
    configuration.revWidth = 80;
    configuration.revDamping = 30;
    configuration.choLevel = 30;
    configuration.choDepth = 40;
    configuration.choFrequency = 20;
    core.configure(configuration);
    core.setSampleRate(SAMPLE_RATE);
    core.setMaxBlockSize(BLOCK_SIZE);
    core.setMaxPolyphony(256);

    std::mt19937 random(1);
    std::vector<float> dataL(BLOCK_SIZE), dataR(BLOCK_SIZE);
    int current = -1;
    while (!stop->load())
    {
        // Commands of the interface
        Command command;
        while (queue->pop(command))
        {
            if (command.type == Command::COMMAND_NOTE)
            {
                if (command.velocity > 0)
                    processor.processKeyOn(command.channel, command.key, command.velocity);
                else
                    processor.processKeyOff(command.channel, command.key);
            }
            else
            {
                // The previous soundfont is not played anymore and its voices stop quickly
                if (current >= 0)
                {
                    soundfonts->remove(current);
                    core.releaseVoices(current, -1, -2, -2, true);
                }
                current = command.sf2Index;
                for (int channel = 0; channel < 16; channel++)
                    channels.setChannel(channel, current, -1);
                processor.resetDefaultControllerValues();
                command.soundfont->applyDefaultControllerValues(processor);
                results->replacementCount++;
            }
        }

        // Notes and programs of a MIDI input, many keys staying held
        for (int i = 0; i < 4; i++)
        {
            int channel = std::uniform_int_distribution<int>(0, 15)(random);
            int key = std::uniform_int_distribution<int>(24, 100)(random);
            int choice = std::uniform_int_distribution<int>(0, 99)(random);
            if (choice < 45)
                processor.processKeyOn(channel, key, std::uniform_int_distribution<int>(1, 127)(random));
            else if (choice < 90)
                processor.processKeyOff(channel, key);
            else if (choice < 93)
                processor.processProgramChanged(channel, static_cast<uint8_t>(key));
            else if (choice < 96)
                processor.processControllerChanged(true, channel, 1, key);
            else
                processor.processBendChanged(channel, (key - 62) / 38.f);
        }

        // Removed soundfonts without voices, then the computation
        soundfonts->checkRemoved(core);
        core.render(dataL.data(), dataR.data(), BLOCK_SIZE);

        float peak = results->peak.load();
        for (uint32_t i = 0; i < BLOCK_SIZE; i++)
        {
            if (!std::isfinite(dataL[i]) || !std::isfinite(dataR[i]))
                results->hasInvalidValue = true;
            peak = std::max(peak, std::max(std::fabs(dataL[i]), std::fabs(dataR[i])));
        }
        results->peak = peak;
        int voiceCount = core.getVoiceList()->getVoiceCount();
        if (voiceCount > results->maxVoiceCount)
            results->maxVoiceCount = voiceCount;
        results->blockCount++;
    }
}

// Soundfont whose samples are read by two threads while it is played, as the plugin does (PluginSoundfont). The threads
// stop before the soundfont is deleted.
struct Loading
{
    std::vector<char> content; // Content of a file in memory, read until all samples are read
    std::atomic<bool> stop { false };
    std::vector<std::thread> threads;
};

static std::vector<char> readFile(const std::string &path)
{
    std::ifstream stream(path, std::ios::binary);
    return std::vector<char>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
}

// Soundfont read at once (loading null) or progressively, from the file or from its content kept in memory (a soundfont
// stored in the project of a host)
static std::shared_ptr<SynthSoundfontFile> load(const std::string &path, bool fromMemory, bool isProgressive,
                                                std::string &error)
{
    if (!isProgressive)
    {
        if (!fromMemory)
            return std::shared_ptr<SynthSoundfontFile>(SynthSoundfontFile::load(path, error));
        std::vector<char> data = readFile(path);
        return std::shared_ptr<SynthSoundfontFile>(SynthSoundfontFile::load(data.data(), data.size(), error));
    }

    std::shared_ptr<Loading> loading = std::make_shared<Loading>();
    if (fromMemory)
        loading->content = readFile(path);
    SynthSoundfontFile * soundfont = fromMemory ?
                                         SynthSoundfontFile::open(loading->content.data(), loading->content.size(), error) :
                                         SynthSoundfontFile::open(path, error);
    if (soundfont == nullptr)
        return nullptr;
    for (int i = 0; i < 2; i++)
        loading->threads.emplace_back([soundfont, loading = loading.get()] {
            while (!loading->stop.load() && soundfont->loadNextSample()) {}
        });
    return std::shared_ptr<SynthSoundfontFile>(soundfont, [loading](SynthSoundfontFile * file) {
        loading->stop = true;
        for (std::thread &thread : loading->threads)
            thread.join();
        delete file;
    });
}

// A sample asked by a note is read first, and the data read progressively (by two threads, one of them waiting for the
// other one at the end) is the one read at once
static bool checkProgressiveReading(const std::string &path)
{
    std::string error;
    std::unique_ptr<SynthSoundfontFile> reference(SynthSoundfontFile::load(path, error));
    std::unique_ptr<SynthSoundfontFile> soundfont(SynthSoundfontFile::open(path, error));
    if (reference == nullptr || soundfont == nullptr)
        return false;
    int count = 0;
    while (soundfont->getSynthSample(count) != nullptr)
        count++;
    if (count == 0)
        return true;

    ISynthSample * asked = soundfont->getSynthSample(count / 2);
    bool isFirst = !soundfont->isLoaded() && !asked->loadData() && soundfont->loadNextSample() && asked->loadData();
    std::thread other([&soundfont] { while (soundfont->loadNextSample()) {} });
    soundfont->loadAllSamples();
    other.join();
    bool isSame = soundfont->isLoaded() && soundfont->getLoadedRatio() == 1.0f;
    for (int i = 0; i < count && isSame; i++)
    {
        ISynthSample * sample = soundfont->getSynthSample(i);
        ISynthSample * expected = reference->getSynthSample(i);
        uint32_t length, expectedLength;
        int16_t * data16, * expectedData16;
        uint8_t * data24, * expectedData24;
        sample->getData(length, data16, data24, false);
        expected->getData(expectedLength, expectedData16, expectedData24, false);
        isSame = sample->loadData() && length == expectedLength && sample->getLength() == expected->getLength() &&
                 sample->getPeak() == expected->getPeak() && (data24 == nullptr) == (expectedData24 == nullptr) &&
                 (length == 0 || memcmp(data16, expectedData16, length * sizeof(int16_t)) == 0) &&
                 (data24 == nullptr || memcmp(data24, expectedData24, length) == 0);
    }
    printf("%s: sample asked read first: %s, same data as read at once: %s\n", path.c_str(), isFirst ? "yes" : "NO",
           isSame ? "yes" : "NO");
    return isFirst && isSame;
}

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        printf("Usage: %s <seconds> <soundfont> [<soundfont>...]\n", argv[0]);
        return 1;
    }
    double duration = atof(argv[1]);
    std::vector<std::string> paths(argv + 2, argv + argc);

    SynthCore::prepareTables();
    SynthSoundfontSet soundfonts(4);
    SynthQueue<Command> queue(64);
    std::atomic<bool> stop(false);
    Results results;
    std::thread audio(audioThread, &soundfonts, &queue, &stop, &results);

    // Interface: a soundfont replaces the previous one, then notes are played with the virtual keyboard
    std::mt19937 random(2);
    int loadCount = 0, deletionCount = 0, fullQueueCount = 0, fullSetCount = 0;
    bool failed = false;
    auto start = std::chrono::steady_clock::now();
    while (!failed && std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() < duration)
    {
        std::string error;
        std::shared_ptr<SynthSoundfontFile> shared = load(paths[loadCount % paths.size()], (loadCount / 2) % 2 == 1,
                                                          loadCount % 2 == 0, error);
        SynthSoundfontFile * soundfont = shared.get();
        if (soundfont == nullptr)
        {
            printf("Cannot read %s: %s\n", paths[loadCount % paths.size()].c_str(), error.c_str());
            failed = true;
            break;
        }
        loadCount++;

        // All indexes may be used by soundfonts whose voices are not finished
        int index;
        while ((index = soundfonts.add(shared)) < 0)
        {
            fullSetCount++;
            deletionCount += soundfonts.deleteUnused();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        Command command;
        command.type = Command::COMMAND_SOUNDFONT;
        command.sf2Index = index;
        command.soundfont = soundfont;
        while (!queue.push(command))
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        for (int i = 0; i < 200; i++)
        {
            Command note;
            note.channel = std::uniform_int_distribution<int>(0, 15)(random);
            note.key = std::uniform_int_distribution<int>(30, 90)(random);
            note.velocity = std::uniform_int_distribution<int>(0, 1)(random) * 100;
            if (!queue.push(note))
                fullQueueCount++;
            if (i % 20 == 0)
            {
                // Samples of a preset read first, if they are being read
                soundfont->prioritizePreset(std::uniform_int_distribution<int>(0, 9)(random));
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        shared.reset();
        deletionCount += soundfonts.deleteUnused();
    }

    // Some soundfonts may not be deleted yet: they are deleted with the set
    stop = true;
    audio.join();

    printf("Soundfonts read: %d, replaced by the audio thread: %llu, deleted while playing: %d\n", loadCount,
           static_cast<unsigned long long>(results.replacementCount.load()), deletionCount);
    printf("Blocks computed: %llu, maximum number of voices: %d, peak: %.3f\n",
           static_cast<unsigned long long>(results.blockCount.load()), results.maxVoiceCount.load(), results.peak.load());
    printf("Waits: set full %d times, queue full %d times\n", fullSetCount, fullQueueCount);
    for (const std::string &path : paths)
        failed = !checkProgressiveReading(path) || failed;

    if (failed || results.hasInvalidValue || results.blockCount == 0 || results.peak == 0 ||
        (loadCount > 4 && deletionCount == 0))
    {
        printf("FAILED%s\n", results.hasInvalidValue ? " (invalid values computed)" : "");
        return 1;
    }
    printf("OK\n");
    return 0;
}
