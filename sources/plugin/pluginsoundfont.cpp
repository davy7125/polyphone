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


#include "pluginsoundfont.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include "synthsoundfontfile.h"

namespace
{
// A soundfont of the cache, possibly being read by a thread (the others asking for it wait)
struct Entry
{
    std::mutex readingMutex;
    std::weak_ptr<PluginSoundfont> soundfont;
};

std::mutex &getCacheMutex()
{
    static std::mutex mutex;
    return mutex;
}

std::map<std::string, std::shared_ptr<Entry>> &getEntries()
{
    static std::map<std::string, std::shared_ptr<Entry>> entries;
    return entries;
}

// Key of a file: its path without links, its size and its modification time
std::string getFileKey(const std::filesystem::path &filePath)
{
    std::error_code error;
    std::filesystem::path path = std::filesystem::canonical(filePath, error);
    if (error)
        path = filePath;
    const std::filesystem::path::string_type &native = path.native();
    std::string key = "file|";
    key.append(reinterpret_cast<const char *>(native.data()), native.size() * sizeof(native[0]));
    uintmax_t size = std::filesystem::file_size(path, error);
    key += "|" + std::to_string(error ? 0 : size);
    std::filesystem::file_time_type time = std::filesystem::last_write_time(path, error);
    key += "|" + std::to_string(error ? 0 : static_cast<long long>(time.time_since_epoch().count()));
    return key;
}

// Key of a content: its size and a hash (the contents are compared when the keys are the same)
std::string getContentKey(const char * data, size_t length)
{
    uint64_t hash = 0xcbf29ce484222325ULL ^ static_cast<uint64_t>(length);
    size_t i = 0;
    for (; i + 8 <= length; i += 8)
    {
        uint64_t word;
        memcpy(&word, data + i, 8);
        hash = (hash ^ word) * 0x100000001b3ULL;
        hash ^= hash >> 29;
    }
    for (; i < length; i++)
        hash = (hash ^ static_cast<uint8_t>(data[i])) * 0x100000001b3ULL;
    return "content|" + std::to_string(length) + "|" + std::to_string(hash);
}

// Soundfont of the cache, or read and added to it. isSame checks a soundfont found with the key (null: always).
std::shared_ptr<PluginSoundfont> getOrRead(const std::string &key,
                                           const std::function<std::shared_ptr<PluginSoundfont>()> &read,
                                           const std::function<bool(const PluginSoundfont &)> &isSame)
{
    std::shared_ptr<Entry> entry;
    {
        std::lock_guard<std::mutex> lock(getCacheMutex());

        // The entries of the soundfonts deleted are removed, unless a thread is reading them
        std::map<std::string, std::shared_ptr<Entry>> &entries = getEntries();
        for (auto it = entries.begin(); it != entries.end(); )
        {
            if (it->second.use_count() == 1 && it->second->soundfont.expired())
                it = entries.erase(it);
            else
                ++it;
        }

        std::shared_ptr<Entry> &found = entries[key];
        if (found == nullptr)
            found = std::make_shared<Entry>();
        entry = found;
    }

    // Read once, by the first thread (the entry is written with both mutexes, read with one of them)
    std::lock_guard<std::mutex> lock(entry->readingMutex);
    std::shared_ptr<PluginSoundfont> soundfont = entry->soundfont.lock();
    if (soundfont != nullptr && isSame && !isSame(*soundfont))
        return read(); // Another content with the same key: not shared
    if (soundfont == nullptr)
    {
        soundfont = read();
        if (soundfont != nullptr)
        {
            std::lock_guard<std::mutex> cacheLock(getCacheMutex());
            entry->soundfont = soundfont;
        }
    }
    return soundfont;
}
} // namespace

PluginSoundfont::PluginSoundfont() {}

PluginSoundfont::~PluginSoundfont() {}

std::shared_ptr<PluginSoundfont> PluginSoundfont::load(const std::filesystem::path &filePath, std::string &error)
{
    return getOrRead(getFileKey(filePath), [&filePath, &error]() -> std::shared_ptr<PluginSoundfont> {
        SynthSoundfontFile * file = SynthSoundfontFile::load(filePath, error);
        if (file == nullptr)
            return nullptr;
        std::shared_ptr<PluginSoundfont> soundfont(new PluginSoundfont());
        soundfont->_soundfont.reset(file);
        return soundfont;
    }, nullptr);
}

std::shared_ptr<PluginSoundfont> PluginSoundfont::load(const char * data, size_t length, std::string &error)
{
    return getOrRead(getContentKey(data, length), [data, length, &error]() -> std::shared_ptr<PluginSoundfont> {
        SynthSoundfontFile * file = SynthSoundfontFile::load(data, length, error);
        if (file == nullptr)
            return nullptr;
        std::shared_ptr<PluginSoundfont> soundfont(new PluginSoundfont());
        soundfont->_soundfont.reset(file);
        soundfont->_content.assign(data, data + length);
        return soundfont;
    }, [data, length](const PluginSoundfont &soundfont) {
        return soundfont._content.size() == length && (length == 0 || memcmp(soundfont._content.data(), data, length) == 0);
    });
}

std::shared_ptr<SynthSoundfontFile> PluginSoundfont::getSynthSoundfont()
{
    return std::shared_ptr<SynthSoundfontFile>(shared_from_this(), _soundfont.get());
}

int PluginSoundfont::getCount()
{
    std::lock_guard<std::mutex> lock(getCacheMutex());
    int count = 0;
    for (const auto &entry : getEntries())
        if (!entry.second->soundfont.expired())
            count++;
    return count;
}
