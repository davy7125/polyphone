#include "toolfactory.h"
#include "abstracttool.h"

#include "trim_end/tooltrimend.h"
#include "auto_loop/toolautoloop.h"
#include "external_command/toolexternalcommand.h"
#include "trim_start/tooltrimstart.h"
#include "frequency_filter/toolfrequencyfilter.h"
#include "change_volume/toolchangevolume.h"
#include "balance_adjustment/toolbalanceadjustment.h"
#include "transpose_smpl/tooltransposesmpl.h"
#include "link_sample/toollinksample.h"
#include "unlink_sample/toolunlinksample.h"
#include "change_attenuation/toolchangeattenuation.h"
#include "global_settings/toolglobalsettings.h"
#include "celeste_tuning/toolcelestetuning.h"

ToolFactory::ToolFactory(QWidget * parent)
{
    // Initialize the tools
    AbstractTool::initialize(parent);

    // Register all possible tools
    _tools << new ToolTrimEnd() // Samples
           << new ToolAutoLoop()
           << new ToolExternalCommand()
           << new ToolTrimStart()
           << new ToolFrequencyFilter()
           << new ToolChangeVolume()
           << new ToolBalanceAdjustment()
           << new ToolTransposeSmpl()
           << new ToolLinkSample()
           << new ToolUnlinkSample()
           << new ToolCelesteTuning() // Instruments
           << new ToolChangeAttenuation() // Instruments, presets
           << new ToolGlobalSettings();
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
