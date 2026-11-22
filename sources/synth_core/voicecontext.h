/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#ifndef VOICECONTEXT_H
#define VOICECONTEXT_H

#include <atomic>
#include "imidivalues.h"
#include "synthcoreconfig.h"
class ModulatorData;

// Settings of a synth read by its voices: tuning, values of the MIDI controllers and default modulators
// Each synth has its own, so that several synths in a process (instances of a plugin) are independent
class VoiceContext
{
public:
    VoiceContext();

    // Values read by the modulators, not deleted (null: the values of DummyMidiDevice)
    void setMidiValues(IMidiValues * midiValues);
    IMidiValues * getMidiValues() const { return _midiValues.load(std::memory_order_relaxed); }

    // Reference pitch (0.1 Hz), fine tune in cents and use of each key from C to B relative to the key relativeKey,
    // the key referenceKey (not relative) keeping its pitch in the equal temperament (see SynthCoreConfig)
    void setTuning(int referencePitch, const float temperament[12], const bool used[12], int relativeKey,
                   int referenceKey);
    int getReferencePitch() const { return _referencePitch.load(std::memory_order_relaxed); }
    float getTemperamentFineTune(int key) const;
    bool isKeyUsed(int key) const;

    // Default modulators: the velocity to filter cutoff is the one of the version 2.01 (0), 2.04 (1) or none (2)
    void setModulatorVelToFilterType(int type);
    const ModulatorData * getDefaultModulators(int &number) const;

    // Global envelope: change (EnvelopeChange) and value of each EnvelopeStage (see SynthCoreConfig), true being
    // returned if something changed
    bool setEnvelope(const int changes[ENVELOPE_STAGE_COUNT], const int values[ENVELOPE_STAGE_COUNT]);
    int getEnvelopeChange(int stage, int &value) const
    {
        value = _envelopeValues[stage].load(std::memory_order_relaxed);
        return _envelopeChanges[stage].load(std::memory_order_relaxed);
    }

private:
    DummyMidiDevice _defaultMidiValues;
    std::atomic<IMidiValues *> _midiValues;
    std::atomic<int> _referencePitch;
    std::atomic<float> _temperament[12];
    std::atomic<int> _temperamentRelativeKey;
    std::atomic<int> _temperamentReferenceKey;
    std::atomic<int> _temperamentUsedKeys; // One bit per key from C to B, relative to the key _temperamentRelativeKey
    std::atomic<int> _modulatorVelToFilterType;
    std::atomic<int> _envelopeChanges[ENVELOPE_STAGE_COUNT];
    std::atomic<int> _envelopeValues[ENVELOPE_STAGE_COUNT];
};

#endif // VOICECONTEXT_H
