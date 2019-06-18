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

#include "tooltranspose_parameters.h"
#include "contextmanager.h"

void ToolTranspose_parameters::loadConfiguration()
{
    _semiTones = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "semitones", 12.).toDouble();
    _adaptKeyRanges = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", true).toBool();
}

void ToolTranspose_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "semitones", _semiTones);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", _adaptKeyRanges);
}
