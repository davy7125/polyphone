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

#include "oscsinus.h"
#include "qmath.h"


OscSinus::OscSinus(qint32 sampleRate, double delay) :
    _sampleRate(sampleRate),
    _previousFreq(-1),
    _delayTime(delay * sampleRate),
    _currentDelay(0)
{
}

// Générateur Gordon-Smith
void OscSinus::getSinus(float * data, qint32 len, double freq)
{
    // Attente
    qint64 total = qMin(_delayTime - _currentDelay, len);
    for (int i = 0; i < total; i++)
        data[i] = 0;
    _currentDelay += total;

    // Sinus
    if (total != len)
    {
        if (_previousFreq == -1)
        {
            // Initialisation du système
            _previousFreq = freq;
            computeEpsilon(freq, _theta, _epsilon);
            _posPrec = qSin(-_theta);
            _posPrecQuad = qCos(-_theta);
        }
        if (_previousFreq != freq)
        {
            double theta2, epsilon2;
            computeEpsilon(freq, theta2, epsilon2);
            _previousFreq = freq;

            double progEpsilon;
            for (int i = total; i < len; i++)
            {
                progEpsilon = (double)(len - i) / (len - total) * _epsilon
                        + (double)(i - total) / (len - total) * epsilon2;
                _posPrecQuad -= progEpsilon * _posPrec;
                _posPrec     += progEpsilon * _posPrecQuad;
                data[i] = _posPrec;
            }

            // Mise à jour valeurs
            _theta = theta2;
            _epsilon = epsilon2;
            _previousFreq = freq;
        }
        else
        {
            for (int i = total; i < len; i++)
            {
                _posPrecQuad -= _epsilon * _posPrec;
                _posPrec     += _epsilon * _posPrecQuad;
                data[i] = _posPrec;
            }
        }
    }
}

void OscSinus::computeEpsilon(double freq, double &theta, double &epsilon)
{
    theta = 2. * M_PI * freq / _sampleRate;
    epsilon = 2. * qSin(theta / 2.);
}
