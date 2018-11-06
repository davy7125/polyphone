#include "toolrelease_parameters.h"
#include "contextmanager.h"

void ToolRelease_parameters::loadConfiguration()
{
    _baseDuration = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "baseDuration", 0.3).toDouble();
    _evolution = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "evolution", 2.0).toDouble();
    _detune = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "detune", 0.0).toDouble();
}

void ToolRelease_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "baseDuration", _baseDuration);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "evolution", _evolution);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "release", "detune", _detune);
}
