#include "toolunlinksample.h"
#include "soundfontmanager.h"

void ToolUnlinkSample::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);

    // Link sample
    EltID otherId = id;
    otherId.indexElt = sm->get(id, champ_wSampleLink).wValue;

    // Unlink both sample
    unlink(sm, id);
    if (sm->isValid(otherId))
        unlink(sm, otherId);
}

void ToolUnlinkSample::unlink(SoundfontManager * sm, EltID idSample)
{
    // Sample type
    Valeur value;
    SFSampleLink type = sm->get(idSample, champ_sfSampleType).sfLinkValue;
    if (type == leftSample || type == rightSample || type == linkedSample)
    {
        value.sfLinkValue = monoSample;
        sm->set(idSample, champ_sfSampleType, value);
    }
    else if (type == RomLeftSample || type == RomRightSample || type == RomLinkedSample)
    {
        value.sfLinkValue = RomMonoSample;
        sm->set(idSample, champ_sfSampleType, value);
    }

    // Linked sample id
    if (sm->get(idSample, champ_wSampleLink).wValue != 0)
    {
        value.wValue = 0;
        sm->set(idSample, champ_wSampleLink, value);
    }
}
