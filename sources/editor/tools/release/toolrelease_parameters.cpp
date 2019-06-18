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

#include "toolrelease_parameters.h"
#include "contextmanager.h"

void ToolRelease_parameters::loadConfiguration()
{
    _baseDuration = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "baseDuration", 0.3).toDouble();
    _evolution = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "evolution", 2.0).toDouble();
    _detune = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "detune", 0.0).toDouble();
}

void ToolRelease_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "baseDuration", _baseDuration);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "evolution", _evolution);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "detune", _detune);
}
