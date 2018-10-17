#include "toolmonitor_parameters.h"
#include "contextmanager.h"

void ToolMonitor_parameters::loadConfiguration()
{
    _instAttribute = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "monitor", "attribute", 0).toInt();
    _prstAttribute = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "monitor", "attribute", 0).toInt();
    _instLog = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "monitor", "log", false).toBool();
    _prstLog = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "monitor", "log", false).toBool();
}

void ToolMonitor_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "monitor", "attribute", _instAttribute);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "monitor", "attribute", _prstAttribute);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "monitor", "log", _instLog);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "monitor", "log", _prstLog);
}
