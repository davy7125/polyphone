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

#include "toolexternalcommand_parameters.h"
#include "contextmanager.h"

const int ToolExternalCommand_parameters::HISTORY_SIZE = 10;

void ToolExternalCommand_parameters::loadConfiguration()
{
    // Command history
    _commandHistory.clear();
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        QString previous = ContextManager::configuration()->getToolValue(
                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                    "previous_" + QString("%1").arg(i + 1, 2, 10, QChar('0')), "").toString();
        if (!previous.isEmpty() && !_commandHistory.contains(previous))
            _commandHistory << previous;
    }

    // Stereo
    _stereo = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_SAMPLE, "command", "stereo", true).toBool();

    // Replace info
    _replaceInfo = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_SAMPLE, "command", "replace_info", false).toBool();
}

void ToolExternalCommand_parameters::saveConfiguration()
{
    // Command history
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        QString tmp = (i < _commandHistory.count()) ? _commandHistory[i] : "";
        ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command",
                                                      "previous_" + QString("%1").arg(i + 1, 2, 10, QChar('0')), tmp);
    }

    // Stereo and replace info
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command", "stereo", _stereo);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "command", "replace_info", _replaceInfo);
}
