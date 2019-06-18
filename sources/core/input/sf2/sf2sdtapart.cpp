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

#include "sf2sdtapart.h"
#include <QDataStream>

Sf2SdtaPart::Sf2SdtaPart() :
    _isValid(false),
    _sdtaSize(0),
    _startSmplOffset(0),
    _startSm24Offset(0)
{

}

void skipData(QDataStream &in, unsigned int number)
{
    // number can be more than the maximum of an int
    int bigSkip = number / 0x7FFFFFFF;
    for (int i = 0; i < bigSkip; i++)
        in.skipRawData(0x7FFFFFFF);
    in.skipRawData(number % 0x7FFFFFFF);
}

QDataStream & operator >> (QDataStream &in, Sf2SdtaPart &sdta)
{
    // 4 char, should be "LIST"
    if (in.readRawData(sdta._LIST, 4) != 4)
        return in;

    // Size of the section "sdta"
    in >> sdta._sdtaSize;

    // 4 char, should be "sdta"
    if (in.readRawData(sdta._sdta, 4) != 4)
        return in;

    // Next block: can be directly the samples or smpl
    QString block;
    {
        char buffer[4];
        if (in.readRawData(buffer, 4) != 4)
            return in;
        block = QString::fromLatin1(buffer, 4);
    }
    if (block == "smpl")
    {
        // Size of the block "smpl"
        quint32Reversed smplSize;
        in >> smplSize;

        // Keep the position of "smpl" and skip the block
        sdta._startSmplOffset = 20;
        skipData(in, smplSize.value);

        // Block sm24?
        {
            char buffer[4];
            if (in.readRawData(buffer, 4) != 4)
                return in;
            block = QString::fromLatin1(buffer, 4);
        }
        if (block == "sm24")
        {
            // Size of the block "smpl"
            quint32Reversed sm24Size;
            in >> sm24Size;

            // Keep the position of "sm24" and skip the block
            sdta._startSm24Offset = 20 + smplSize.value + 8;
            skipData(in, sm24Size.value);
        }
        else
        {
            // No "sm24" block, we go back
            QIODevice * fi = in.device();
            fi->seek(fi->pos() - 4);

            // No "sm24" position
            sdta._startSm24Offset = 0;
        }
    }
    else
    {
        // Memorize the positions and skip the part
        sdta._startSmplOffset = 12;
        sdta._startSm24Offset = 0;
        skipData(in, sdta._sdtaSize.value - 8);
    }

    sdta._isValid = true;
    return in;
}
