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

#include "toolloopcrossfade.h"
#include "toolloopcrossfade_gui.h"
#include "toolloopcrossfade_parameters.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolLoopCrossfade::ToolLoopCrossfade() : AbstractToolIterating(elementSmpl, new ToolLoopCrossfade_parameters(), new ToolLoopCrossfade_gui())
{

}

void ToolLoopCrossfade::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    ToolLoopCrossfade_parameters * params = dynamic_cast<ToolLoopCrossfade_parameters *>(parameters);

    // Get sample data
    QVector<float> vData = sm->getDataFloat(id);
    quint32 sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
    quint32 loopStart = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 loopEnd = sm->get(id, champ_dwEndLoop).dwValue;
    if (loopStart >= loopEnd)
        return;

    // Length of the crossfade
    int durationMs = params->getDurationMs();
    quint32 crossfadeLength = durationMs * sampleRate / 1000;
    if (crossfadeLength > loopStart)
        crossfadeLength = loopStart;

    // Apply the crossfade
    vData = SampleUtils::loopStep2(vData, loopStart, loopEnd, crossfadeLength, false);
    sm->setData(id, vData);
}
