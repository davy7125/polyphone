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

#ifndef SYNTHSOUNDFONTFILE_H
#define SYNTHSOUNDFONTFILE_H

#include <atomic>
#include <condition_variable>
#include <deque>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "isynthsoundfont.h"
#include "sf2reader.h"
class MidiProcessor;

// Soundfont read from a file without the editor, for a plugin: sf2 and sf3 (Sf2Reader), sfz (SfzReader, the samples
// being the files referenced) and dls (DlsReader). The files are read by the readers of the editor, the sfz and dls files
// being converted as the editor imports them, so that a soundfont sounds the same.
// The structure is read first (open), the sample data afterwards, possibly by other threads while the soundfont is played
// (loadNextSample): a sample is played once its data is complete (and the one of its linked sample), the notes asking for
// it before being silent. The samples of an sf3 file are decompressed: once everything is read, nothing is read from the
// disk while playing, and the content does not change anymore (the synth can read it from any thread).
class SynthSoundfontFile: public ISynthSoundfont
{
public:
    // Structure of a file (format given by its extension), or of the content of a file in memory (sf2, sf3 or dls) that
    // must stay valid until all samples are read: null in case of error (described by error). velToFilterType is the
    // default modulator of the velocity to the filter of the synth (see ModulatorData), the sfz files not adding the same
    // one.
    static SynthSoundfontFile * open(const std::filesystem::path &filePath, std::string &error, int velToFilterType = 1);
    static SynthSoundfontFile * open(const char * data, size_t length, std::string &error);

    // Structure and all sample data read at once
    static SynthSoundfontFile * load(const std::filesystem::path &filePath, std::string &error, int velToFilterType = 1);
    static SynthSoundfontFile * load(const char * data, size_t length, std::string &error);

    // Extensions of the files that can be read (lower case, with the dot)
    static bool isSupported(const std::filesystem::path &filePath);
    ~SynthSoundfontFile() override;

    /// Sample data, read by one thread or more. The order: the samples asked by the notes, the ones of the presets given
    /// to prioritizePreset (the last one first), then the others in the order of the file. A sample whose data cannot be
    /// read is silent.

    // Read the next sample, with its linked sample (stereo): false if no sample is left to read (some can still be read by
    // other threads)
    bool loadNextSample();

    // Read the samples left, the other threads reading samples being waited for: all data is read when it returns
    void loadAllSamples();

    // Any thread: the samples of a preset are read first, from the middle of the keyboard
    void prioritizePreset(int presetIndex);

    // Any thread: share of the data read (1 when everything is read), true if everything is read
    float getLoadedRatio() const;
    bool isLoaded() const;

    // True if the data of the samples is compressed (sf3): its decompression can be shared by several threads
    bool hasCompressedSamples() const { return _hasCompressedSamples; }

    // True if the memory was missing for the data of a sample (silent)
    bool hasMemoryError() const { return _hasMemoryError.load(); }

    /// Content

    // Text of the INFO chunk ("INAM" for the name, "ICOP", "ICMT"...) in UTF-8, empty if missing
    std::string getInfo(const std::string &key) const;

    // Name of a preset or an instrument by index, in UTF-8 (empty if the index is not used)
    std::string getPresetName(int index) const;
    std::string getInstrumentName(int index) const;

    // The controllers that are the source of a modulator have a default value depending on its polarity and on its
    // direction, given to the processor as the editor does when it loads a soundfont
    void applyDefaultControllerValues(MidiProcessor &processor) const;

    // A zone muted, or an element played with any other one of its kind, as in the editor (rendering tests): to call
    // before playing, false if the element or the zone does not exist
    bool setZoneMuted(bool isPreset, int index, int zonePosition, bool isMuted);
    bool setAlwaysPlayed(bool isPreset, int index, bool isAlwaysPlayed);

    // ISynthSoundfont implementation
    ISynthSample * getSynthSample(int index) override;
    ISynthInstPrst * getSynthInstrument(int index) override;
    ISynthInstPrst * getSynthPreset(int index) override;
    int getInstrumentCount() override;
    ISynthInstPrst * getInstrumentAt(int position) override;
    int getPresetCount() override;
    ISynthInstPrst * getPresetAt(int position) override;
    ISynthZone * getDefaultModulators() override;

private:
    class Sample;
    class Zone;
    class Element;
    class Builder;

    // Where the sample data is: in the file read (sf2, sf3, dls), or in the files referenced by an sfz file
    enum SourceType
    {
        SOURCE_SF2,
        SOURCE_DLS,
        SOURCE_FILES
    };

    SynthSoundfontFile();
    static SynthSoundfontFile * loadAll(SynthSoundfontFile * soundfont, std::string &error);
    static SynthSoundfontFile * openSf2(std::unique_ptr<Sf2Reader::Source> source, std::string &error);
    static SynthSoundfontFile * openSfz(const std::filesystem::path &filePath, std::string &error, int velToFilterType);
    static SynthSoundfontFile * openDls(std::unique_ptr<Sf2Reader::Source> source, const std::string &filePath,
                                        std::string &error);
    void addElements(const std::vector<Sf2Reader::Element> &elements, bool isPreset, bool withControllers = true);
    void addControllerSources(const Sf2Reader::Zone &zone);
    void build(Builder &builder);
    void prepareLoading();
    int takeNextSamples(Sample * samples[2]); // With _loadingMutex
    void readData(Sample &sample);

    std::map<std::string, std::string> _infos;
    std::vector<std::unique_ptr<Sample>> _samples;
    std::vector<std::unique_ptr<Element>> _instruments;
    std::vector<std::unique_ptr<Element>> _presets;
    std::unique_ptr<Zone> _defaultModulators; // Null without DMOD chunk
    std::vector<SFModulator> _controllerSources; // In the order of the modulators added by the editor

    // Reading of the sample data: the source is read by one thread at a time, and released once everything is read
    SourceType _sourceType;
    std::unique_ptr<Sf2Reader::Source> _source;
    std::mutex _sourceMutex;
    bool _hasCompressedSamples;
    std::atomic<bool> _hasMemoryError;

    // Samples to read: the ones asked by the notes (counted by the audio threads), then the priorities and the order of
    // the file (with the mutex)
    std::atomic<uint32_t> _requestCount;
    mutable std::mutex _loadingMutex;
    std::condition_variable _loadedCondition;
    uint32_t _checkedRequestCount;
    std::deque<int> _requested;
    std::deque<int> _priorities;
    size_t _nextInFile;
    std::atomic<size_t> _readCount; // Written with the mutex
    uint64_t _totalSize;
    std::atomic<uint64_t> _readSize;
};

#endif // SYNTHSOUNDFONTFILE_H
