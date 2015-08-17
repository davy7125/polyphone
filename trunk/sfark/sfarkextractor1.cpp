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

#include "sfarkextractor1.h"
#include "sfarkglobal.h"
#include "zlib.h"
#include "stdint.h"

static const char * SfArkId = ".sfArk";

SfArkExtractor1::SfArkExtractor1(const char * fileName) : AbstractExtractor(),
    _sfArkInfo(new SfArkInfo)
{
    _error = SFARKERR_OK;
    _error = (SfArkError)SfarkOpen(fileName);
}

SfArkExtractor1::SfArkExtractor1(const char * fileName, bool &isVersion1) : AbstractExtractor(),
    _sfArkInfo(new SfArkInfo)
{
    _error = SFARKERR_OK;

    if (SfarkOpen(fileName) == 0)
        isVersion1 = (_sfArkInfo->CompressType < 3);

    cleanFiles();
}

SfArkExtractor1::~SfArkExtractor1()
{
    delete _sfArkInfo;
}

void SfArkExtractor1::extract()
{
    if (_error == SFARKERR_OK)
    {
        if (!(_error = (SfArkError)SfarkBeginExtract("output.sf2")))
        {
            do
            {
            } while (!(_error = (SfArkError)SfarkExtract()));
        }
    }

    if (_error > SFARKERR_OK)
        _error = SFARKERR_OK;

    cleanFiles();
}

bool SfArkExtractor1::getData(char *&data, qint32 &size)
{
    if (_error == SFARKERR_OK || _error == SFARKERR_CHKSUM) // checksum errors accepted
    {
        data = _fileManager.retrieveData("output.sf2", size);
        return true;
    }

    data = NULL;
    return false;
}


// ===============================================================
// File I/O
// ===============================================================

void SfArkExtractor1::closeFile(qint32 fh)
{
    _fileManager.close(fh);
}

/****************** openSfarkFile() *****************
 * Opens the sfArk file to load.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::openSfarkFile(const char * name)
{
    qint32 fileHandler = _fileManager.openReadOnly(name);
    if (fileHandler < 0)
        return SFARKERR_SFARKOPEN;
    _sfArkInfo->InputFileHandle = fileHandler;
    return 0;
}

/****************** createSfFile() *****************
 * Opens the soundfont file to save.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::createSfFile(const void * name)
{
    qint32 fileHandler = _fileManager.create((const char *)name);
    if (fileHandler < 0)
        return SFARKERR_SFOPEN;
    _sfArkInfo->OutputFileHandle = fileHandler;
    return 0;
}

/****************** setSfarkFilePos() *****************
 * Sets the sfArk file position relative to current
 * position.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::setSfarkFilePos(long offset)
{
    if (!_fileManager.setPos(_sfArkInfo->InputFileHandle, offset))
        return SFARKERR_POS;
    return 0;
}

/****************** readSfarkFile() *****************
 * Reads the specified number of bytes from the sfArk
 * file, and puts them in the supplied buffer.
 *
 * RETURN: The number of bytes read if success (ie, may
 * read less bytes than requested if the end of file),
 * or negative error number.
 */

qint32 SfArkExtractor1::readSfarkFile(void * ptr, quint32 count)
{
    int	result = _fileManager.read(_sfArkInfo->InputFileHandle, (char *)ptr, count);
    if (result == -1)
        return SFARKERR_SFARKREAD;
    return result;
}

/****************** writeToFontFile() *****************
 * Writes the specified number of bytes from the
 * supplied buffer to the soundfont file.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::writeToFontFile(const void * ptr, quint32 count)
{
    if (_fileManager.write(_sfArkInfo->OutputFileHandle, (const char *)ptr, count) != (int)count)
        return SFARKERR_SAVE;
    return 0;
}


// ===============================================================
// LPC decoder
// ===============================================================

void SfArkExtractor1::schur(quint32 dataSize, qint32 * dest)
{
    float		sum;

    sum = _sfArkInfo->v2.Lpc.AcHist[0] + _sfArkInfo->v2.Lpc.AcHist[129] + _sfArkInfo->v2.Lpc.AcHist[(129*2)] + _sfArkInfo->v2.Lpc.AcHist[(129*3)];
    if (sum == 0.0)
        memset(dest, 0, dataSize * sizeof(quint32));
    else
    {
        quint32	i, j;
        float			floatArray[128];
        float			floatArray2[128];

        for (i = 1; i < dataSize + 1; ++i) floatArray[i - 1] = floatArray2[i - 1] = _sfArkInfo->v2.Lpc.AcHist[i] + _sfArkInfo->v2.Lpc.AcHist[129+i] + _sfArkInfo->v2.Lpc.AcHist[(129*2)+i] + _sfArkInfo->v2.Lpc.AcHist[(129*3)+i];

        j = 0;
        for (;;)
        {
            float		sum2;

            sum2 = -floatArray[0] / sum;
            sum = floatArray[0] * sum2 + sum;
            dest[j] = (qint32)(sum2 * 16384.0);
            if (++j >= dataSize) break;
            for (i = 0; i < dataSize - j; ++i)
            {
                floatArray[i] = (sum2 * floatArray2[i]) + floatArray[i + 1];
                floatArray2[i] = (sum2 * floatArray[i + 1]) + floatArray2[i];
            }
        }
    }
}

static void lpcCorrelation(quint32 frameSize, const float * src, quint32 totalFrames, float * dest)
{
    quint32	i;

    while (totalFrames--)
    {
        const float *	ptr;
        float			val;

        val = 0.0;
        i = 0;
        ptr = &src[totalFrames];
        if (frameSize - totalFrames > 15)
        {
            while (i < frameSize - totalFrames - 15)
            {
                val += (src[i] * ptr[i]
                    + src[i+1] * ptr[i + 1]
                    + src[i+2] * ptr[i + 2]
                    + src[i+3] * ptr[i + 3]
                    + src[i+4] * ptr[i + 4]
                    + src[i+5] * ptr[i + 5]
                    + src[i+6] * ptr[i + 6]
                    + src[i+7] * ptr[i + 7]
                    + src[i+8] * ptr[i + 8]
                    + src[i+9] * ptr[i + 9]
                    + src[i+10] * ptr[i + 10]
                    + src[i+11] * ptr[i + 11]
                    + src[i+12] * ptr[i + 12]
                    + src[i+13] * ptr[i + 13]
                    + src[i+14] * ptr[i + 14]
                    + src[i+15] * ptr[i + 15]);
                i += 16;
            }
        }

        while (i < frameSize - totalFrames)
        {
            val += (src[i] * ptr[i]);
            ++i;
        }

        dest[totalFrames] = val;
    }
}

void SfArkExtractor1::lpcDecode(const qint32 * array, quint32 frameSize, quint32 numFrames, const qint16 * src, qint16 * dest)
{
    quint32	i;
    qint32			sum, temp;

    while (numFrames--)
    {
        sum = *src++;

        i = frameSize;
        while (i--)
        {
            temp = _sfArkInfo->v2.Lpc.DecodeHist[i] * array[i];
            if (temp < 0)
                temp = -(-temp >> 14);
            else
                temp >>= 14;
            sum -= temp;
            temp = sum * array[i];
            if (temp < 0)
                temp = -(-temp >> 14);
            else
                temp >>= 14;
            _sfArkInfo->v2.Lpc.DecodeHist[i + 1] = temp + _sfArkInfo->v2.Lpc.DecodeHist[i];
        }

        *dest++ = _sfArkInfo->v2.Lpc.DecodeHist[0] = sum;
    }
}

static void lpcAddAcHist(const float * src, quint32 frameSize, float * dest)
{
    quint32	frameEnd;
    quint32	i;

    frameEnd = frameSize - 1;
    while (frameSize--)
    {
        const float *	ptr;
        float			val;

        val = 0.0;
        ptr = &src[frameSize];
        i = frameEnd - frameSize;
        while (i < frameEnd)
        {
            val += (src[i] * ptr[i]);
            ++i;
        }

        dest[frameSize] += val;
    }
}

void SfArkExtractor1::lpcClear()
{
    _sfArkInfo->v2.LpcCurrHistNum = 0;
    memset(&_sfArkInfo->v2.Lpc, 0, sizeof(_sfArkInfo->v2.Lpc));
}

void SfArkExtractor1::lpcUnpack(quint32 dataSizeInWords, quint32 maskVal)
{
    qint16 *			dest;
    const qint16 *		src;
    const qint16 *		srcEnd;
    quint32		cntToDo;
    quint32		frameSize;

    dest = (qint16 *)_sfArkInfo->WorkBuffer2;
    src = (qint16 *)_sfArkInfo->WorkBuffer1;
    frameSize = _sfArkInfo->v2.NumShortsInLpcFrame;

    srcEnd = &src[dataSizeInWords];
    cntToDo = dataSizeInWords;
    do
    {
        if (cntToDo >= 128)
        {
            quint32	numFrames, i, j;
            quint32	mask;

            mask = maskVal;

            numFrames = 128;

            for (i = 0; i < dataSizeInWords; i += numFrames)
            {
                if (mask & 1)
                {
                    lpcClear();
                    for (j = 0; j < numFrames; ++j) dest[j + i] = src[j + i];
                }
                else
                {
                    qint32		intArray[128];

                    schur(frameSize, &intArray[0]);
                    lpcDecode(&intArray[0], frameSize, numFrames, &src[i], &dest[i]);
                }

                mask >>= 1;

                {
                float		sumtable[256];

                j = numFrames;
                for (j = 0; j < frameSize; ++j) sumtable[j] = (float)_sfArkInfo->v2.Lpc.History[j];

                j = numFrames;
                if (frameSize > j) j = frameSize;
                while (j--) sumtable[frameSize + j] = (float)dest[i + j];
                lpcAddAcHist(&sumtable[0], frameSize + 1, &_sfArkInfo->v2.Lpc.AcHist[129 * _sfArkInfo->v2.LpcCurrHistNum]);

//				for (j = 0; j < numFrames; ++j) sumtable[frameSize + j] = (float)dest[i + j];
                if (++_sfArkInfo->v2.LpcCurrHistNum >= 4) _sfArkInfo->v2.LpcCurrHistNum = 0;
                lpcCorrelation(numFrames, &sumtable[frameSize], frameSize + 1, &_sfArkInfo->v2.Lpc.AcHist[129 * _sfArkInfo->v2.LpcCurrHistNum]);
                }

                for (j = 0; j < frameSize; ++j) _sfArkInfo->v2.Lpc.History[j] = dest[j + i];
            }
        }
        else
            memcpy(dest, src, cntToDo * sizeof(qint16));

        src += dataSizeInWords;
        dest += dataSizeInWords;
        cntToDo -= dataSizeInWords;
    } while (src < srcEnd);
}

// ===============================================================
// Delta compression decoding, and Bit packing decoder
// ===============================================================

static void delta1(qint16 * destptr, qint16 * srcptr, quint32 dataSizeInWords, qint16 * prevVal)
{
    qint16 *	dataend;

    dataend = &srcptr[dataSizeInWords];
    *destptr++ = *prevVal + *srcptr++;
    while (srcptr < dataend)
    {
        *destptr = *(destptr - 1) + *srcptr;
        ++destptr;
        ++srcptr;
    }

    *prevVal = *(destptr - 1);
}

static void delta2(qint16 * dest, qint16 * src, quint32 dataSizeInWords, qint16 * prevVal)
{
    qint16 *	srcptr;
    qint16 *	destptr;

    srcptr = &src[dataSizeInWords - 1];
    destptr = &dest[dataSizeInWords - 1];
    *destptr-- = *srcptr--;
    while (srcptr > src)
    {
        *destptr = ((destptr[1] + srcptr[-1]) >> 1) + srcptr[0];
        --srcptr;
        --destptr;
    }
    *destptr = (destptr[1] >> 1) + srcptr[0];
    *prevVal = destptr[dataSizeInWords - 1];
}

static void delta3(qint16 * dest, qint16 * src, quint32 dataSizeInWords, qint16 * prevVal)
{
    qint16 *	srcEnd;
    qint16		prevVal2;
    qint16		temp;

    prevVal2 = *prevVal;
    srcEnd = &src[dataSizeInWords];
    while (src < srcEnd)
    {
        *dest = prevVal2 + *src;
        if (*src < 0)
            temp = -(-*src >> 1);
        else
            temp = *src >> 1;
        prevVal2 += temp;
        ++src;
        ++dest;
    }

    *prevVal = prevVal2;
}

static quint32 shiftBitsLookup(quint32 val)
{
    quint32	low;
    unsigned char bitN;

    if (val > 1)
    {
        low = 4;
        bitN = 2;
        while (val >= low)
        {
            ++bitN;
            low <<= 1;
        }
        val = bitN;
    }

    return val;
}

static void applyShift(qint16 * dataptr, quint32 dataSizeInWords, quint16 * srcptr)
{
    qint16 *		endptr;

    endptr = &dataptr[dataSizeInWords];
    while (dataptr < endptr)
    {
        short	val;

        if ((val = *srcptr++))
        {
            quint32	i;

            i = 64;
            if (dataptr + i > endptr) i = ((char *)endptr - (char *)dataptr) / sizeof(qint16);
            while (i--) *dataptr++ <<= val;
        }
        else
            dataptr += 64;
    }
}

qint32 SfArkExtractor1::bitRegisterFill()
{
    if (_sfArkInfo->NumBitsInRegister < 16)
    {
        if (_sfArkInfo->InputBufIndex >= sizeof(_sfArkInfo->InputBuf)/sizeof(quint16))
        {
            if (readSfarkFile(_sfArkInfo->InputBuf, sizeof(_sfArkInfo->InputBuf)) <= 0) return SFARKERR_SFARKREAD;
            _sfArkInfo->InputBufIndex = 0;
        }

        _sfArkInfo->BitRegister = _sfArkInfo->InputBuf[_sfArkInfo->InputBufIndex++] | (_sfArkInfo->BitRegister << 16);
        _sfArkInfo->NumBitsInRegister += 16;
    }

    return 0;
}

qint32 SfArkExtractor1::bitRead(unsigned char bitsToRead)
{
    qint32 retVal;

    if (bitRegisterFill()) return 0;

    _sfArkInfo->NumBitsInRegister -= bitsToRead;
    retVal = (_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister) & 0xFFFF;
    _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
    return retVal;
}

unsigned char SfArkExtractor1::bitReadFlag()
{
    return bitRead(1) ? 1 : 0;
}

qint32 SfArkExtractor1::bitReadBuf(unsigned char * bufptr, quint32 bytesToRead)
{
    while (bytesToRead--)
    {
        if (bitRegisterFill()) return SFARKERR_SFARKREAD;

        _sfArkInfo->NumBitsInRegister -= 8;
        *bufptr++ = (unsigned char)(_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister);
        _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
    }

    return 0;
}


/*********************** unpackBitData() ***********************
 * Unpacks (and returns) the next of bit-packed "encode count".
 *
 * prevVal =	The previous count (will be added to the
 *				returned count to create the final count).
 *
 * RETURN: Encode count if success, or negative error number
 * other than -1 or -2).
 */

