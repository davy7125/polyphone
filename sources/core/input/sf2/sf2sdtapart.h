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

#ifndef SF2SDTAPART_H
#define SF2SDTAPART_H

#include "basetypes.h"

class Sf2SdtaPart
{
public:
    Sf2SdtaPart();

    bool _isValid;

    char _LIST[4]; // Should be "LIST"
    quint32Reversed _sdtaSize; // Size of the section sdta
    char _sdta[4]; // Should be "sdta"
    unsigned int _startSmplOffset; // Relative position of the first sample in the section "smpl" (absolute is 20 + infoSize + this value)
    unsigned int _startSm24Offset; // Same for the section "sm24"
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2SdtaPart &sdta);
//QDataStream & operator << (QDataStream &out, const Sf2SdtaPart &sdta);

#endif // SF2SDTAPART_H
