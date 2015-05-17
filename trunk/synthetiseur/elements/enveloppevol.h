/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef ENVELOPPEVOL_H
#define ENVELOPPEVOL_H

#include "voiceparam.h"
#include "sf2_types.h"

class EnveloppeVol
{
public:
    EnveloppeVol(VoiceParam * voiceParam, quint32 sampleRate, bool isMod);
    bool applyEnveloppe(float *data, quint32 size, bool release, int note, int velocity, VoiceParam * voiceParam,
                        double gain, bool applyOn1 = false);
    void quickRelease();

    static float fastPow2(float p)
    {
        float offset = (p < 0) ? 1.0f : 0.0f;
        float clipp = (p < -126) ? -126.0f : p;
        int w = clipp;
        float z = clipp - w + offset;
        union { quint32 i; float f; } v =
        { static_cast<quint32> ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };
        return v.f;
    }

private:
    enum EnveloppePhase
    {
        phase1delay,
        phase2attack,
        phase3hold,
        phase4decay,
        phase5sustain,
        phase6release,
        phase7off
    };

    // Etat du système
    quint32 m_currentSmpl;
    double m_precValue;
    EnveloppePhase m_currentPhase;

    // Paramètres de l'enveloppe
    quint32 m_timeDelay;
    quint32 m_timeAttack;
    quint32 m_timeHold;
    quint32 m_timeDecay;
    double m_levelSustain;
    quint32 m_timeRelease;
    double m_noteToHold, m_noteToDecay;

    // Volume
    double m_volume;
    int m_fixedVelocity;

    // Echantillonnage
    quint32 m_sampleRate;

    // Release autorisée ?
    bool m_allowRelease;
};

#endif // ENVELOPPEVOL_H
