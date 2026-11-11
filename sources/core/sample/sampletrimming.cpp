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
#include <cmath>

quint32 SampleTrimming::trimStart(SoundfontManager * sm, IdList ids, double thresholdDb)
{
    if (ids.isEmpty())
        return 0;

    // Get the data of all samples and compute the position where the sound starts (the earliest of all channels, so that
    // the sound of no channel is cut)
    quint32 pos = 0;
    QList<QVector<float> > vDataList;
    for (int i = 0; i < ids.count(); i++)
    {
        vDataList << sm->getDataFloat(ids[i]);
        quint32 length = static_cast<quint32>(vDataList[i].size());
        bool looped = SampleUtils::isLooped(sm->get(ids[i], champ_dwStartLoop).dwValue, sm->get(ids[i], champ_dwEndLoop).dwValue, length);
        quint32 start = SampleUtils::findTrimStart(vDataList[i], sm->get(ids[i], champ_dwSampleRate).dwValue, looped, thresholdDb);
        if (i == 0 || start < pos)
            pos = start;
    }
    if (pos == 0)
        return 0;
    foreach (QVector<float> vData, vDataList)
        if (static_cast<quint32>(vData.size()) <= pos + 8)
            return 0; // Not enough data would remain

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

    return pos;
}

QList<quint32> SampleTrimming::trimEnd(SoundfontManager * sm, IdList ids, double thresholdDb)
{
    QList<quint32> removed;
    for (int i = 0; i < ids.count(); i++)
        removed << 0;

    // Last position reaching the threshold among all channels, with the fade-out of this channel. The data of a loop and
    // the 8 values after it are kept
    QList<QVector<float> > vDataList;
    quint32 lastPosition = 0, fadeLength = 0, minFadeStart = 0, maxLength = 0;
    bool found = false;
    for (int i = 0; i < ids.count(); i++)
    {
        vDataList << sm->getDataFloat(ids[i]);
        quint32 length = static_cast<quint32>(vDataList[i].size());
        maxLength = qMax(maxLength, length);
        quint32 loopEnd = sm->get(ids[i], champ_dwEndLoop).dwValue;
        if (SampleUtils::isLooped(sm->get(ids[i], champ_dwStartLoop).dwValue, loopEnd, length))
            minFadeStart = qMax(minFadeStart, loopEnd + 8);

        quint32 sampleRate = sm->get(ids[i], champ_dwSampleRate).dwValue;
        double threshold = std::isnan(thresholdDb) ? SampleUtils::findBlankThresholdAtEnd(vDataList[i], sampleRate) : thresholdDb;
        quint32 position, fade;
        if (SampleUtils::findTrimEnd(vDataList[i], sampleRate, threshold, position, fade))
        {
            if (!found || position > lastPosition || (position == lastPosition && fade > fadeLength))
            {
                lastPosition = position;
                fadeLength = fade;
            }
            found = true;
        }
    }
    if (!found)
        return removed; // Nothing but blank
    quint32 fadeStart = qMax(lastPosition + 1, minFadeStart);
    if (fadeStart >= maxLength)
        return removed; // Sounding until the end

    // Fade-out, the data ending where the values become negligible (below the 16-bit resolution, -90 dB)
    const float negligible = 1.0f / 32768;
    quint32 fadeEnd = qMin(fadeStart + fadeLength, maxLength);
    quint32 end = fadeStart;
    for (int i = 0; i < vDataList.count(); i++)
    {
        SampleUtils::fadeOut(vDataList[i], fadeStart, fadeEnd - fadeStart);
        const float * data = vDataList[i].constData();
        for (quint32 pos = qMin(fadeEnd, static_cast<quint32>(vDataList[i].size())); pos > end; pos--)
        {
            if (std::fabs(data[pos - 1]) >= negligible)
            {
                end = pos;
                break;
            }
        }
    }

    // Update the samples
    for (int i = 0; i < ids.count(); i++)
    {
        EltID id = ids[i];
        quint32 length = static_cast<quint32>(vDataList[i].size());
        if (fadeStart >= length)
            continue;

        // Data
        quint32 newLength = qMin(length, end);
        vDataList[i].resize(static_cast<int>(newLength));
        sm->setData(id, vDataList[i]);
        removed[i] = length - newLength;
        if (removed[i] == 0)
            continue;

        // Update length
        AttributeValue val;
        val.dwValue = newLength;
        sm->set(id, champ_dwLength, val);

        // Loop points (not a loop) beyond the end
        if (sm->get(id, champ_dwStartLoop).dwValue > newLength)
        {
            val.dwValue = newLength;
            sm->set(id, champ_dwStartLoop, val);
        }
        if (sm->get(id, champ_dwEndLoop).dwValue > newLength)
        {
            val.dwValue = newLength;
            sm->set(id, champ_dwEndLoop, val);
        }
    }

    return removed;
}

