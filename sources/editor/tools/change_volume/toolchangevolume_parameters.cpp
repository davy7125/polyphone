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

#include "toolchangevolume_parameters.h"
#include "contextmanager.h"

void ToolChangeVolume_parameters::loadConfiguration()
{
    // Mode
    _mode = getToolValue("mode", 0).toInt();

    // Values
    _addValue = getToolValue("add", 0.).toDouble();
    _multiplyValue = getToolValue("multiply", 1.).toDouble();
    _normalizeValue = getToolValue("normalize", 90.).toDouble();
}

void ToolChangeVolume_parameters::saveConfiguration()
{
    // Mode
    setToolValue("mode", _mode);

    // Values
    setToolValue("add", _addValue);
    setToolValue("multiply", _multiplyValue);
    setToolValue("normalize", _normalizeValue);
}
