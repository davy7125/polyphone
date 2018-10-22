#include "tooltrimstart.h"
#include "soundfontmanager.h"

void ToolTrimStart::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
    Q_UNUSED(sm);

    trim(id);
}

void ToolTrimStart::trim(EltID id)
{
    // Get the sample data and trim it
    SoundfontManager * sm = SoundfontManager::getInstance();
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 pos = 0;
    baData = Sound::enleveBlanc(baData, 0.001, 24, pos);
    if (pos == 0)
        return;

    // Update data
    sm->set(id, champ_sampleDataFull24, baData);

    // Update length
    AttributeValue val;
    val.dwValue = baData.size()/3;
    sm->set(id, champ_dwLength, val);

    // Update loop start
    if (sm->get(id, champ_dwStartLoop).dwValue > pos)
        val.dwValue = sm->get(id, champ_dwStartLoop).dwValue - pos;
    else
        val.dwValue = 0;
    sm->set(id, champ_dwStartLoop, val);

    // Update loop end
    if (sm->get(id, champ_dwEndLoop).dwValue > pos)
        val.dwValue = sm->get(id, champ_dwEndLoop).dwValue - pos;
    else
        val.dwValue = 0;
    sm->set(id, champ_dwEndLoop, val);
}
