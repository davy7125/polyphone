#include "toolmixturecreation.h"
#include "toolmixturecreation_gui.h"
#include "toolmixturecreation_parameters.h"
#include "soundfontmanager.h"

ToolMixtureCreation::ToolMixtureCreation() : AbstractToolIterating(elementInst, new ToolMixtureCreation_parameters(), new ToolMixtureCreation_gui())
{

}

void ToolMixtureCreation::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolMixtureCreation_parameters * params = (ToolMixtureCreation_parameters *)parameters;
}
