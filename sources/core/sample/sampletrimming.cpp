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

#include "sampletrimming.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

void SampleTrimming::trimToLoop(SoundfontManager * sm, EltID id)
{
    QVector<float> baData = sm->getDataFloat(id);

    // Loop
    quint32 startLoop = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;

    // Possibly resize data
    if (endLoop != 0 && endLoop + 8 < static_cast<quint32>(baData.size()))
    {
        // Crop data, the 8 values after the loop end being those after the loop start (interpolation at the loop point)
        if (startLoop < endLoop)
            baData = SampleUtils::loopStep2(std::move(baData), startLoop, endLoop, 0, true);
        else
            baData.resize(endLoop + 8);
        sm->setData(id, baData);

        // Update length
        AttributeValue val;
        val.dwValue = baData.size();
        sm->set(id, champ_dwLength, val);
    }
}

void SampleTrimming::trimStart(SoundfontManager * sm, IdList ids)
{
    if (ids.isEmpty())
        return;

    // Get the data of all samples and compute the position where the sound starts (the earliest of all channels, so that
    // the sound of no channel is cut)
    quint32 pos = 0;
    QList<QVector<float> > vDataList;
    for (int i = 0; i < ids.count(); i++)
    {
        vDataList << sm->getDataFloat(ids[i]);
        quint32 length = static_cast<quint32>(vDataList[i].size());
        bool looped = SampleUtils::isLooped(sm->get(ids[i], champ_dwStartLoop).dwValue, sm->get(ids[i], champ_dwEndLoop).dwValue, length);
        quint32 start = SampleUtils::findTrimStart(vDataList[i], sm->get(ids[i], champ_dwSampleRate).dwValue, looped);
        if (i == 0 || start < pos)
            pos = start;
    }
    if (pos == 0)
        return;
    foreach (QVector<float> vData, vDataList)
        if (static_cast<quint32>(vData.size()) <= pos + 8)
            return; // Not enough data would remain

    // Update the samples
    for (int i = 0; i < ids.count(); i++)
    {
        EltID id = ids[i];

        // Data
        vDataList[i] = SampleUtils::trimStart(std::move(vDataList[i]), pos);
        sm->setData(id, vDataList[i]);

        // Update length
        AttributeValue val;
        val.dwValue = vDataList[i].size();
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
}
