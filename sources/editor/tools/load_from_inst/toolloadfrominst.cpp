/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "toolloadfrominst.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolLoadFromInst::ToolLoadFromInst() : AbstractToolOneStep(nullptr, nullptr)
{

}

bool ToolLoadFromInst::isCompatible(IdList ids)
{
    return !ids.getSelectedIds(elementSmpl).empty();
}

void ToolLoadFromInst::process(SoundfontManager * sm, IdList ids, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)
    if (ids.isEmpty())
        return;

    // Scan all instruments for the overriding root keys
    QMap<int, int> rootKeyFromInst;
    EltID idInst(elementInst, ids[0].indexSf2);
    foreach (int i, sm->getSiblings(idInst))
    {
        // Current instrument
        idInst.indexElt = i;

        // Load the overriding root key from the global division, if set
        int rootKeyGlobalValue = -1;
        if (sm->isSet(idInst, champ_overridingRootKey))
            rootKeyGlobalValue = sm->get(idInst, champ_overridingRootKey).wValue;

        // Browse all divisions
        EltID idInstSmpl = idInst;
        idInstSmpl.typeElement = elementInstSmpl;
        foreach (int j, sm->getSiblings(idInstSmpl))
        {
            // Current instrument division
            idInstSmpl.indexElt2 = j;

            int rootKey = -1;
            if (sm->isSet(idInstSmpl, champ_overridingRootKey))
                rootKey = sm->get(idInstSmpl, champ_overridingRootKey).wValue;
            if (rootKey == -1)
                rootKey = rootKeyGlobalValue;

            if (rootKey != -1)
            {
                // A root key is defined at the instrument level for a sample
                int sampleId = sm->get(idInstSmpl, champ_sampleID).wValue;
                if (rootKeyFromInst.contains(sampleId) && rootKeyFromInst[sampleId] != rootKey)
                {
                    // Another root key is already specified
                    rootKeyFromInst[sampleId] = -1;
                }
                else
                    rootKeyFromInst[sampleId] = rootKey;
            }
        }
    }

    // Load the root keys at the sample level
    foreach (EltID id, ids)
    {
        if (id.typeElement != elementSmpl)
            continue;

        if (rootKeyFromInst.contains(id.indexElt))
        {
            int rootKeyToLoad = rootKeyFromInst[id.indexElt];
            if (rootKeyToLoad == -1)
                _smplIdsWithSeveralValues << id.indexElt;
            else
            {
                _smplIdsWithOneValue << id.indexElt;

                // Change the sample root key
                AttributeValue val;
                val.wValue = rootKeyToLoad;
                sm->set(id, champ_byOriginalPitch, val);
            }
        }
        else
            _smplIdsWithNoValues << id.indexElt;
    }
}

QString ToolLoadFromInst::getConfirmation()
{
    return _smplIdsWithOneValue.count() > 0 ?
                tr("Successfully retrieved the root key of %n sample(s) from the instruments.", "", _smplIdsWithOneValue.count()) : "";
}

QString ToolLoadFromInst::getWarning()
{
    return _smplIdsWithOneValue.count() == 0 ? tr("No root keys could have been retrieved from the instruments.") : "";
}
