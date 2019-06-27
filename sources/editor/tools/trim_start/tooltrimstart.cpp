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

#include "tooltrimstart.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

void ToolTrimStart::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
    Q_UNUSED(sm);

    trim(id);
}

void ToolTrimStart::trim(EltID id)
{
    // Get the sample data and trim it
    SoundfontManager * sm = SoundfontManager::getInstance();
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 pos = 0;
    baData = SampleUtils::enleveBlanc(baData, 0.001, 24, pos);
    if (pos == 0)
        return;

    // Update data
    sm->set(id, champ_sampleDataFull24, baData);

    // Update length
    AttributeValue val;
    val.dwValue = baData.size()/3;
    sm->set(id, champ_dwLength, val);

    // Update loop start
    if (sm->get(id, champ_dwStartLoop).dwValue > pos)
        val.dwValue = sm->get(id, champ_dwStartLoop).dwValue - pos;
    else
        val.dwValue = 0;
    sm->set(id, champ_dwStartLoop, val);

    // Update loop end
    if (sm->get(id, champ_dwEndLoop).dwValue > pos)
        val.dwValue = sm->get(id, champ_dwEndLoop).dwValue - pos;
    else
        val.dwValue = 0;
    sm->set(id, champ_dwEndLoop, val);
}
