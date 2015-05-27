/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) Jeff Glatt                                              **
**                2014-2015 Davy Triponney                                **
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

#ifndef SFARKGLOBAL_H
#define SFARKGLOBAL_H

#include "qglobal.h"
#include "zlib.h"

class SfArkInfo_LPC
{
public:
    SfArkInfo_LPC();

    float   AcHist[129*4];
    qint16  History[256];
    qint16  DecodeHist[128];
};

class SfArkInfo_v2
{
public:
    SfArkInfo_v2();

    short           BitFramePackMethod;
    z_stream        Zip;
    SfArkInfo_LPC   Lpc;
    qint16          PrevDataShiftVal[20];
    quint16         PrevUnpack2EncodeCnt;
    quint16         PrevUnpack3EncodeCnt;
    quint16         CalcShiftEncodeCnt;
    quint16         ShiftEncodeCnt;
    quint16         NumShortsInLpcBlock;	// 4096 or 1024
    quint16         NumShortsInLpcFrame;	// 128 or 8
    quint8          Unpack2EncodeLimit;
    quint8          Unpack3EncodeLimit;
    unsigned char   LpcCurrHistNum;
};

class PackItem
{
public:
    PackItem();

    unsigned char           EncodeCnt;
    unsigned char           BitShiftAmt;
    union
    {
        unsigned short      Data1;
        PackItem *          NextItemArray;
    } u;
};

class SfArkInfo_v1
{
public:
    SfArkInfo_v1();

    int                 PartFileHandle;
    unsigned char *     OutbufPtr;
    unsigned char *     DecompBuf;
    const char *        AppFontName;
    PackItem *          PackItemArray;
    PackItem *          CurPackItemArray;
    unsigned int        VarDecodeDWord;
    unsigned int        VarDecodeByteCnt;
    unsigned int        EncodeCnt;
    unsigned short      PrevBytesInDecompBuf;
    unsigned short      BytesInDecompBuf;
    unsigned int        FirstEncodeCnt;
    unsigned int        SecondEncodeCnt;
    unsigned char       PartNum;
    unsigned char       BlockFlags;
};


class SfArkInfo
{
public:
    SfArkInfo();
    ~SfArkInfo();

    unsigned char * WorkBuffer1;
    unsigned char * WorkBuffer2;
    int             InputFileHandle;
    int             OutputFileHandle;

    // ==========================
    quint32         FileUncompSize;			// Read from sfArk file header
    qint32          FileChksum;				// Read from sfArk file header
    quint32         LeadingPadUncompSize;	// Read from sfArk file header
    quint32         BitPackEndOffset;		// Read from sfArk file header
    // ==========================

    quint32         RunningUncompSize;
    qint32          RunningChksum;
    quint32         NumBitsInRegister;
    quint32         BitRegister;
    unsigned short  InputBufIndex;
    unsigned short  InputBuf[2048];
    unsigned char   CompressType;			// Read from sfArk file header
    unsigned char   RunState;
    unsigned char   Percent;
    unsigned char   Flags;
    SfArkInfo_v2    v2;
    SfArkInfo_v1    v1;
};

#endif // SFARKGLOBAL_H
