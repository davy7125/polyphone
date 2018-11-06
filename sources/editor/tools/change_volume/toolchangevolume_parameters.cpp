#include "toolchangevolume_parameters.h"
#include "contextmanager.h"

void ToolChangeVolume_parameters::loadConfiguration()
{
    // Mode
    _mode = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "mode", 0).toInt();

    // Values
    _addValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "add", 0.).toDouble();
    _multiplyValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "multiply", 1.).toDouble();
    _normalizeValue = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "normalize", 90.).toDouble();
}

void ToolChangeVolume_parameters::saveConfiguration()
{
    // Mode
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "mode", _mode);

    // Values
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "add", _addValue);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "multiply", _multiplyValue);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "volume", "normalize", _normalizeValue);
}
