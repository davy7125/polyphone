#include "toolremovemods.h"
#include "soundfontmanager.h"

ToolRemoveMods::ToolRemoveMods() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst, NULL, NULL)
{

}

void ToolRemoveMods::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolRemoveMods::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)
}
