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


#ifndef SOUNDFONTMANAGERBUILDER_H
#define SOUNDFONTMANAGERBUILDER_H

#include "soundfontbuilder.h"
#include "basetypes.h"
class SoundfontManager;

// Soundfont of the editor written by the readers shared with the plugin (SfzReader, DlsReader)
class SoundfontManagerBuilder: public ISoundfontBuilder
{
public:
    // Soundfont already existing, or created by the reader (sf2Index -1)
    SoundfontManagerBuilder(SoundfontManager * sm, int sf2Index);
    int getSf2Index() const { return _sf2Index; }

    int add(const Id &id) override;
    void set(const Id &id, AttributeType champ, AttributeValue value) override;
    void setText(const Id &id, AttributeType champ, const std::string &text) override;
    bool isSet(const Id &id, AttributeType champ) override;
    AttributeValue get(const Id &id, AttributeType champ) override;
    std::string getText(const Id &id, AttributeType champ) override;
    void reset(const Id &id, AttributeType champ) override;
    void setSoundProperty(const Id &id, AttributeType champ, AttributeValue value) override;
    std::vector<int> getSiblings(const Id &id) override;
    bool isAvailable(int bank, int preset) override;
    int closestAvailablePreset(int bank, int preset) override;
    void simplify(const Id &id, AttributeType champ) override;

private:
    EltID toEltId(const Id &id) const;

    SoundfontManager * _sm;
    int _sf2Index;
};

#endif // SOUNDFONTMANAGERBUILDER_H
