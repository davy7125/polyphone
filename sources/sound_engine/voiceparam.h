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

#ifndef VOICEPARAM_H
#define VOICEPARAM_H

#include "modulatorgroup.h"
#include "modulatedparameter.h"
class Division;
class Smpl;
class InstPrst;

// Class gathering all parameters useful to create a sound
// Parameters can evolve in real-time depending on the modulators
class VoiceParam
{
public:
    VoiceParam();
    ~VoiceParam();

    // Initialize a set of parameters (prst and inst can be unknown)
    void initialize(InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv,
                    Smpl * smpl, qint8 channel, qint8 key, qint8 vel, qint8 type);

    void setPan(float val);
    void setLoopMode(quint16 val);
    void setLoopStart(quint32 val);
    void setLoopEnd(quint32 val);
    void setFineTune(qint16 val);

    // Update parameters before reading them (modulators)
    void computeModulations();

    // Get a param
    float getFloat(AttributeType type);
    qint32 getInteger(AttributeType type);
    quint32 getPosition(AttributeType type);

    // Identification
    qint8 getChannel() { return _channel; }
    qint8 getKey() { return _key; }
    qint8 getType() { return _type; }
    int getSf2Id() { return _sf2Id; }
    int getPresetId() { return _presetId; }

private:
    // Identification
    qint8 _channel;
    qint8 _key;
    qint8 _type;
    int _sf2Id;
    int _presetId;
    qint32 _wPresetNumber;

    // All parameters
    ModulatedParameter _parameters[champ_endOper];
    ModulatorGroup _modulatorGroupInst, _modulatorGroupPrst;
    qint32 _sampleLength, _sampleLoopStart, _sampleLoopEnd, _sampleFineTune;

    // Initialization of the parameters
    void prepareParameters();
    void prepareForSmpl(int key, SFSampleLink link);
    void readSmpl(Smpl * smpl);
    void readDivisionAttributes(Division * globalDivision, Division * division, bool isPrst);
    void readDivisionModulators(Division * globalDivision, Division * division, bool isPrst);
};

#endif // VOICEPARAM_H
