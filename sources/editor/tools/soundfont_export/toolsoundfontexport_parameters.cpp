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
