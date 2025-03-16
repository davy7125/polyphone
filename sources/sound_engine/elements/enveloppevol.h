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

#ifndef ENVELOPPEVOL_H
#define ENVELOPPEVOL_H

#include <QtGlobal>
class VoiceParam;

class EnveloppeVol
{
public:
    EnveloppeVol() {}

    void initialize(quint32 chunkRate, bool isMod);

    // Apply an envelop on data
    // Return true if the end of the release is reached
    bool getEnvelope(float *data, quint32 chunkCount, bool release, int note, float gain, VoiceParam * voiceParam);

    // Call a quick release
    void quickRelease();

    // Call a quick attack, just before the release (sample mode "release")
    void quickAttack();

private:
    enum EnveloppePhase
    {
        phase1delay,
        phase2attack,
        phase3hold,
        phase4decay,
        phase5sustain,
        phase6quickAttack,
        phase6release,
        phase7off
    };

    // State of the system
    quint32 _currentChunk;
    float _lastValue;
    EnveloppePhase _currentPhase;

    quint32 _chunkRate;
    bool _isMod;
    bool _quickRelease;
    float _quickAttackTarget;
};

#endif // ENVELOPPEVOL_H
