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

#include "tooltrimend.h"
#include "soundfontmanager.h"

void ToolTrimEnd::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
    Q_UNUSED(sm);

    trim(id);
}

void ToolTrimEnd::trim(EltID id)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    QVector<float> baData = sm->getData(id);

    // End of loop
    qint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;

    // Possibly resize data
    if (endLoop != 0 && endLoop + 8 < baData.size())
    {
        // Crop data
        baData.resize(endLoop + 8);
        sm->set(id, baData);

        // Update length
        AttributeValue val;
        val.dwValue = baData.size();
        sm->set(id, champ_dwLength, val);
    }
}
