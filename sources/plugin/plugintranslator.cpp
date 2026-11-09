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


#include "plugintranslator.h"
#include <juce_data_structures/juce_data_structures.h>
#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include "pluginprocessor.h"
#include "PluginTranslations.h"

// Names of the languages in the languages themselves, as in the editor (TranslationManager::getNativeLanguageName). A
// new translation file needs its name here, its code being shown otherwise.
static const char * const LANGUAGE_NAMES[][2] = {
    { "ca", "catal\xc3\xa0" }, // Catalan
    { "cs", "\xc4\x8d" "e\xc5\xa1tina" }, // Czech
    { "da", "dansk" },
    { "de", "Deutsch" },
    { "en", "English" },
    { "es", "espa\xc3\xb1ol" },
    { "fr", "fran\xc3\xa7" "ais" },
    { "it", "italiano" },
    { "ja", "\xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e" }, // Japanese
    { "ko", "\xed\x95\x9c\xea\xb5\xad\xec\x96\xb4" }, // Korean
    { "nl", "Nederlands" },
    { "pl", "polski" },
    { "pt", "portugu\xc3\xaas" },
    { "ru", "\xd1\x80\xd1\x83\xd1\x81\xd1\x81\xd0\xba\xd0\xb8\xd0\xb9" }, // Russian
    { "sr", "\xd1\x81\xd1\x80\xd0\xbf\xd1\x81\xd0\xba\xd0\xb8" }, // Serbian
    { "zh", "\xe7\xae\x80\xe4\xbd\x93\xe4\xb8\xad\xe6\x96\x87" } // Simplified Chinese
};

// Texts of JUCE (TRANS) shown by the plugin: standalone application (window, audio and MIDI settings), file chooser
// of JUCE (Linux without zenity or kdialog) and message boxes. To be checked when JUCE is updated.
static const char * const JUCE_TEXTS[] = {
    // Standalone window (juce_StandaloneFilterWindow.h)
    QT_TRANSLATE_NOOP("PluginJuce", "Options"), // Text of a button that JUCE does not translate, see PluginEditor
    QT_TRANSLATE_NOOP("PluginJuce", "Audio/MIDI Settings..."),
    QT_TRANSLATE_NOOP("PluginJuce", "Save current state..."),
    QT_TRANSLATE_NOOP("PluginJuce", "Load a saved state..."),
    QT_TRANSLATE_NOOP("PluginJuce", "Reset to default state"),
    QT_TRANSLATE_NOOP("PluginJuce", "Audio/MIDI Settings"),
    QT_TRANSLATE_NOOP("PluginJuce", "Save current state"),
    QT_TRANSLATE_NOOP("PluginJuce", "Load a saved state"),
    QT_TRANSLATE_NOOP("PluginJuce", "Error whilst saving"),
    QT_TRANSLATE_NOOP("PluginJuce", "Couldn't write to the specified file!"),
    QT_TRANSLATE_NOOP("PluginJuce", "Error whilst loading"),
    QT_TRANSLATE_NOOP("PluginJuce", "Couldn't read from the specified file!"),

    // Audio and MIDI settings (juce_AudioDeviceSelectorComponent.cpp)
    QT_TRANSLATE_NOOP("PluginJuce", "Audio device type:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Output:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Input:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Device:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Test"),
    QT_TRANSLATE_NOOP("PluginJuce", "Plays a test tone"),
    QT_TRANSLATE_NOOP("PluginJuce", "Control Panel"),
    QT_TRANSLATE_NOOP("PluginJuce", "Opens the device's own control panel"),
    QT_TRANSLATE_NOOP("PluginJuce", "Reset Device"),
    QT_TRANSLATE_NOOP("PluginJuce", "Resets the audio interface - sometimes needed after changing a device's "
                                    "properties in its custom control panel"),
    QT_TRANSLATE_NOOP("PluginJuce", "Active output channels:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Active input channels:"),
    QT_TRANSLATE_NOOP("PluginJuce", "(no audio output channels found)"),
    QT_TRANSLATE_NOOP("PluginJuce", "(no audio input channels found)"),
    QT_TRANSLATE_NOOP("PluginJuce", "Sample rate:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Audio buffer size:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Show advanced settings..."),
    QT_TRANSLATE_NOOP("PluginJuce", "Active MIDI inputs:"),
    QT_TRANSLATE_NOOP("PluginJuce", "No MIDI inputs available"),
    QT_TRANSLATE_NOOP("PluginJuce", "MIDI Output:"),
    QT_TRANSLATE_NOOP("PluginJuce", "none"),
    QT_TRANSLATE_NOOP("PluginJuce", "Bluetooth MIDI"),
    QT_TRANSLATE_NOOP("PluginJuce", "Scan for bluetooth MIDI devices"),
    QT_TRANSLATE_NOOP("PluginJuce", "Error when trying to open audio device!"),

    // File chooser of JUCE (juce_FileBrowserComponent.cpp, juce_FileChooserDialogBox.cpp)
    QT_TRANSLATE_NOOP("PluginJuce", "Open"),
    QT_TRANSLATE_NOOP("PluginJuce", "Save"),
    QT_TRANSLATE_NOOP("PluginJuce", "Choose"),
    QT_TRANSLATE_NOOP("PluginJuce", "file:"),
    QT_TRANSLATE_NOOP("PluginJuce", "Go up to parent directory"),
    QT_TRANSLATE_NOOP("PluginJuce", "Home folder"),
    QT_TRANSLATE_NOOP("PluginJuce", "Desktop"),
    QT_TRANSLATE_NOOP("PluginJuce", "Documents"),
    QT_TRANSLATE_NOOP("PluginJuce", "Music"),
    QT_TRANSLATE_NOOP("PluginJuce", "Pictures"),
    QT_TRANSLATE_NOOP("PluginJuce", "Hard Drive"),
    QT_TRANSLATE_NOOP("PluginJuce", "CD/DVD drive"),
    QT_TRANSLATE_NOOP("PluginJuce", "New Folder"),
    QT_TRANSLATE_NOOP("PluginJuce", "Create Folder"),
    QT_TRANSLATE_NOOP("PluginJuce", "Please enter the name for the folder"),
    QT_TRANSLATE_NOOP("PluginJuce", "Couldn't create the folder!"),
    QT_TRANSLATE_NOOP("PluginJuce", "File already exists"),
    QT_TRANSLATE_NOOP("PluginJuce", "There's already a file called: FLNM"), // FLNM: name of the file
    QT_TRANSLATE_NOOP("PluginJuce", "Are you sure you want to overwrite it?"),
    QT_TRANSLATE_NOOP("PluginJuce", "Overwrite"),

    // Message boxes and lists (juce_MessageBoxOptions.cpp, juce_AlertWindow.cpp, juce_ComboBox.cpp)
    QT_TRANSLATE_NOOP("PluginJuce", "OK"),
    QT_TRANSLATE_NOOP("PluginJuce", "Cancel"),
    QT_TRANSLATE_NOOP("PluginJuce", "Yes"),
    QT_TRANSLATE_NOOP("PluginJuce", "No"),
    QT_TRANSLATE_NOOP("PluginJuce", "(no choices)")
};

