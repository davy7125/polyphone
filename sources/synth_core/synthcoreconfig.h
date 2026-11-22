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

#ifndef SYNTHCORECONFIG_H
#define SYNTHCORECONFIG_H

#include "synthconstants.h"

// Stages of the global envelope (SynthCoreConfig::envelopeChanges)
enum EnvelopeStage
{
    ENVELOPE_ATTACK,
    ENVELOPE_DECAY,
    ENVELOPE_SUSTAIN,
    ENVELOPE_RELEASE,
    ENVELOPE_STAGE_COUNT
};

// Change of a stage by the global envelope: none, relative (a value added) or fixed (a value replacing the one of the
// soundfont)
enum EnvelopeChange
{
    ENVELOPE_UNCHANGED,
    ENVELOPE_RELATIVE,
    ENVELOPE_FIXED
};

class SynthCoreConfig
{
public:
    SynthCoreConfig() :
        choLevel(0),
        choDepth(0),
        choFrequency(0),
        revLevel(0),
        revSize(0),
        revWidth(0),
        revDamping(0),
        masterGain(0),
        referencePitch(4400),
        temperamentRelativeKey(0),
        temperamentReferenceKey(9),
        modulatorVelToFilterType(1),
        cpuBudget(DEFAULT_CPU_BUDGET)
    {
        for (int i = 0; i < 12; i++)
        {
            temperament[i] = 0;
            temperamentUsed[i] = true;
        }
        for (int i = 0; i < ENVELOPE_STAGE_COUNT; i++)
        {
            envelopeChanges[i] = ENVELOPE_UNCHANGED;
            envelopeValues[i] = 0;
        }
    }

    // Effects, from 0 to 100
    int choLevel;
    int choDepth;
    int choFrequency;

    int revLevel;
    int revSize;
    int revWidth;
    int revDamping;

    // Master gain (dB) and reference pitch (0.1 Hz)
    double masterGain;
    int referencePitch;

    // Fine tune in cents and use of each key from C to B, relative to the key temperamentRelativeKey (tonality)
    // The fine tunes are shifted so that the key temperamentReferenceKey (0 = C, 11 = B, not relative: note of the
    // reference pitch, A by default) keeps its pitch in the equal temperament. The keys not used are not played.
    float temperament[12];
    bool temperamentUsed[12];
    int temperamentRelativeKey;
    int temperamentReferenceKey;

    // Velocity to filter cutoff among the default modulators: the one of the version 2.01 (0), 2.04 (1) or none (2)
    int modulatorVelToFilterType;

    // Global envelope (plugin), applied to the zones of the presets except the drums (bank 128) after their own values
    // and before their modulators, for each EnvelopeStage (EnvelopeChange, value):
    // * attack, decay and release: timecents added to the durations of the volume and modulation envelopes (relative),
    //   or duration of the volume envelope in timecents (fixed);
    // * sustain of the volume envelope: centibels added to its attenuation (relative), or its attenuation in centibels
    //   (fixed).
    int envelopeChanges[ENVELOPE_STAGE_COUNT];
    int envelopeValues[ENVELOPE_STAGE_COUNT];

    // Share of the duration of the audio (%) that computing the voices can take in real time, above which the quietest
    // notes fade out
    int cpuBudget;
};

#endif // SYNTHCORECONFIG_H
