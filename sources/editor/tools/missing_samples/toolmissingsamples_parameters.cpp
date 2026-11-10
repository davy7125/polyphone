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

#include "toolmissingsamples_parameters.h"

void ToolMissingSamples_parameters::loadConfiguration()
{
    _mode = getToolValue("mode", 0).toInt() == 1 ? MODE_MAX_SPACING : MODE_KEYS;
    _keys.clear();
    foreach (QVariant key, getToolValue("keys", QVariantList()).toList())
    {
        bool ok;
        int value = key.toInt(&ok);
        if (ok && value >= 0 && value <= 127 && !_keys.contains(value))
            _keys << value;
    }
    std::sort(_keys.begin(), _keys.end());
    _maxSpacing = qBound(1, getToolValue("max_spacing", 3).toInt(), 24);
}

void ToolMissingSamples_parameters::saveConfiguration()
{
    setToolValue("mode", static_cast<int>(_mode));
    QVariantList keys;
    foreach (int key, _keys)
        keys << key;
    setToolValue("keys", keys);
    setToolValue("max_spacing", _maxSpacing);
}
