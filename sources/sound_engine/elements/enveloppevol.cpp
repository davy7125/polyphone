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

#include "enveloppevol.h"
#include "qmath.h"


EnveloppeVol::EnveloppeVol(VoiceParam * voiceParam, quint32 sampleRate, bool isMod) :
    _currentSmpl(0),
    _precValue(0),
    _currentPhase(phase1delay),
    _sampleRate(sampleRate)
{
    // Stockage des paramètres initiaux
    if (isMod)
    {
        _timeDelay       = voiceParam->modDelayTime * _sampleRate;
        _timeAttack      = voiceParam->modAttackTime * _sampleRate;
        _timeHold        = voiceParam->modHoldTime * _sampleRate;
        _timeDecay       = voiceParam->modDecayTime * _sampleRate;
        _levelSustain    = voiceParam->modSustainLevel;
        _timeRelease     = voiceParam->modReleaseTime * _sampleRate;
        _noteToHold      = (float)voiceParam->modKeynumToHold / 1200;
        _noteToDecay     = (float)voiceParam->modKeynumToDecay / 1200;
        _volume          = 0;
    }
    else
    {
        _timeDelay       = voiceParam->volDelayTime * _sampleRate;
        _timeAttack      = voiceParam->volAttackTime * _sampleRate;
        _timeHold        = voiceParam->volHoldTime * _sampleRate;
        _timeDecay       = voiceParam->volDecayTime * _sampleRate;
        _levelSustain    = voiceParam->volSustainLevel;
        _timeRelease     = voiceParam->volReleaseTime * _sampleRate;
        _noteToHold      = (float)voiceParam->volKeynumToHold / 1200;
        _noteToDecay     = (float)voiceParam->volKeynumToDecay / 1200;
        _volume          = -voiceParam->attenuation;
    }

    _fixedVelocity   = voiceParam->fixedVelocity;
}

bool EnveloppeVol::applyEnveloppe(float * data, quint32 size, bool release, int note,
                                  int velocity, VoiceParam * voiceParam,
                                  double gain, bool applyOn1)
{
    // Pour l'instant pas de mise à jour en temps réel des paramètres
    Q_UNUSED(voiceParam);

    // Application de l'enveloppe sur des données
    // renvoie true si la fin de la release est atteint

    // Beginning of the release phase?
    if (release && _currentPhase != phase7off && _currentPhase != phase6release)
    {
        _currentPhase = phase6release;
        _currentSmpl = 0;
    }

    // Ajustement sustain level
    float levelSustain = 0;
    if (applyOn1)
        levelSustain = 1.f - _levelSustain / 100; // %
    else
        levelSustain = static_cast<float>(qPow(10, -static_cast<double>(_levelSustain) / 20)); // dB

    // Ajustement hold time et volume
    quint32 timeHold = static_cast<quint32>(_timeHold * fastPow2(_noteToHold * (60 - note)));
    quint32 timeDecay = static_cast<quint32>(_timeDecay * fastPow2(_noteToDecay * (60 - note)));
    float volume = static_cast<float>(qPow(10, (static_cast<double>(_volume) + gain) / 50)); // normalement division par 10
    if (_fixedVelocity < 0)
        volume *= (_fixedVelocity * _fixedVelocity) / 16129;
    else
        volume *= (velocity * velocity) / 16129;

    // Avancement
    bool fin = false;
    quint32 avancement = 0;
    quint32 duree = 0;
    float val2 = 0;
    float coef, valTmp;
    while (avancement < size)
    {
        switch (_currentPhase)
        {
        case phase1delay:
            // Nombre de valeurs restantes dans la phase
            duree = _timeDelay - _currentSmpl;
            if (duree <= size - avancement)
            {
                _currentPhase = phase2attack;
                _currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                _currentSmpl += duree;
            }
            val2 = 0;
            for (quint32 i = 0; i < duree; i++)
                data[avancement + i] = 0;
            break;
        case phase2attack:
            // Nombre de valeurs restantes dans la phase
            duree = _timeAttack - _currentSmpl;
            if (duree <= size - avancement)
            {
                _currentPhase = phase3hold;
                _currentSmpl = 0;
                val2 = 1;
            }
            else
            {
                duree = size - avancement;
                _currentSmpl += duree;
                val2 = static_cast<float>(_currentSmpl) / _timeAttack;
            }
            // Attaque convexe => amplitude linéaire
            coef = (val2 - this->_precValue) / duree;
            valTmp = 0;
            if (applyOn1)
            {
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (valTmp + this->_precValue);
                    valTmp += coef;
                }
            }
            else
            {
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * (data[avancement + i] *
                                                     (valTmp + this->_precValue));
                    valTmp += coef;
                }
            }
            break;
        case phase3hold:
            // Nombre de valeurs restantes dans la phase
            duree = timeHold - _currentSmpl;
            if (duree <= size - avancement)
            {
                _currentPhase = phase4decay;
                _currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                _currentSmpl += duree;
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
            duree = timeDecay - _currentSmpl;
            if (duree <= size - avancement)
            {
                _currentPhase = phase5sustain;
                _currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                _currentSmpl += duree;
            }
            if (applyOn1)
            {
                // Décroissance exponentielle
                coef = static_cast<float>(qPow(0.001 / (1. - static_cast<double>(levelSustain) + 0.001), 1. / timeDecay));
                val2 = (_precValue - levelSustain) * coef + levelSustain;
                for (quint32 i = 0; i < duree; i++)
                {
                    data[avancement + i] = volume * val2;
                    val2 = (val2 - levelSustain) * coef + levelSustain;
                }
            }
            else
            {
                // Décroissance exponentielle
                coef = static_cast<float>(qPow(0.00001585 / (1. - static_cast<double>(levelSustain) + 0.00001585), 1. / timeDecay));
                val2 = (_precValue - levelSustain) * coef + levelSustain;
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
            duree = _timeRelease - _currentSmpl;
            if (duree <= size - avancement)
            {
                _currentPhase = phase7off;
                _currentSmpl = 0;
            }
            else
            {
                duree = size - avancement;
                _currentSmpl += duree;
            }
            if (applyOn1)
            {
                // Linéaire
                coef = -1.f / _timeRelease;
                val2 = _precValue + coef;
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
                coef = static_cast<float>(qPow(0.00001585, 1. / _timeRelease));
                val2 = _precValue * coef;
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
        _precValue = val2;
        avancement += duree;
    }
    return fin;
}

void EnveloppeVol::quickRelease()
{
    // Arrêt par classe exclusive : release très courte
    _timeRelease = static_cast<quint32>(0.02 * _sampleRate);
    _currentPhase = phase6release;
    _currentSmpl = 0;
}
