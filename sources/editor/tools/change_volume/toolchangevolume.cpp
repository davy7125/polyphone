#include "toolchangevolume.h"
#include "toolchangevolume_gui.h"
#include "toolchangevolume_parameters.h"
#include "soundfontmanager.h"
#include <qmath.h>

ToolChangeVolume::ToolChangeVolume() : AbstractToolIterating(elementSmpl, new ToolChangeVolume_parameters(), new ToolChangeVolume_gui())
{

}

void ToolChangeVolume::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolChangeVolume_parameters * params = (ToolChangeVolume_parameters *)parameters;

    // Sample data
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);

    // Change the volume
    double db = 0;
    switch (params->getMode())
    {
    case 0: // Add dB
        // Compute the factor
        baData = Sound::multiplier(baData, qPow(10, params->getAddValue() / 20.0), 24, db);
        break;
    case 1: // Multiply by a factor
        baData = Sound::multiplier(baData, params->getMultiplyValue(), 24, db);
        break;
    case 2: // Normalize
        baData = Sound::normaliser(baData, params->getNormalizeValue() / 100, 24, db);
        break;
    default:
        // Nothing
        return;
    }

    // Update the sample data
    sm->set(id, champ_sampleDataFull24, baData);
}
