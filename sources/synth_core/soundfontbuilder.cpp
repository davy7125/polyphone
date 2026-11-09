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


#include "soundfontbuilder.h"

bool ISoundfontBuilder::isAvailable(int bank, int preset)
{
    Id id(TYPE_PRESET);
    for (int i : getSiblings(id))
    {
        id.element = i;
        if (get(id, champ_wBank).wValue == bank && get(id, champ_wPreset).wValue == preset)
            return false;
    }
    return true;
}

int ISoundfontBuilder::closestAvailablePreset(int bank, int preset)
{
    return findClosestAvailablePreset(preset, [this, bank](int number) { return isAvailable(bank, number); });
}

int ISoundfontBuilder::findClosestAvailablePreset(int preset, const std::function<bool(int)> &isAvailable)
{
    int initVal = preset;
    int delta = 0;
    int sens = 0;
    do
    {
        if (initVal + delta < 128)
        {
            if (isAvailable(initVal + delta))
                sens = 1;
        }
        if (initVal - delta > -1 && sens == 0)
        {
            if (isAvailable(initVal - delta))
                sens = -1;
        }
        delta++;
    } while (sens == 0 && delta < 128);
    if (sens == 1 || sens == -1)
        return initVal + sens * (delta - 1);
    else
        return -1;
}
