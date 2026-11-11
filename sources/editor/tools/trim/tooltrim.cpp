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

#include "tooltrim.h"
#include "tooltrim_gui.h"
#include "tooltrim_parameters.h"
#include "soundfontmanager.h"
#include "sampletrimming.h"

ToolTrim::ToolTrim() : AbstractToolIterating(elementSmpl, new ToolTrim_parameters(), new ToolTrim_gui()) {}

void ToolTrim::beforeProcess(IdList ids)
{
    _processedSamples.clear();
    _keptAfterLoop.clear();
    _divisions.clear();
    _notContinuouslyLooped.clear();
    if (ids.isEmpty())
        return;

    // Divisions using the samples, and samples whose loop is not always played continuously (the data after the loop
    // end is then heard)
    SoundfontManager * sm = SoundfontManager::getInstance();
    _divisions = SampleTrimming::getDivisionsBySample(sm, ids[0].indexSf2);
    for (auto it = _divisions.constBegin(); it != _divisions.constEnd(); ++it)
    {
        foreach (EltID idDiv, it.value())
        {
            int mode = 0;
            EltID idInst(elementInst, idDiv.indexSf2, idDiv.indexElt);
            if (sm->isSet(idDiv, champ_sampleModes))
                mode = sm->get(idDiv, champ_sampleModes).wValue;
            else if (sm->isSet(idInst, champ_sampleModes))
                mode = sm->get(idInst, champ_sampleModes).wValue;
            if (mode != 1)
            {
                _notContinuouslyLooped << it.key();
                break;
            }
        }
    }
}

void ToolTrim::process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters)
{
    ToolTrim_parameters * params = dynamic_cast<ToolTrim_parameters *>(parameters);
    bool withLink = false;
    EltID id2 = id;

    // Sample already processed?
    _mutex.lock();
    if (_processedSamples.contains(id))
    {
        _mutex.unlock();
        return;
    }
    else
    {
        _processedSamples << id;

        // The linked sample is processed at the same time if it is also selected
        SFSampleLink typeLien = sm->get(id, champ_sfSampleType).sfLinkValue;
        if (typeLien != monoSample && typeLien != RomMonoSample)
        {
            id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
            if (this->isProcessed(id2))
            {
                withLink = true;
                _processedSamples << id2;
            }
        }
    }
    _mutex.unlock();

    // Elements to process
    IdList ids(id);
    if (withLink)
        ids << id2;
    double threshold = params->getAutomaticThreshold() ? SampleTrimming::AUTOMATIC_THRESHOLD : params->getThreshold();

    // End (first: the noise before the sound is a reference for the blank at the end)
    switch (params->getEndMode())
    {
    case ToolTrim_parameters::END_LOOP:
        foreach (EltID idSmpl, ids)
        {
            if (!SampleTrimming::hasDataAfterLoop(sm, idSmpl))
                continue;

            // Data heard after the loop end in an instrument
            if (_notContinuouslyLooped.contains(idSmpl.indexElt))
            {
                _mutex.lock();
                _keptAfterLoop << sm->getQstr(idSmpl, champ_name);
                _mutex.unlock();
                continue;
            }

            quint32 removed = SampleTrimming::trimToLoop(sm, idSmpl);
            SampleTrimming::updateEndOffsets(sm, _divisions.value(idSmpl.indexElt), removed);
        }
        break;
    case ToolTrim_parameters::END_BLANK: {
        QList<quint32> removed = SampleTrimming::trimEnd(sm, ids, threshold);
        for (int i = 0; i < ids.count(); i++)
            SampleTrimming::updateEndOffsets(sm, _divisions.value(ids[i].indexElt), removed[i]);
    } break;
    default:
        break;
    }

    // Start
    if (params->getStart())
    {
        quint32 removed = SampleTrimming::trimStart(sm, ids, threshold);
        foreach (EltID idSmpl, ids)
            SampleTrimming::updateStartOffsets(sm, _divisions.value(idSmpl.indexElt), removed);
    }
}

QString ToolTrim::getWarning()
{
    if (_keptAfterLoop.isEmpty())
        return "";

    _keptAfterLoop.sort(Qt::CaseInsensitive);
    QString txt = tr("The data after the end of the loop has been kept in the following samples, because at least one "
                     "instrument doesn't play their loop continuously:") + "<ul>";
    foreach (QString name, _keptAfterLoop)
        txt += "<li>" + name.toHtmlEscaped() + "</li>";
    return txt + "</ul>";
}
