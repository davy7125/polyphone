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


#ifndef TIMBRERENDERING_H
#define TIMBRERENDERING_H

#include "basetypes.h"
#include "timbremodel.h"

// Check of the model by the sound engine: a key of an instrument is rendered offline at a velocity, then the harmonics
// of the sustained part are measured (power of the two channels added)
class TimbreRendering
{
public:
    static const double SAMPLE_RATE;

    // Spectrum heard, empty in case of error (can be called by another thread if the soundfont doesn't change)
    static TimbreSpectrum render(EltID idInst, int key, int velocity, double expectedFrequency, QString &error);
};

#endif // TIMBRERENDERING_H
