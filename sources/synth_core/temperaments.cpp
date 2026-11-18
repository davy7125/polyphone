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

#include "temperaments.h"
#include "textconversions.h"

#ifndef QT_TRANSLATE_NOOP
#define QT_TRANSLATE_NOOP(context, sourceText) sourceText
#endif

// The context starts with "Plugin" so that the plugin embeds these translations (see plugin/plugintranslator.h)
const char * const Temperaments::TRANSLATION_CONTEXT = "PluginTemperament";

// Translated texts of temperaments.csv, found here by lupdate: levels of the categories and generic names
extern const char * const TEMPERAMENT_TRANSLATED_TEXTS[];
const char * const TEMPERAMENT_TRANSLATED_TEXTS[] = {
    // Categories
    QT_TRANSLATE_NOOP("PluginTemperament", "Historical temperaments"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Pythagorean"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Just intonation"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Meantone"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Irregular temperaments"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Historical organs"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Western scales"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Modes"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Harmonic and melodic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Pentatonic and blues"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Symmetric scales"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Jazz"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Just scales"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Arabic maqamat"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Turkish makams"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Persian dastgahs"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Indian thaats"),
    QT_TRANSLATE_NOOP("PluginTemperament", "East Asia"),
    QT_TRANSLATE_NOOP("PluginTemperament", "China"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Japan"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Korea"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Southeast Asia"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Africa"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Eastern Europe and Mediterranean"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Equal divisions of the octave"),

    // Western scales
    QT_TRANSLATE_NOOP("PluginTemperament", "Ionian (major)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Dorian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Phrygian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Lydian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Mixolydian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Aeolian (natural minor)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Locrian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Harmonic minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Melodic minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Harmonic major"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Major pentatonic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Minor pentatonic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Blues"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Major blues"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Whole tone"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Diminished (half-whole)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Diminished (whole-half)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Augmented"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Bebop dominant"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Lydian dominant"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Altered"),

    // Just scales
    QT_TRANSLATE_NOOP("PluginTemperament", "Pure major"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Pure minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Pure pentatonic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Harmonic series (8 to 15)"),

    // Other generic names
    QT_TRANSLATE_NOOP("PluginTemperament", "Slendro (5 equal divisions)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Thai (7 equal divisions)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Tizita major"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Tizita minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Bati major"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Bati minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Freygish (Phrygian dominant)"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Ukrainian Dorian"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Hungarian minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Double harmonic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Neapolitan minor"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Neapolitan major"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Flamenco mode"),
    QT_TRANSLATE_NOOP("PluginTemperament", "Enigmatic"),
    QT_TRANSLATE_NOOP("PluginTemperament", "5 equal divisions"),
    QT_TRANSLATE_NOOP("PluginTemperament", "7 equal divisions"),
    QT_TRANSLATE_NOOP("PluginTemperament", "8 equal divisions"),
    QT_TRANSLATE_NOOP("PluginTemperament", "9 equal divisions"),
    QT_TRANSLATE_NOOP("PluginTemperament", "10 equal divisions"),
    QT_TRANSLATE_NOOP("PluginTemperament", "11 equal divisions"),
};

TemperamentData::TemperamentData()
{
    for (int i = 0; i < 12; i++)
    {
        fineTunes[i] = 0.0f;
        used[i] = true;
    }
}

std::vector<TemperamentData> Temperaments::parse(const std::string &csv)
{
    std::vector<TemperamentData> temperaments;
    std::string text = TextConversions::replaceAll(csv, "\r", "");
    for (const std::string &rawLine : TextConversions::split(text, "\n", false))
    {
        // Comments and empty lines skipped
        std::string line = TextConversions::trimmed(rawLine);
        if (line.empty() || line[0] == '#')
            continue;

        // Category, name, 12 fine tunes
        std::vector<std::string> parts = TextConversions::split(line, ",", true);
        if (parts.size() != 14)
            continue;
        TemperamentData temperament;
        temperament.category = TextConversions::trimmed(parts[0]);
        temperament.name = TextConversions::trimmed(parts[1]);
        if (temperament.name.empty())
            continue;

        bool ok = true;
        for (int i = 0; i < 12 && ok; i++)
        {
            std::string value = TextConversions::trimmed(parts[static_cast<size_t>(i) + 2]);
            temperament.used[i] = (value != "-");
            if (temperament.used[i])
                temperament.fineTunes[i] = static_cast<float>(TextConversions::toDouble(value, &ok));
        }
        if (ok)
            temperaments.push_back(temperament);
    }
    return temperaments;
}
