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

#ifndef OSCSINUS_H
#define OSCSINUS_H

#include "sound.h"

class OscSinus
{
public:
    OscSinus(qint32 sampleRate, double delay = 0);
    void getSinus(float *data, qint32 len, double freq);

private:
    // Attributs privés
    qint32 _sampleRate;
    double _previousFreq;
    qint32 _delayTime, _currentDelay;
    double _posPrec, _posPrecQuad;
    double _theta, _epsilon;

    // Méthode privée
    void computeEpsilon(double freq, double &theta, double &epsilon);
};

#endif // OSCSINUS_H
