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

#include "enveloppevol.h"
#include "qmath.h"


EnveloppeVol::EnveloppeVol(VoiceParam * voiceParam, quint32 sampleRate, bool isMod) :
    m_currentSmpl(0),
    m_precValue(0),
    m_currentPhase(phase1delay),
    m_sampleRate(sampleRate)
{
    // Stockage des paramètres initiaux
    if (isMod)
    {
        m_timeDelay       = voiceParam->modDelayTime * m_sampleRate;
        m_timeAttack      = voiceParam->modAttackTime * m_sampleRate;
        m_timeHold        = voiceParam->modHoldTime * m_sampleRate;
        m_timeDecay       = voiceParam->modDecayTime * m_sampleRate;
        m_levelSustain    = voiceParam->modSustainLevel;
        m_timeRelease     = voiceParam->modReleaseTime * m_sampleRate;
        m_noteToHold      = (double)voiceParam->modKeynumToHold / 1200;
        m_noteToDecay     = (double)voiceParam->modKeynumToDecay / 1200;
        m_volume          = 0;
        m_fixedVelocity   = voiceParam->fixedVelocity;
        m_allowRelease    = (voiceParam->loopMode != 3);
    }
    else
    {
        m_timeDelay       = voiceParam->volDelayTime * m_sampleRate;
        m_timeAttack      = voiceParam->volAttackTime * m_sampleRate;
        m_timeHold        = voiceParam->volHoldTime * m_sampleRate;
        m_timeDecay       = voiceParam->volDecayTime * m_sampleRate;
        m_levelSustain    = voiceParam->volSustainLevel;
        m_timeRelease     = voiceParam->volReleaseTime * m_sampleRate;
        m_noteToHold      = (double)voiceParam->volKeynumToHold / 1200;
        m_noteToDecay     = (double)voiceParam->volKeynumToDecay / 1200;
        m_volume          = -voiceParam->attenuation;
        m_fixedVelocity   = voiceParam->fixedVelocity;
        m_allowRelease    = (voiceParam->loopMode != 3);
    }
}

