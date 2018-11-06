#include "tooltranspose_parameters.h"
#include "contextmanager.h"

void ToolTranspose_parameters::loadConfiguration()
{
    _semiTones = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "semitones", 12.).toDouble();
    _adaptKeyRanges = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", true).toBool();
}

void ToolTranspose_parameters::saveConfiguration()
{
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "semitones", _semiTones);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "transposition", "keyrange", _adaptKeyRanges);
}
