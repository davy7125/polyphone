/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

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
    AttributeValue val;
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
