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

#include "toolmergesamples_parameters.h"
#include "contextmanager.h"

void ToolMergeSamples_parameters::loadConfiguration()
{
    // Preset property
    _presetName = getToolValue("name", "").toString();

    // Instrument property
    _densityType = getToolValue("density", 1).toInt();
    
    // Sample properties
    _stereoSample = getToolValue("stereo", true).toBool();
    _loopSample = getToolValue("loop", true).toBool();
    _sustainDuration = getToolValue("sustain_duration", 5).toInt();
    _releaseDuration = getToolValue("release_duration", 2).toInt();
}

void ToolMergeSamples_parameters::saveConfiguration()
{
    // Preset property
    setToolValue("name", _presetName);

    // Instrument property
    setToolValue("density", _densityType);
    
    // Sample properties
    setToolValue("stereo", _stereoSample);
    setToolValue("loop", _loopSample);
    setToolValue("sustain_duration", _sustainDuration);
    setToolValue("release_duration", _releaseDuration);
}
