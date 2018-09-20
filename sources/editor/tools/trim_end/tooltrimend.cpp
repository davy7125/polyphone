#include "trim_end/tooltrimend.h"
#include "soundfontmanager.h"

void ToolTrimEnd::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);

    // Number of values to remove
    qint32 size = baData.size() / 3;
    qint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;
    if (endLoop == 0)
        return;
    if (endLoop < size - 8)
    {
        qint32 pos = size - (8 + endLoop);

        // Crop data
        baData = baData.left(baData.size() - 3 * pos);
        sm->set(id, champ_sampleDataFull24, baData);

        // Update length
        Valeur val;
        val.dwValue = baData.size() / 3;
        sm->set(id, champ_dwLength, val);
    }
}
