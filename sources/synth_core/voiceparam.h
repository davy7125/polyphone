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
#include "isynthsoundfont.h"

// Class gathering all parameters useful to create a sound
// Parameters can evolve in real-time depending on the modulators
class VoiceParam
{
public:
    VoiceParam();
    ~VoiceParam();

    // Initialize a set of parameters (prst and inst can be unknown)
    void initialize(int sf2Id, ISynthInstPrst * prst, ISynthZone * prstZone, ISynthInstPrst * inst, ISynthZone * instZone,
                    ISynthSample * smpl, int8_t channel, int8_t key, int8_t vel, int8_t type);

    void setPan(float val);
    void setLoopMode(uint16_t val);
    void setLoopStart(uint32_t val);
    void setLoopEnd(uint32_t val);
    void setFineTune(int16_t val);

    // Get a param
    float getFloat(AttributeType type);
    int32_t getInteger(AttributeType type);
    uint32_t getPosition(AttributeType type);

    // Identification
    int8_t getChannel() { return _channel; }
    int8_t getKey() { return _key; }
    int8_t getType() { return _type; }
    int getSf2Id() { return _sf2Id; }
    int getPresetId() { return _presetId; }

    // Update modulators with MIDI events
    void processPolyPressureChanged(int pressure);
    void processMonoPressureChanged(int value);
    void processControllerChanged(int num, int value);
    void processBendChanged(float value);
    void processBendSensitivityChanged(float semitones);

private:
    // Identification
    int8_t _channel;
    int8_t _key;
    int8_t _type;
    int _sf2Id;
    int _presetId;
    int32_t _wPresetNumber;

    // All parameters
    ModulatedParameter _parameters[champ_endOper];
    ModulatorGroup _modulatorGroupInst, _modulatorGroupPrst;
    int32_t _sampleLength, _sampleLoopStart, _sampleLoopEnd, _sampleFineTune;

    // Initialization of the parameters
    void prepareParameters();
    void prepareForSmpl(int key, SFSampleLink link);
    void readSmpl(ISynthSample * smpl);
    void readZoneAttributes(ISynthZone * globalZone, ISynthZone * zone, bool isPrst);
    void readZoneModulators(ISynthZone * globalZone, ISynthZone * zone, bool isPrst);
    void computeModulations();
};

#endif // VOICEPARAM_H
