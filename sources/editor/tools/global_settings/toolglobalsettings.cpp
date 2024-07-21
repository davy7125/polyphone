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

#include "toolglobalsettings.h"
#include "toolglobalsettings_gui.h"
#include "toolglobalsettings_parameters.h"
#include "soundfontmanager.h"
#include <qmath.h>
#include "utils.h"

ToolGlobalSettings::ToolGlobalSettings() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                                 new ToolGlobalSettings_parameters(), new ToolGlobalSettings_gui())
{

}

void ToolGlobalSettings::beforeProcess(IdList ids)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolGlobalSettings::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolGlobalSettings_parameters * params = static_cast<ToolGlobalSettings_parameters *>(parameters);

    // Format data
    QVector<double> dValues = (_isInst ? params->getInstValues() : params->getPrstValues());
    double dMin = (_isInst ? params->getInstMin() : params->getPrstMin());
    double dMax = (_isInst ? params->getInstMax() : params->getPrstMax());
    for (int i = 0; i < dValues.size(); i++)
        dValues[i] = dValues.at(i) * (dMax - dMin) + dMin;

    // Velocity range
    int velMin = (_isInst ? params->getInstMinVel() : params->getPrstMinVel());
    int velMax = (_isInst ? params->getInstMaxVel() : params->getPrstMaxVel());

    // Update all elements in id
    EltID idLinked(_isInst ? elementInstSmpl : elementPrstInst, id.indexSf2, id.indexElt);
    foreach (int i, sm->getSiblings(idLinked))
    {
        idLinked.indexElt2 = i;

        // Is the velocity range included in [velMin;velMax]?
        int currentVelMin = 0;
        int currentVelMax = 127;
        if (sm->isSet(idLinked, champ_velRange))
        {
            currentVelMin = sm->get(idLinked, champ_velRange).rValue.byLo;
            currentVelMax = sm->get(idLinked, champ_velRange).rValue.byHi;
        }
        else if (sm->isSet(id, champ_velRange))
        {
            currentVelMin = sm->get(id, champ_velRange).rValue.byLo;
            currentVelMax = sm->get(id, champ_velRange).rValue.byHi;
        }
        if (currentVelMin > currentVelMax)
        {
            int tmp = currentVelMin;
            currentVelMin = currentVelMax;
            currentVelMax = tmp;
        }

        if (currentVelMin >= velMin && currentVelMax <= velMax)
        {
            // Position of the closest point to the middle of the range
            int pos = Utils::round32(0.5 * (sm->get(idLinked, champ_keyRange).rValue.byLo + sm->get(idLinked, champ_keyRange).rValue.byHi) * dValues.size() / 127.);
            if (pos < 0)
                pos = 0;
            else if (pos >= dValues.size())
                pos = dValues.size() - 1;

            // Attribute to change
            AttributeType champ = static_cast<AttributeType>(_isInst ? params->getInstAttribute() : params->getPrstAttribute());

            // Current value
            AttributeValue value;
            if (sm->isSet(idLinked, champ))
                value = sm->get(idLinked, champ);
            else if (sm->isSet(id, champ)) // Global div
                value = sm->get(id, champ);
            else // Default value
                value = Attribute::getDefaultStoredValue(champ, !_isInst);

            // Apply the change
            if (champ == champ_keyRange || champ == champ_velRange)
            {
                // Separately process left and right limits
                RangesType range = value.rValue;

                int lower = static_cast<int>(range.byLo);
                int upper = static_cast<int>(range.byHi);
                switch (_isInst ? params->getInstModifType() : params->getPrstModifType())
                {
                case 0: // Addition
                    lower = Utils::round32(dValues.at(pos) + lower);
                    upper = Utils::round32(dValues.at(pos) + upper);
                    break;
                case 1: // Multiplication
                    lower = Utils::round32(dValues.at(pos) * lower);
                    upper = Utils::round32(dValues.at(pos) * upper);
                    break;
                case 2: // Replacement
                    lower = Utils::round32(dValues.at(pos));
                    upper = Utils::round32(dValues.at(pos));
                    break;
                }

                // Limit
                if (lower < 0)
                    lower = 0;
                if (upper < 0)
                    upper = 0;
                if (lower > 127)
                    lower = 127;
                if (upper > 127)
                    upper = 127;

                // Possibly reorder
                if (lower > upper)
                {
                    int tmp = lower;
                    lower = upper;
                    upper = tmp;
                }

                // Save the range
                AttributeValue val;
                val.rValue.byLo = static_cast<quint8>(lower);
                val.rValue.byHi = static_cast<quint8>(upper);
                sm->set(idLinked, champ, val);
            }
            else
            {
                // Compute the new real value
                double amount = Attribute::toRealValue(champ, !_isInst, value);
                switch (_isInst ? params->getInstModifType() : params->getPrstModifType())
                {
                case 0: // Addition
                    amount += dValues.at(pos);
                    break;
                case 1: // Multiplication
                    amount *= dValues.at(pos);
                    break;
                case 2: // Replacement
                    amount = dValues.at(pos);
                    break;
                }
                sm->set(idLinked, champ, Attribute::fromRealValue(champ, !_isInst, amount));
            }
        }
    }
}
