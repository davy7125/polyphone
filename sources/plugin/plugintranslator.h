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


#ifndef PLUGINTRANSLATOR_H
#define PLUGINTRANSLATOR_H

#include <juce_core/juce_core.h>
#include <vector>

// Translations of the plugin, without Qt. The texts are in the translation files of the editor
// (sources/resources/translations/polyphone_XX.ts), in the contexts whose name starts with "Plugin": lupdate finds them
// in the sources of the plugin as in the ones of the editor (target update_translations), they are translated with Qt
// Linguist, and the CMake project of the plugin extracts these contexts and embeds them (translations.cmake).
//
// A class declares its function tr() with Q_DECLARE_TR_FUNCTIONS(Class), the context of its texts being the name of
// the class, and the texts that are translated later are marked with QT_TRANSLATE_NOOP("Context", "text"), as with Qt.
// The context "PluginJuce" holds the texts of JUCE (TRANS), translated by juce::LocalisedStrings.
//
// The language is the one chosen in the settings of the plugin, shared by all instances, or the language of the system.
// The functions can be called by any thread.
class PluginTranslator
{
public:
    struct Language
    {
        juce::String code; // "en", "fr"...
        juce::String name; // In the language itself, as in the editor
    };

    // English and the languages embedded, sorted by name as in the editor
    static std::vector<Language> getLanguages();

    // Language chosen in the settings, empty for the language of the system
    static juce::String getLanguageSetting();
    static void setLanguageSetting(const juce::String &code);

    // Language of the texts, and the number of times it has been loaded (an interface compares it to update its texts)
    static juce::String getLanguage();
    static int getLanguageVersion();

    // Text translated, the text itself if there is no translation
    static juce::String translate(const char * context, const char * sourceText, const char * disambiguation = nullptr);

    // Text with %1 replaced by the argument
    static juce::String arg(const juce::String &text, const juce::String &argument);
};

// Same declaration as Qt (found by lupdate): static function tr() of a class, its name being the context
#define Q_DECLARE_TR_FUNCTIONS(context) \
public: \
    static juce::String tr(const char * sourceText, const char * disambiguation = nullptr) \
    { \
        return PluginTranslator::translate(#context, sourceText, disambiguation); \
    } \
private:

// Text marked for the translation (found by lupdate), translated later with its context
#define QT_TRANSLATE_NOOP(context, sourceText) sourceText

#endif // PLUGINTRANSLATOR_H
