#include "toolexternalcommand_parameters.h"
#include "contextmanager.h"

const int ToolExternalCommand_parameters::HISTORY_SIZE = 10;

void ToolExternalCommand_parameters::loadConfiguration()
{
    // Load previous commands
    _commandHistory.clear();
    for (int i = 0; i < HISTORY_SIZE; i++)
    {
        QString previous = ContextManager::configuration()->getToolValue(
                    ConfManager::TOOL_TYPE_SAMPLE, "command",
                    "previous_" + QString("%1").arg(i+1, 2, 10, QChar('0')), "").toString();
        if (!previous.isEmpty())
            _commandHistory << previous;
    }

    // Stereo
    _stereo = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_SAMPLE, "command", "stereo", true).toBool();

    // Replace info
    _replaceInfo = ContextManager::configuration()->getToolValue(
                ConfManager::TOOL_TYPE_SAMPLE, "command", "replace_info", false).toBool();
}

void ToolExternalCommand_parameters::saveConfiguration()
{

}
