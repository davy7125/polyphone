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

#include "toolsoundspatialization.h"
#include "toolsoundspatialization_gui.h"
#include "toolsoundspatialization_parameters.h"
#include "soundfontmanager.h"

ToolSoundSpatialization::ToolSoundSpatialization() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                     new ToolSoundSpatialization_parameters(), new ToolSoundSpatialization_gui())
{

}

void ToolSoundSpatialization::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolSoundSpatialization::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolSoundSpatialization_parameters * params = (ToolSoundSpatialization_parameters *)parameters;

    // Liste des éléments liés avec leur lien (stéréo) le cas échéant
    QList<EltID> list1;
    QList<AttributeValue> listRange;
    QList<EltID> list2;
    AttributeValue amount;
    bool found;
    int pos = 0;
    int noteMin = 128;
    int noteMax = 0;
    id.typeElement = _isInst ? elementInstSmpl : elementPrstInst;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt2 = i;
        if (sm->isSet(id, champ_keyRange))
            amount = sm->get(id, champ_keyRange);
        else
        {
            amount.rValue.byLo = 0;
            amount.rValue.byHi = 127;
        }
        if (amount.rValue.byLo < noteMin) noteMin = amount.rValue.byLo;
        if (amount.rValue.byHi > noteMax) noteMax = amount.rValue.byHi;

        // Recherche d'une note liée ayant la même étendue sur le clavier
        found = false;
        if (_isInst)
        {
            pos = 0;
            while (pos < list1.size() && !found)
            {
                if (amount.rValue.byHi == listRange.at(pos).rValue.byHi &&
                        amount.rValue.byLo == listRange.at(pos).rValue.byLo &&
                        list2.at(pos).indexElt2 == -1)
                {
                    // Les samples sont-ils liés ?
                    EltID idSmpl1 = id;
                    idSmpl1.indexElt = sm->get(id, champ_sampleID).wValue;
                    idSmpl1.typeElement = elementSmpl;
                    EltID idSmpl2 = list1.at(pos);
                    idSmpl2.indexElt = sm->get(idSmpl2, champ_sampleID).wValue;
                    idSmpl2.typeElement = elementSmpl;
                    if (idSmpl1.indexElt == sm->get(idSmpl2, champ_wSampleLink).wValue)
                    {
                        SFSampleLink type1 = sm->get(idSmpl1, champ_sfSampleType).sfLinkValue;
                        SFSampleLink type2 = sm->get(idSmpl2, champ_sfSampleType).sfLinkValue;
                        if (((type1 == rightSample || type1 == RomRightSample) && (type2 == leftSample || type2 == RomLeftSample)) ||
                                ((type1 == leftSample || type1 == RomLeftSample) && (type2 == rightSample || type2 == RomRightSample)))
                            found = true;
                    }
                }
                if (!found)
                    pos++;
            }
        }
        if (found)
        {
            // Lien
            list2[pos] = id;
        }
        else
        {
            // Ajout à liste 1
            list1.append(id);
            // Element nul dans liste 2
            id.indexElt2 = -1;
            list2.append(id);
            // Etendue
            listRange.append(amount);
        }
    }

    // Spatialisation
    double pan = 0;
    int note = 64;
    EltID id2, id3;
    int sampleG;
    AttributeValue val;
    for (int i = 0; i < list1.size(); i++)
    {
        note = (listRange.at(i).rValue.byLo + listRange.at(i).rValue.byHi) / 2;
        pan = params->getMapPan().value(note);

        // Lien ?
        if (list2.at(i).indexElt2 == -1)
        {
            // pas de lien
            val.shValue = 1000 * pan - 500;
            sm->set(list1.at(i), champ_pan, val);
        }
        else
        {
            // Quel sample est à gauche ?
            sampleG = 0;
            // Sample correspondant 1
            id2 = list1.at(i);
            id2.indexElt = sm->get(id2, champ_sampleID).wValue;
            id2.typeElement = elementSmpl;
            SFSampleLink type1 = sm->get(id2, champ_sfSampleType).sfLinkValue;
            // Sample correspondant 2
            id3 = list2.at(i);
            id3.indexElt = sm->get(id3, champ_sampleID).wValue;
            id3.typeElement = elementSmpl;
            SFSampleLink type2 = sm->get(id3, champ_sfSampleType).sfLinkValue;
            if ((type1 == leftSample || type1 == RomLeftSample) &&
                    type2 != leftSample && type2 != RomLeftSample)
            {
                sampleG = 0;
            }
            else if ((type1 == rightSample || type1 == RomRightSample) &&
                     type2 != rightSample && type2 != RomRightSample)
            {
                sampleG = 1;
            }
            else
            {
                if (sm->get(list1.at(i), champ_pan).shValue <
                        sm->get(list2.at(i), champ_pan).shValue)
                    sampleG = 0;
                else if (sm->get(list1.at(i), champ_pan).shValue >
                         sm->get(list2.at(i), champ_pan).shValue)
                    sampleG = 1;
            }
            if (sampleG == 0)
            {
                // Inversion
                id2 = list1.at(i);
                list1[i] = list2.at(i);
                list2[i] = id2;
            }
            // lien
            if (pan < 0.5)
            {
                // Gauche
                val.shValue = -500;
                sm->set(list1.at(i), champ_pan, val);
                // Droite
                val.shValue = 2000 * pan - 500;
                sm->set(list2.at(i), champ_pan, val);
            }
            else
            {
                // Gauche
                val.shValue = 2000 * pan - 1500;
                sm->set(list1.at(i), champ_pan, val);
                // Droite
                val.shValue = 500;
                sm->set(list2.at(i), champ_pan, val);
            }
        }
    }
}
