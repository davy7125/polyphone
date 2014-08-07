/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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
#include <QMessageBox>

DWORD freadSize(QDataStream * stream)
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
    return (((DWORD) b0) << 24) | (((DWORD) b1) << 16) | (((DWORD) b2) << 8) | b3;
}

DWORD readDWORD(const char *chaine, int pos)
{
    unsigned char b0, b1, b2, b3;
    b3 = chaine[pos];
    b2 = chaine[pos+1];
    b1 = chaine[pos+2];
    b0 = chaine[pos+3];
    // Assuming file is little-endian.
    // for big endian, swap the order to b3...b0
    return (((DWORD) b0) << 24) | (((DWORD) b1) << 16) | (((DWORD) b2) << 8) | b3;
}
WORD readWORD(const char *chaine, int pos)
{
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    return (((WORD) b0) << 8) | b1;
}
SfVersionTag readSFVersionTag(const char *chaine, int pos)
{
    SfVersionTag Version;
    unsigned char b0, b1;
    b1 = chaine[pos];
    b0 = chaine[pos+1];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    Version.wMajor = (((WORD) b0) << 8) | b1;
    b1 = chaine[pos+2];
    b0 = chaine[pos+3];
    // Assuming file is little-endian.
    // for big endian, swap the order to b1...b0
    Version.wMinor = (((WORD) b0) << 8) | b1;
    return Version;
}
BYTE readBYTE(const char *chaine, int pos)
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
    b1 = chaine[pos];
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
    MonSFMod.Index = WORD(b1 & 0x7F);
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
    MyAmount.wAmount = (((WORD) b0) << 8) | b1;
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

SfVersionTag create_Version(WORD wMajor, WORD wMinor)
{
    SfVersionTag Version;
    Version.wMajor = wMajor;
    Version.wMinor = wMinor;
    return Version;
}

QString decrementerQstr(QString chaine)
{
    int num;
    sscanf(chaine.toStdString().c_str(), "%d", &num);
    num--;
    char str[20];
    sprintf(str, "%d", num);
    return QString(str);
}

void MESSAGE(int iVal)
{
    char T[20];
    sprintf(T, "%d", iVal);
    QMessageBox::information(NULL, "pour info", T);
}
void MESSAGE(double dVal)
{
    char T[40];
    sprintf(T, "%f", dVal);
    QMessageBox::information(NULL, "pour info", T);
}
void MESSAGE(QString qStr)
{
    QMessageBox::information(NULL, "pour info", qStr);
}
void MESSAGE(char * text)
{
    QMessageBox::information(NULL, "pour info", QString::fromUtf8(text));
}
void MESSAGE(EltID id)
{
    QMessageBox::information(NULL, "pour info", id.toString());
}
void MESSAGE(SFModulator sfMod)
{
    char str[200];
    sprintf(str, "Type: %d\nDirection: %d\nPolarité: %d\nCC: %d\nIndex: %d", sfMod.Type, sfMod.D, sfMod.P, sfMod.CC, sfMod.Index);
    QMessageBox::information(NULL, "pour info", QString::fromUtf8(str));
}