qint16 SfArkExtractor1::getBitPackCnt(qint16 prevVal)
{
    short		retVal;

    retVal = 0;

    // Find the bit-pack "marker"
    while (!_sfArkInfo->BitRegister)
    {
        retVal += _sfArkInfo->NumBitsInRegister;
        if (_sfArkInfo->InputBufIndex >= sizeof(_sfArkInfo->InputBuf)/sizeof(quint16))
        {
            if (readSfarkFile(_sfArkInfo->InputBuf, sizeof(_sfArkInfo->InputBuf)) <= 0)
bad:			return 0;
            _sfArkInfo->InputBufIndex = 0;
        }

        _sfArkInfo->BitRegister = _sfArkInfo->InputBuf[_sfArkInfo->InputBufIndex++];
        _sfArkInfo->NumBitsInRegister = 16;
    }

    retVal += _sfArkInfo->NumBitsInRegister;
    do
    {
        --_sfArkInfo->NumBitsInRegister;
    } while ((_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister) != 1);

    _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);

    retVal -= (_sfArkInfo->NumBitsInRegister + 1);
    if (retVal)
    {
        if (bitRegisterFill()) goto bad;

        --_sfArkInfo->NumBitsInRegister;
        if ((_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister) & 0xFFFF) retVal = -retVal;
        _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
    }

    return prevVal + retVal;
}


/********************** unpackBitData() *********************
 * Unpacks the next block of bit-packed data. The data is
 * divided up into 256 or 32 16-bit "frames", depending upon
 * whether "turbo" or "fast" method, respectively, was used
 * to create the sfark file. Then those 256 or 32 words are
 * bit-packed into a bitstream of the number of bits of the
 * highest value. In other words, a bitpacked version of the
 * frame is created. Before this is written to the sfark, a
 * bit-packed "encode count" is written, which tells how many
 * bits each word was reduced to. But note that this count is
 * added to the previous frame's count, to arrive at the final
 * value. This is done to try to reduce the size of storing the
 * count itself. Also note the encode count is initially
 * assumed 8 bits.
 *
 * RETURN: 0 if success, or negative error number.
 */

qint32 SfArkExtractor1::unpackBitData(quint32 dataSizeInWords)
{
    quint16 *	dataptr;

    dataptr = (quint16 *)_sfArkInfo->WorkBuffer1;
    while (dataSizeInWords)
    {
        quint32 framesize;

        // We're going to decompress the data into 256 or 32 shorts
        if (_sfArkInfo->CompressType == 4)
            framesize = 256;
        else
            framesize = 32;
        if (framesize > dataSizeInWords) framesize = dataSizeInWords;
        dataSizeInWords -= framesize;

        _sfArkInfo->v2.BitFramePackMethod = getBitPackCnt(_sfArkInfo->v2.BitFramePackMethod);
        if (_sfArkInfo->v2.BitFramePackMethod == SFARKERR_SFARKREAD) goto err;		// NOTE: SFARKERR_SFARKREAD must be a negative value, but not -1 or -2

        if ((unsigned short)_sfArkInfo->v2.BitFramePackMethod > 13)
        {
            if (_sfArkInfo->v2.BitFramePackMethod == 14)
            {
                do
                {
                    if (bitRegisterFill())
err:					return SFARKERR_SFARKREAD;

                    _sfArkInfo->NumBitsInRegister -= 16;
                    *dataptr++ = _sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister;
                    _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
                } while (--framesize);
            }

            else if (_sfArkInfo->v2.BitFramePackMethod == -1)
            {
                do
                {
                    if (bitRegisterFill()) goto err;

                    --_sfArkInfo->NumBitsInRegister;
                    *dataptr++ = -(_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister);
                    _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
                } while (--framesize);
            }

            else if (_sfArkInfo->v2.BitFramePackMethod == -2)
            {
                memset(dataptr, 0, framesize * sizeof(quint16));
                dataptr += framesize;
            }

            else
                return SFARKERR_BADUNPACK;
        }

        else do
        {
            short			result3;
            quint32		oldBits;

            if (bitRegisterFill()) goto err;

            result3 = 0;
            _sfArkInfo->NumBitsInRegister -= (_sfArkInfo->v2.BitFramePackMethod + 1);
            oldBits = _sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister;
            _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
            while (!_sfArkInfo->BitRegister)
            {
                result3 += _sfArkInfo->NumBitsInRegister;
                if (_sfArkInfo->InputBufIndex >= sizeof(_sfArkInfo->InputBuf)/sizeof(quint16))
                {
                    if (readSfarkFile(_sfArkInfo->InputBuf, sizeof(_sfArkInfo->InputBuf)) <= 0) goto err;
                    _sfArkInfo->InputBufIndex = 0;
                }

                _sfArkInfo->NumBitsInRegister = 16;
                _sfArkInfo->BitRegister = _sfArkInfo->InputBuf[_sfArkInfo->InputBufIndex++];
            }

            {
            short	oldRem;

            oldRem = _sfArkInfo->NumBitsInRegister + result3;
            do
            {
                --_sfArkInfo->NumBitsInRegister;
            } while ((_sfArkInfo->BitRegister >> _sfArkInfo->NumBitsInRegister) != 1);

            _sfArkInfo->BitRegister &= ((1 << _sfArkInfo->NumBitsInRegister) - 1);
            *dataptr++ = -(oldBits & 1) ^ (((signed int)oldBits >> 1) | ((quint16)(oldRem - (_sfArkInfo->NumBitsInRegister + 1)) << _sfArkInfo->v2.BitFramePackMethod));
            }
        } while (--framesize);
    }

    return 0;
}


/********************* addToChksum() *********************
 * sfark V2 checksum.
 */

void SfArkExtractor1::addToChksum(quint32 dataSizeInWords)
{
    qint32	sum;
    qint16 *	dataEnd;
    qint16 *	dataptr;

    sum = 0;
    dataptr = (qint16 *)_sfArkInfo->WorkBuffer1;
    dataEnd = &dataptr[dataSizeInWords];
    while (dataptr < dataEnd)
    {
        sum += (((qint32)*dataptr) >> 15) ^ ((qint32)*dataptr);
        ++dataptr;
    }

    _sfArkInfo->RunningChksum = sum + (2 * _sfArkInfo->RunningChksum);
}


