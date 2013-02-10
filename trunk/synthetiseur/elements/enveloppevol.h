/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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
    bool applyEnveloppe(qint32 * data, quint32 size, bool release, int note, int velocity, VoiceParam * voiceParam,
                        double gain, qint32 valFixe = 0);
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
};

#endif // ENVELOPPEVOL_H