namespace
{
// Translations of a language: context, source text and disambiguation (separated by \x04) give the translation
using Translations = std::map<std::string, juce::String>;

std::string getKey(const char * context, const char * sourceText, const char * disambiguation)
{
    std::string key(context);
    key += '\x04';
    key += sourceText;
    key += '\x04';
    if (disambiguation != nullptr)
        key += disambiguation;
    return key;
}

// Content of a translation file extracted by translations.cmake: the contexts "Plugin..." of polyphone_XX.ts
std::shared_ptr<const Translations> readTranslations(const juce::String &code)
{
    auto translations = std::make_shared<Translations>();
    for (int i = 0; i < PluginTranslations::namedResourceListSize; i++)
    {
        const char * resource = PluginTranslations::namedResourceList[i];
        if (juce::String(PluginTranslations::getNamedResourceOriginalFilename(resource)) != code + ".xml")
            continue;

        int size = 0;
        const char * data = PluginTranslations::getNamedResource(resource, size);
        std::unique_ptr<juce::XmlElement> root = juce::XmlDocument::parse(juce::String::fromUTF8(data, size));
        if (root == nullptr)
            break;
        for (const juce::XmlElement * context : root->getChildWithTagNameIterator("context"))
        {
            juce::String contextName = context->getChildElementAllSubText("name", {});
            for (const juce::XmlElement * message : context->getChildWithTagNameIterator("message"))
            {
                // Translations unfinished, obsolete or with plural forms not used (lrelease -nounfinished)
                const juce::XmlElement * translation = message->getChildByName("translation");
                if (translation == nullptr || translation->hasAttribute("type") ||
                    translation->getChildByName("numerusform") != nullptr)
                    continue;
                juce::String text = translation->getAllSubText();
                if (text.isEmpty())
                    continue;
                (*translations)[getKey(contextName.toRawUTF8(),
                                       message->getChildElementAllSubText("source", {}).toRawUTF8(),
                                       message->getChildElementAllSubText("comment", {}).toRawUTF8())] = text;
            }
        }
        break;
    }
    return translations;
}

// Language of the system, as the editor (QLocale::system()) except on Windows and macOS where the language of the
// interface is used
juce::String getSystemLanguage()
{
    juce::String language;
#if JUCE_LINUX || JUCE_BSD
    for (const char * variable : { "LC_ALL", "LC_MESSAGES", "LANG" })
    {
        language = juce::SystemStats::getEnvironmentVariable(variable, {});
        if (language.isNotEmpty())
            break;
    }
    if (language.isNotEmpty() && language != "C" && language != "POSIX")
    {
        // The first language of LANGUAGE has the priority (gettext)
        juce::String first = juce::SystemStats::getEnvironmentVariable("LANGUAGE", {})
            .upToFirstOccurrenceOf(":", false, false);
        if (first.isNotEmpty())
            language = first;
    }
#else
    language = juce::SystemStats::getDisplayLanguage();
#endif
    // "fr_FR.UTF-8", "fr-FR", "zh-Hans-CN"...
    return language.upToFirstOccurrenceOf("_", false, false).upToFirstOccurrenceOf("-", false, false)
        .upToFirstOccurrenceOf(".", false, false).upToFirstOccurrenceOf("@", false, false).toLowerCase();
}

// The LV2 description (names of the parameters and of the outputs) is written by juce_lv2_helper during the build: it
// is always in English, whatever the computer building the plugin
bool isBuildingLv2Description()
{
    return juce::File::getSpecialLocation(juce::File::hostApplicationPath).getFileName().startsWith("juce_lv2_helper");
}

juce::String escape(const juce::String &text)
{
    // As read by juce::LocalisedStrings (no backslash in the texts)
    return text.replace("\"", "\\\"").replace("\n", "\\n").replace("\r", "\\r").replace("\t", "\\t");
}

class State
{
public:
    static State &getInstance()
    {
        static State state;
        return state;
    }