/********************** uncompressTurbo() *********************
 * Unpacks the next block of bit-packed data for an sfark file
 * compressed with the "turbo" method.
 *
 * RETURN: 0 if success, or negative error number.
 */

qint32 SfArkExtractor1::uncompressTurbo(quint32 dataSizeInWords)
{
    int			encodeCnt;
    int			result;

    encodeCnt = getBitPackCnt(_sfArkInfo->v2.PrevUnpack2EncodeCnt);
    if (encodeCnt < 0 || encodeCnt > _sfArkInfo->v2.Unpack2EncodeLimit)
    {
        result = SFARKERR_BADCNT;
        goto bad;
    }
    _sfArkInfo->v2.PrevUnpack2EncodeCnt = encodeCnt;

    result = unpackBitData(dataSizeInWords);
    if (result < 0)
bad:	return result;

    if (encodeCnt)
    {
        qint16 *	ptr;
        void *		tempptr;

        ptr = &_sfArkInfo->v2.PrevDataShiftVal[--encodeCnt];
        do
        {
            if (!encodeCnt)	addToChksum(dataSizeInWords);

            delta1((qint16 *)_sfArkInfo->WorkBuffer2, (qint16 *)_sfArkInfo->WorkBuffer1, dataSizeInWords, ptr--);

            tempptr = _sfArkInfo->WorkBuffer1;
            _sfArkInfo->WorkBuffer1 = _sfArkInfo->WorkBuffer2;
            _sfArkInfo->WorkBuffer2 = (unsigned char *)tempptr;

        } while (--encodeCnt >= 0);
    }

    return 0;
}

/********************** uncompressFast() *********************
 * Unpacks the next block of bit-packed data for an sfark file
 * compressed with the "fast" method.
 *
 * RETURN: 0 if success, or negative error number.
 */

qint32 SfArkExtractor1::uncompressFast(quint32 dataSizeInWords)
{
    int				result;
    unsigned short	shiftArray[76];
    unsigned char	shiftFlag;
    unsigned char	delta3flag;

    // If the shift flag is set, setup shiftArray[]
    if ((shiftFlag = bitReadFlag()))
    {
        quint32	i, shiftPos, calcShiftPos;

        shiftPos = i = 0;
        calcShiftPos = (dataSizeInWords + 63) / 64;
        if (calcShiftPos > 76) goto shterr;

        while (bitReadFlag())
        {
            short			encodeCnt;

            shiftPos += bitRead(shiftBitsLookup(calcShiftPos - shiftPos - 1));

            if (_sfArkInfo->v2.CalcShiftEncodeCnt)
                encodeCnt = getBitPackCnt(0);
            else
                _sfArkInfo->v2.ShiftEncodeCnt = encodeCnt = getBitPackCnt(_sfArkInfo->v2.ShiftEncodeCnt);

            if (shiftPos > calcShiftPos)
            {
shterr:			result = SFARKERR_BADSHIFTCNT;
                goto fastOut;
            }

            while (i < shiftPos) shiftArray[i++] = _sfArkInfo->v2.CalcShiftEncodeCnt;

            _sfArkInfo->v2.CalcShiftEncodeCnt = encodeCnt;
        }

        while (i < calcShiftPos) shiftArray[i++] = _sfArkInfo->v2.CalcShiftEncodeCnt;
    }


    {
    unsigned char	flagArray[24];
    int				encodeCnt;

    // The next bit tells whether the delta decomp uses method 3 (versus 2 or 1)
    if ((delta3flag = bitReadFlag()))
    {
        encodeCnt = getBitPackCnt(_sfArkInfo->v2.PrevUnpack3EncodeCnt);
        if (encodeCnt < 0 || encodeCnt > _sfArkInfo->v2.Unpack3EncodeLimit)
        {
badcnt:		result = SFARKERR_BADCNT;
fastOut:	return result;
        }
        _sfArkInfo->v2.PrevUnpack3EncodeCnt = encodeCnt;
    }
    else
    {
        encodeCnt = getBitPackCnt(_sfArkInfo->v2.PrevUnpack2EncodeCnt);
        if (encodeCnt < 0 || encodeCnt > _sfArkInfo->v2.Unpack2EncodeLimit) goto badcnt;

        _sfArkInfo->v2.PrevUnpack2EncodeCnt = encodeCnt;
        for (result = 0; result < encodeCnt; ++result) flagArray[result] = bitReadFlag();
    }

    // If LPC compression is also used, we first read a 32-bit mask, later passed to lpcUnpack
    {
        quint32	lpcMask = 0;

        if (_sfArkInfo->CompressType != 5)
        {
            if (bitReadFlag())
            {
                quint32	temp4;

                temp4 = bitRead(16);
                lpcMask = (bitRead(16) << 16) | temp4;
            }
        }

        if ((result = unpackBitData(dataSizeInWords)) < 0) goto fastOut;

        if (_sfArkInfo->CompressType != 5)
        {
            lpcUnpack(dataSizeInWords, lpcMask);
            goto swap;
        }
    }

    while (encodeCnt--)
    {
        void *	tempptr;

        tempptr = &_sfArkInfo->v2.PrevDataShiftVal[encodeCnt];
        if (delta3flag)
            delta3((qint16 *)_sfArkInfo->WorkBuffer2, (qint16 *)_sfArkInfo->WorkBuffer1, dataSizeInWords, (qint16*)tempptr);
        else if (flagArray[encodeCnt])
        {
            if (flagArray[encodeCnt] == 1)
                delta2((qint16 *)_sfArkInfo->WorkBuffer2, (qint16 *)_sfArkInfo->WorkBuffer1, dataSizeInWords, (qint16*)tempptr);
        }
        else
            delta1((qint16 *)_sfArkInfo->WorkBuffer2, (qint16 *)_sfArkInfo->WorkBuffer1, dataSizeInWords, (qint16*)tempptr);

swap:	tempptr = _sfArkInfo->WorkBuffer1;
        _sfArkInfo->WorkBuffer1 = _sfArkInfo->WorkBuffer2;
        _sfArkInfo->WorkBuffer2 = (unsigned char *)tempptr;
    }

    if (shiftFlag) applyShift((qint16 *)_sfArkInfo->WorkBuffer1, dataSizeInWords, &shiftArray[0]);

    addToChksum(dataSizeInWords);
    }

    return 0;
}

static quint32 get_ulong(unsigned char * ptr)
{
    return ((quint32)ptr[0]) | ((quint32)ptr[1] << 8) | ((quint32)ptr[2] << 16) | ((quint32)ptr[3] << 24);
}

static quint16 get_ushort(unsigned char * ptr)
{
    return ((quint16)ptr[0]) | ((quint16)ptr[1] << 8);
}


// ==========================================================
// Extract version 1 Sfark
// ==========================================================


static const quint32 ChecksumVals[256] = {0,
                                          0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
                                          0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E,
                                          0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
                                          0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D,
                                          0x6DDDE4EB, 0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0,
                                          0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63,
                                          0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
                                          0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA,
                                          0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75,
                                          0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A, 0xC8D75180,
                                          0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
                                          0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87,
                                          0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
                                          0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5,
                                          0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
                                          0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01, 0x6B6B51F4,
                                          0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B,
                                          0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA,
                                          0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
                                          0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541,
                                          0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC,
                                          0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F,
                                          0xDD0D7CC9, 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
                                          0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 0x5EDEF90E,
                                          0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
                                          0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C,
                                          0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
                                          0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B,
                                          0x9309FF9D, 0x0A00AE27, 0x7D079EB1, 0xF00F9344, 0x8708A3D2,
                                          0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671,
                                          0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
                                          0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8,
                                          0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767,
                                          0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6,
                                          0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
                                          0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795,
                                          0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
                                          0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B,
                                          0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
                                          0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713, 0x95BF4A82,
                                          0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D,
                                          0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8,
                                          0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
                                          0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF,
                                          0xF862AE69, 0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE,
                                          0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D,
                                          0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
                                          0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9, 0xBDBDF21C,
                                          0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
                                          0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02,
                                          0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D};

static const quint32	BitsMasks[17] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0x0FFFF};

