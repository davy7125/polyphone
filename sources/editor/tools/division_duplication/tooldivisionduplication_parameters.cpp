/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "tooldivisionduplication_parameters.h"
#include "contextmanager.h"

void ToolDivisionDuplication_parameters::loadConfiguration()
{
    // Duplication types
    _instDuplicKey = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationKeys", true).toBool();
    _prstDuplicKey = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationKeys", true).toBool();
    _instDuplicVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationVelocity", true).toBool();
    _prstDuplicVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationVelocity", true).toBool();
    
    // Velocity ranges
    QList<QVariant> defaultList;
    defaultList << 0 << 127;
    
    _instVelocityRanges.clear();
    QList<QVariant> listTmp = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "velocities", defaultList).toList();
    for (int i = 0; i < listTmp.size() / 2; i++)
        _instVelocityRanges << QPair<int, int>(listTmp[2 * i].toInt(), listTmp[2 * i + 1].toInt());
    
    _prstVelocityRanges.clear();
    listTmp = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "velocities", defaultList).toList();
    for (int i = 0; i < listTmp.size() / 2; i++)
        _prstVelocityRanges << QPair<int, int>(listTmp[2 * i].toInt(), listTmp[2 * i + 1].toInt());
}

void ToolDivisionDuplication_parameters::saveConfiguration()
{
    // Duplication types
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationKeys", _instDuplicKey);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationKeys", _prstDuplicKey);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "duplicationVelocity", _instDuplicVel);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "duplicationVelocity", _prstDuplicVel);
    
    // Velocity ranges
    QVariantList listTmp;
    for (int i = 0; i < _instVelocityRanges.size(); i++)
        listTmp << _instVelocityRanges[i].first << _instVelocityRanges[i].second;
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "duplication", "velocities", listTmp);
    
    listTmp.clear();
    for (int i = 0; i < _prstVelocityRanges.size(); i++)
        listTmp << _prstVelocityRanges[i].first << _prstVelocityRanges[i].second;
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "duplication", "velocities", listTmp);
}
