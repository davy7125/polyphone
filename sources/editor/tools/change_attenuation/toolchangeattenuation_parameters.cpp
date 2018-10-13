#include "toolchangeattenuation_parameters.h"
#include "contextmanager.h"

void ToolChangeAttenuation_parameters::loadConfiguration()
{
    // Values
    _instValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "volume", "dB", 0.).toDouble();
    _prstValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "volume", "dB", 0.).toDouble();
}

void ToolChangeAttenuation_parameters::saveConfiguration()
{
    // Values
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "volume", "dB", _instValue);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "volume", "dB", _prstValue);
}
