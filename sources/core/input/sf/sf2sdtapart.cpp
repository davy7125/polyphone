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

#include "sf2sdtapart.h"
#include <QDataStream>
#include "sound.h"

Sf2SdtaPart::Sf2SdtaPart() :
    _isValid(false),
    _sdtaSize(0),
    _smplSize(0),
    _sm24Size(0),
    _startSmplOffset(0),
    _startSm24Offset(0),
    _sample24bits(0)
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
        in >> sdta._smplSize;

        // Keep the position of "smpl" and skip the block
        sdta._startSmplOffset = 20;
        skipData(in, sdta._smplSize.value);

        // Block sm24?
        {
            char buffer[4];
            if (in.readRawData(buffer, 4) != 4)
                return in;
            block = QString::fromLatin1(buffer, 4);
        }
        if (block == "sm24")
        {
            // Size of the block "sm24"
            in >> sdta._sm24Size;

            // Keep the position of "sm24" and skip the block
            sdta._startSm24Offset = 20 + sdta._smplSize.value + 8;
            skipData(in, sdta._sm24Size.value);
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

quint32 Sf2SdtaPart::prepareBeforeWritingData()
{
    // Constant strings
    memcpy(_LIST, "LIST", 4);
    memcpy(_sdta, "sdta", 4);

    // Sizes
    _smplSize.value = 0;
    _sm24Size.value = 0;
    foreach (Sound * sound, _sounds)
        _smplSize.value += 2 * (sound->getUInt32(champ_dwLength) + 46); // Extra '0' required by the format
    _sdtaSize.value = _smplSize.value + 12;

    if (_sample24bits)
    {
        _sm24Size.value = _smplSize.value / 2;
        _sm24Size.value += (_sm24Size.value % 2); // Even size
        _sdtaSize.value += _sm24Size.value + 8;
    }

    return 8 + _sdtaSize.value;
}

QDataStream & operator << (QDataStream &out, Sf2SdtaPart &sdta)
{
    // "LIST"
    if (out.writeRawData(sdta._LIST, 4) != 4)
        return out;

    // Size of the section "pdta"
    out << sdta._sdtaSize;

    // "sdta"
    if (out.writeRawData(sdta._sdta, 4) != 4)
        return out;

    // "smpl" with its size
    if (out.writeRawData("smpl", 4) != 4)
        return out;
    out << sdta._smplSize;

    // 16-bit part
    quint32 sampleLength;
    qint16 * data16;
    quint8 * data24;
    AttributeValue val;
    val.dwValue = sdta._position + 5 * 4;
    foreach (Sound * sound, sdta._sounds)
    {
        sound->getData(sampleLength, data16, data24, false, false);
        sampleLength *= sizeof(qint16);
        if (out.writeRawData((char*)data16, sampleLength) != sampleLength)
            return out;
        sound->set(champ_dwStart16, val);

        // Extra 46 values
        for (int i = 0; i < 46; i++)
            out.writeRawData("\0\0", 2);

        val.dwValue += sampleLength + 46 * 2;
    }

    // Possible extra 8-bit part
    if (sdta._sample24bits)
    {
        // "sm24" with its size
        if (out.writeRawData("sm24", 4) != 4)
            return out;
        out << sdta._sm24Size;

        val.dwValue += 2 * 4;
        foreach (Sound * sound, sdta._sounds)
        {
            sound->getData(sampleLength, data16, data24, false, false);
            if (out.writeRawData((char*)data24, sampleLength) != sampleLength)
                return out;
            sound->set(champ_dwStart24, val);

            // Extra 46 values
            for (int i = 0; i < 46; i++)
                out.writeRawData("\0", 1);

            val.dwValue += sampleLength + 46;
        }

        // Possible ending 0 (for an even number of bytes)
        if (val.dwValue % 2)
            out.writeRawData("\0", 1);
    }

    sdta._isValid = true;
    return out;
}
