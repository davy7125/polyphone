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

#include "toolfrequencyfilter_parameters.h"
#include "contextmanager.h"

void ToolFrequencyFilter_parameters::loadConfiguration()
{
    // Curve
    QList<QVariant> listTmp = getToolValue("curve", QList<QVariant>()).toList();
    _curve.resize(listTmp.size());
    for (int i = 0; i < listTmp.size(); i++)
        _curve[i] = listTmp.at(i).toFloat();
}

void ToolFrequencyFilter_parameters::saveConfiguration()
{
    // Curve
    QVariantList listTmp;
    for (int i = 0; i < _curve.size(); i++)
        listTmp << _curve.at(i);
    setToolValue("curve", listTmp);
}
