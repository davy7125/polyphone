#include "toolmonitor.h"
#include "toolmonitor_gui.h"
#include "toolmonitor_parameters.h"

ToolMonitor::ToolMonitor() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                   new ToolMonitor_parameters(), new ToolMonitor_gui())
{

}

void ToolMonitor::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolMonitor::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(sm)
    Q_UNUSED(id)
    Q_UNUSED(parameters)
}
