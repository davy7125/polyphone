#include "toolcleanunused.h"
#include "soundfontmanager.h"

void ToolCleanUnused::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)

    _unusedSmpl = 0;
    _unusedInst = 0;
}

void ToolCleanUnused::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    id.typeElement = elementSmpl;
    QList<int> nbSmpl = sm->getSiblings(id);
    id.typeElement = elementInst;
    QList<int> nbInst = sm->getSiblings(id);
    id.typeElement = elementPrst;
    QList<int> nbPrst = sm->getSiblings(id);
    bool smplUsed, instUsed;

    // Delete unused instruments
    foreach (int i, nbInst)
    {
        instUsed = false;

        // We check each preset
        foreach (int j, nbPrst)
        {
            // Made of instruments
            id.indexElt = j;
            id.typeElement = elementPrstInst;
            foreach (int k, sm->getSiblings(id))
            {
                id.indexElt2 = k;
                if (sm->get(id, champ_instrument).wValue == i)
                    instUsed = true;
            }
        }
        if (!instUsed)
        {
            // Deletion of the instrument
            _unusedInst++;
            id.typeElement = elementInst;
            id.indexElt = i;
            int message;
            sm->remove(id, &message);
        }
    }

    // Delete unused samples
    foreach (int i, nbSmpl)
    {
        smplUsed = false;

        // We check each instrument
        foreach (int j, nbInst)
        {
            // Made of samples
            id.indexElt = j;
            id.typeElement = elementInstSmpl;
            foreach (int k, sm->getSiblings(id))
            {
                id.indexElt2 = k;
                if (sm->get(id, champ_sampleID).wValue == i)
                    smplUsed = true;
            }
        }

        if (!smplUsed)
        {
            // Deletion of the sample
            _unusedSmpl++;
            id.typeElement = elementSmpl;
            id.indexElt = i;
            int message;
            sm->remove(id, &message);
        }
    }
}

QString ToolCleanUnused::getConfirmation()
{
    QString qStr;

    if (_unusedSmpl < 2)
        qStr = QString::number(_unusedSmpl) + trUtf8(" échantillon et ");
    else
        qStr = QString::number(_unusedSmpl) + trUtf8(" échantillons et ");
    if (_unusedInst < 2)
        qStr += QString::number(_unusedInst) + trUtf8(" instrument ont été supprimés.");
    else
        qStr += QString::number(_unusedInst) + trUtf8(" instruments ont été supprimés.");

    return qStr;
}
