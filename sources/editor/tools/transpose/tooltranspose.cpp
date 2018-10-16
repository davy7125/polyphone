#include "tooltranspose.h"
#include "tooltranspose_gui.h"
#include "tooltranspose_parameters.h"
#include "soundfontmanager.h"

ToolTranspose::ToolTranspose() : AbstractToolIterating(elementInst, new ToolTranspose_parameters(), new ToolTranspose_gui())
{

}

void ToolTranspose::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolTranspose_parameters * params = (ToolTranspose_parameters *)parameters;
    
}

