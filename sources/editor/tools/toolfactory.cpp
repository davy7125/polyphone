#include "toolfactory.h"
#include "abstracttool.h"

#include "trim_end/tooltrimend.h"
#include "auto_loop/toolautoloop.h"
#include "external_command/toolexternalcommand.h"
#include "trim_start/tooltrimstart.h"

ToolFactory::ToolFactory(QWidget * parent)
{
    // Initialize the tools
    AbstractTool::initialize(parent);

    // Register all possible tools
    _tools << new ToolTrimEnd()
           << new ToolAutoLoop()
           << new ToolExternalCommand()
           << new ToolTrimStart();
}

ToolFactory::~ToolFactory()
{
    while (!_tools.empty())
        delete _tools.takeFirst();
}

QList<AbstractTool *> ToolFactory::getTools(IdList ids)
{
    QList<AbstractTool *> possibleTools;
    foreach (AbstractTool * tool, _tools)
        if (tool->setIds(ids))
            possibleTools << tool;
    return possibleTools;
}
