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

#include "tooldivisionduplication.h"
#include "tooldivisionduplication_gui.h"
#include "tooldivisionduplication_parameters.h"
#include "duplicationtool.h"
#include "soundfontmanager.h"
#include <qmath.h>

ToolDivisionDuplication::ToolDivisionDuplication() : AbstractToolIterating(QList<ElementType>() << elementInst << elementPrst,
                                                                           new ToolDivisionDuplication_parameters(), new ToolDivisionDuplication_gui())
{

}

void ToolDivisionDuplication::beforeProcess(IdList ids)
{
    _elementsInError.clear();
    _isInst = (ids.count() == 0 || ids[0].typeElement == elementInst || ids[0].typeElement == elementInstSmpl);
}

void ToolDivisionDuplication::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolDivisionDuplication_parameters * params = (ToolDivisionDuplication_parameters *)parameters;

    bool duplicKey = _isInst ? params->getInstDuplicKey() : params->getPrstDuplicKey();
    bool duplicVel = _isInst ? params->getInstDuplicVel() : params->getPrstDuplicVel();
    if (!duplicKey && !duplicVel)
        return;

    // Type of element to duplicate
    EltID divId = id;
    if (_isInst)
        divId.typeElement = elementInstSmpl;
    else
        divId.typeElement = elementPrstInst;

    if (sm->getSiblings(divId).empty())
    {
        // No divisions => error
        _mutex.lock();
        _elementsInError << sm->getQstr(id, champ_name);
        _mutex.unlock();
    }
    else
    {
        // Duplication
        DuplicationTool tool(divId);
        if (duplicKey)
            tool.duplicateByKey();
        if (duplicVel)
            tool.duplicateByVelocity(_isInst ? params->getInstVelocityRanges() : params->getPrstVelocityRanges());
    }
}

QString ToolDivisionDuplication::getWarning()
{
    QString txt = "";

    if (!_elementsInError.empty())
    {
        txt = _isInst ? trUtf8("An instrument comprising no samples is not compatible with this tool:") :
                          trUtf8("A preset comprising no instruments is not compatible with this tool:");
        txt += "<ul>";
        foreach (QString element, _elementsInError)
            txt += "<li>" + element + "</li>";
        txt += "</ul>";
    }

    return txt;
}
