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

#include "toolcelestetuning.h"
#include "toolcelestetuning_gui.h"
#include "toolcelestetuning_parameters.h"
#include "soundfontmanager.h"

ToolCelesteTuning::ToolCelesteTuning() : AbstractToolIterating(elementInst, new ToolCelesteTuning_parameters(), new ToolCelesteTuning_gui())
{

}

void ToolCelesteTuning::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolCelesteTuning_parameters * params = dynamic_cast<ToolCelesteTuning_parameters *>(parameters);

    // For each linked element
    id.typeElement = elementInstSmpl;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt2 = i;

        // Find the corresponding sample
        EltID idSmpl = id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = sm->get(id, champ_sampleID).wValue;

        // Root key of the sample
        int numNoteSmpl = sm->get(idSmpl, champ_byOriginalPitch).bValue;

        // Corresponding key on the keyboard
        int numBase = numNoteSmpl;
        if (sm->isSet(id, champ_overridingRootKey))
            numBase = sm->get(id, champ_overridingRootKey).wValue;

        // Key range on the keyboard
        int numBas = 0;
        int numHaut = 0;
        if (sm->isSet(id, champ_keynum))
        {
            numBas = numNoteSmpl + sm->get(id, champ_keynum).wValue - numBase;
            numHaut = numBas;
        }
        else
        {
            numBas = numNoteSmpl + sm->get(id, champ_keyRange).rValue.byLo - numBase;
            numHaut = numNoteSmpl + sm->get(id, champ_keyRange).rValue.byHi - numBase;
        }

        // Compute beating frequency
        double keyMoy = (double)(sm->get(id, champ_keyRange).rValue.byHi +
                                 sm->get(id, champ_keyRange).rValue.byLo) / 2.;
        double bps = params->getBaseFrequency() * qPow(params->getCoefficient(), (60. - keyMoy) / 12.);
        if (bps < -30)
            bps = -30;
        else if (bps > 30)
            bps = 30;

        // Middle key in the range
        double noteMoy = (double)(numBas + numHaut) / 2;

        // Calcul du désaccordage, passage en frequence
        double freqMoy = qPow(2., (noteMoy + 36.3763) / 12);

        // Ajout du désaccordage
        // - octave ondulante : division par 2 de bps
        // - diminution désaccordage vers les graves
        // - accentuation désaccordage vers les aigus
        double freqMod = freqMoy + 1.2 * qPow(2., (noteMoy - 60)/30) * bps / 2;

        // Retour en pitch
        double noteMod = 12 * qLn(freqMod) / 0.69314718056 - 36.3763;

        // Décalage
        int decalage = ceil(100 * (noteMod - noteMoy) - 0.5);
        if (bps > 0)
        {
            if (decalage < 1)
                decalage = 1;
            else if (decalage > 50)
                decalage = 50;
        }
        else if (bps < 0)
        {
            if (decalage < -50)
                decalage = -50;
            else if (decalage > -1)
                decalage = -1;
        }

        // Modification instSmpl
        if (sm->get(id, champ_fineTune).shValue != decalage)
        {
            AttributeValue val;
            val.shValue = decalage;
            sm->set(id, champ_fineTune, val);
        }
    }
}

