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

#ifndef TOOLCHORDS_PARAMETERS_H
#define TOOLCHORDS_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QList>
#include <QString>
#include "qmath.h"


class ChordInfo
{
public:
    int chordType1; // 0: none, 1: yes
    int chordType1Attenuation;

    int chordType3; // 0: none, 1: major, 2: minor
    int chordType3Attenuation;
    bool chordType3Inversion;

    int chordType5; // 0: none, 1: perfect, 2: diminished, 3: augmented
    int chordType5Attenuation;
    bool chordType5Inversion;

    int chordType7; // 0: none, 1: minor, 2: major, 3: diminished
    int chordType7Attenuation;
    bool chordType7Inversion;

    int chordType9; // 0: none, 1: major, 2: minor
    int chordType9Attenuation;
    bool chordType9Inversion;

    int octave; // range is [-3; +3]
};

class ToolChords_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    QString getInstrumentName() { return _instrumentName; }
    void setInstrumentName(QString instrumentName) { _instrumentName = instrumentName; }
    
    int getDensityType() { return _densityType; }
    void setDensityType(int densityType) { _densityType = densityType; }
    
    bool getStereoSample() { return _stereoSample; }
    void setStereoSample(bool stereoSample) { _stereoSample = stereoSample; }
    
    bool getLoopSample() { return _loopSample; }
    void setLoopSample(bool loopSample) { _loopSample = loopSample; }
    
    ChordInfo & getChordConfiguration() { return _chordInfo; }
    void setChordConfiguration(ChordInfo chordInfo) { _chordInfo = chordInfo; }
    
private:
    QString _instrumentName;
    int _densityType;
    bool _stereoSample;
    bool _loopSample;
    ChordInfo _chordInfo;
};

#endif // TOOLCHORDS_PARAMETERS_H
