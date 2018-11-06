#include "toolpresetlist.h"
#include "toolpresetlist_gui.h"
#include "toolpresetlist_parameters.h"
#include "soundfontmanager.h"

ToolPresetList::ToolPresetList() : AbstractToolIterating(elementSf2, new ToolPresetList_parameters(), new ToolPresetList_gui())
{

}

void ToolPresetList::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(sm)
    Q_UNUSED(id)
    Q_UNUSED(parameters)

    // Nothing here
}

