#include "toolchangeattenuation.h"
#include "toolchangeattenuation_gui.h"
#include "toolchangeattenuation_parameters.h"
#include "soundfontmanager.h"
#include <qmath.h>

ToolChangeAttenuation::ToolChangeAttenuation() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                                       new ToolChangeAttenuation_parameters(), new ToolChangeAttenuation_gui())
{

}

void ToolChangeAttenuation::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolChangeAttenuation::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolChangeAttenuation_parameters * params = (ToolChangeAttenuation_parameters *)parameters;

    // Compute the attenuation offset
    int offset = round(10.0 * (_isInst ? params->getInstValue() : params->getPrstValue()));
    if (offset == 0)
        return;

    // Apply the offset on the element
    Valeur val;
    int defaultAttenuation = sm->isSet(id, champ_initialAttenuation) ? sm->get(id, champ_initialAttenuation).shValue : 0;
    val.shValue = limitOffset(defaultAttenuation + offset);
    sm->set(id, champ_initialAttenuation, val);

    // And for each element linked
    EltID idLinked(_isInst ? elementInstSmpl : elementPrstInst, id.indexSf2, id.indexElt);
    foreach (int i, sm->getSiblings(idLinked))
    {
        idLinked.indexElt2 = i;
        if (sm->isSet(idLinked, champ_initialAttenuation))
        {
            val.shValue = limitOffset(sm->get(idLinked, champ_initialAttenuation).shValue + offset);
            sm->set(idLinked, champ_initialAttenuation, val);
        }
    }
}

int ToolChangeAttenuation::limitOffset(int offset)
{
    if (offset > 1440)
        return 1440;
    if (_isInst && offset < 0)
        return 0;
    if (!_isInst && offset < -1440)
        return -1440;
    return offset;
}
