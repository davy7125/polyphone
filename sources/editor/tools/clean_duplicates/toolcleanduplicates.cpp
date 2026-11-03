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

#include "toolcleanduplicates.h"
#include "soundfontmanager.h"

void ToolCleanDuplicates::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)

    _duplicateSmpl = 0;
}

void ToolCleanDuplicates::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)

    // Each duplicate is replaced by the first identical sample
    QMap<int, int> duplicates = sm->getDuplicateSamples(id.indexSf2);
    QMap<int, int> replacements;
    foreach (int i, duplicates.keys())
    {
        int original = duplicates[i];
        while (duplicates.contains(original))
            original = duplicates[original];
        replacements[i] = original;
    }

    // Update the instruments using the duplicates
    id.typeElement = elementInst;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt = i;
        id.typeElement = elementInstSmpl;
        foreach (int j, sm->getSiblings(id))
        {
            id.indexElt2 = j;
            int smplIndex = sm->get(id, champ_sampleID).wValue;
            if (replacements.contains(smplIndex))
            {
                AttributeValue value;
                value.wValue = static_cast<quint16>(replacements[smplIndex]);
                sm->set(id, champ_sampleID, value);
            }
        }
        id.typeElement = elementInst;
    }

    // Delete the duplicates, now unused
    id.typeElement = elementSmpl;
    foreach (int i, replacements.keys())
    {
        id.indexElt = i;
        sm->remove(id);
        _duplicateSmpl++;
    }
}

QString ToolCleanDuplicates::getConfirmation()
{
    return tr("%n duplicate sample(s) have been deleted.", "", _duplicateSmpl);
}