static const quint16	DataWordMap1[31] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
static const quint16	DataWordMap2[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

static const unsigned char	EncodeCntMap1[31] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99};
static const unsigned char	EncodeCntMap2[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

static const unsigned char	DataOffsets[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


qint32 SfArkExtractor1::readSfarkByte()
{
    unsigned char *	ptr;

    ptr = (unsigned char *)_sfArkInfo->InputBuf;

    if (_sfArkInfo->InputBufIndex >= sizeof(_sfArkInfo->InputBuf))
    {
        if (readSfarkFile(ptr, sizeof(_sfArkInfo->InputBuf)) <= 0)
            //			return SFARKERR_SFARKREAD;
            return 0;
        _sfArkInfo->InputBufIndex = 0;
    }

    return ptr[_sfArkInfo->InputBufIndex++];
}

quint32 SfArkExtractor1::readSfarkBits(quint16 numOfBits)
{
    quint32	i, result;

    for (i = _sfArkInfo->NumBitsInRegister; i < numOfBits; i += 8) _sfArkInfo->BitRegister|= readSfarkByte() << i;
    result = _sfArkInfo->BitRegister& BitsMasks[numOfBits];
    _sfArkInfo->NumBitsInRegister = i - numOfBits;
    _sfArkInfo->BitRegister>>= numOfBits;
    return result;
}

static void freeMemList(PackItem **list)
{
    PackItem *	mem;
    PackItem *	next;

    next = *list;
    while (next)
    {
        mem = next - 1;
        next = mem->u.NextItemArray;
        free(mem);
    }

    *list = 0;
}

static qint32 makePackItemList(quint32 * dataArray, quint32 dataArraySize, quint32 lowLimit, const quint16 * dataWordMap, const unsigned char *encodeCntMap, PackItem ** packArrayList, quint32 * encodeCntPtr)
{
    int				result;
    quint32	i;
    int				array1[17];

    memset(&array1[0], 0, sizeof(array1));
    i = dataArraySize;
    while (i--) ++array1[dataArray[i]];

    if (array1[0] == (int)dataArraySize)
    {
        *packArrayList = 0;
        *encodeCntPtr = 0;
        result = 0;
    }
    else
    {
        int					val;
        quint32		encodeCnt, highOffset, lowOffset;

        encodeCnt = *encodeCntPtr;

        lowOffset = 1;
        while (lowOffset < 17 && !array1[lowOffset]) ++lowOffset;

        highOffset = 16;
        while (highOffset && !array1[highOffset]) --highOffset;

        if (lowOffset > encodeCnt) encodeCnt = lowOffset;
        if (encodeCnt > highOffset) encodeCnt = highOffset;
        *encodeCntPtr = encodeCnt;

        val = 1 << lowOffset;
        if (lowOffset >= highOffset)
        {
            PackItem *	memArray[16];
            quint32 *		bigArrayPtr;
            quint32		numItems;
            int					curMemArrayIndex;
            quint32		cnt;
            int					encodeCntDiff;
            quint32		array3[17];
            quint32		bigArray[288];
            numItems = 0;

start:		if (val < array1[highOffset]) goto ret2;
            val -= array1[highOffset];
            array1[highOffset] += val;

            {
                quint32 *	ptr;
                quint32 *	ptr3;

                cnt = 0;
                ptr = (quint32 *)&array1[1];
                array3[1] = cnt = 0;
                ptr3 = &array3[2];
                i = highOffset;
                while (1)
                {
                    char temp;

                    temp = i-- == 1;
                    if (temp) break;
                    cnt += *ptr++;
                    *ptr3++ = cnt;
                }
            }

            i = 0;
            do
            {
                quint32 temp;

                if (dataArray[i])
                {
                    temp = array3[dataArray[i]]++;
                    bigArray[temp] = i;
                }
            } while (++i < dataArraySize);
            array3[0] = 0;

            i = 0;
            bigArrayPtr = &bigArray[0];
            curMemArrayIndex = -1;
            encodeCntDiff = -encodeCnt;
            if (lowOffset > highOffset)
out:			result = val && highOffset != 1;
            else
            {
                for (;;)
                {
                    cnt = array1[lowOffset];
findNewLow:			if (cnt--) break;
                    if (++lowOffset > highOffset) goto out;
                }

                // ----------------------
                while (1)
                {
                    if ((signed int)(encodeCnt + encodeCntDiff) >= (signed int)lowOffset)
                    {
                        PackItem item;

                        if (curMemArrayIndex == -1) goto bad;

                        item.BitShiftAmt = lowOffset - encodeCntDiff;
                        item.EncodeCnt = 99;

                        if (&bigArray[dataArraySize] > bigArrayPtr)
                        {
                            quint32 v31;

                            v31 = *bigArrayPtr++;
                            if (v31 >= lowLimit)
                            {
                                item.EncodeCnt = encodeCntMap[v31 - lowLimit];
                                item.u.Data1 = dataWordMap[v31 - lowLimit];
                            }
                            else
                            {
                                item.EncodeCnt = 16;
                                if (v31 >= 256) item.EncodeCnt = 15;
                                item.u.Data1 = (unsigned short)v31;
                            }
                        }

                        {
                            quint32 between, j;

                            between = 1 << (lowOffset - encodeCntDiff);
                            for (j = i >> encodeCntDiff; numItems > j; j += between)
                                memcpy((char *)memArray[curMemArrayIndex] + (j * sizeof(PackItem)), &item, sizeof(PackItem));

                            for (j = 1 << (lowOffset - 1); j & i; j >>= 1) i ^= j;
                            i ^= j;
                        }

                        while ((i & ((1 << encodeCntDiff) - 1)) != array3[curMemArrayIndex])
                        {
                            --curMemArrayIndex;
                            encodeCntDiff -= encodeCnt;
                        }

                        goto findNewLow;
                    }

                    {
                        PackItem *	mem;
                        quint32		diff;
                        {
                            quint32		highBound;
                            quint32		increment;

                            encodeCntDiff += encodeCnt;
                            highBound = highOffset - encodeCntDiff <= encodeCnt ? highOffset - encodeCntDiff : encodeCnt;
                            diff = lowOffset - encodeCntDiff;
                            increment = 1 << (lowOffset - encodeCntDiff);
                            if (increment > cnt + 1)
                            {
                                quint32 *	ptr;

                                increment -= (cnt + 1);
                                ptr = (quint32 *)&array1[lowOffset];
                                for (;;)
                                {
                                    quint32	wordInc;

                                    if (++diff >= highBound) break;
                                    wordInc = 2 * increment;
                                    ++ptr;
                                    if (wordInc <= *ptr) break;
                                    increment = wordInc - *ptr;
                                }
                            }
                        }

                        numItems = 1 << diff;

                        if (++curMemArrayIndex >= 16) goto bad;

                        // Alloc a MEMHEAD, followed immediately by an array of PACKARRAYITEMs
                        mem = (PackItem *)malloc((sizeof(PackItem) * numItems) + sizeof(PackItem));
                        if (!mem) break;

                        *packArrayList = memArray[curMemArrayIndex] = &mem[1];
                        packArrayList = &mem->u.NextItemArray;
                        mem->u.NextItemArray = 0;

                        if (curMemArrayIndex)
                        {
                            PackItem *	itemPtr;
                            quint32		temp;

                            array3[curMemArrayIndex] = i;

                            temp = i >> (encodeCntDiff - (unsigned char)encodeCnt);
                            itemPtr = memArray[curMemArrayIndex - 1] + temp;
                            itemPtr->EncodeCnt = diff + 16;
                            itemPtr->BitShiftAmt = encodeCnt;
                            itemPtr->u.NextItemArray = memArray[curMemArrayIndex];
                        }
                    }
                }

                if (curMemArrayIndex) freeMemList(memArray);
bad:			result = -1;
            }
        }
        else
        {
            qint32 *			ptr;
            quint32	low;

            low = lowOffset;
            ptr = &array1[lowOffset];
            while (val >= *ptr)
            {
                val -= *ptr++;
                val *= 2;
                if (++low >= highOffset) goto start;
            }

ret2:		result = 2;
        }
    }

    return result;
}

void SfArkExtractor1::flushToOutBuf()
{
    quint32	amt;

    amt = _sfArkInfo->v1.BytesInDecompBuf - _sfArkInfo->v1.PrevBytesInDecompBuf;
    if (amt)
    {
        memcpy(_sfArkInfo->v1.OutbufPtr, &_sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.PrevBytesInDecompBuf], amt);
        _sfArkInfo->v1.OutbufPtr += amt;
        _sfArkInfo->v1.PrevBytesInDecompBuf += amt;
    }
}

unsigned char SfArkExtractor1::decodeData(quint32 * dataRet, PackItem * arrayPtr, qint32 encodeCnt)
{
    PackItem *	itemPtr;
    unsigned char		cnt;

    for (;;)
    {
        while ((int)_sfArkInfo->NumBitsInRegister < encodeCnt)
        {
            _sfArkInfo->BitRegister|= readSfarkByte() << _sfArkInfo->NumBitsInRegister;
            _sfArkInfo->NumBitsInRegister += 8;
        }

        itemPtr = &arrayPtr[_sfArkInfo->BitRegister& BitsMasks[encodeCnt]];
        cnt = itemPtr->EncodeCnt;
        if (cnt == 99) break;
        _sfArkInfo->BitRegister>>= itemPtr->BitShiftAmt;
        _sfArkInfo->NumBitsInRegister -= itemPtr->BitShiftAmt;
        if (cnt <= 16)
        {
            *dataRet = itemPtr->u.Data1;
            break;
        }

        arrayPtr = itemPtr->u.NextItemArray;
        encodeCnt = cnt - 16;
    }

    return cnt;
}

quint32 SfArkExtractor1::flushPackItemsToOutBuf(quint32 len)
{
    quint32	i;
    quint32	moveAmt;
    unsigned char	numOfBits;

    i = 0;
    while (i < len)
    {
        if (_sfArkInfo->v1.BlockFlags & 0x10) goto skip;

        numOfBits = decodeData(&_sfArkInfo->v1.VarDecodeDWord, _sfArkInfo->v1.PackItemArray, _sfArkInfo->v1.FirstEncodeCnt);
        if (numOfBits == 99)
            return (quint32)-1;

        if (numOfBits != 16)
        {
            if (numOfBits == 15)
            {
                _sfArkInfo->v1.BlockFlags &= 0xC0;
                break;
            }

            _sfArkInfo->v1.VarDecodeDWord += readSfarkBits(numOfBits);
            numOfBits = decodeData(&_sfArkInfo->v1.VarDecodeByteCnt, _sfArkInfo->v1.CurPackItemArray, _sfArkInfo->v1.SecondEncodeCnt);
            if (numOfBits == 99)
                return (quint32)-1;

            _sfArkInfo->v1.VarDecodeByteCnt = _sfArkInfo->v1.BytesInDecompBuf - (_sfArkInfo->v1.VarDecodeByteCnt + readSfarkBits(numOfBits));

skip:		_sfArkInfo->v1.BlockFlags &= ~0x10;
            do
            {
                _sfArkInfo->v1.VarDecodeByteCnt &= 0x7FFF;
                if (_sfArkInfo->v1.VarDecodeByteCnt <= _sfArkInfo->v1.BytesInDecompBuf)
                    moveAmt = _sfArkInfo->v1.BytesInDecompBuf;
                else
                    moveAmt = _sfArkInfo->v1.VarDecodeByteCnt;
                moveAmt = 32768 - moveAmt;
                if (moveAmt > _sfArkInfo->v1.VarDecodeDWord) moveAmt = _sfArkInfo->v1.VarDecodeDWord;
                if (moveAmt + i > len)
                {
                    _sfArkInfo->v1.BlockFlags |= 0x10;
                    moveAmt = len - i;
                }
                i += moveAmt;
                _sfArkInfo->v1.VarDecodeDWord -= moveAmt;

                if (moveAmt > _sfArkInfo->v1.BytesInDecompBuf - _sfArkInfo->v1.VarDecodeByteCnt)
                {
                    do
                    {
                        _sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.BytesInDecompBuf++] = _sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.VarDecodeByteCnt++];
                    } while (--moveAmt);
                }
                else
                {
                    memcpy(&_sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.BytesInDecompBuf], &_sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.VarDecodeByteCnt], moveAmt);
                    _sfArkInfo->v1.BytesInDecompBuf += moveAmt;
                    _sfArkInfo->v1.VarDecodeByteCnt += moveAmt;
                }

                if (_sfArkInfo->v1.BytesInDecompBuf >= 32768)
                {
                    flushToOutBuf();
                    _sfArkInfo->v1.BytesInDecompBuf = _sfArkInfo->v1.PrevBytesInDecompBuf = 0;
                }

            } while (_sfArkInfo->v1.VarDecodeDWord && !(_sfArkInfo->v1.BlockFlags & 0x10));
        }
        else
        {
            _sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.BytesInDecompBuf++] = (unsigned char)_sfArkInfo->v1.VarDecodeDWord;
            if (_sfArkInfo->v1.BytesInDecompBuf >= 32768)
            {
                flushToOutBuf();
                _sfArkInfo->v1.BytesInDecompBuf = _sfArkInfo->v1.PrevBytesInDecompBuf = 0;
            }
            ++i;
        }
    }

    flushToOutBuf();
    return i;
}


