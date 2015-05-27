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

/* Warning: this file has to be compiled with no SSE and no optimization ! */

#ifndef SFARKEXTRACTOR1_H
#define SFARKEXTRACTOR1_H

#include "abstractextractor.h"

class SfArkInfo;
class PackItem;

class SfArkExtractor1: public AbstractExtractor
{
public:
    SfArkExtractor1(const char * fileName);
    virtual ~SfArkExtractor1();
    virtual void extract();
    virtual bool getData(char *&data, qint32 &size);

    SfArkExtractor1(const char * fileName, bool &isVersion1);

private:
    enum SfArkError
    {
        SFARKERR_OK             =  0,   // Success
        SFARKERR_APP            = -1,   // Program error
        SFARKERR_MEM            = -2,   // Need more RAM
        SFARKERR_BADHDR         = -3,   // Corrupt header
        SFARKERR_NOID           = -4,   // ID not found. File is corrupt or not a compressed soundfont
        SFARKERR_BADHDRSUM      = -5,   // Header checksum fail
        SFARKERR_OLDVERS        = -6,   // Encrypted file
        SFARKERR_UNKCOMP        = -7,   // Unknown compression
        SFARKERR_BADCOMP        = -8,   // Bad compressed data
        SFARKERR_BADLEN         = -9,   // Bad bit-packed block length
        SFARKERR_BADUNPACK      = -10,  // Bad bit unpacking
        SFARKERR_BADCNT         = -11,  // Bad encode count
        SFARKERR_BADSHIFTCNT    = -12,  // Bad shift position
        SFARKERR_CHKSUM         = -13,  // Data checksum isn't correct
        SFARKERR_SAVE           = -14,  // Can't save data to soundfont
        SFARKERR_SFARKOPEN      = -15,  // Can't open compressed file
        SFARKERR_SFOPEN         = -16,  // Can't create soundfont file
        SFARKERR_POS            = -17,  // Can't set file position
        SFARKERR_SFARKREAD      = -18   // Can't read from compressed file
    };

    enum RunState
    {
        SFARK_DONE      = 0,
        SFARK_BITPACK   = 2,
        SFARK_LZIP      = 3
    };

    enum Flags
    {
        SFARK_OUT_OPEN  = 1,
        SFARK_IN1_OPEN  = 2,
        SFARK_IN2_OPEN  = 4,
        SFARK_TEMP_MADE = 8
    };

    qint32 extractSfArk(const char *inputFile, const char *outputFile);
    qint32 openSfarkFile(const char *name);
    qint32 createSfFile(const void * name);
    qint32 setSfarkFilePos(long offset);
    qint32 readSfarkFile(void *ptr, quint32 count);
    qint32 writeToFontFile(const void * ptr, quint32 count);
    void schur(quint32 dataSize, qint32 * dest);
    void lpcDecode(const qint32 * array, quint32 frameSize, quint32 numFrames, const qint16 * src, qint16 * dest);
    void lpcClear();
    void lpcUnpack(quint32 dataSizeInWords, quint32 maskVal);
    qint32 bitRegisterFill();
    qint32 bitRead(unsigned char bitsToRead);
    unsigned char bitReadFlag();
    qint32 bitReadBuf(unsigned char * bufptr, quint32 bytesToRead);
    void addToChksum(quint32 dataSizeInWords);
    qint32 uncompressTurbo(quint32 dataSizeInWords);
    qint32 uncompressFast(quint32 dataSizeInWords);
    qint32 readSfarkByte();
    quint32 readSfarkBits(quint16 numOfBits);
    void flushToOutBuf();
    unsigned char decodeData(quint32 * dataRet, PackItem * arrayPtr, qint32 encodeCnt);
    quint32 flushPackItemsToOutBuf(quint32 len);
    quint32 decompBlock3(quint32 blockSize);
    quint32 decompBlock2(quint32 blockSize);
    quint32 decompBlock1(quint32 len);
    qint32 decompLspackBlock();
    qint32 assembleParts();
    void getTempPartName(char partNum);
    void initV1vars();
    qint32 seekNextLspackPart();
    void addToCrc32(quint32 count);
    qint32 SfarkExtract();
    qint16 getBitPackCnt(qint16 prevVal);
    qint32 unpackBitData(quint32 dataSizeInWords);
    qint32 SfarkBeginExtract(const void *sfontName);
    qint32 loadSfarkHeader();
    void cleanFiles();
    qint32 skipEmbeddedText();
    qint32 SfarkOpen(const char *sfarkName);
    void closeFile(qint32 fh);

    SfArkInfo * _sfArkInfo;
    SfArkFileManager _fileManager;
    SfArkError _error;
};

#endif // SFARKEXTRACTOR1_H
