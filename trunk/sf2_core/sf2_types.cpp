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

#include "sf2_types.h"
#include "sound.h"
#include <QDataStream>

quint32 freadSize(QDataStream * stream)
{
    unsigned char b0, b1, b2, b3;
    if (stream->readRawData((char*)&b3, 1) != 1 ||
        stream->readRawData((char*)&b2, 1) != 1 ||
        stream->readRawData((char*)&b1, 1) != 1 ||
        stream->readRawData((char*)&b0, 1) != 1)
    {
        return 0;
    }
    // Assuming file is little-endian.
    // for big endian, swap the order to b3...b0
    return (((quint32) b0) << 24) | (((quint32) b1) << 16) | (((quint32) b2) << 8) | b3;
}

quint32 readDWORD(const char *chaine, int pos)
{
    unsigned char b0, b1, b2, b3;
    b3 = chaine[pos];
    b2 = chaine[pos+1];
    b1 = chaine[pos+2];
    b0 = chaine[pos+3];
    // Assuming file is little-endian.
    // for big endian, swap the order to b3...b0
    return (((quint32) b0) << 24) | (((quint32) b1) << 16) | (((quint32) b2) << 8) | b3;
}
quint16 readWORD(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return (((quint16) b0) << 8) | b1;
}
SfVersionTag readSFVersionTag(const char *chaine, int pos)
{
    SfVersionTag Version;
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    Version.wMajor = (((quint16) b0) << 8) | b1;
    b1 = chaine[pos+2];
    b0 = chaine[pos+3];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    Version.wMinor = (((quint16) b0) << 8) | b1;
    return Version;
}
quint8 readQUINT8(const char *chaine, int pos)
{
    unsigned char b0;
    b0 = chaine[pos];
    return b0;
}
short readSHORT(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return (((short) b0) << 8) | b1;
}
SFSampleLink readSFSL(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos] & 0xEF; // Remove sf3 flag if any
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return SFSampleLink((b0 << 8) | b1);
}
Champ readSFGenerator(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return Champ((b0 << 8) | b1);
}
SFModulator readSFModulator(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    SFModulator MonSFMod;
    MonSFMod.Type = (int) b0 >> 2;
    MonSFMod.P = bool((b0 >> 1) & 1);
    MonSFMod.D = bool(b0 & 1);
    MonSFMod.CC = bool(b1 >> 7);
    MonSFMod.Index = quint16(b1 & 0x7F);
    return MonSFMod;
}
SFTransform readSFTransform(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return SFTransform((b0 << 8) | b1);
}
genAmountType readGenAmountType(const char *chaine, int pos)
{
    genAmountType MyAmount;
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    MyAmount.wAmount = (((quint16) b0) << 8) | b1;
    return MyAmount;
}
void readbloc(char *bloc, const char * bloc_data, int pos)
{
    int i = -1;
    while (i++ < 4) {bloc[i] = bloc_data[pos+i];}
    bloc[i-1] = '\0';
}
char *readdata(char *data, const char * bloc_data, int pos, int length)
{
    int i = -1;
    while (i++ < length) {data[i] = bloc_data[pos+i];}
    data[length] = '\0';
    return data;
}

SfVersionTag create_Version(quint16 wMajor, quint16 wMinor)
{
    SfVersionTag Version;
    Version.wMajor = wMajor;
    Version.wMinor = wMinor;
    return Version;
}

QString decrementerQstr(QString chaine)
{
    return QString::number(chaine.toInt() - 1);
}
