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
    AttributeValue value;
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
