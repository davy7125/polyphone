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

#ifndef SYNTHCORECONFIG_H
#define SYNTHCORECONFIG_H

class SynthCoreConfig
{
public:
    SynthCoreConfig() :
        choLevel(0),
        choDepth(0),
        choFrequency(0),
        revLevel(0),
        revSize(0),
        revWidth(0),
        revDamping(0),
        masterGain(0),
        referencePitch(4400),
        temperamentRelativeKey(0)
    {
        for (int i = 0; i < 12; i++)
            temperament[i] = 0;
    }

    // Effects, from 0 to 100
    int choLevel;
    int choDepth;
    int choFrequency;

    int revLevel;
    int revSize;
    int revWidth;
    int revDamping;

    // Master gain (dB) and reference pitch (0.1 Hz)
    double masterGain;
    int referencePitch;

    // Fine tune in cents of each key from C to B, relative to the key temperamentRelativeKey
    float temperament[12];
    int temperamentRelativeKey;
};

#endif // SYNTHCORECONFIG_H
