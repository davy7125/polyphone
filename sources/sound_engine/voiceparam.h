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

#ifndef VOICEPARAM_H
#define VOICEPARAM_H

#include "basetypes.h"
#include "modulatorgroup.h"
class SoundfontManager;
class ModulatedParameter;

// Class gathering all parameters useful to create a sound
// Parameters can evolve in real-time depending on the modulators
class VoiceParam
{
public:
    // Initialize a set of parameters (idPrstInst and idInstSmpl can be unknown)
    VoiceParam(EltID idPrstInst, EltID idInstSmpl, EltID idSmpl, int key, int vel);

    // Sample reading
    void prepareForSmpl(int key, SFSampleLink link);
    void setPan(double val);
    void setLoopMode(quint16 val);
    void setLoopStart(quint32 val);
    void setLoopEnd(quint32 val);
    void setFineTune(qint16 val);

    // Destructor
    ~VoiceParam();

    // Update parameters before reading them (modulators)
    void computeModulations();

    // Get a param
    double getDouble(AttributeType type);
    qint32 getInteger(AttributeType type);
    quint32 getPosition(AttributeType type);

private:
    SoundfontManager * _sm;

    // All parameters
    QMap<AttributeType, ModulatedParameter *> _parameters;
    ModulatorGroup _modulatorGroupInst, _modulatorGroupPrst;
    qint32 _sampleLength, _sampleLoopStart, _sampleLoopEnd, _sampleFineTune;
    qint32 _wPresetNumber;

    // Initialization of the parameters
    void prepareParameters();
    void readSmpl(EltID idSmpl);
    void readDivisionAttributes(EltID idDivision);
    void readDivisionModulators(EltID idDivision);
};

#endif // VOICEPARAM_H
