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

#include "tooldivisionduplication_parameters.h"
#include "contextmanager.h"

void ToolDivisionDuplication_parameters::loadConfiguration()
{
    // Duplication types
    _duplicKey = getToolValue("duplicationKeys", true).toBool();
    _duplicVel = getToolValue("duplicationVelocity", true).toBool();
    
    // Velocity ranges
    QList<QVariant> defaultList;
    defaultList << 0 << 127;
    
    _velocityRanges.clear();
    QList<QVariant> listTmp = getToolValue("velocities", defaultList).toList();
    for (int i = 0; i < listTmp.size() / 2; i++)
        _velocityRanges << QPair<int, int>(listTmp[2 * i].toInt(), listTmp[2 * i + 1].toInt());
}

void ToolDivisionDuplication_parameters::saveConfiguration()
{
    // Duplication types
    setToolValue("duplicationKeys", _duplicKey);
    setToolValue("duplicationVelocity", _duplicVel);
    
    // Velocity ranges
    QVariantList listTmp;
    for (int i = 0; i < _velocityRanges.size(); i++)
        listTmp << _velocityRanges[i].first << _velocityRanges[i].second;
    setToolValue("velocities", listTmp);
}
