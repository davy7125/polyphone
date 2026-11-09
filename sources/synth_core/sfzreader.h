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


#ifndef SFZREADER_H
#define SFZREADER_H

#include <functional>
#include <map>
#include <string>
#include <vector>
#include "sfzparametergroup.h"
#include "soundfontbuilder.h"

// Reader of an sfz file, converted into a soundfont (a preset made of an instrument by group) by the editor and by the
// plugin. The samples are the files referenced, read with libsndfile (SampleFileReader).
class SfzReader
{
public:
    struct Options
    {
        // Default modulator of the velocity to the filter (see ModulatorData), for not adding the same modulator
        int velToFilterType = 0;

        // Key number of a name in lower case or of a number, -1 if not valid (null: names of the sfz format)
        std::function<int(const std::string &)> getKeyNumber;

        // Warnings: opcode not supported (name without "_" and value), block unknown (possibly null)
        std::function<void(const std::string &, const std::string &)> onUnsupportedOpcode;
        std::function<void(const std::string &)> onUnknownBlock;

        // Names of an instrument without name and of an existing soundfont receiving the file (UTF-8)
        std::string untitledName = "untitled";
        std::string importName = "Sfz import";
    };

    enum Error
    {
        ERROR_NONE,
        ERROR_RECURSION,      // A file includes itself (#include)
        ERROR_CANNOT_OPEN     // A file cannot be opened (getErrorFile)
    };

    // Read a file (UTF-8 path, '/' being the separator) into a new soundfont, or into an existing one: false in case
    // of error. The builder of a new soundfont receives first the addition of the soundfont (Id(TYPE_SF2)).
    bool read(const std::string &filePath, ISoundfontBuilder &builder, bool isNewSoundfont, const Options &options);
    Error getError() const { return _error; }
    const std::string &getErrorFile() const { return _errorFile; }

private:
    enum Bloc
    {
        BLOC_UNKNOWN,
        BLOC_CONTROL,
        BLOC_GLOBAL,
        BLOC_GROUP,
        BLOC_REGION
    };

    bool parseFile(const std::string &filename);
    std::string applyReplacements(std::string opcodeValue);
    std::string getFilePathFromInclude(std::string str);
    void changeBloc(const std::string &bloc);
    void addOpcode(const std::string &opcode, std::string value);

    void createSf2(ISoundfontBuilder &sm, const std::string &filename, bool isNewSoundfont, bool isChannel10);
    std::string getInstrumentName(const std::string &filePath, int &numBank, int &numPreset);

    Options _options;
    SfzContext _context;
    std::vector<SfzParameterGroup> _presetList;
    Bloc _currentBloc;
    SfzParameterRegion _globalZone;
    std::vector<std::string> _openFilePaths;
    std::string _rootDir;
    std::map<std::string, std::string> _replacements;
    Error _error;
    std::string _errorFile;
};

#endif // SFZREADER_H
