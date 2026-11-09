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


#ifndef PLUGINSOUNDFONT_H
#define PLUGINSOUNDFONT_H

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
class SynthSoundfontFile;

// Soundfont read for the plugin, shared by all its instances in the process: a file played by several instances (one per
// track for instance) is read once, and stays in memory as long as one of them plays it. A file is read again if it
// changed (size or modification time). The content of a file stored in the state of an instance is shared in the same
// way.
class PluginSoundfont: public std::enable_shared_from_this<PluginSoundfont>
{
public:
    // Soundfont of a file, or of the content of a file: null in case of error (described by error). Several threads can
    // ask for the same file at the same time, it is read once.
    static std::shared_ptr<PluginSoundfont> load(const std::filesystem::path &filePath, std::string &error);
    static std::shared_ptr<PluginSoundfont> load(const char * data, size_t length, std::string &error);
    ~PluginSoundfont();

    PluginSoundfont(const PluginSoundfont &) = delete;
    PluginSoundfont &operator=(const PluginSoundfont &) = delete;

    // Soundfont played, sharing the ownership of this object
    std::shared_ptr<SynthSoundfontFile> getSynthSoundfont();

    // Content read in memory, empty if the soundfont has been read from a file
    const std::vector<char> &getContent() const { return _content; }

    // Soundfonts in memory, for the tests
    static int getCount();

private:
    PluginSoundfont();

    std::unique_ptr<SynthSoundfontFile> _soundfont;
    std::vector<char> _content;
};

#endif // PLUGINSOUNDFONT_H
