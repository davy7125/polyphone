#include "toolfrequencyfilter.h"
#include "toolfrequencyfilter_gui.h"
#include "toolfrequencyfilter_parameters.h"
#include "soundfontmanager.h"

ToolFrequencyFilter::ToolFrequencyFilter() : AbstractToolIterating(elementSmpl, new ToolFrequencyFilter_parameters(), new ToolFrequencyFilter_gui())
{

}

void ToolFrequencyFilter::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    // Get the curve used to cut frequencies
    ToolFrequencyFilter_parameters * params = (ToolFrequencyFilter_parameters *)parameters;
    QVector<double> dValues = params->getCurve();

    // Get data and sample rate of the sample
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 dwSmplRate = sm->get(id, champ_dwSampleRate).dwValue;

    // Apply the filter and update data
    baData = Sound::cutFilter(baData, dwSmplRate, dValues, 24, 20000);
    sm->set(id, champ_sampleDataFull24, baData);
}
