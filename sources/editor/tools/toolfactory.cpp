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
#include "auto_distribution/toolautodistribution.h"
#include "clean_unused_elements/toolcleanunused.h"
#include "division_duplication/tooldivisionduplication.h"
#include "frequency_peaks/toolfrequencypeaks.h"
#include "mixture_creation/toolmixturecreation.h"
#include "monitor/toolmonitor.h"
#include "preset_list/toolpresetlist.h"
#include "remove_mods/toolremovemods.h"
#include "sound_spatialization/toolsoundspatialization.h"
#include "transpose/tooltranspose.h"
#include "release/toolrelease.h"
#include "chords/toolchords.h"
#include "sample_export/toolsampleexport.h"


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
           << new ToolFrequencyPeaks()
           << new ToolSampleExport()
           << new ToolCelesteTuning() // Instruments
           << new ToolAutoDistribution()
           << new ToolMixtureCreation()
           << new ToolChords()
           << new ToolSoundSpatialization()
           << new ToolTranspose()
           << new ToolRelease()
           << new ToolChangeAttenuation() // Instruments, presets
           << new ToolGlobalSettings()
           << new ToolDivisionDuplication()
           << new ToolMonitor()
           << new ToolRemoveMods()
           << new ToolCleanUnused()   // Sf2
           << new ToolPresetList();
}

ToolFactory::~ToolFactory()
{
    // Delete all tools
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
