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

#include "toolmonitor.h"
#include "toolmonitor_gui.h"
#include "toolmonitor_parameters.h"
#include "soundfontmanager.h"

ToolMonitor::ToolMonitor() : AbstractTool(new ToolMonitor_parameters(), new ToolMonitor_gui())
{

}

bool ToolMonitor::isCompatible(IdList ids)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (ids.getSelectedIds(elementInst).count() == 1)
    {
        // The instrument must comprise at least one sample
        EltID id = ids.getSelectedIds(elementInst)[0];
        id.typeElement = elementInstSmpl;
        return !sm->getSiblings(id).empty();
    }
    else if (ids.getSelectedIds(elementPrst).count() == 1)
    {
        // The preset must comprise at least one instrument
        EltID id = ids.getSelectedIds(elementPrst)[0];
        id.typeElement = elementPrstInst;
        return !sm->getSiblings(id).empty();
    }

    return false;
}

void ToolMonitor::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);

    Q_UNUSED(sm)
    Q_UNUSED(parent)
    Q_UNUSED(parameters)
}