/*********************** decompBlock3() ***********************
 * Performs LsPack decompression on the next block of data
 * compressed via method 3, and writes it to the output buffer.
 *
 * RETURN: # of bytes decompressed if success, or negative
 * error number.
 */

quint32 SfArkExtractor1::decompBlock3(quint32 blockSize)
{
    quint32	i;

    if (!(_sfArkInfo->v1.BlockFlags & 0x20))
    {
        quint32 *	dataArrayPtr;
        quint32	dataArray[316];
        quint32	dataArray1stCnt;
        quint32	dataArray2ndCnt;
        quint32	limit;
        quint32	mask;
        quint32	data;

        // Break off the next 5 bits as the count of additional fields we need in dataArray[] (beyond the first 257 ints)
        dataArray2ndCnt = readSfarkBits(5) + 257;

        // Repeat the above to get the count for the first (257) fields of dataArray[]
        dataArray1stCnt = readSfarkBits( 5) + 1;

        // Error-check counts
        if (dataArray2ndCnt > 286 || dataArray1stCnt > 30) goto bad;

        // Break off the next four bits as a count of how many 3-bit vals to read into dataArray[]
        data = readSfarkBits(4) + 4;
        i = 0;
        while (i < data) dataArray[DataOffsets[i++]] = readSfarkBits(3);
        while (i < 19) dataArray[DataOffsets[i++]] = 0;

        _sfArkInfo->v1.FirstEncodeCnt = 7;
        i = makePackItemList(&dataArray[0], 19, 19, 0, 0, &_sfArkInfo->v1.PackItemArray, &_sfArkInfo->v1.FirstEncodeCnt);
        if (i)
        {
            if (i == 1) freeMemList(&_sfArkInfo->v1.PackItemArray);
            goto bad;
        }

        limit = dataArray1stCnt + dataArray2ndCnt;
        mask = BitsMasks[_sfArkInfo->v1.FirstEncodeCnt];
        data = i = 0;

        while (i < limit)
        {
            while (_sfArkInfo->NumBitsInRegister < _sfArkInfo->v1.FirstEncodeCnt)
            {
                _sfArkInfo->BitRegister|= readSfarkByte() << _sfArkInfo->NumBitsInRegister;
                _sfArkInfo->NumBitsInRegister += 8;
            }

            {
                unsigned char	shiftAmt;

                _sfArkInfo->v1.CurPackItemArray = &_sfArkInfo->v1.PackItemArray[_sfArkInfo->BitRegister& mask];
                shiftAmt = _sfArkInfo->v1.CurPackItemArray->BitShiftAmt;
                _sfArkInfo->BitRegister>>= shiftAmt;
                _sfArkInfo->NumBitsInRegister -= shiftAmt;
            }

            {
                quint32 nextData;

                nextData = _sfArkInfo->v1.CurPackItemArray->u.Data1;
                if (nextData >= 16)
                {
                    quint32 cnt;

                    if (nextData == 16)
                        cnt = readSfarkBits(2) + 3;
                    else
                    {
                        data = 0;
                        if (nextData == 17)
                            cnt = readSfarkBits(3) + 3;
                        else
                            cnt = readSfarkBits(7) + 11;
                    }

                    if (cnt + i > limit) goto bad;

                    dataArrayPtr = &dataArray[i];
                    while (cnt--)
                    {
                        *(dataArrayPtr)++ = data;
                        ++i;
                    }
                }
                else
                    dataArray[i++] = data = nextData;
            }
        }

        freeMemList(&_sfArkInfo->v1.PackItemArray);

        // =========================
        _sfArkInfo->v1.FirstEncodeCnt = 9;
        i = makePackItemList(&dataArray[0], dataArray2ndCnt, 257, &DataWordMap1[0], &EncodeCntMap1[0], &_sfArkInfo->v1.PackItemArray, &_sfArkInfo->v1.FirstEncodeCnt);
        if (i)
        {
            if (i == 1) freeMemList(&_sfArkInfo->v1.PackItemArray);
bad:		return (quint32)-1;
        }

        _sfArkInfo->v1.SecondEncodeCnt = 6;
        i = makePackItemList(&dataArray[dataArray2ndCnt], dataArray1stCnt, 0, &DataWordMap2[0], &EncodeCntMap2[0], &_sfArkInfo->v1.CurPackItemArray, &_sfArkInfo->v1.SecondEncodeCnt);
        if (i)
        {
            if (i == 1) freeMemList(&_sfArkInfo->v1.CurPackItemArray);
            freeMemList(&_sfArkInfo->v1.PackItemArray);
            goto bad;
        }

        _sfArkInfo->v1.BlockFlags |= 0x20u;
    }

    i = flushPackItemsToOutBuf(blockSize);
    if (i == (quint32)-1 || !(_sfArkInfo->v1.BlockFlags & 0x20))
    {
        freeMemList(&_sfArkInfo->v1.PackItemArray);
        freeMemList(&_sfArkInfo->v1.CurPackItemArray);
    }

    return i;
}


/*********************** decompBlock2() ***********************
 * Performs LsPack decompression on the next block of data
 * compressed via method 2, and writes it to the output buffer.
 *
 * RETURN: # of bytes decompressed if success, or negative
 * error number.
 */

quint32 SfArkExtractor1::decompBlock2(quint32 blockSize)
{
    quint32	i;
    int				ret;
    quint32	dataArray[288];

    if (!(_sfArkInfo->v1.BlockFlags & 0x20))
    {
        i = 0;
        do
        {
            dataArray[i++] = 8;
        } while (i < 144);
        do
        {
            dataArray[i++] = 9;
        } while (i < 256);
        do
        {
            dataArray[i++] = 7;
        } while (i < 280);
        do
        {
            dataArray[i++] = 8;
        } while (i < 288);

        _sfArkInfo->v1.FirstEncodeCnt = 7;
        ret = makePackItemList(dataArray, 288, 257, &DataWordMap1[0], &EncodeCntMap1[0], &_sfArkInfo->v1.PackItemArray, &_sfArkInfo->v1.FirstEncodeCnt);
        if (ret)
        {
            if (ret == 1) freeMemList(&_sfArkInfo->v1.PackItemArray);
bad:		return (quint32)-1;
        }

        i = 0;
        do
        {
            dataArray[i] = 5;
        } while (++i < 30);

        _sfArkInfo->v1.SecondEncodeCnt = 5;
        ret = makePackItemList(dataArray, 30, 0, &DataWordMap2[0], &EncodeCntMap2[0], &_sfArkInfo->v1.CurPackItemArray, &_sfArkInfo->v1.SecondEncodeCnt);
        if (ret & 0xFFFFFFFE)
        {
            freeMemList(&_sfArkInfo->v1.PackItemArray);
            goto bad;
        }

        _sfArkInfo->v1.BlockFlags |= 0x20;
    }

    ret = flushPackItemsToOutBuf(blockSize);
    if ((quint32)ret == (quint32)-1 || !(_sfArkInfo->v1.BlockFlags & 0x20))
    {
        freeMemList(&_sfArkInfo->v1.PackItemArray);
        freeMemList(&_sfArkInfo->v1.CurPackItemArray);
    }

    return ret;
}


/*********************** decompBlock1() ***********************
 * Performs LsPack decompression on the next block of data
 * compressed via method 1, and writes it to the output buffer.
 *
 * RETURN: # of bytes decompressed if success, or negative
 * error number.
 */

quint32 SfArkExtractor1::decompBlock1(quint32 len)
{
    quint32	i;

    if (!(_sfArkInfo->v1.BlockFlags & 0x20))
    {
        if (_sfArkInfo->NumBitsInRegister > 7)
            return (quint32)-1;

        _sfArkInfo->BitRegister= _sfArkInfo->NumBitsInRegister = 0;
        _sfArkInfo->v1.EncodeCnt = readSfarkByte();
        _sfArkInfo->v1.EncodeCnt |= readSfarkByte() << 8;
        i = readSfarkByte();
        if ((((readSfarkByte() << 8) | i) ^ 0xFFFF) != _sfArkInfo->v1.EncodeCnt)
            return (quint32)-1;
        _sfArkInfo->v1.BlockFlags |= 0x20u;
    }

    i = 0;
    if (len)
    {
        while (_sfArkInfo->v1.EncodeCnt--)
        {
            _sfArkInfo->v1.DecompBuf[_sfArkInfo->v1.BytesInDecompBuf++] = readSfarkByte();
            if (_sfArkInfo->v1.BytesInDecompBuf >= 32768)
            {
                flushToOutBuf();
                _sfArkInfo->v1.BytesInDecompBuf = _sfArkInfo->v1.PrevBytesInDecompBuf = 0;
            }

            if (len <= ++i)
            {
                flushToOutBuf();
                return i;
            }
        }
        _sfArkInfo->v1.BlockFlags &= 0xC0u;
    }

    flushToOutBuf();
    return i;
}


/********************* decompLspackBlock() *********************
 * Performs LsPack decompression on the next block of data
 * in the sfark file, and write it to the output buffer.
 * Each time this is called, it does another 1024 (or so) block
 * of data.
 *
 * RETURN: # of bytes decompressed if success, or negative
 * error number.
 */

qint32 SfArkExtractor1::decompLspackBlock()
{
    quint32	i;
    unsigned char	flags;
    quint32	result;

    _sfArkInfo->v1.BlockFlags |= 0x40;
    _sfArkInfo->v1.OutbufPtr = _sfArkInfo->WorkBuffer2;
    result = i = 0;
    do
    {
        if (!(_sfArkInfo->v1.BlockFlags & 3))
        {
            if (_sfArkInfo->v1.BlockFlags & 0x80) break;

            if (_sfArkInfo->NumBitsInRegister < 1)
            {
                _sfArkInfo->BitRegister|= (readSfarkByte() << _sfArkInfo->NumBitsInRegister);
                _sfArkInfo->NumBitsInRegister += 8;
            }

            if (_sfArkInfo->BitRegister& 1) _sfArkInfo->v1.BlockFlags |= 0x80;
            _sfArkInfo->BitRegister>>= 1;
            --_sfArkInfo->NumBitsInRegister;

            if (_sfArkInfo->NumBitsInRegister < 2)
            {
                _sfArkInfo->BitRegister|= (readSfarkByte() << _sfArkInfo->NumBitsInRegister);
                _sfArkInfo->NumBitsInRegister += 8;
            }

            flags = (unsigned char)(_sfArkInfo->BitRegister& 3) + 1;
            if (flags & 0xFC)
                return -1;

            _sfArkInfo->v1.BlockFlags |= flags;
            _sfArkInfo->BitRegister>>= 2;
            _sfArkInfo->NumBitsInRegister -= 2;
        }

        if ( (_sfArkInfo->v1.BlockFlags & 3) == 1 )
            result = decompBlock1(1024 - i);
        else if ((_sfArkInfo->v1.BlockFlags & 3) == 2)
            result = decompBlock2(1024 - i);
        else if ((_sfArkInfo->v1.BlockFlags & 3) == 3)
            result = decompBlock3(1024 - i);
        else
            return -1;

        if (result == (quint32)-1)
            return -1;

        if (!result && (_sfArkInfo->v1.BlockFlags & 0x80)) break;

        i += result;
    } while (i < 1024);

    return (int)i;
}


