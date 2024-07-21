/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "grandorgueswitch.h"
#include "grandorguedatathrough.h"
#include "soundfontmanager.h"
#include "grandorguestop.h"

GrandOrgueSwitch::GrandOrgueSwitch(GrandOrgueDataThrough * godt, int id) :
    _godt(godt),
    _id(id)
{

}

void GrandOrgueSwitch::readData(QString key, QString value)
{
    _properties[key] = value;
}


void GrandOrgueSwitch::preProcess()
{

}

void GrandOrgueSwitch::process(SoundfontManager * sm, int sf2Index, QMap<int, GrandOrgueStop *> &stops, QMap<int, GrandOrgueRank *> &ranks)
{
    // Is this switch displayed?
    if (_properties.contains("displayed") && _properties["displayed"].toLower() == "n")
        return;

    // At least one instrument directly triggered by this switch?
    bool atLeastOne = false;
    foreach (GrandOrgueStop * stop, stops.values())
    {
        if (stop->isTriggeredByThisSwitch(_id))
        {
            atLeastOne = true;
            break;
        }
    }
    if (!atLeastOne)
        return;

    // Create a new preset
    EltID idPrst(elementPrst, sf2Index);
    idPrst.typeElement = elementPrst;
    idPrst.indexElt = sm->add(idPrst);

    // Name
    sm->set(idPrst, champ_name, _properties.contains("name") ? _properties["name"] : QObject::tr("untitled"));

    // Bank and preset numbers
    int bank, preset;
    _godt->getNextBankPreset(bank, preset);
    AttributeValue value;
    value.wValue = bank;
    sm->set(idPrst, champ_wBank, value);
    value.wValue = preset;
    sm->set(idPrst, champ_wPreset, value);

    // Search all stops triggered by this switch
    foreach (GrandOrgueStop * stop, stops.values())
    {
        if (stop->isTriggeredByThisSwitch(_id))
        {
            stop->process(sm, sf2Index, ranks, idPrst.indexElt);
        }
    }
}
