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

#include "sfarkglobal.h"

SfArkInfo::SfArkInfo() :
    WorkBuffer1(NULL),
    WorkBuffer2(NULL),
    InputFileHandle(-1),
    OutputFileHandle(-1),
    FileUncompSize(0),
    FileChksum(0),
    LeadingPadUncompSize(0),
    BitPackEndOffset(0),
    RunningUncompSize(0),
    RunningChksum(0),
    NumBitsInRegister(0),
    BitRegister(0),
    InputBufIndex(0),
    CompressType(0),
    RunState(0),
    Percent(0),
    Flags(0)
{
    // Get buffers needed for in-memory decompression. The largest "compressed block"
    // in an sfArk V2 file decompresses to 262144 bytes max
    WorkBuffer1 = new unsigned char [262144];
    WorkBuffer2 = new unsigned char [262144];

    for (int i = 0; i < 262144; i++)
        WorkBuffer1[i] = WorkBuffer2[i] = 0;

    for (int i = 0; i < 2048; i++)
        InputBuf[i] = 0;
}

SfArkInfo::~SfArkInfo()
{
    delete WorkBuffer1;
    delete WorkBuffer2;
}


SfArkInfo_v1::SfArkInfo_v1() :
    PartFileHandle(-1),
    OutbufPtr(NULL),
    DecompBuf(NULL),
    AppFontName(NULL),
    PackItemArray(NULL),
    CurPackItemArray(NULL),
    VarDecodeDWord(0),
    VarDecodeByteCnt(0),
    EncodeCnt(0),
    PrevBytesInDecompBuf(0),
    BytesInDecompBuf(0),
    FirstEncodeCnt(0),
    SecondEncodeCnt(0),
    PartNum(0),
    BlockFlags(0)
{}

PackItem::PackItem() :
    EncodeCnt(0),
    BitShiftAmt(0)
{
    u.NextItemArray = NULL;
    u.Data1 = 0;
}

SfArkInfo_v2::SfArkInfo_v2() :
    BitFramePackMethod(0),
    PrevUnpack2EncodeCnt(0),
    PrevUnpack3EncodeCnt(0),
    CalcShiftEncodeCnt(0),
    ShiftEncodeCnt(0),
    NumShortsInLpcBlock(0),
    NumShortsInLpcFrame(0),
    Unpack2EncodeLimit(0),
    Unpack3EncodeLimit(0),
    LpcCurrHistNum(0)
{
    for (int i = 0; i < 20; i++)
        PrevDataShiftVal[i] = 0;
}

SfArkInfo_LPC::SfArkInfo_LPC()
{
    for (int i = 0; i < 129*4; i++)
        AcHist[i] = 0;
    for (int i = 0; i < 256; i++)
        History[i] = 0;
    for (int i = 0; i < 128; i++)
        DecodeHist[i] = 0;
}
