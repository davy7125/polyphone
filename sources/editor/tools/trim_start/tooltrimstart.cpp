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

#include "tooltrimstart.h"
#include "soundfontmanager.h"
#include "sampletrimming.h"

void ToolTrimStart::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _processedSamples.clear();
}

void ToolTrimStart::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
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

        // The sample may be linked
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
    SampleTrimming::trimStart(sm, ids);
}
