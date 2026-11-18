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

#ifndef TEMPERAMENTS_H
#define TEMPERAMENTS_H

#include <string>
#include <vector>

// A temperament or a scale of resources/misc/temperaments.csv, shared by the editor and the plugin
struct TemperamentData
{
    TemperamentData();

    // Category, its levels being separated by "/", and name: English texts, the categories and the generic names
    // being translated with the context Temperaments::TRANSLATION_CONTEXT (proper names are not translated)
    std::string category;
    std::string name;

    // Fine tune in cents and use of each note, from the tonic (C) to B: the tonic is then moved to the tonality
    float fineTunes[12];
    bool used[12];
};

class Temperaments
{
public:
    // Lines of temperaments.csv (see this file for the format), the equal temperament not included
    static std::vector<TemperamentData> parse(const std::string &csv);

    // Translation context of the categories and of the generic names
    static const char * const TRANSLATION_CONTEXT;
};

#endif // TEMPERAMENTS_H