bool EnveloppeVol::applyEnveloppe(float * data, quint32 size, bool release, int note,
                                  int velocity, VoiceParam * voiceParam,
                                  double gain, bool applyOn1)
{
    // Pour l'instant pas de mise à jour en temps réel des paramètres
    Q_UNUSED(voiceParam);

    // Application de l'enveloppe sur des données
    // renvoie 1 si la fin de la release est atteint

    if (release && m_currentPhase != phase7off && m_currentPhase != phase6release)
    {
        if (m_allowRelease)
        {
            m_currentPhase = phase6release;
            m_currentSmpl = 0;
        }
        else if (m_currentPhase != phase5sustain)
        {
            if (m_precValue < m_levelSustain)
            {
                m_levelSustain = m_precValue;
                m_currentPhase = phase5sustain;
            }
            else
                m_currentPhase = phase4decay;
        }
    }

    // Ajustement sustain level
    double levelSustain = 0;
    if (applyOn1)
        levelSustain = 1. - m_levelSustain / 100; // %
    else
        levelSustain = qPow(10, -m_levelSustain / 20); // dB

    // Ajustement hold time et volume
    quint32 timeHold = m_timeHold * fastPow2(m_noteToHold * (60 - note));
    quint32 timeDecay = m_timeDecay * fastPow2(m_noteToDecay * (60 - note));
    double volume = qPow(10, (m_volume + gain) / 50); // normalement division par 10
    if (m_fixedVelocity != -1)
        volume *= (double)(m_fixedVelocity * m_fixedVelocity) / 16129;
    else
        volume *= (double)(velocity * velocity) / 16129;

    // Avancement
    bool fin = false;
    quint32 avancement = 0;
    quint32 duree = 0;
    double val2 = 0;
    double coef, valTmp;
    while (avancement < size)
    {
        switch (m_currentPhase)
        {
        case phase1delay:
            // Nombre de valeurs restantes dans la phase
            duree = m_timeDelay - m_currentSmpl;
            if (duree <= size - avancement)
            {
                m_currentPhase = phase2attack;
                m_currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                m_currentSmpl += duree;
            }
            val2 = 0;
            for (quint32 i = 0; i < duree; i++)
                data[avancement + i] = 0;
            break;
        case phase2attack:
            // Nombre de valeurs restantes dans la phase
            duree = m_timeAttack - m_currentSmpl;
            if (duree <= size - avancement)
            {
                m_currentPhase = phase3hold;
                m_currentSmpl = 0;
                val2 = 1;
            }
            else
            {
                duree = size - avancement;
                m_currentSmpl += duree;
                val2 = (double)m_currentSmpl / m_timeAttack;
            }
            // Attaque convexe => amplitude linéaire
            coef = (val2 - this->m_precValue) / duree;
            valTmp = 0;
            if (applyOn1)
            {
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (valTmp + this->m_precValue);
                    valTmp += coef;
                }
            }
            else
            {
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (data[avancement + i] *
                                                     (valTmp + this->m_precValue));
                    valTmp += coef;
                }
            }
            break;
        case phase3hold:
            // Nombre de valeurs restantes dans la phase
            duree = timeHold - m_currentSmpl;
            if (duree <= size - avancement)
            {
                m_currentPhase = phase4decay;
                m_currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                m_currentSmpl += duree;
            }
            val2 = 1;
            if (applyOn1)
            {
                for (quint32 i = 0; i < duree; i++)
                    data[avancement + i] = volume;
            }
            else
            {
                for (quint32 i = 0; i < duree; i++)
                    data[avancement + i] = volume * data[avancement + i];
            }
            break;
        case phase4decay:
            // Nombre de valeurs restantes dans la phase
            duree = timeDecay - m_currentSmpl;
            if (duree <= size - avancement)
            {
                m_currentPhase = phase5sustain;
                m_currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                m_currentSmpl += duree;
            }
            if (applyOn1)
            {
                // Décroissance exponentielle
                coef = qPow(0.001 / (1. - levelSustain + 0.001), 1. / timeDecay);
                val2 = (m_precValue - levelSustain) * coef + levelSustain;
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * val2;
                    val2 = (val2 - levelSustain) * coef + levelSustain;
                }
            }
            else
            {
                // Décroissance exponentielle
                coef = qPow(0.00001585 / (1. - levelSustain + 0.00001585), 1. / timeDecay);
                val2 = (m_precValue - levelSustain) * coef + levelSustain;
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (data[avancement + i] * val2);
                    val2 = (val2 - levelSustain) * coef + levelSustain;
                }
            }
            break;
        case phase5sustain:
            // Nombre de valeurs
            duree = size - avancement;
            val2 = levelSustain;
            if (applyOn1)
            {
                for (quint32 i = 0; i < duree; i++)
                    data[avancement + i] = volume * val2;
            }
            else
            {
                for (quint32 i = 0; i < duree; i++)
                    data[avancement + i] = volume * (data[avancement + i] * val2);
            }
            break;
        case phase6release:
            // Nombre de valeurs restantes dans la phase
            duree = m_timeRelease - m_currentSmpl;
            if (duree <= size - avancement)
            {
                m_currentPhase = phase7off;
                m_currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                m_currentSmpl += duree;
            }
            if (applyOn1)
            {
                // Linéaire
                coef = -1. / m_timeRelease;
                val2 = m_precValue + coef;
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * val2;
                    val2 += coef;
                    if (val2 < 0)
                        val2 = 0;
                }
            }
            else
            {
                // Décroissance exponentielle
                coef = qPow(0.00001585, 1. / m_timeRelease);
                val2 = m_precValue * coef;
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (data[avancement + i] * val2);
                    val2 *= coef;
                }
            }
            break;
        case phase7off:
            // Nombre de valeurs
            duree = size - avancement;
            val2 = 0;
            for (quint32 i = 0; i < duree; i++)
                data[avancement + i] = 0;
            // Fin
            fin = true;
        }
        // On retient la dernière valeur et on avance
        m_precValue = val2;
        avancement += duree;
    }
    return fin;
}

void EnveloppeVol::quickRelease()
{
    // Arrêt par classe exclusive : release très courte
    m_timeRelease = 0.02 * m_sampleRate;
    m_currentPhase = phase6release;
    m_currentSmpl = 0;
}
