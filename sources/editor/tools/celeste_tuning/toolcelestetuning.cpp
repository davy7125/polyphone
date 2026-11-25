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

#include "toolcelestetuning.h"
#include "toolcelestetuning_gui.h"
#include "toolcelestetuning_parameters.h"
#include "soundfontmanager.h"
#include "attribute.h"
#include <qmath.h>

ToolCelesteTuning::ToolCelesteTuning() : AbstractToolIterating(elementInst, new ToolCelesteTuning_parameters(), new ToolCelesteTuning_gui())
{

}

void ToolCelesteTuning::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolCelesteTuning_parameters * params = dynamic_cast<ToolCelesteTuning_parameters *>(parameters);
    EltID idInst = id;

    // Value of a division, or of the global division if not set
    auto effective = [&](EltID idDiv, AttributeType champ) -> AttributeValue {
        if (sm->isSet(idDiv, champ))
            return sm->get(idDiv, champ);
        if (sm->isSet(idInst, champ))
            return sm->get(idInst, champ);
        return AttributeBase::getDefaultStoredValue(champ, false);
    };

    // For each linked element
    id.typeElement = elementInstSmpl;
    foreach (int i, sm->getSiblings(id))
    {
        id.indexElt2 = i;

        // Middle of the key range on the keyboard
        double keyMid;
        int keynum = effective(id, champ_keynum).shValue;
        if (keynum >= 0)
            keyMid = keynum;
        else
        {
            RangesType keyRange = effective(id, champ_keyRange).rValue;
            keyMid = 0.5 * (keyRange.byLo + keyRange.byHi);
        }

        // Note heard at this key
        EltID idSmpl(elementSmpl, id.indexSf2, sm->get(id, champ_sampleID).wValue);
        int samplePitch = sm->get(idSmpl, champ_byOriginalPitch).bValue;
        int rootKey = effective(id, champ_overridingRootKey).shValue;
        if (rootKey < 0)
            rootKey = samplePitch;
        double noteMid = samplePitch + 0.01 * effective(id, champ_scaleTuning).shValue * (keyMid - rootKey) +
                         effective(id, champ_coarseTune).shValue;
        double frequency = 440. * qPow(2., (noteMid - 69.) / 12.);

        // Number of beats per second with an instrument tuned normally, the coefficient dividing it at each octave above
        double bps = params->getBaseFrequency() * qPow(params->getCoefficient(), (60. - keyMid) / 12.);
        if (bps < -30)
            bps = -30;
        else if (bps > 30)
            bps = 30;

        // The fundamentals beat at the frequency difference: f + bps
        int fineTune = 0;
        if (bps > 0)
            fineTune = qBound(1, qRound(1200. * qLn(1. + bps / frequency) / qLn(2.)), 99);
        else if (bps < 0)
            fineTune = (frequency + bps > 0) ? qBound(-99, qRound(1200. * qLn(1. + bps / frequency) / qLn(2.)), -1) : -99;

        // Modification instSmpl
        if (sm->get(id, champ_fineTune).shValue != fineTune)
        {
            AttributeValue val;
            val.shValue = fineTune;
            sm->set(id, champ_fineTune, val);
        }
    }
}
