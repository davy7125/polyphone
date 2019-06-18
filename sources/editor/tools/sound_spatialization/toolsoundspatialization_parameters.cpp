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

#include "toolsoundspatialization_parameters.h"
#include "contextmanager.h"

void ToolSoundSpatialization_parameters::loadConfiguration()
{
    // Pattern description
    _instPattern = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "pattern", 0).toInt();
    _prstPattern = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "pattern", 0).toInt();
    _instDivisionNumber = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "divisions", 1).toInt();
    _prstDivisionNumber = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "divisions", 1).toInt();
    _instSpreading = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "spreading", 100).toInt();
    _prstSpreading = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "spreading", 100).toInt();
    _instFilling = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "filling", 100).toInt();
    _prstFilling = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "filling", 100).toInt();
    _instOffset = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "offset", 50).toInt();
    _prstOffset = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "offset", 50).toInt();
    
    // Flip / flop
    _instFlip = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "flip", false).toBool();
    _prstFlip = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "flip", false).toBool();
    _instFlop = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "flop", false).toBool();
    _prstFlop = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "flop", false).toBool();
}

void ToolSoundSpatialization_parameters::saveConfiguration()
{
    // Pattern description
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "pattern", _instPattern);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "pattern", _prstPattern);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "divisions", _instDivisionNumber);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "divisions", _prstDivisionNumber);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "spreading", _instSpreading);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "spreading", _prstSpreading);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "filling", _instFilling);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "filling", _prstFilling);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "offset", _instOffset);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "offset", _prstOffset);
    
    // Flip / flop
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "flip", _instFlip);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "flip", _prstFlip);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "space", "flop", _instFlop);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "space", "flop", _prstFlop);
}
