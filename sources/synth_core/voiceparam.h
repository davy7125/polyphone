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

#include <atomic>
#include "modulatorgroup.h"
#include "modulatedparameter.h"
#include "isynthsoundfont.h"
class VoiceContext;

// Class gathering all parameters useful to create a sound
// Parameters can evolve in real-time depending on the modulators
class VoiceParam
{
public:
    // The context (settings of the synth) is read by the modulators
    VoiceParam(const VoiceContext * context);
    ~VoiceParam();

    // Initialize a set of parameters (prst and inst can be unknown)
    void initialize(int sf2Id, ISynthInstPrst * prst, ISynthZone * prstZone, ISynthInstPrst * inst, ISynthZone * instZone,
                    ISynthSample * smpl, int8_t channel, int8_t key, int8_t vel, int8_t type);

    void setPan(float val);
    void setLoopMode(uint16_t val);
    void setLoopStart(uint32_t val);
    void setLoopEnd(uint32_t val);
    void setFineTune(int16_t val);

    // Get a param, by the thread computing the voice: the values are computed again only after a change of the
    // parameters (initialization, setters, MIDI events)
    float getFloat(AttributeType type)
    {
        return _parameters[type].getRealValue(_version.load(std::memory_order_acquire));
    }
    int32_t getInteger(AttributeType type)
    {
        // Notes:
        // * if fineTune is required: add the finetune from the sample level
        // * if wPreset is required, it will be stored in a special variable
        if (type == champ_wPreset)
            return _wPresetNumber;
        return (type == champ_fineTune ? _sampleFineTune : 0) +
               _parameters[type].getIntValue(_version.load(std::memory_order_acquire));
    }
    uint32_t getPosition(AttributeType type);

    // Same values always computed, by another thread
    float getCurrentFloat(AttributeType type);
    int32_t getCurrentInteger(AttributeType type);

    // Version of the parameters, changing with them: values computed from them can be kept for a version
    uint32_t getVersion() { return _version.load(std::memory_order_acquire); }

    // Identification
    int8_t getChannel() { return _channel; }
    int8_t getKey() { return _key; }
    int8_t getType() { return _type; }
    int getSf2Id() { return _sf2Id; }
    int getPresetId() { return _presetId; }
    const VoiceContext * getContext() { return _context; }

    // Update the inputs of the modulators with MIDI events, the modulations being computed again by the thread
    // computing the voice (updateModulations): the parameters are never modified while it reads them
    // (initialize also computes them, before the voice is computed)
    void processPolyPressureChanged(int pressure);
    void processMonoPressureChanged(int value);
    void processControllerChanged(int num, int value);
    void processBendChanged(float value);
    void processBendSensitivityChanged(float semitones);

    // Compute the modulations again if an input changed, by the thread computing the voice
    void updateModulations()
    {
        // A load first: no write in the usual case
        if (_modulationsChanged.load(std::memory_order_acquire) != 0 &&
            _modulationsChanged.exchange(0, std::memory_order_acquire) != 0)
            computeModulations();
    }

private:
    const VoiceContext * _context;

    // Identification
    int8_t _channel;
    int8_t _key;
    int8_t _type;
    int _sf2Id;
    int _presetId;
    int32_t _wPresetNumber;

    // All parameters
    ModulatedParameter _parameters[champ_endOper];
    std::atomic<uint32_t> _version; // Incremented after each change of the parameters
    std::atomic<int> _modulationsChanged; // Next to _version: read at each computation, with no other cache line
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