/********************* assembleParts() *********************
 * Takes the 2 "part files", created in the temp dir, performs
 * the final delta decompression, and writes out the SF2 file.
 * each time this is called, it does another 8192 block of
 * data, and clears _sfArkInfo->RunState when done.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::assembleParts()
{
    quint32 i;

    // Read another 8192 byte block from part 1 file
    quint32 blocksize = readSfarkFile(_sfArkInfo->WorkBuffer1, 8192 + 1);
    if (blocksize > 8192 + 1)
        return SFARKERR_SFARKREAD;
    else if (blocksize == 0)
    {
        // Entire soundfont is now extracted
        _sfArkInfo->RunState = SFARK_DONE;
        return 0;
    }
    blocksize--;

    // Read another 8192 byte block from part 2 file
    {
        int	fileHandle;

        fileHandle = _sfArkInfo->InputFileHandle;
        _sfArkInfo->InputFileHandle = _sfArkInfo->v1.PartFileHandle;
        i = readSfarkFile(_sfArkInfo->WorkBuffer1 + 8192 + 1, blocksize);
        _sfArkInfo->InputFileHandle = fileHandle;
        if (i > blocksize)
            return SFARKERR_SFARKREAD;
        if (i != blocksize)
            return SFARKERR_BADCOMP;
    }

    {
        quint16 * ptr = (quint16 *)_sfArkInfo->WorkBuffer2;

        // Combine input bytes (from both part files) to words, and store in out buf
        for (i = 0; i < blocksize; i++)
        {
            quint16		val;

            val = _sfArkInfo->WorkBuffer1[i + 1] << 8;
            if (val >= 0x8000)
                val |= (quint16)(0xFF - _sfArkInfo->WorkBuffer1[i+8192+1]);
            else
                val |= (quint16)_sfArkInfo->WorkBuffer1[i+8192+1];

            ptr[i] = val;
        }

        // Do delta shift on out buf
        for (i = 0; i < _sfArkInfo->WorkBuffer1[0]; i++)
        {
            quint32	j;
            quint16		val;

            val = 0;
            for (j = 0; j < blocksize; j++)
            {
                ptr[j] += val;
                val = ptr[j];
            }
        }

        // Store 16-bit words in little endian order
        for (i = 0; i < blocksize; i++)
        {
            unsigned char *	ptr2;
            quint16		val;

            val = ptr[i];
            ptr2 = (unsigned char *)&ptr[i];
            *ptr2++ = (unsigned char)val;
            *ptr2 = (unsigned char)(val >> 8);
        }

        // write block to soundfont
        i = writeToFontFile(ptr, blocksize << 1);
    }

    return (int)i;
}


/******************* getTempPartName() *******************
 * Formats the filename for one of the "part files", to be
 * created in the temp dir.
 *
 * partNum = '1' or '2'
 */

void SfArkExtractor1::getTempPartName(char partNum)
{
    quint32	len;
    char *			buffer;

    buffer = (char *)_sfArkInfo->WorkBuffer2;
    {
        char * tmpdir;

        if (!(tmpdir = getenv("TMPDIR")))
            strcpy(buffer, "/tmp");
        else
            strcpy(buffer, tmpdir);
        len = strlen(buffer);
        buffer[len++] = '/';
    }

    strcpy(&buffer[len], (const char *)_sfArkInfo->WorkBuffer2 + 262144 - 1024);
    len += _sfArkInfo->LeadingPadUncompSize;
    buffer[len++] = partNum;
    buffer[len] = 0;
}


/********************** initV1vars() ***********************
 * Reset _sfArkInfo fields in prep for its extracting another
 * file.
 */

void SfArkExtractor1::initV1vars()
{
    _sfArkInfo->InputBufIndex = sizeof(_sfArkInfo->InputBuf);
    _sfArkInfo->RunningUncompSize = _sfArkInfo->BitRegister= _sfArkInfo->NumBitsInRegister = _sfArkInfo->Percent = _sfArkInfo->v1.BlockFlags = 0;
    _sfArkInfo->v1.DecompBuf = _sfArkInfo->WorkBuffer1;
    _sfArkInfo->v1.BytesInDecompBuf = _sfArkInfo->v1.PrevBytesInDecompBuf = 0;
    _sfArkInfo->RunningChksum = -1;
}


/****************** seekNextLspackPart() *******************
 * Locates the next "part file" within the sfark file, and
 * preps for its extraction (LsPack) decompression.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::seekNextLspackPart()
{
    unsigned char *	fileHdrStartPtr;
    int				result;
    quint32	headerSize, slop;

    // Flush remaining bytes in input buffer
    result = _sfArkInfo->InputBufIndex;
    setSfarkFilePos(- (sizeof(_sfArkInfo->InputBuf) - result));

    // Read next lspack header
    fileHdrStartPtr = _sfArkInfo->WorkBuffer1;

    for (;;)
    {
        if ((headerSize = readSfarkFile(fileHdrStartPtr, 36+4)) > 36+4)
        {
bad:		result = SFARKERR_SFARKREAD;
            break;
        }
        if (headerSize != 36+4)
        {
hdr:		result = SFARKERR_BADHDR;
            break;
        }

        // Get the filename len
        headerSize = get_ushort(fileHdrStartPtr + 36);
        slop = get_ushort(fileHdrStartPtr + 38) + headerSize;

        // Read in the filename (plus an additional str we ignore)
        if ((result = readSfarkFile(fileHdrStartPtr + 100, slop)) < 0) goto bad;
        if ((quint32)result != slop) goto hdr;

        // The Lspack entry we want has a filename that ends in ".sfArk$1" or ".sfArk$2"
        if (headerSize > 8 && !memcmp(&fileHdrStartPtr[headerSize + 100 - 8], &SfArkId[0], strlen((const char *)SfArkId)))
        {
            _sfArkInfo->FileUncompSize = get_ulong(fileHdrStartPtr + 22);
            _sfArkInfo->FileChksum = get_ulong(fileHdrStartPtr + 14);

            // Create the second part file
            getTempPartName(fileHdrStartPtr[headerSize + 100 - 1]);
            if (!(result = createSfFile(_sfArkInfo->WorkBuffer2)))
            {
                _sfArkInfo->Flags |= SFARK_OUT_OPEN;
                initV1vars();
            }

            break;
        }

        // Keep looking for the header
        result = get_ulong(fileHdrStartPtr + 18);
        setSfarkFilePos(result);
    }

    return result;
}


/********************* addToCrc32() *********************
 * sfark V1 checksum. The checksum is done only on each
 * (LsPack compressed) "part file" within the sfark file,
 * and not on the final soundfont file. Bad design.
 * preps for its extraction.
 */

void SfArkExtractor1::addToCrc32(quint32 count)
{
    quint32			result;
    unsigned char *	buffer;

    buffer = _sfArkInfo->WorkBuffer2;
    result = _sfArkInfo->RunningChksum;
    while (count--) result = (result >> 8) ^ ChecksumVals[(result ^ *buffer++) & 0xFF];
    _sfArkInfo->RunningChksum = result;
}


// ==============================================================
// App API
// ==============================================================

/*********************** SfarkExtract() **********************
 * Extracts another block of data within an sfArk file, and
 * saves it to the soundfont file.
 *
 * sf =			Handle returned by SfarkAlloc().
 *
 * RETURN: 0 = success, 1 if the soundfont is fully extracted,
 * or negative error number.
 */

