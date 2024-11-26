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

#include "toolglobalsettings_parameters.h"
#include "basetypes.h"

void ToolGlobalSettings_parameters::loadConfiguration()
{
    // Pattern
    _pattern = getToolValue("pattern", 0).toInt();

    // Pattern parameter
    _param = getToolValue("param", 50.).toDouble();

    // Min / max
    _min = getToolValue("min", 0.).toDouble();
    _max = getToolValue("max", 1.).toDouble();
    _minX = getToolValue("minX", 0).toInt();
    _maxX = getToolValue("maxX", 127).toInt();

    // Curve
    _values = getStoredValues();

    // Modification type
    _modifType = getToolValue("modification", 0).toInt();

    // Attribute to change
    _attribute = getToolValue("attribute", champ_initialAttenuation).toInt();

    // Min / max velocity
    _minVel = getToolValue("minVel", 0).toInt();
    _maxVel = getToolValue("maxVel", 127).toInt();
}

void ToolGlobalSettings_parameters::saveConfiguration()
{
    // Pattern
    setToolValue("pattern", _pattern);

    // Pattern parameter
    setToolValue("param", _param);

    // Min / max
    setToolValue("min", _min);
    setToolValue("max", _max);
    setToolValue("minX", _minX);
    setToolValue("maxX", _maxX);

    // Curve
    storeValues(_values);

    // Modification type
    setToolValue("modification", _modifType);

    // Attribute to change
    setToolValue("attribute", _attribute);

    // Min / max velocity
    setToolValue("minVel", _minVel);
    setToolValue("maxVel", _maxVel);
}

QVector<float> ToolGlobalSettings_parameters::getStoredValues()
{
    QList<QVariant> listTmp = getToolValue("values", QList<QVariant>()).toList();
    QVector<float> vectRet;
    vectRet.resize(listTmp.size());
    for (int i = 0; i < listTmp.size(); i++)
        vectRet[i] = listTmp.at(i).toFloat();
    return vectRet;
}

void ToolGlobalSettings_parameters::storeValues(QVector<float> values)
{
    QVariantList listTmp;
    for (int i = 0; i < values.size(); i++)
        listTmp << values.at(i);
    setToolValue("values", listTmp);
}
