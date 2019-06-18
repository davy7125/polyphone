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

#include "toolchords_parameters.h"
#include "contextmanager.h"

void ToolChords_parameters::loadConfiguration()
{
    // Instrument properties
    _instrumentName = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "name", "").toString();
    _densityType = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "density", 1).toInt();
    
    // Sample properties
    _stereoSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "stereo", true).toBool();
    _loopSample = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "loop", true).toBool();
    
    // Chord configuration
    _chordInfo.chordType1 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type1", 1).toInt();
    _chordInfo.chordType1Attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type1att", 0).toInt();

    _chordInfo.chordType3 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3", 1).toInt();
    _chordInfo.chordType3Attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3att", 0).toInt();
    _chordInfo.chordType3Inversion = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3inv", 0).toBool();

    _chordInfo.chordType5 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5", 1).toInt();
    _chordInfo.chordType5Attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5att", 0).toInt();
    _chordInfo.chordType5Inversion = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5inv", 0).toBool();

    _chordInfo.chordType7 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7", 0).toInt();
    _chordInfo.chordType7Attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7att", 0).toInt();
    _chordInfo.chordType7Inversion = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7inv", false).toBool();

    _chordInfo.chordType9 = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9", 0).toInt();
    _chordInfo.chordType9Attenuation = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9att", 0).toInt();
    _chordInfo.chordType9Inversion = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9inv", false).toBool();

    _chordInfo.octave = ContextManager::configuration()->getToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "octave", 0).toInt();
}

void ToolChords_parameters::saveConfiguration()
{
    // Instrument properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "name", _instrumentName);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "density", _densityType);
    
    // Sample properties
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "stereo", _stereoSample);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "loop", _loopSample);
    
    // Chord configuration
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type1", _chordInfo.chordType1);

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3", _chordInfo.chordType3);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3att", _chordInfo.chordType3Attenuation);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type3inv", _chordInfo.chordType3Inversion);

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5", _chordInfo.chordType5);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5att", _chordInfo.chordType5Attenuation);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type5inv", _chordInfo.chordType5Inversion);

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7", _chordInfo.chordType7);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7att", _chordInfo.chordType7Attenuation);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type7inv", _chordInfo.chordType7Inversion);

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9", _chordInfo.chordType9);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9att", _chordInfo.chordType9Attenuation);
    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "type9inv", _chordInfo.chordType9Inversion);

    ContextManager::configuration()->setToolValue(ConfManager::TOOL_TYPE_INSTRUMENT, "chord", "octave", _chordInfo.octave);
}
