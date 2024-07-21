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

#include "toolfasteditsmpl_parameters.h"
#include "contextmanager.h"
#include "basetypes.h"

void ToolFastEditSmpl_parameters::loadConfiguration()
{
    // Mode
    _mode = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "mode", 0).toInt();

    // Values
    _addValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "add", 0.).toInt();
    _multiplyValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "multiply", 1.).toDouble();

    // Parameter
    _parameter = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "parameter", champ_chPitchCorrection).toInt();
}

void ToolFastEditSmpl_parameters::saveConfiguration()
{
    // Mode
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "mode", _mode);

    // Values
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "add", _addValue);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "multiply", _multiplyValue);

    // Parameter
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "fast_edit", "parameter", _parameter);
}
