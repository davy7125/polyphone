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

#include "toolglobalsettings_parameters.h"

void ToolGlobalSettings_parameters::loadConfiguration()
{
    // Pattern
    _instPattern = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "pattern", 0).toInt();
    _prstPattern = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "pattern", 0).toInt();

    // Pattern parameter
    _instParam = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "param", 50.).toDouble();
    _prstParam = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "param", 50.).toDouble();

    // Min / max
    _instMin = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "min", 0.).toDouble();
    _prstMin = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "min", 0.).toDouble();
    _instMax = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "max", 1.).toDouble();
    _prstMax = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "max", 1.).toDouble();
    _instMinX = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "minX", 0).toInt();
    _prstMinX = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "minX", 0).toInt();
    _instMaxX = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "maxX", 127).toInt();
    _prstMaxX = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "maxX", 127).toInt();

    // Curve
    _instValues = getStoredValues(ConfManager::TOOL_TYPE_INSTRUMENT);
    _prstValues = getStoredValues(ConfManager::TOOL_TYPE_PRESET);

    // Modification type
    _instModifType = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "modification", 0).toInt();
    _prstModifType = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "modification", 0).toInt();

    // Attribute to change
    _instAttribute = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "attribute", 0).toInt();
    _prstAttribute = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "attribute", 0).toInt();

    // Min / max velocity
    _instMinVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "minVel", 0).toInt();
    _prstMinVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "minVel", 0).toInt();
    _instMaxVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "maxVel", 127).toInt();
    _prstMaxVel = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "maxVel", 127).toInt();
}

void ToolGlobalSettings_parameters::saveConfiguration()
{
    // Pattern
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "pattern", _instPattern);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "pattern", _prstPattern);

    // Pattern parameter
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "param", _instParam);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "param", _prstParam);

    // Min / max
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "min", _instMin);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "min", _prstMin);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "max", _instMax);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "max", _prstMax);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "minX", _instMinX);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "minX", _prstMinX);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "maxX", _instMaxX);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "maxX", _prstMaxX);

    // Curve
    storeValues(_instValues, ConfManager::TOOL_TYPE_INSTRUMENT);
    storeValues(_prstValues, ConfManager::TOOL_TYPE_PRESET);

    // Modification type
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "modification", _instModifType);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "modification", _prstModifType);

    // Attribute to change
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "attribute", _instAttribute);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "attribute", _prstAttribute);

    // Min / max velocity
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "minVel", _instMinVel);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "minVel", _prstMinVel);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "global", "maxVel", _instMaxVel);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_PRESET, "global", "maxVel", _prstMaxVel);
}

QVector<double> ToolGlobalSettings_parameters::getStoredValues(ConfManager::ToolType toolType)
{
    QList<QVariant> listTmp = ContextManager::configuration()->getToolValue(toolType, "global", "values", QList<QVariant>()).toList();
    QVector<double> vectRet;
    vectRet.resize(listTmp.size());
    for (int i = 0; i < listTmp.size(); i++)
        vectRet[i] = listTmp.at(i).toDouble();
    return vectRet;
}

void ToolGlobalSettings_parameters::storeValues(QVector<double> values, ConfManager::ToolType toolType)
{
    QVariantList listTmp;
    for (int i = 0; i < values.size(); i++)
        listTmp << values.at(i);
    ContextManager::configuration()->setToolValue(toolType, "global", "values", listTmp);
}
