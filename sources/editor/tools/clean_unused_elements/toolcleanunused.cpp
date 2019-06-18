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
    return trUtf8("%1 sample(s) and %2 instrument(s) have been deleted.").arg(_unusedSmpl).arg(_unusedInst);
}
