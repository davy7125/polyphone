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


#ifndef ENGINEVOICEPARAMETERS_H
#define ENGINEVOICEPARAMETERS_H

#include "midiprocessor.h"
#include "voicecontext.h"
class VoiceParam;
class Soundfont;
class InstPrst;

// Parameters of the voices started by the sound engine, computed without playing them by the same code (VoiceParam)
// with the default settings of an offline rendering: A4 = 440 Hz, equal temperament, default values of the controllers
// as in the editor (volume and expression at 127, pan at 64, the controllers read by the modulators of the soundfont at
// 64 if bipolar, 127 if decreasing). Used by describe_key (MCP) and by the timbre view.
class EngineVoiceParameters
{
public:
    // The default values of the controllers depend on the modulators of the soundfont
    EngineVoiceParameters(int indexSf2, Soundfont * soundfont);
    ~EngineVoiceParameters();

    // Parameters of the voice started for a division of an instrument (possibly reached from a preset division) at a
    // key and a velocity, whatever the key and velocity ranges, modulators included; null if the division has no
    // sample. The result is valid until the next call.
    VoiceParam * compute(InstPrst * prst, int prstDivIndex, InstPrst * inst, int instDivIndex, int key, int velocity);

    // Transposition (cents) applied to the sample data, without what changes over time (modulation envelope and LFOs
    // to the pitch): (key used - root key used) x scale tuning + fine tune + pitch correction + 100 x coarse tune
    static int getStaticPitchShift(VoiceParam * param);

private:
    int _indexSf2;
    Soundfont * _soundfont;
    MidiProcessor _midiValues;
    VoiceContext _context;
    VoiceParam * _param;
};

#endif // ENGINEVOICEPARAMETERS_H
