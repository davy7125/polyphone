#include "toolpresetlist_parameters.h"
#include "contextmanager.h"

void ToolPresetList_parameters::loadConfiguration()
{
    _format = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SF2, "presetlist", "format", 0).toInt();
}

void ToolPresetList_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SF2, "presetlist", "format", _format);
}
