#include "toolsoundspatialization.h"
#include "toolsoundspatialization_gui.h"
#include "toolsoundspatialization_parameters.h"
#include "soundfontmanager.h"

ToolSoundSpatialization::ToolSoundSpatialization() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                     new ToolSoundSpatialization_parameters(), new ToolSoundSpatialization_gui())
{

}

void ToolSoundSpatialization::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolSoundSpatialization::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *)parameters;

}
