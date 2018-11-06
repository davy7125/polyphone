#include "tooltransposesmpl_parameters.h"
#include "contextmanager.h"

void ToolTransposeSmpl_parameters::loadConfiguration()
{
    // Number of semitones
    _semiTones = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_SAMPLE, "transposition", "ton", 12.).toDouble();
}

void ToolTransposeSmpl_parameters::saveConfiguration()
{
    // Number of semitones
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_SAMPLE, "transposition", "ton", _semiTones);
}