qint32 SfArkExtractor1::SfarkExtract()
{
    int				result;

    if (_sfArkInfo->RunState)
    {
        // Vers 1?
        if (_sfArkInfo->CompressType == 2)
        {
            // Still unpacking the 2 lspack entries?
            if (_sfArkInfo->RunState > 1)
            {
                result = decompLspackBlock();
                if (result >= 0)
                {
                    quint32	uncompressDataSize;

                    if (!(uncompressDataSize = (quint32)result)) goto corrupt;

                    // Update crc32
                    addToCrc32(uncompressDataSize);

                    // Write the uncompressed block to the soundfont file
                    if (!(result = writeToFontFile(_sfArkInfo->WorkBuffer2, uncompressDataSize)))
                    {
                        _sfArkInfo->RunningUncompSize += uncompressDataSize;
                        if (_sfArkInfo->RunningUncompSize >= _sfArkInfo->FileUncompSize)
                        {
                            if (~_sfArkInfo->RunningChksum != _sfArkInfo->FileChksum)
                                result = SFARKERR_CHKSUM;

                            // Close part file we finished
                            closeFile(_sfArkInfo->OutputFileHandle);
                            _sfArkInfo->Flags &= ~SFARK_OUT_OPEN;

                            // Do we need to extract the other part?
                            if (--_sfArkInfo->RunState > 1)
                                result = seekNextLspackPart();
                            else
                            {
                                // Create the final sf
                                if ((unsigned char *)_sfArkInfo->v1.AppFontName == _sfArkInfo->WorkBuffer2)
                                    strcpy((char *)_sfArkInfo->WorkBuffer2, (const char *)_sfArkInfo->WorkBuffer2 + 262144 - 1024);
                                if (!(result = createSfFile(_sfArkInfo->v1.AppFontName)))
                                {
                                    // Close the orig sfark, and open the 2 temp part files
                                    closeFile(_sfArkInfo->InputFileHandle);
                                    _sfArkInfo->Flags &= ~SFARK_IN1_OPEN;
                                    _sfArkInfo->Flags |= SFARK_OUT_OPEN;

                                    getTempPartName('2');
                                    if (!(result = openSfarkFile((const char *)_sfArkInfo->WorkBuffer2)))
                                    {
                                        _sfArkInfo->Flags |= SFARK_IN2_OPEN;
                                        _sfArkInfo->v1.PartFileHandle = _sfArkInfo->InputFileHandle;
                                        getTempPartName('1');
                                        if ((result = openSfarkFile((const char *)_sfArkInfo->WorkBuffer2)))
                                            _sfArkInfo->InputFileHandle = _sfArkInfo->v1.PartFileHandle;
                                        else
                                        {
                                            _sfArkInfo->RunningChksum = _sfArkInfo->FileChksum;
                                            _sfArkInfo->Percent = 0;
                                            _sfArkInfo->Flags |= SFARK_IN1_OPEN;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // phase 2: Put the 2 pieces back together with additional delta decomp
            else
                result = assembleParts();
        }

        // delta
        else if (_sfArkInfo->RunState == SFARK_BITPACK)
        {
            quint32	uncompressDataSize;
            quint32	numShorts;
            {
                quint32	bitPackEnd;
                quint32	runningUncompSize;

                // Check if we got to the end of the delta compressed blocks. If so, the next
                // block (after this one) will be LZIP compressed
                numShorts = _sfArkInfo->v2.NumShortsInLpcBlock;
                uncompressDataSize = sizeof(qint16) * numShorts;
                runningUncompSize = _sfArkInfo->RunningUncompSize;
                bitPackEnd = _sfArkInfo->BitPackEndOffset;
                if (uncompressDataSize + runningUncompSize >= bitPackEnd)
                {
                    uncompressDataSize = bitPackEnd - runningUncompSize;
                    numShorts = uncompressDataSize / sizeof(qint16);
                    _sfArkInfo->RunState = SFARK_LZIP;	// Next block is LZip
                }
            }

            if (_sfArkInfo->CompressType == 4)
                result = uncompressTurbo(numShorts);
            else
                result = uncompressFast(numShorts);
            if (!result)
            {
                quint16 *		from;
                unsigned char *	to;
                quint16		val;

                // Store 16-bit words in little endian order
                from = (quint16 *)_sfArkInfo->WorkBuffer1;
                while (numShorts--)
                {

                    to = (unsigned char *)from;
                    val = *from++;
                    *to++ = (unsigned char)val;
                    *to = (unsigned char)(val >> 8);
                }

                if (!(result = writeToFontFile(_sfArkInfo->WorkBuffer1, uncompressDataSize)))
                    _sfArkInfo->RunningUncompSize += uncompressDataSize;
            }
        }

        // LZip
        else
        {
            quint32		blockSize;

            // Read LZip compressed blockSize (quint32)
            if (!(result = bitReadBuf(_sfArkInfo->WorkBuffer1, sizeof(quint32))))
            {
                blockSize = get_ulong(_sfArkInfo->WorkBuffer1);

                // An sfark compressed block has a size limit of 262144 bytes
                if (blockSize > 262144)
                    result = SFARKERR_BADLEN;
                else
                {
                    quint32		bytesExtracted;

                    // Read in and unpack bit-packed bytes
                    if (!(result = bitReadBuf(_sfArkInfo->WorkBuffer1, blockSize)))
                    {
                        // Decompress LZIP'ed data into 2nd buffer and get size of uncompressed block
                        _sfArkInfo->v2.Zip.next_in = (Bytef *)_sfArkInfo->WorkBuffer1;
                        _sfArkInfo->v2.Zip.avail_in = blockSize;

                        _sfArkInfo->v2.Zip.next_out = _sfArkInfo->WorkBuffer2;
                        _sfArkInfo->v2.Zip.avail_out = 262144;

                        _sfArkInfo->v2.Zip.zalloc = (alloc_func)0;
                        _sfArkInfo->v2.Zip.zfree = (free_func)0;

                        result = inflateInit(&_sfArkInfo->v2.Zip);
                        if (result == Z_OK)
                        {
                            result = inflate(&_sfArkInfo->v2.Zip, Z_FINISH);
                            if (result != Z_STREAM_END)
                            {
                                inflateEnd(&_sfArkInfo->v2.Zip);
                                if (result == Z_OK) result = Z_MEM_ERROR;
                            }
                            else
                            {
                                bytesExtracted = _sfArkInfo->v2.Zip.total_out;
                                result = inflateEnd(&_sfArkInfo->v2.Zip);
                            }
                        }

                        if (result)
                        {
                            switch (result)
                            {
                            case Z_MEM_ERROR:
                            {
                                result = SFARKERR_MEM;
                                break;
                            }

                                // Z_DATA_ERROR if the input data was corrupted.
                            default:
bad:								result = SFARKERR_BADCOMP;
                            }
                        }
                        else
                        {
                            // Update checksum
                            _sfArkInfo->RunningChksum = adler32(_sfArkInfo->RunningChksum, _sfArkInfo->WorkBuffer2, bytesExtracted);

                            // Write the uncompressed block to the soundfont file
                            if (!(result = writeToFontFile(_sfArkInfo->WorkBuffer2, bytesExtracted)))
                            {
                                // Inc total uncompressed size and see if the entire soundfont is now extracted
                                _sfArkInfo->RunningUncompSize += bytesExtracted;
                                if (_sfArkInfo->RunningUncompSize < _sfArkInfo->FileUncompSize)
                                {
                                    if (_sfArkInfo->RunningUncompSize < _sfArkInfo->LeadingPadUncompSize)
corrupt:								goto bad;

                                    // Only the non-waveform data of an Sfark file uses a LZIP compression. The audio data
                                    // is one of two delta compress schemes which we'll call "fast" and "turbo". Since we
                                    // just did LZIP, then the next block must be one of the delta variants
                                    _sfArkInfo->RunState = SFARK_BITPACK;
                                    memset(&_sfArkInfo->v2.Zip, 0, sizeof(z_stream));
                                }
                                else
                                    // Entire soundfont is now extracted
                                    _sfArkInfo->RunState = SFARK_DONE;
                            }
                        }
                    }
                }
            }
        }

        if (!result)
        {
            // Update % done
            if ((_sfArkInfo->FileUncompSize - (_sfArkInfo->FileUncompSize - _sfArkInfo->RunningUncompSize)) > (_sfArkInfo->FileUncompSize / 10) * _sfArkInfo->Percent) ++_sfArkInfo->Percent;

            // Entire soundfont is now extracted?
            if (!_sfArkInfo->RunState)
            {
                // Test the checksum for the whole file
                result = 1;
                if (_sfArkInfo->RunningChksum != _sfArkInfo->FileChksum) result = SFARKERR_CHKSUM;
            }
        }

        // Close files if an error or we're done
        if (result) cleanFiles();

        return result;
    }

    return SFARKERR_APP;
}


/********************* SfarkBeginExtract() *******************
 * Begins extracting the soundfont within an sfArk file, and
 * saves it to a new file (in soundfont's original format, such
 * as SF2).
 *
 * sf =			Handle returned by SfarkAlloc().
 * sfontName =	Nul-terminated name of the soundfont file, or
 *				0 if the original name is desired.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::SfarkBeginExtract(const void * sfontName)
{
    qint32 errCode;

    if (_sfArkInfo->Flags & SFARK_IN1_OPEN)
    {
        // If app didn't specify a name, use the original sfont name (that SfarkOpen stored
        // in WorkBuffer2)
        if (!sfontName) sfontName = _sfArkInfo->WorkBuffer2;

        if (_sfArkInfo->CompressType == 2)
        {
            // For version 1, we need to extract two temp files from sfark in the first phase, then
            // recombine those 2 pieces into the soundfont. We saved the original sf name in the
            // 1024 bytes at the end of WorkBuffer2. Now let's also save the app's choice of sf
            // name at the end of WorkBuffer1
            _sfArkInfo->v1.AppFontName = (const char *)sfontName;
            if (sfontName != _sfArkInfo->WorkBuffer2)
            {
                _sfArkInfo->v1.AppFontName = (const char *)(_sfArkInfo->WorkBuffer1 + 262144 - 1024);
                strcpy((char *)_sfArkInfo->v1.AppFontName, (char *)sfontName);
            }

            // Now we need to create the intermediary "part file" in the temp dir. Its name will be
            // the orig soundfont name, with an extra '1' or '2' appended (depending on whether we're
            // extracting the 1st or 2nd part)
            getTempPartName(_sfArkInfo->v1.PartNum);
            sfontName = _sfArkInfo->WorkBuffer2;

            initV1vars();
        }
        else
        {
            // Init decomp routine vars
            _sfArkInfo->RunningChksum = _sfArkInfo->RunningUncompSize = _sfArkInfo->BitRegister = _sfArkInfo->NumBitsInRegister = _sfArkInfo->Percent = 0;
            _sfArkInfo->InputBufIndex = sizeof(_sfArkInfo->InputBuf)/sizeof(quint16);
            memset(&_sfArkInfo->v2, 0, sizeof(_sfArkInfo->v2));
            _sfArkInfo->v2.BitFramePackMethod = 8;
            _sfArkInfo->v2.NumShortsInLpcBlock = 4096;

            switch (_sfArkInfo->CompressType)
            {
            case 4:
                _sfArkInfo->v2.Unpack2EncodeLimit = 3;
                // _sfArkInfo->v2.Unpack3EncodeLimit = 0;
                break;
            case 5:
                _sfArkInfo->v2.Unpack2EncodeLimit = _sfArkInfo->v2.Unpack3EncodeLimit = 20;
                _sfArkInfo->v2.NumShortsInLpcBlock = 1024;
                break;
            case 6:
                _sfArkInfo->v2.Unpack2EncodeLimit = _sfArkInfo->v2.Unpack3EncodeLimit = 3;
                _sfArkInfo->v2.NumShortsInLpcFrame = 8;
                break;
            case 7:
                _sfArkInfo->v2.Unpack2EncodeLimit = 3;
                _sfArkInfo->v2.Unpack3EncodeLimit = 5;
                _sfArkInfo->v2.NumShortsInLpcFrame = 128;
            }
        }

        // Open (create) the output (soundfont) file
        if (!(errCode = createSfFile(sfontName)))
        {
            _sfArkInfo->Flags |= SFARK_OUT_OPEN;
            if (_sfArkInfo->CompressType == 2) _sfArkInfo->Flags |= SFARK_TEMP_MADE;

            // First V2 block always uses LZIP
            _sfArkInfo->RunState = SFARK_LZIP;
        }

        return errCode;
    }

    return SFARKERR_APP;
}


/********************** loadSfarkHeader() *******************
 * Searches for, and loads (into _sfArkInfo->WorkBuffer1), the
 * sfArk file's header.
 *
 * RETURN: 0 = success, or negative error number.
 *
 * Caller must account for little endian order, and
 * Ansi C filename, in the header.
 */

qint32 SfArkExtractor1::loadSfarkHeader()
{
    unsigned char *	fileHdrStartPtr = NULL;
    unsigned char *	filename;
    quint32	bytesToProcess;
    int				result;
    quint32	headerSize;
    unsigned char	flags;

    flags = 0;

    bytesToProcess = 0;

    // Look for the sfArk ID (5 ASCII chars). If an sfArk file, this should be
    // located at byte offset of 26 within the sfArk file header
    for (;;)
    {
        // Do we need to read more bytes in order to check for the ID?
        while (bytesToProcess < 26 + 5)
        {
            memmove(_sfArkInfo->WorkBuffer1, fileHdrStartPtr, bytesToProcess);
            fileHdrStartPtr = _sfArkInfo->WorkBuffer1;

            if (!(result = readSfarkFile(_sfArkInfo->WorkBuffer1 + bytesToProcess, (44 + 256) - bytesToProcess)))
            {
bad2:			result = SFARKERR_NOID;
                if (flags & 0x01) result = SFARKERR_BADHDR;
                if (flags & 0x02) result = SFARKERR_OLDVERS;
                if (flags & 0x04) result = SFARKERR_UNKCOMP;
                if (flags & 0x08) result = SFARKERR_BADHDRSUM;
            }

            if (result < 0)
                return result;

            bytesToProcess += (quint32)result;
            if (bytesToProcess < 26 + 5) goto bad2;
        }

        // Is the ID where we expect it?
        if (!memcmp(fileHdrStartPtr + 26, &SfArkId[1], strlen((const char *)SfArkId) - 1))
        {
            // Move header to buffer start so it's quint32 aligned. Some CPUs need this, if we access a quint32 field
            if (fileHdrStartPtr != _sfArkInfo->WorkBuffer1)
            {
                memmove(_sfArkInfo->WorkBuffer1, fileHdrStartPtr, bytesToProcess);
                fileHdrStartPtr = _sfArkInfo->WorkBuffer1;
            }

            // Do we need to read more bytes in order to grab the header upto the filename? Upto filename is
            // 44 bytes. we also make sure we've read the first char of the filename in prep of parsing it below
            if (bytesToProcess < 45)
            {
                if ((result = readSfarkFile(_sfArkInfo->WorkBuffer1 + bytesToProcess, (44 + 256) - bytesToProcess)) < 0)
                    return result;
                bytesToProcess += (quint32)result;
                if (bytesToProcess < 45) goto bad2;
            }

            // We found a legit sfArk header
            flags |= 0x01;

            // Version 1 sfArk files have a CompressType < 3 (ie,2)
            if (fileHdrStartPtr[31] > 3)
            {
                if (fileHdrStartPtr[31] <= 7)  break;
                flags |= 0x04;
            }

            // Check that it's version 1
            if (get_ulong(fileHdrStartPtr) == 0x04034c46 && fileHdrStartPtr[31] == 2)
            {
                // It's a V1 file
                flags |= 0x02;

                // We don't bother with decrypted files
                if (!(fileHdrStartPtr[7] & 0x40))
                {
                    quint32	slop;

                    // Get the filename len
                    headerSize = get_ushort(fileHdrStartPtr + 36);
                    slop = get_ushort(fileHdrStartPtr + 38) + headerSize;

                    // Make sure we read in the filename now (plus an additional str we ignore)
                    if (bytesToProcess < 40 + slop)
                    {
                        if ((result = readSfarkFile(_sfArkInfo->WorkBuffer1 + bytesToProcess, (slop + 40) - bytesToProcess)) < 0)
                            return result;
                        bytesToProcess += (quint32)result;
                        if (bytesToProcess < 40 + slop) goto bad2;
                    }

                    // Copy certain fields to the equivolent fields in the V2 header
                    memmove(fileHdrStartPtr + 42, fileHdrStartPtr + 40, headerSize);

                    // The Lspack entry we want has a filename that ends in ".sfArk$1" or ".sfArk$2"
                    if (headerSize > 8 && !memcmp(&fileHdrStartPtr[headerSize + 42 - 8], &SfArkId[0], strlen((const char *)SfArkId)))
                    {
                        fileHdrStartPtr[0] = fileHdrStartPtr[headerSize + 42 - 1];	// save whether part 1 or 2
                        headerSize -= 8;
                        fileHdrStartPtr[headerSize + 42] = 0;
                        memcpy(fileHdrStartPtr + 4, fileHdrStartPtr + 22, 4);
                        memmove(fileHdrStartPtr + 12, fileHdrStartPtr + 14, 4);
                        memcpy(fileHdrStartPtr + 38, fileHdrStartPtr + 18, 4);
                        fileHdrStartPtr[34] = (unsigned char)(headerSize & 0xFF);
                        fileHdrStartPtr[35] = (unsigned char)((headerSize >> 8) & 0xFF);
                        fileHdrStartPtr[36] = fileHdrStartPtr[37] = 0;
                        headerSize = 40 + slop;
                        goto out;
                    }
                }
            }
        }

        // Keep looking for the V2 header
more:	++fileHdrStartPtr;
        if (bytesToProcess) --bytesToProcess;
    }

    // Read in upto 256 ASCII chars of nul-terminated filename
    filename = fileHdrStartPtr + 42;
    do
    {
        // Read in more chars of filename?
        while ((quint32)(filename - fileHdrStartPtr) >= bytesToProcess)
        {
            if ((result = readSfarkFile(filename, (44 + 256) - bytesToProcess)) < 0)
                return result;
            if (!result) goto bad2;
            bytesToProcess += (quint32)result;
        }

    } while (*filename++);

    headerSize = (filename - fileHdrStartPtr);

    // Verify the header checksum (with the header checksum field zeroed, as that isn't included. But
    // first save that value so we can later compare it)
    flags |= 0x08;
    result = get_ulong(fileHdrStartPtr + 16);
    *((quint32 *)fileHdrStartPtr + 4) = 0;
    if (adler32(0, fileHdrStartPtr, headerSize) != (quint32)result) goto more;

    // Set file position to the end of the header
out:
    result = 0;
    if ((bytesToProcess -= headerSize)) result = setSfarkFilePos(-((int)bytesToProcess));

    return result;
}

void SfArkExtractor1::cleanFiles()
{
    if (_sfArkInfo->Flags & SFARK_OUT_OPEN) closeFile(_sfArkInfo->OutputFileHandle);
    if (_sfArkInfo->Flags & SFARK_IN1_OPEN) closeFile(_sfArkInfo->InputFileHandle);
    if (_sfArkInfo->Flags & SFARK_IN2_OPEN) closeFile(_sfArkInfo->v1.PartFileHandle);
    if (_sfArkInfo->Flags & SFARK_TEMP_MADE)
    {
        getTempPartName('2');
        unlink((char *)_sfArkInfo->WorkBuffer2);
        getTempPartName('1');
        unlink((char *)_sfArkInfo->WorkBuffer2);
    }

    _sfArkInfo->Flags &= ~(SFARK_OUT_OPEN|SFARK_IN1_OPEN|SFARK_IN2_OPEN|SFARK_TEMP_MADE);
    _sfArkInfo->RunState = 0;
}


/******************** skipEmbeddedText() *******************
 * Skips over the embedded (compressed) text in the sfArk file
 * at the current position.
 *
 * NOTE: Must not alter WorkBuffer1 or WorkBuffer2.
 */

qint32 SfArkExtractor1::skipEmbeddedText()
{
    int			result;
    quint32		size;
    unsigned char			buf[4];

    // Read the compressed size (quint32) of text
    if ((result = readSfarkFile(buf, 4)) != 4 || (size = get_ulong(buf)) > 262144)
    {
        if (result >= 0)
            result = SFARKERR_BADLEN;
    }

    // Just skip over it
    else
        result = setSfarkFilePos(size);

    return result;
}


/*********************** SfarkOpen() **********************
 * Opens an sfArk file.
 *
 * sf =			Handle returned by SfarkAlloc().
 * sfarkName =	Nul-terminated name of the sfArk file.
 *
 * RETURN: 0 = success, or negative error number.
 */

qint32 SfArkExtractor1::SfarkOpen(const char * sfarkName)
{
    qint32 errCode;

    // Close any prev open files
    cleanFiles();

    // Open sfArk file
    if (!(errCode = openSfarkFile(sfarkName)))
    {
        // Read the sfArk header into WorkBuffer1
        if (!(errCode = loadSfarkHeader()))
        {
            // Copy over header to _sfArkInfo. Note: int16/int32 data is stored in little
            // endian (80x86) order, so adjust if this system uses big endian
            _sfArkInfo->FileUncompSize = get_ulong(_sfArkInfo->WorkBuffer1 + 4);
            _sfArkInfo->FileChksum = get_ulong(_sfArkInfo->WorkBuffer1 + 12);
            _sfArkInfo->CompressType = _sfArkInfo->WorkBuffer1[31];
            _sfArkInfo->LeadingPadUncompSize = get_ulong(_sfArkInfo->WorkBuffer1 + 34);
            _sfArkInfo->BitPackEndOffset = get_ulong(_sfArkInfo->WorkBuffer1 + 38);

            // Copy the original sound name to WorkBuffer2, in preparation of the
            // app passing no filename to SfarkBeginExtract()
            strcpy((char *)_sfArkInfo->WorkBuffer2, (const char *)_sfArkInfo->WorkBuffer1 + 42);

            // Vers 1
            if (_sfArkInfo->CompressType == 2)
            {
                // For version 1, we need to extract two temp files from sfark in the first phase, then
                // recombine those 2 pieces into the soundfont. We need to save the original sf name
                // somewhere permanent so let's use the 1024 bytes at the end of WorkBuffer2 since it's much
                // larger than we need and won't be overwritten
                strcpy((char *)_sfArkInfo->WorkBuffer2 + 262144 - 1024, (const char *)_sfArkInfo->WorkBuffer1 + 42);

                // Note the part file we're extracting first. Could be '1' or '2'
                _sfArkInfo->v1.PartNum = _sfArkInfo->WorkBuffer1[0];
            }
            else
            {
                // See if there's an embedded readme and/or license file. If so, skip over them. If someone
                // is stupid enough to use the undocumented, proprietary sfArk format to compress his
                // soundfont, just to save a few hundred bytes over open standards like zip, then we don't
                // care about the idiot's docs
                if (_sfArkInfo->WorkBuffer1[0] & 2) errCode = skipEmbeddedText();
                if ((_sfArkInfo->WorkBuffer1[0] & 1) && !errCode) errCode = skipEmbeddedText();
            }
        }

        if (errCode)
            closeFile(_sfArkInfo->InputFileHandle);
        else
            _sfArkInfo->Flags |= SFARK_IN1_OPEN;
    }

    return errCode;
}
