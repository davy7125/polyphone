#include "tooldivisionduplication.h"
#include "tooldivisionduplication_gui.h"
#include "tooldivisionduplication_parameters.h"
#include "soundfontmanager.h"
#include <qmath.h>

ToolDivisionDuplication::ToolDivisionDuplication() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                                       new ToolDivisionDuplication_parameters(), new ToolDivisionDuplication_gui())
{

}

void ToolDivisionDuplication::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolDivisionDuplication::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolDivisionDuplication_parameters * params = (ToolDivisionDuplication_parameters *)parameters;

}