bool SampleTrimming::hasDataAfterLoop(SoundfontManager * sm, EltID id)
{
    quint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;
    return endLoop != 0 && endLoop + 8 < sm->get(id, champ_dwLength).dwValue;
}

quint32 SampleTrimming::trimToLoop(SoundfontManager * sm, EltID id)
{
    QVector<float> baData = sm->getDataFloat(id);
    quint32 length = static_cast<quint32>(baData.size());

    // Loop
    quint32 startLoop = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;

    // Possibly resize data
    if (endLoop == 0 || endLoop + 8 >= length)
        return 0;

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

    return length - val.dwValue;
}

QMap<int, QList<EltID> > SampleTrimming::getDivisionsBySample(SoundfontManager * sm, int indexSf2)
{
    QMap<int, QList<EltID> > divisions;
    EltID idInst(elementInst, indexSf2);
    foreach (int indexInst, sm->getSiblings(idInst))
    {
        EltID idDiv(elementInstSmpl, indexSf2, indexInst);
        foreach (int indexDiv, sm->getSiblings(idDiv))
        {
            idDiv.indexElt2 = indexDiv;
            divisions[sm->get(idDiv, champ_sampleID).wValue] << idDiv;
        }
    }
    return divisions;
}

void SampleTrimming::updateStartOffsets(SoundfontManager * sm, const QList<EltID> &divisions, quint32 removed)
{
    if (removed == 0)
        return;
    foreach (EltID idDiv, divisions)
        updateOffset(sm, idDiv, champ_startAddrsOffset, champ_startAddrsCoarseOffset, removed, true);
}

void SampleTrimming::updateEndOffsets(SoundfontManager * sm, const QList<EltID> &divisions, quint32 removed)
{
    if (removed == 0)
        return;
    foreach (EltID idDiv, divisions)
        updateOffset(sm, idDiv, champ_endAddrsOffset, champ_endAddrsCoarseOffset, removed, false);
}

void SampleTrimming::updateOffset(SoundfontManager * sm, EltID idDiv, AttributeType champFine, AttributeType champCoarse,
                                  qint64 removed, bool start)
{
    // Current offset, possibly inherited from the global division
    AttributeType champs[2] = {champFine, champCoarse};
    qint64 inherited[2];
    qint64 values[2];
    for (int i = 0; i < 2; i++)
    {
        inherited[i] = getInheritedValue(sm, idDiv, champs[i]).shValue;
        values[i] = sm->isSet(idDiv, champs[i]) ? sm->get(idDiv, champs[i]).shValue : inherited[i];
    }
    qint64 offset = values[0] + 32768 * values[1];

    // A start offset moves the start later (positive), an end offset moves the end earlier (negative): the removed values
    // are subtracted, the offset not going beyond the new start or end
    qint64 newOffset = offset;
    if (start && offset > 0)
        newOffset = qMax(0LL, offset - removed);
    else if (!start && offset < 0)
        newOffset = qMin(0LL, offset + removed);
    if (newOffset == offset)
        return;

    // Split again, a part being set unless it is equal to the inherited value
    qint64 newValues[2] = {newOffset % 32768, newOffset / 32768};
    for (int i = 0; i < 2; i++)
    {
        if (newValues[i] == inherited[i])
        {
            if (sm->isSet(idDiv, champs[i]))
                sm->reset(idDiv, champs[i]);
        }
        else if (newValues[i] != values[i] || !sm->isSet(idDiv, champs[i]))
        {
            AttributeValue value;
            value.shValue = static_cast<qint16>(newValues[i]);
            sm->set(idDiv, champs[i], value);
        }
    }
}

AttributeValue SampleTrimming::getInheritedValue(SoundfontManager * sm, EltID idDiv, AttributeType champ)
{
    EltID idInst(elementInst, idDiv.indexSf2, idDiv.indexElt);
    if (sm->isSet(idInst, champ))
        return sm->get(idInst, champ);
    return Attribute::getDefaultStoredValue(champ, false);
}
