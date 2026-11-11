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

#include "tooltrim_parameters.h"

void ToolTrim_parameters::loadConfiguration()
{
    _start = getToolValue("start", true).toBool();
    int endMode = getToolValue("end", END_BLANK).toInt();
    _endMode = (endMode >= END_UNCHANGED && endMode <= END_BLANK) ? static_cast<EndMode>(endMode) : END_BLANK;
    _automaticThreshold = getToolValue("automatic_threshold", true).toBool();
    _threshold = getToolValue("threshold", -70).toDouble();
}

void ToolTrim_parameters::saveConfiguration()
{
    setToolValue("start", _start);
    setToolValue("end", static_cast<int>(_endMode));
    setToolValue("automatic_threshold", _automaticThreshold);
    setToolValue("threshold", _threshold);
}
