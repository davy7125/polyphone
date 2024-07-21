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

#include "toolfasteditsmpl.h"
#include "toolfasteditsmpl_gui.h"
#include "toolfasteditsmpl_parameters.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolFastEditSmpl::ToolFastEditSmpl() : AbstractToolIterating(elementSmpl, new ToolFastEditSmpl_parameters(), new ToolFastEditSmpl_gui())
{

}

void ToolFastEditSmpl::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolFastEditSmpl_parameters * params = dynamic_cast<ToolFastEditSmpl_parameters *>(parameters);

    // Parameter
    AttributeType param = static_cast<AttributeType>(params->getParameter());

    // Current value of the parameter
    AttributeValue initialVal = sm->get(id, param);
    double dVal = Attribute::toRealValue(param, false, initialVal);

    // Apply modification
    if (params->getMode() == 1)
        dVal *= params->getMultiplyValue();
    else
        dVal += params->getAddValue();

    // Limit the result
    switch (param)
    {
    case champ_byOriginalPitch:
        if (dVal < 0)
            dVal = 0;
        else if (dVal > 127)
            dVal = 127;
        break;
    case champ_chPitchCorrection:
        if (dVal < -99)
            dVal = -99;
        else if (dVal > 99)
            dVal = 99;
        break;
    case champ_dwStartLoop: case champ_dwEndLoop:
        if (dVal < 0)
            dVal = 0;
        else if (dVal > 4294967295)
            dVal = 4294967295;
        break;
    default:
        // Do nothing more
        return;
    }

    // Get the final attribute value
    AttributeValue finalVal = Attribute::fromRealValue(param, false, dVal);

    // Update the parameter value if different
    if (finalVal.dwValue != initialVal.dwValue)
        sm->set(id, param, finalVal);
}
