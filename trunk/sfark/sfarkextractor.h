/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) Jeff Glatt                                              **
**                2014 Davy Triponney                                     **
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

#ifndef SFARKEXTRACTORV1_H
#define SFARKEXTRACTORV1_H

#include "sfarkfilemanager.h"

class SfArkInfo;
class PackItem;

class SfArkExtractor
{
public:
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

    SfArkExtractor(const char * fileName);
    ~SfArkExtractor();
    SfArkError extract(char *&data, int &size);

private:
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

    int extractSfArk(const char *inputFile, const char *outputFile);
    int openSfarkFile(const char *name);
    int createSfFile(const void * name);
    int setSfarkFilePos(long offset);
    int readSfarkFile(void *ptr, unsigned int count);
    int writeToFontFile(const void * ptr, unsigned int count);
    void schur(unsigned int dataSize, qint32 * dest);
    void lpcDecode(const qint32 * array, unsigned int frameSize, unsigned int numFrames, const qint16 * src, qint16 * dest);
    void lpcClear();
    void lpcUnpack(unsigned int dataSizeInWords, quint32 maskVal);
    int bitRegisterFill();
    int bitRead(register unsigned char bitsToRead);
    unsigned char bitReadFlag();
    int bitReadBuf(unsigned char * bufptr, unsigned int bytesToRead);
    void addToChksum(unsigned int dataSizeInWords);
    int uncompressTurbo(unsigned int dataSizeInWords);
    int uncompressFast(unsigned int dataSizeInWords);
    int readSfarkByte();
    unsigned int readSfarkBits(unsigned short numOfBits);
    void flushToOutBuf();
    unsigned char decodeData(unsigned int * dataRet, PackItem * arrayPtr, int encodeCnt);
    unsigned int flushPackItemsToOutBuf(unsigned int len);
    unsigned int decompBlock3(unsigned int blockSize);
    unsigned int decompBlock2(unsigned int blockSize);
    unsigned int decompBlock1(unsigned int len);
    int decompLspackBlock();
    int assembleParts();
    void getTempPartName(char partNum);
    void initV1vars();
    int seekNextLspackPart();
    void addToCrc32(unsigned int count);
    int SfarkExtract();
    short getBitPackCnt(short prevVal);
    int unpackBitData(unsigned int dataSizeInWords);
    int SfarkBeginExtract(const void *sfontName);
    int loadSfarkHeader();
    void cleanFiles();
    int skipEmbeddedText();
    int SfarkOpen(const char *sfarkName);
    void closeFile(int fh);

    SfArkInfo * _sfArkInfo;
    SfArkFileManager _fileManager;
    SfArkError _error;
};

#endif // SFARKEXTRACTORV1_H
