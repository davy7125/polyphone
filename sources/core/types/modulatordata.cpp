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

#include "modulatordata.h"
#include "attribute.h"


bool SFModulator::operator==(const SFModulator& other)
{
    return this->Type == other.Type &&
            this->isBipolar == other.isBipolar &&
            this->isDescending == other.isDescending &&
            this->CC == other.CC &&
            this->Index == other.Index;
}

quint16 SFModulator::toWord()
{
    //    quint16
    //        Type  : 6, // 6 bits for the type
    //        P     : 1, // Polarity
    //        D     : 1, // Direction
    //        CC    : 1,
    //        Index : 7;
    return ((Type << 10) + (isBipolar << 9) + (isDescending << 8) + (CC << 7) + Index) & 0xFFFF;
}

void ModulatorData::loadDefaultModulator(quint16 num)
{
    // Common to all default modulators
    this->transOper = SFTransform::linear;
    this->index = 65535; // Max so that it has no interaction with possible links

    switch (num)
    {
    case 0:
        // 1. MIDI Note-On Velocity to Initial Attenuation
        this->srcOper = SFModulator(GC_noteOnVelocity, typeConcave, true, false);
        this->amtSrcOper = SFModulator();
        this->amount = 960;
        this->destOper = champ_initialAttenuation;
        break;
    case 1:
        // 2. MIDI Note-On Velocity to Filter Cutoff
        this->srcOper = SFModulator(GC_noteOnVelocity, typeLinear, true, false);
        this->amtSrcOper = SFModulator();
        this->amount = -2400;
        this->destOper = champ_initialFilterFc;
        break;
    case 2:
        // 3. MIDI Channel Pressure to Vibrato LFO Pitch Depth
        this->srcOper = SFModulator(GC_channelPressure, typeLinear, false, false);
        this->amtSrcOper = SFModulator();
        this->amount = 50;
        this->destOper = champ_vibLfoToPitch;
        break;
    case 3:
        // 4. MIDI Continuous Controller 1 to Vibrato LFO Pitch Depth
        this->srcOper = SFModulator(1, typeLinear, false, false);
        this->amtSrcOper = SFModulator();
        this->amount = 50;
        this->destOper = champ_vibLfoToPitch;
        break;
    case 4:
        // 5. MIDI Continuous Controller 7 to Initial Attenuation
        this->srcOper = SFModulator(7, typeConcave, true, false);
        this->amtSrcOper = SFModulator();
        this->amount = 960;
        this->destOper = champ_initialAttenuation;
        break;
    case 5:
        // 6. MIDI Continuous Controller 10 to Pan Position
        this->srcOper = SFModulator(10, typeLinear, false, true);
        this->amtSrcOper = SFModulator();
        this->amount = 1000;
        this->destOper = champ_pan;
        break;
    case 6:
        // 7. MIDI Continuous Controller 11 to Initial Attenuation
        this->srcOper = SFModulator(11, typeConcave, true, false);
        this->amtSrcOper = SFModulator();
        this->amount = 960;
        this->destOper = champ_initialAttenuation;
        break;
    case 7:
        // 8. MIDI Continuous Controller 91 to Reverb Effects Send
        this->srcOper = SFModulator(91, typeLinear, false, false);
        this->amtSrcOper = SFModulator();
        this->amount = 200; // Yes, only 20% added...
        this->destOper = champ_reverbEffectsSend;
        break;
    case 8:
        // 9. MIDI Continuous Controller 93 to Chorus Effects Send
        this->srcOper = SFModulator(93, typeLinear, false, false);
        this->amtSrcOper = SFModulator();
        this->amount = 200;
        this->destOper = champ_chorusEffectsSend;
        break;
    case 9:
        // 10. MIDI Pitch Wheel to Initial Pitch Controlled by MIDI Pitch Wheel Sensitivity
        this->srcOper = SFModulator(GC_pitchWheel, typeLinear, false, true);
        this->amtSrcOper = SFModulator(GC_pitchWheelSensitivity, typeLinear, false, false);
        this->amount = 12700;
        this->destOper = champ_fineTune;
        break;
    default:
        // Nothing
        break;
    }
}
