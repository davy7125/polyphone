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

#include "toolsoundfontexport_parameters.h"
#include "contextmanager.h"

void ToolSoundfontExport_parameters::loadConfiguration()
{
    _directory = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_EXPORT);
    _format = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "type", 0).toInt();
    _quality = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "quality", 1).toInt();
    _presetPrefix = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "preset_prefix", true).toBool();
    _bankDirectory = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "bank_directory", false).toBool();
    _gmSort = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "gm_sort", false).toBool();
}

void ToolSoundfontExport_parameters::saveConfiguration()
{
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_EXPORT, _directory + "/soundfont.sfz");
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "type", _format);
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "quality", _quality);
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "preset_prefix", _presetPrefix);
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "bank_directory", _bankDirectory);
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "gm_sort", _gmSort);
}
