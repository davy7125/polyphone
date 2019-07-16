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

#include "tooldefaultmod.h"
#include "tooldefaultmod_gui.h"
#include "tooldefaultmod_parameters.h"
#include "soundfontmanager.h"

ToolDefaultMod::ToolDefaultMod() : AbstractToolIterating (elementInst, new ToolDefaultMod_parameters(), new ToolDefaultMod_gui())
{

}

void ToolDefaultMod::process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters)
{
    // Get the parameters
    ToolDefaultMod_parameters * params = dynamic_cast<ToolDefaultMod_parameters *>(parameters);
    QList<ModulatorData> mods = params->getModulators();

    // For each element
    id.typeElement = elementInstMod;
    AttributeValue value;
    foreach (ModulatorData mod, mods)
    {
        // Create a modulator
        id.indexMod = sm->add(id);

        // Configure it
        value.sfModValue = mod.srcOper;
        sm->set(id, champ_sfModSrcOper, value);

        value.sfModValue = mod.amtSrcOper;
        sm->set(id, champ_sfModAmtSrcOper, value);

        value.shValue = mod.amount;
        sm->set(id, champ_modAmount, value);

        value.sfTransValue = mod.transOper;
        sm->set(id, champ_sfModTransOper, value);

        value.wValue = mod.destOper;
        sm->set(id, champ_sfModDestOper, value);
    }
}
