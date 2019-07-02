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

#ifndef INFOSOUND_H
#define INFOSOUND_H

#include "basetypes.h"

class InfoSound
{
public:
    InfoSound()
    {
        // Initialization
        reset();
    }

    void reset()
    {
        dwStart = 0;
        dwStart2 = 0;
        dwLength = 0;
        dwSampleRate = 0;
        wChannels = 0;
        wBpsFile = 0;
        dwStartLoop = 0;
        dwEndLoop = 0;
        dwRootKey = 60; // Middle C
        wChannel = 0;
        iFineTune = 0;
        pitchDefined = false;
    }

    quint32 dwStart;
    quint32 dwStart2; // for sf2 : 24-bit data are stored on 2 blocs
    quint32 dwLength;
    quint32 dwSampleRate;
    quint16 wChannels;
    quint16 wBpsFile; // number of bytes for a value
    quint32 dwStartLoop;
    quint32 dwEndLoop;
    quint32 dwRootKey;
    quint16 wChannel;
    int iFineTune; // from -100 (-1 semi tone) to 100 (+1 semi tone)
    bool pitchDefined;
};

#endif // INFOSOUND_H
