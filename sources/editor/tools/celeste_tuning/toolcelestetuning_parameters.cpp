#include "toolcelestetuning_parameters.h"
#include "contextmanager.h"

void ToolCelesteTuning_parameters::loadConfiguration()
{
    // Values
    _baseFrequency = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "celeste_tuning", "frequency", 4.).toDouble();
    _coefficient = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "celeste_tuning", "coefficient", 1.).toDouble();
}

void ToolCelesteTuning_parameters::saveConfiguration()
{
    // Values
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "celeste_tuning", "frequency", _baseFrequency);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "celeste_tuning", "coefficient", _coefficient);
}
