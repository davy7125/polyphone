#include "toolmonitor.h"
#include "toolmonitor_gui.h"
#include "toolmonitor_parameters.h"
#include "soundfontmanager.h"

ToolMonitor::ToolMonitor() : AbstractTool(new ToolMonitor_parameters(), new ToolMonitor_gui())
{

}

bool ToolMonitor::isCompatible(IdList ids)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (ids.getSelectedIds(elementInst).count() == 1)
    {
        // The instrument must comprise at least one sample
        EltID id = ids.getSelectedIds(elementInst)[0];
        id.typeElement = elementInstSmpl;
        return !sm->getSiblings(id).empty();
    }
    else if (ids.getSelectedIds(elementPrst).count() == 1)
    {
        // The preset must comprise at least one instrument
        EltID id = ids.getSelectedIds(elementPrst)[0];
        id.typeElement = elementPrstInst;
        return !sm->getSiblings(id).empty();
    }

    return false;
}

void ToolMonitor::run(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);

    Q_UNUSED(sm)
    Q_UNUSED(parent)
    Q_UNUSED(parameters)
}
