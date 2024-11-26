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

#include "toolchords_parameters.h"
#include "contextmanager.h"

void ToolChords_parameters::loadConfiguration()
{
    // Instrument properties
    _instrumentName = getToolValue("name", "").toString();
    _densityType = getToolValue("density", 1).toInt();
    
    // Sample properties
    _stereoSample = getToolValue("stereo", true).toBool();
    _loopSample = getToolValue("loop", true).toBool();
    
    // Chord configuration
    _chordInfo.chordType1 = getToolValue("type1", 1).toInt();
    _chordInfo.chordType1Attenuation = getToolValue("type1att", 0).toInt();

    _chordInfo.chordType3 = getToolValue("type3", 1).toInt();
    _chordInfo.chordType3Attenuation = getToolValue("type3att", 0).toInt();
    _chordInfo.chordType3Inversion = getToolValue("type3inv", 0).toBool();

    _chordInfo.chordType5 = getToolValue("type5", 1).toInt();
    _chordInfo.chordType5Attenuation = getToolValue("type5att", 0).toInt();
    _chordInfo.chordType5Inversion = getToolValue("type5inv", 0).toBool();

    _chordInfo.chordType7 = getToolValue("type7", 0).toInt();
    _chordInfo.chordType7Attenuation = getToolValue("type7att", 0).toInt();
    _chordInfo.chordType7Inversion = getToolValue("type7inv", false).toBool();

    _chordInfo.chordType9 = getToolValue("type9", 0).toInt();
    _chordInfo.chordType9Attenuation = getToolValue("type9att", 0).toInt();
    _chordInfo.chordType9Inversion = getToolValue("type9inv", false).toBool();

    _chordInfo.octave = getToolValue("octave", 0).toInt();
}

void ToolChords_parameters::saveConfiguration()
{
    // Instrument properties
    setToolValue("name", _instrumentName);
    setToolValue("density", _densityType);
    
    // Sample properties
    setToolValue("stereo", _stereoSample);
    setToolValue("loop", _loopSample);
    
    // Chord configuration
    setToolValue("type1", _chordInfo.chordType1);
    setToolValue("type1att", _chordInfo.chordType1Attenuation);

    setToolValue("type3", _chordInfo.chordType3);
    setToolValue("type3att", _chordInfo.chordType3Attenuation);
    setToolValue("type3inv", _chordInfo.chordType3Inversion);

    setToolValue("type5", _chordInfo.chordType5);
    setToolValue("type5att", _chordInfo.chordType5Attenuation);
    setToolValue("type5inv", _chordInfo.chordType5Inversion);

    setToolValue("type7", _chordInfo.chordType7);
    setToolValue("type7att", _chordInfo.chordType7Attenuation);
    setToolValue("type7inv", _chordInfo.chordType7Inversion);

    setToolValue("type9", _chordInfo.chordType9);
    setToolValue("type9att", _chordInfo.chordType9Attenuation);
    setToolValue("type9inv", _chordInfo.chordType9Inversion);

    setToolValue("octave", _chordInfo.octave);
}
