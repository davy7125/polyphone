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

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "isynthsoundfont.h"
#include "sf2reader.h"
class MidiProcessor;

// Soundfont read from a file without the editor, for a plugin: sf2 and sf3 (Sf2Reader), sfz (SfzReader, the samples
// being the files referenced) and dls (DlsReader). Everything is read at once, the samples of an sf3 file being
// decompressed: nothing is read from the disk while playing, and the content does not change afterwards (the synth can
// read it from any thread). The files are read by the readers of the editor, the sfz and dls files being converted as
// the editor imports them, so that a soundfont sounds the same.
class SynthSoundfontFile: public ISynthSoundfont
{
public:
    // Read a file (format given by its extension), or the content of a file in memory (sf2, sf3 or dls): null in case of
    // error (described by error). velToFilterType is the default modulator of the velocity to the filter of the synth
    // (see ModulatorData), the sfz files not adding the same one.
    static SynthSoundfontFile * load(const std::filesystem::path &filePath, std::string &error, int velToFilterType = 1);
    static SynthSoundfontFile * load(const char * data, size_t length, std::string &error);

    // Extensions of the files that can be read (lower case, with the dot)
    static bool isSupported(const std::filesystem::path &filePath);
    ~SynthSoundfontFile() override;

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

private:
    class Sample;
    class Zone;
    class Element;
    class Builder;

    SynthSoundfontFile();
    static SynthSoundfontFile * load(Sf2Reader::Source &source, std::string &error);
    static SynthSoundfontFile * loadSfz(const std::filesystem::path &filePath, std::string &error, int velToFilterType);
    static SynthSoundfontFile * loadDls(Sf2Reader::Source &source, const std::string &filePath, std::string &error);
    void addElements(const std::vector<Sf2Reader::Element> &elements, bool isPreset, bool withControllers = true);
    void build(Builder &builder, Sf2Reader::Source * dlsSource);

    std::map<std::string, std::string> _infos;
    std::vector<std::unique_ptr<Sample>> _samples;
    std::vector<std::unique_ptr<Element>> _instruments;
    std::vector<std::unique_ptr<Element>> _presets;
    std::vector<SFModulator> _controllerSources; // In the order of the modulators added by the editor
};

#endif // SYNTHSOUNDFONTFILE_H
