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


EnveloppeVol::EnveloppeVol(quint32 sampleRate, bool isMod) :
    _currentSmpl(0),
    _precValue(0),
    _currentPhase(phase1delay),
    _sampleRate(sampleRate),
    _isMod(isMod),
    _fastRelease(false)
{
}

bool EnveloppeVol::applyEnveloppe(float * data, quint32 size, bool release, int note, float gain, VoiceParam * voiceParam)
{
    // Load parameters
    quint32 v_timeDelay;
    quint32 v_timeAttack;
    quint32 v_timeHold;
    quint32 v_timeDecay;
    float v_levelSustain;
    quint32 v_timeRelease;
    float v_noteToHold, v_noteToDecay;

    if (_isMod)
    {
        v_timeDelay = voiceParam->getDouble(champ_delayModEnv) * _sampleRate;
        v_timeAttack = voiceParam->getDouble(champ_attackModEnv) * _sampleRate;
        v_timeHold = voiceParam->getDouble(champ_holdModEnv) * _sampleRate;
        v_timeDecay = voiceParam->getDouble(champ_decayModEnv) * _sampleRate;
        v_levelSustain = voiceParam->getDouble(champ_sustainModEnv);
        v_timeRelease = voiceParam->getDouble(champ_releaseModEnv) * _sampleRate;
        v_noteToHold = (float)voiceParam->getInteger(champ_keynumToModEnvHold) / 1200;
        v_noteToDecay = (float)voiceParam->getInteger(champ_keynumToModEnvDecay) / 1200;
    }
    else
    {
        v_timeDelay = voiceParam->getDouble(champ_delayVolEnv) * _sampleRate;
        v_timeAttack = voiceParam->getDouble(champ_attackVolEnv) * _sampleRate;
        v_timeHold = voiceParam->getDouble(champ_holdVolEnv) * _sampleRate;
        v_timeDecay = voiceParam->getDouble(champ_decayVolEnv) * _sampleRate;
        v_levelSustain = voiceParam->getDouble(champ_sustainVolEnv);
        v_timeRelease = voiceParam->getDouble(champ_releaseVolEnv) * _sampleRate;
        v_noteToHold = (float)voiceParam->getInteger(champ_keynumToVolEnvHold) / 1200;
        v_noteToDecay = (float)voiceParam->getInteger(champ_keynumToVolEnvDecay) / 1200;
    }

    if (_fastRelease)
        v_timeRelease = static_cast<quint32>(0.02 * _sampleRate);

    // Beginning of the release phase?
    if (release && _currentPhase != phase7off && _currentPhase != phase6release)
    {
        _currentPhase = phase6release;
        _currentSmpl = 0;
    }

    // Compute the sustain level
    float levelSustain = _isMod ?
                (1.f - v_levelSustain / 100) : // percentage
                static_cast<float>(qPow(10, -0.05 * static_cast<double>(v_levelSustain))); // decrease in dB

    // Update hold / decay time and volume according to the key
    quint32 timeHold = static_cast<quint32>(v_timeHold * fastPow2(v_noteToHold * (60 - note)));
    quint32 timeDecay = static_cast<quint32>(v_timeDecay * fastPow2(v_noteToDecay * (60 - note)));

    // Avancement
    bool fin = false;
    quint32 avancement = 0;
    quint32 duration = 0;
    float lastValue = 0;
    float coef;

    while (avancement < size)
    {
        switch (_currentPhase)
        {
        case phase1delay:
            // Number of remaining points in the phase
            duration = _currentSmpl < v_timeDelay ? v_timeDelay - _currentSmpl : 0;
            if (duration <= size - avancement)
            {
                _currentPhase = phase2attack;
                _currentSmpl = 0;
            }
            else
            {
                duration = size - avancement;
                _currentSmpl += duration;
            }
            lastValue = 0;
            for (quint32 i = 0; i < duration; i++)
                data[avancement + i] = 0;
            break;
        case phase2attack:
            // Number of remaining points in the phase
            duration = _currentSmpl < v_timeAttack ? v_timeAttack - _currentSmpl : 0;
            if (duration <= size - avancement)
            {
                _currentPhase = phase3hold;
                _currentSmpl = 0;
            }
            else
            {
                duration = size - avancement;
                _currentSmpl += duration;
            }

            lastValue = this->_precValue;
            if (_isMod)
            {
                // Convex attack
                float stiffness = 0.000001f;
                coef = static_cast<float>(qPow(static_cast<double>(stiffness), 0.4 / v_timeAttack));
                float correction = static_cast<float>(qPow(static_cast<double>(stiffness), 0.4));
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] = gain * lastValue;
                    lastValue = 1.0f - coef * (1.0f - lastValue + stiffness + correction) + stiffness + correction;
                }
            }
            else
            {
                // Linear amplitude => convex attack (dB)
                coef = 1.f / v_timeAttack; // Target is 1.f
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] *= gain * lastValue ;
                    lastValue += coef;
                }
            }
            break;
        case phase3hold:
            // Number of remaining points in the phase
            duration = _currentSmpl < timeHold ? timeHold - _currentSmpl : 0;
            if (duration <= size - avancement)
            {
                _currentPhase = phase4decay;
                _currentSmpl = 0;
            }
            else
            {
                duration = size - avancement;
                _currentSmpl += duration;
            }
            lastValue = 1;
            if (_isMod)
            {
                for (quint32 i = 0; i < duration; i++)
                    data[avancement + i] = gain;
            }
            else
            {
                for (quint32 i = 0; i < duration; i++)
                    data[avancement + i] *= gain;
            }
            break;
        case phase4decay:
            // Number of remaining points in the phase
            duration = _currentSmpl < timeDecay ? timeDecay - _currentSmpl : 0;
            if (duration <= size - avancement)
            {
                _currentPhase = phase5sustain;
                _currentSmpl = 0;
            }
            else
            {
                duration = size - avancement;
                _currentSmpl += duration;
            }
            if (_isMod)
            {
                // Linear decay
                coef = -1.f / timeDecay;
                lastValue = _precValue + coef;
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] = gain * lastValue;
                    lastValue += coef;
                    if (lastValue < levelSustain)
                        lastValue = levelSustain;
                }
            }
            else
            {
                // Exponential decay
                coef = static_cast<float>(qPow(0.00001585 / (1. - static_cast<double>(levelSustain) + 0.00001585), 1. / timeDecay));
                lastValue = (_precValue - levelSustain) * coef + levelSustain;
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] *= gain * lastValue;
                    lastValue = (lastValue - levelSustain) * coef + levelSustain;
                }
            }
            break;
        case phase5sustain:
            // Number of values
            duration = size - avancement;
            lastValue = levelSustain;
            if (_isMod)
            {
                for (quint32 i = 0; i < duration; i++)
                    data[avancement + i] = gain * lastValue;
            }
            else
            {
                for (quint32 i = 0; i < duration; i++)
                    data[avancement + i] *= gain * lastValue;
            }
            break;
        case phase6release:
            // Number of remaining points in the phase
            duration = _currentSmpl < v_timeRelease ? v_timeRelease - _currentSmpl : 0;
            if (duration <= size - avancement)
            {
                _currentPhase = phase7off;
                _currentSmpl = 0;
            }
            else
            {
                duration = size - avancement;
                _currentSmpl += duration;
            }
            if (_isMod)
            {
                // Linear decay
                coef = -1.f / v_timeRelease;
                lastValue = _precValue + coef;
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] = gain * lastValue;
                    lastValue += coef;
                    if (lastValue < 0)
                        lastValue = 0;
                }
            }
            else
            {
                // Exponential decay
                coef = static_cast<float>(qPow(0.00001585, 1. / v_timeRelease));
                lastValue = _precValue * coef;
                for (quint32 i = 0; i < duration; i++)
                {
                    data[avancement + i] *= gain * lastValue;
                    lastValue *= coef;
                }
            }
            break;
        case phase7off:
            // Number of values
            duration = size - avancement;
            lastValue = 0;
            for (quint32 i = 0; i < duration; i++)
                data[avancement + i] = 0;

            // End
            fin = true;
        }

        // We keep the last value and we go on
        _precValue = lastValue;
        avancement += duration;
    }
    return fin;
}

void EnveloppeVol::quickRelease()
{
    // Stopped by an exclusive class: very short release
    _fastRelease = true;
    _currentPhase = phase6release;
    _currentSmpl = 0;
}
