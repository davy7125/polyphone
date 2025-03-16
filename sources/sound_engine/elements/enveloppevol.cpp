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

#include "enveloppevol.h"
#include "qmath.h"
#include "voiceparam.h"
#include "fastmaths.h"

void EnveloppeVol::initialize(quint32 chunkRate, bool isMod)
{
    _currentChunk = 0;
    _lastValue = 0.0f;
    _currentPhase = phase1delay;
    _chunkRate = chunkRate;
    _isMod = isMod;
    _quickRelease = false;
}

bool EnveloppeVol::getEnvelope(float * data, quint32 chunkCount, bool release, int note, float gain, VoiceParam * voiceParam)
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
        v_timeDelay = voiceParam->getFloat(champ_delayModEnv) * _chunkRate;
        v_timeAttack = voiceParam->getFloat(champ_attackModEnv) * _chunkRate;
        v_timeHold = voiceParam->getFloat(champ_holdModEnv) * _chunkRate;
        v_timeDecay = voiceParam->getFloat(champ_decayModEnv) * _chunkRate;
        v_levelSustain = voiceParam->getFloat(champ_sustainModEnv);
        v_timeRelease = voiceParam->getFloat(champ_releaseModEnv) * _chunkRate;
        v_noteToHold = static_cast<float>(voiceParam->getInteger(champ_keynumToModEnvHold)) * 0.000833333f /* 1:1200 */;
        v_noteToDecay = static_cast<float>(voiceParam->getInteger(champ_keynumToModEnvDecay)) * 0.000833333f /* 1:1200 */;
    }
    else
    {
        v_timeDelay = voiceParam->getFloat(champ_delayVolEnv) * _chunkRate;
        v_timeAttack = voiceParam->getFloat(champ_attackVolEnv) * _chunkRate;
        v_timeHold = voiceParam->getFloat(champ_holdVolEnv) * _chunkRate;
        v_timeDecay = voiceParam->getFloat(champ_decayVolEnv) * _chunkRate;
        v_levelSustain = voiceParam->getFloat(champ_sustainVolEnv);
        v_timeRelease = voiceParam->getFloat(champ_releaseVolEnv) * _chunkRate;
        v_noteToHold = static_cast<float>(voiceParam->getInteger(champ_keynumToVolEnvHold)) * 0.000833333f /* 1:1200 */;
        v_noteToDecay = static_cast<float>(voiceParam->getInteger(champ_keynumToVolEnvDecay)) * 0.000833333f /* 1:1200 */;
    }

    // Duration of a quick release
    if (_quickRelease)
        v_timeRelease = static_cast<quint32>(0.04f * _chunkRate);

    // Duration of the attack for the release mode
    if (_currentPhase == phase6quickAttack)
        v_timeAttack = static_cast<quint32>(0.04f * _chunkRate);

    // Beginning of the release phase?
    if (release && _currentPhase != phase7off && _currentPhase != phase6release && _currentPhase != phase6quickAttack)
    {
        _currentPhase = phase6release;
        _currentChunk = 0;
    }

    // Compute the sustain level
    float levelSustain = _isMod ?
                (1.f - 0.01f * v_levelSustain) : // percentage
                FastMaths::fastPow10(-0.05f * v_levelSustain); // decrease in dB

    // Update hold / decay time and volume according to the key
    quint32 timeHold = static_cast<quint32>(v_timeHold * FastMaths::fastPow2(v_noteToHold * static_cast<float>(60 - note)));
    quint32 timeDecay = static_cast<quint32>(v_timeDecay * FastMaths::fastPow2(v_noteToDecay * static_cast<float>(60 - note)));

    // Avancement
    bool end = false;
    quint32 chunk = 0;
    quint32 duration = 0;
    float coef;

    while (chunk < chunkCount)
    {
        switch (_currentPhase)
        {
        case phase1delay:
            // Number of remaining points in the phase
            duration = _currentChunk < v_timeDelay ? v_timeDelay - _currentChunk : 0;
            if (duration <= chunkCount - chunk)
            {
                _currentPhase = phase2attack;
                _currentChunk = 0;
            }
            else
            {
                duration = chunkCount - chunk;
                _currentChunk += duration;
            }

            for (quint32 i = 0; i < duration; i++)
                data[chunk++] = 0;
            break;
        case phase2attack:
            // Number of remaining points in the phase
            duration = _currentChunk < v_timeAttack ? v_timeAttack - _currentChunk : 0;
            if (duration <= chunkCount - chunk)
            {
                _currentPhase = phase3hold;
                _currentChunk = 0;
            }
            else
            {
                duration = chunkCount - chunk;
                _currentChunk += duration;
            }

            if (_isMod)
            {
                // Convex attack
                float stiffness = 0.000001f;
                coef = static_cast<float>(qPow(static_cast<double>(stiffness), 0.4 / v_timeAttack));
                float correction = static_cast<float>(qPow(static_cast<double>(stiffness), 0.4));
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue = 1.0f - coef * (1.0f - _lastValue + stiffness + correction) + stiffness + correction;
                    data[chunk++] = gain * _lastValue;
                }
            }
            else
            {
                // Linear amplitude => convex attack (dB)
                coef = 1.f / v_timeAttack; // Target is 1.f
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue += coef;
                    data[chunk++] = gain * _lastValue;
                }
            }
            break;
        case phase3hold:
            // Number of remaining points in the phase
            duration = _currentChunk < timeHold ? timeHold - _currentChunk : 0;
            if (duration <= chunkCount - chunk)
            {
                _currentPhase = phase4decay;
                _currentChunk = 0;
            }
            else
            {
                duration = chunkCount - chunk;
                _currentChunk += duration;
            }

            _lastValue = 1;
            for (quint32 i = 0; i < duration; i++)
                data[chunk++] = gain;
            break;
        case phase4decay:
            // Number of remaining points in the phase
            duration = _currentChunk < timeDecay ? timeDecay - _currentChunk : 0;
            if (duration <= chunkCount - chunk)
            {
                _currentPhase = phase5sustain;
                _currentChunk = 0;
            }
            else
            {
                duration = chunkCount - chunk;
                _currentChunk += duration;
            }

            if (_isMod)
            {
                // Linear decay
                coef = -1.f / timeDecay;
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue += coef;
                    if (_lastValue < levelSustain)
                        _lastValue = levelSustain;
                    data[chunk++] = gain * _lastValue;
                }
            }
            else
            {
                // Exponential decay
                coef = static_cast<float>(qPow(0.00001585 / (1. - static_cast<double>(levelSustain) + 0.00001585), 1. / timeDecay));
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue = (_lastValue - levelSustain) * coef + levelSustain;
                    data[chunk++] = gain * _lastValue;
                }
            }
            break;
        case phase5sustain:
            // Number of values
            duration = chunkCount - chunk;
            _lastValue = levelSustain;
            for (quint32 i = 0; i < duration; i++)
                data[chunk++] = gain * _lastValue;
            break;
        case phase6quickAttack:
            // Number of remaining points in the phase
            duration = _currentChunk < v_timeAttack ? v_timeAttack - _currentChunk : 0;
            if (duration > chunkCount - chunk)
                duration = chunkCount - chunk;

            coef = _quickAttackTarget / v_timeAttack;
            for (quint32 i = 0; i < duration; i++)
            {
                _lastValue = (_currentChunk + i + 1) * coef;
                data[chunk++] = gain * _lastValue;
            }

            if (duration == 0)
            {
                _currentPhase = phase6release;
                _currentChunk = 0;
            }
            else
                _currentChunk += duration;
            break;
        case phase6release:
            // Number of remaining points in the phase
            duration = _currentChunk < v_timeRelease ? v_timeRelease - _currentChunk : 0;
            if (duration <= chunkCount - chunk)
            {
                _currentPhase = phase7off;
                _currentChunk = 0;
            }
            else
            {
                duration = chunkCount - chunk;
                _currentChunk += duration;
            }

            if (_isMod)
            {
                // Linear decay
                coef = -1.f / v_timeRelease;
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue += coef;
                    if (_lastValue < 0)
                        _lastValue = 0;
                    data[chunk++] = gain * _lastValue;
                }
            }
            else
            {
                // Exponential decay
                coef = static_cast<float>(qPow(0.00001585, 1. / v_timeRelease));
                for (quint32 i = 0; i < duration; i++)
                {
                    _lastValue *= coef;
                    data[chunk++] = gain * _lastValue;
                }
            }
            break;
        case phase7off:
            // Number of values
            duration = chunkCount - chunk;
            _lastValue = 0;
            for (quint32 i = 0; i < duration; i++)
                data[chunk++] = 0;

            // End
            end = true;
        }
    }
    return end;
}

void EnveloppeVol::quickRelease()
{
    // Stopped by an exclusive class: very short release
    _quickRelease = true;
    _currentPhase = phase6release;
    _currentChunk = 0;
}

void EnveloppeVol::quickAttack()
{
    // Start of a sample in mode "release"
    if (_currentPhase <= phase5sustain)
    {
        _currentPhase = phase6quickAttack;
        _currentChunk = 0;
        _quickAttackTarget = _lastValue;
    }
}