    juce::String getSetting()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _setting;
    }

    void setSetting(const juce::String &code)
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (code == _setting)
                return;
            _setting = code;
        }
        juce::PropertiesFile settings(PluginProcessor::getSettingsOptions());
        settings.setValue("language", code);
        settings.saveIfNeeded();
        load();
    }

    juce::String getLanguage()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _language;
    }

    int getVersion()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _version;
    }

    juce::String translate(const char * context, const char * sourceText, const char * disambiguation)
    {
        std::shared_ptr<const Translations> translations;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            translations = _translations;
        }
        auto it = translations->find(getKey(context, sourceText, disambiguation));
        return it == translations->end() ? juce::String::fromUTF8(sourceText) : it->second;
    }

private:
    State() :
        _translations(std::make_shared<Translations>()),
        _version(0)
    {
        if (!isBuildingLv2Description())
        {
            juce::PropertiesFile settings(PluginProcessor::getSettingsOptions());
            _setting = settings.getValue("language");
        }
        load();
    }

    void load()
    {
        juce::String language = isBuildingLv2Description() ? juce::String("en") : getSetting();
        if (language.isEmpty())
            language = getSystemLanguage();
        bool exists = false;
        for (const PluginTranslator::Language &available : PluginTranslator::getLanguages())
            exists = exists || available.code == language;
        if (!exists)
            language = "en";
        std::shared_ptr<const Translations> translations = (language == "en") ? std::make_shared<Translations>() :
                                                                                readTranslations(language);

        // Texts of JUCE
        juce::String mappings;
        for (const char * text : JUCE_TEXTS)
        {
            auto it = translations->find(getKey("PluginJuce", text, nullptr));
            if (it != translations->end())
                mappings << "\"" << escape(text) << "\" = \"" << escape(it->second) << "\"\n";
        }
        mappings = "language: " + language + "\n" + mappings;
        juce::LocalisedStrings::setCurrentMappings(translations->empty() ? nullptr :
                                                                         new juce::LocalisedStrings(mappings, false));

        std::lock_guard<std::mutex> lock(_mutex);
        _language = language;
        _translations = translations;
        _version++;
    }

    std::mutex _mutex;
    juce::String _setting;
    juce::String _language;
    std::shared_ptr<const Translations> _translations;
    int _version;
};
}

std::vector<PluginTranslator::Language> PluginTranslator::getLanguages()
{
    std::vector<Language> languages;
    languages.push_back({ "en", "English" });
    for (int i = 0; i < PluginTranslations::namedResourceListSize; i++)
    {
        juce::String fileName = PluginTranslations::getNamedResourceOriginalFilename(
            PluginTranslations::namedResourceList[i]);
        if (!fileName.endsWith(".xml"))
            continue;
        Language language;
        language.code = fileName.dropLastCharacters(4);
        language.name = language.code;
        for (const auto &name : LANGUAGE_NAMES)
            if (language.code == name[0])
                language.name = juce::String::fromUTF8(name[1]);
        languages.push_back(language);
    }
    std::sort(languages.begin(), languages.end(), [](const Language &a, const Language &b) {
        return a.name.compareIgnoreCase(b.name) < 0;
    });
    return languages;
}

juce::String PluginTranslator::getLanguageSetting()
{
    return State::getInstance().getSetting();
}

void PluginTranslator::setLanguageSetting(const juce::String &code)
{
    State::getInstance().setSetting(code);
}

juce::String PluginTranslator::getLanguage()
{
    return State::getInstance().getLanguage();
}

int PluginTranslator::getLanguageVersion()
{
    return State::getInstance().getVersion();
}

juce::String PluginTranslator::translate(const char * context, const char * sourceText, const char * disambiguation)
{
    return State::getInstance().translate(context, sourceText, disambiguation);
}

juce::String PluginTranslator::arg(const juce::String &text, const juce::String &argument)
{
    return text.replace("%1", argument);
}
