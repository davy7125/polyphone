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

#include "sfarkextractor2.h"
#include <QDebug>
#include <string.h>
#include "zlib.h"

const char SfArkExtractor2::ProgName[] = "sfArkLib";
const char SfArkExtractor2::ProgVersion[] = " 3.00";	// 5 characters xx.xx
const unsigned char SfArkExtractor2::ProgVersionMaj = 30;		// 0-255 = V0 to V25.5xx, etc.
const unsigned char SfArkExtractor2::ProgVersionMin = 00;		// 0-99  = Vx.x99, etc.
const char SfArkExtractor2::LicenseExt[] = ".license.txt";		// File extension for license file
const char SfArkExtractor2::NotesExt[] = ".txt";			// File extension for notes file
const char SfArkExtractor2::SfExt[] = ".sf2";			// File extension for output file
const char SfArkExtractor2::CorruptedMsg[] = "- This file appears to be corrupted.";
const char SfArkExtractor2::UpgradeMsg[] = "Please see Help/About for information on how to obtain an update.";

// --------------------------------------------------------------------------
#define GRP_INBITS(g)                           \
    /* Get grp value from input by counting number of zeroes before we get a 1 */\
    g = 0;                                        \
    while (_bioBits == 0)                          \
{                                             \
    g += _bioRemBits;                            \
    CHECK_INBUFFER                              \
    _bioBits = _bioBuf[_bioP++];                   \
    _bioRemBits = BIO_WBITS;                     \
    }                                             \
    \
    g += _bioRemBits;                              \
    while ((_bioBits >> --_bioRemBits) != 1);       \
    g -= _bioRemBits+1;                            \
    _bioBits = LOWBITS(_bioBits, _bioRemBits)

// --------------------------------------------------------------------------
#define INBITS(w, n)                            \
    /* Input value w using n bits (n must be <= BIO_WBITS) ... */\
    if (_bioRemBits < BIO_WBITS)                   \
{                                             \
    CHECK_INBUFFER                              \
    _bioBits = (_bioBits << BIO_WBITS) | _bioBuf[_bioP++]; \
    _bioRemBits += BIO_WBITS;                    \
    }                                             \
    _bioRemBits -= n;                              \
    w = (quint16) (_bioBits >> _bioRemBits);        \
    _bioBits = LOWBITS(_bioBits, _bioRemBits)

// =========================================================================
#if BYTE_ORDER == BIG_ENDIAN

#define	WFIX(I)		s = bp[I+0]; bp[I+0] = bp[I+1]; bp[I+1] = s;
// Read from disk if needed, and fix endians
#define CHECK_INBUFFER                          \
    if (bioP == BIOBUFSIZE)                       \
{                                             \
    bioWholeBlocks++;                           \
    bioP = 0;                                   \
    int ReadLen = ReadInputFile((quint8 *) bioBuf, BIOBUFSIZE * sizeof(quint16)); \
    if (ReadLen <= 0)  return 0;		\
    quint8 *bp = (quint8 *) bioBuf, *ep = (quint8 *) (bioBuf+BIOBUFSIZE); \
    do {					\
    quint8 s;					\
    WFIX(0); WFIX(2); WFIX(4); WFIX(6);	\
    WFIX(8); WFIX(10); WFIX(12); WFIX(14);	\
    bp += 16;				\
    } while (bp < ep);				\
    }

#else
// Read from disk if needed...
#define CHECK_INBUFFER                          \
    if (_bioP == BIOBUFSIZE)                       \
{                                             \
    _bioWholeBlocks++;                           \
    _bioP = 0;                                   \
    int ReadLen = ReadInputFile((quint8 *) _bioBuf, BIOBUFSIZE * sizeof(quint16)); \
    if (ReadLen <= 0)  return 0;				\
    }
#endif


/////////////////////////////
/// CLASS SfArkExtractor2 ///
/////////////////////////////

SfArkExtractor2::SfArkExtractor2(const char * fileName) : AbstractExtractor(),
    _filename(fileName),
    _outputFileHandle(-1),
    _inputFileHandle(-1),
    _nb_init(0),
    _zbuf1(nullptr),
    _zbuf2(nullptr),
    _outFileNameMain(""),
    _outFileNameNotes(""),
    _outFileNameLicense("")
{
}

SfArkExtractor2::~SfArkExtractor2()
{
    _fileManager.clearData();
}

bool SfArkExtractor2::extract(const char *outputFilePath)
{
    _errorNumber = sfkl_Decode(_filename.toStdString().c_str(), outputFilePath);
    _error = (_errorNumber != SFARKLIB_SUCCESS && _errorNumber != SFARKLIB_ERR_CORRUPT && _errorNumber != SFARKLIB_ERR_FILECHECK);
    return !_error;
}

//////////////////////////
/// SFARKLIB CALLBACKS ///
//////////////////////////

// Message display function
void SfArkExtractor2::sfkl_msg(const char *MessageText, int Flags)
{
    qDebug() << MessageText << "(" << Flags << ")";
}

// Progress indication
void SfArkExtractor2::sfkl_UpdateProgress(int ProgressPercent)
{
    Q_UNUSED(ProgressPercent)
}

// Display/confirm license
bool SfArkExtractor2::sfkl_GetLicenseAgreement(const char *LicenseText, const char *LicenseFileName)
{
    Q_UNUSED(LicenseText)
    Q_UNUSED(LicenseFileName)

    // Delete the file that has been created
    _fileManager.deleteFile(LicenseFileName);

    return true;
}

// Display notes text file
void SfArkExtractor2::sfkl_DisplayNotes(const char *NotesText, const char *NotesFileName)
{
    Q_UNUSED(NotesText)

    // Delete the file that has been created
    _fileManager.deleteFile(NotesFileName);
}

//////////////////////
/// INPUT - OUTPUT ///
//////////////////////

int SfArkExtractor2::ChkErr(const char *ErrorMsg, bool isInput)
{
    char ErrDesc[SFARKLIB_MAX_MSGTEXT];

    if (~GlobalErrorFlag)		// Prevent multiple error messages
    {
        if (isInput)
            sprintf(ErrDesc, "Input: failed to %s", ErrorMsg);
        else
            sprintf(ErrDesc, "Output: failed to %s", ErrorMsg);
        sfkl_msg(ErrDesc, SFARKLIB_MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_FILEIO;
    }

    return GlobalErrorFlag;
}

void SfArkExtractor2::OpenOutputFile(const char *FileName)
{
    _outputFileHandle = _fileManager.create(FileName);
    if (_outputFileHandle == -1)
        ChkErr("create", false);
}

int SfArkExtractor2::WriteOutputFile(const quint8 *Buf, int BytesToWrite)
{
    if (_fileManager.write(_outputFileHandle, (char *)Buf, BytesToWrite) != BytesToWrite)
    {
        ChkErr("write", false);
        return 0;
    }

    return BytesToWrite;
}

bool SfArkExtractor2::SetOutputFilePosition(int NewPos)
{
    if (!_fileManager.setAbsolutePos(_outputFileHandle, NewPos))
    {
        ChkErr("set position", false);
        return false;
    }

    return true;
}

void SfArkExtractor2::CloseOutputFile()
{
    if (_outputFileHandle == -1)
        ChkErr("close", false);
    else
        _fileManager.close(_outputFileHandle);
    _outputFileHandle = -1;
}

void SfArkExtractor2::OpenInputFile(const char *FileName)
{
    _inputFileHandle = _fileManager.openReadOnly(FileName);
    if (_inputFileHandle == -1)
        ChkErr("open", true);
}

int SfArkExtractor2::ReadInputFile(quint8 *Buf, int BytesToRead)
{
    int BytesRead;

    BytesRead = _fileManager.read(_inputFileHandle, (char*)Buf, BytesToRead);
    if (BytesRead < 0)
    {
        ChkErr("read", true);
        BytesRead = 0;
    }

    return BytesRead;
}

bool SfArkExtractor2::SetInputFilePosition(int NewPos)
{
    if (!_fileManager.setAbsolutePos(_inputFileHandle, NewPos))
    {
        ChkErr("set position", true);
        return false;
    }

    return true;
}

void SfArkExtractor2::CloseInputFile()
{
    if (_inputFileHandle == -1)
        ChkErr("close", true);
    else
        _fileManager.close(_inputFileHandle);
    _inputFileHandle = -1;
}

// =========================================================================
// SFKL CRUNCH
// =========================================================================

void SfArkExtractor2::BioDecompInit()
{
    GetNBits(0);									// Initialise nb array
    _bioRemBits= 0;
    _bioBits	= 0;
    _bioP		= BIOBUFSIZE;
    _bioPfb	= 8;
    return;
}

void SfArkExtractor2::BioDecompEnd()
{
    return;
}

quint16 SfArkExtractor2::BioRead(int n)
// Read bits from input, return value
{
    quint16 w;
    INBITS(w, n);
    return w;
}

bool SfArkExtractor2::BioReadFlag(void)
// Read single bit from input, return value as bool
{
    quint16 w;
    INBITS(w, 1);
    return (w != 0);
}

long SfArkExtractor2::BioReadBuf(quint8 *buf, long n)
// Read *bytes* to output, return number of BYTES
{
    int SavebioP = _bioP;
    _bioWholeBlocks = 0;

    while (n--)
    {
        quint16 b;
        INBITS(b, 8);
        *buf++ = (quint8) b;
    }

    return (_bioP - SavebioP + _bioWholeBlocks * BIOBUFSIZE) * sizeof(quint16);
}

qint16 SfArkExtractor2::InputDiff(qint16 Prev)
// Read a value from input as difference from Previous value, return new value
{
    qint16 x;

    GRP_INBITS(x);
    if (x != 0)								// If non-zero, check sign bit
    {
        int sign;  INBITS(sign, SIGNBIT);
        if (sign != 0)  x = -x;
    }
    return x + Prev;
}

long SfArkExtractor2::UnCrunch(qint16 *UnCompBuf, quint16 bufsize)
{
    short     FixBits;
    qint16 *bp = UnCompBuf, *endp = UnCompBuf+bufsize;

    FixBits = InputDiff(_bioPfb);
    _bioPfb = FixBits;

    if (FixBits >= 0  &&  FixBits < 14)
    {
        // Uncompress the block...
        short FSbits = FixBits + SIGNBIT;
        do {
            quint16 grp, LoBits;
            INBITS(LoBits, FSbits);                       // Get LowBits & SignBit
            GRP_INBITS(grp);                              // Get group bits
            *bp = ((grp << FixBits) | (LoBits>>1)) ^ -(LoBits&1);    // NB: -Sign gives 0 or 0xFFFF
        } while (++bp < endp);
    }
    else if (FixBits == 14)       // Non-compressed block?
    {
        do {
            INBITS(*bp, AWORD_BITS);
        } while (++bp < endp);
    }
    else if (FixBits == -1)       // Data all 0 and -1 ?
    {
        do {
            INBITS(*bp, 1);           // Get SignBit
            *bp = -*bp;               // Sign=0 means word is 0, Sign=1 means word is -1;
        } while (++bp < endp);
    }
    else if (FixBits == -2)       // Data all 0?
    {
        do *bp = 0; while (++bp < endp);    // Set buffer to 0
    }
    else                          // Shouldn't happen!
        return -1;									// Issues an error message

    return 0;
}

long SfArkExtractor2::UnCrunchWin(qint16 *buf, quint16 bufsize, quint16 winsize)
{
    quint16 finalwinsize = bufsize % winsize;
    qint16 *endp = buf + bufsize - finalwinsize;

    for ( ; buf < endp; buf += winsize)
    {
        long result = UnCrunch(buf, winsize);
        if (result < 0)  return result;
    }

    if (finalwinsize)
    {
        long result = UnCrunch(buf, finalwinsize);
        if (result < 0)  return result;
    }

    return bufsize;
}

short SfArkExtractor2::GetNBits(short w)
// Get the number of bits used by value w
// e.g. w=000b, n=0
//      w=001b, n=1
//      w=010b, n=2
//      w=011b, n=2
//      w=100b, n=3
//      w=111b, n=3 etc.
{
    // If not initialised (first time), build table in nb[] array...
    if (_nb_init == 0)
    {
        long first = 1, last, i;
        quint8 nbits = 1;
        _nb_init = 1;
        _nb[0] = 0;

        do {
            last = first + (1 << (nbits-1));
            for (i = first; i < last; i++)  _nb[i] = nbits;
            nbits++;
            first = last;
        } while(last <= MAX_AWORD);
    }
    return _nb[w];
}

// ==============================================================
// SFKL CODING
// ==============================================================

quint16 SfArkExtractor2::GetsfArkLibVersion(void)
{
    return (ProgVersionMaj * 10) + ProgVersionMin/10;
}

// Read the File Header....
int SfArkExtractor2::ReadHeader(V2_FILEHEADER *FileHeader, quint8 *fbuf, int bufsize)
{
    int HeaderLen = 0, HdrOffset;
    char	CreatedByProg[HDR_NAME_LEN +1],  CreatedByVersion[HDR_VERS_LEN +1];
    quint32	CalcHdrCheck = 0;
    quint8 *HdrBuf, *bpFileHeader = (quint8 *) FileHeader;

    // Find and process the Header:  This could be a plain sfArk file, a self-extracting file or some other (invalid) file.
    // Also, it could be a sfArk V1 file, which we can't decompress, but should at least recognise.
    // We locate the header by looking for the string "sfArk" within the first HEADER_MAX_OFFSET bytes of the file
    // but because self-extractor code is likely to contain that string, we look for its final occurence
    // by searching backwards.
    // To speed things up, we first check at offset 0 (the case for a standard sfArk file)
    // If we think we've found a (V2) header, we veryify it using the Header checksum.  If the checksum fails,
    // chances are it was a corrupt file, but could conceivably be a non-sfArk file.  either way we report it as corrupt.

    int fbufsize = HEADER_MAX_OFFSET + V2_FILEHEADER_SIZE;		// Amount data to read
    if (fbufsize > bufsize)  fbufsize = bufsize;				// Buffer too small (should never happen)

    SetInputFilePosition(0);						// set to logical start (maybe a predefined offset)
    RETURN_ON_ERROR();
    ReadInputFile(fbuf, fbufsize);					// Read a chunk of data from the start of the file
    RETURN_ON_ERROR();

    bool SigFound = false, SeemsV1 = false, HdrCheckDone = false;		// Some flags to remember what we're doing

    for(int TryOffset = 0; TryOffset < HEADER_MAX_OFFSET; TryOffset++)
    {
        HdrOffset = (TryOffset == 0)? 0 : HEADER_MAX_OFFSET - TryOffset; // Check offset = 0 first, then backwards from end

        quint8 *sigpos = fbuf + HdrOffset + HEADER_SIG_POS;
        if (*sigpos != 's'  ||  memcmp(sigpos, "sfArk", 5) != 0)  continue;

        SigFound = true;					// Set a flag to remember that we at least got this far
        HdrBuf = fbuf + HdrOffset;

        if (V2_FILEHEADER_SIZE != sizeof(V2_FILEHEADER))	// Compare structure size to real size
        {
            // The compiler has messed with structure (alignment), so copy the data to the structure byte by byte...
            quint8 *bptr = HdrBuf;		// Point to start
            // Copy all fields...
#define CPF(f) memcpy(&(FileHeader->f), bptr, sizeof(FileHeader->f)); bptr += sizeof(FileHeader->f)
            CPF(Flags); CPF(OriginalSize); CPF(CompressedSize); CPF(FileCheck); CPF(HdrCheck);
            CPF(ProgVersionNeeded); CPF(ProgVersion); CPF(ProgName); CPF(CompMethod);
            CPF(FileType); CPF(AudioStart); CPF(PostAudioStart); CPF(FileName);
#undef CPF
            if (bptr != HdrBuf+V2_FILEHEADER_SIZE)	return (GlobalErrorFlag = SFARKLIB_ERR_OTHER);	// Sanity check
        }
        else
            memcpy(bpFileHeader, HdrBuf, V2_FILEHEADER_SIZE);	// Copy entire data block to structure

        if (FileHeader->CompMethod < COMPRESSION_v2)	// Looks like a sfArk V1 file?
        {
            SeemsV1 = true;					// set a flag
            continue;						// and keep searching
        }

        // Header->FileName is a null-terminated string, we need it's length to calculate actual length of header data...
        FileHeader->FileName[sizeof(FileHeader->FileName) -1] = '\0'; // Ensure strlen finds a terminator (string may be junk)
        HeaderLen = V2_FILEHEADER_SIZE - sizeof(FileHeader->FileName) + strlen(FileHeader->FileName) + 1;

        // If we get this far, there's a good chance we've got the header...
#if BYTE_ORDER == BIG_ENDIAN
        // FixEndians of all multi-byte integers (currently only relevent to Mac)
#define FIXENDIAN(field)	FixEndian(&(FileHeader->field), sizeof(FileHeader->field))
        FIXENDIAN(Flags); 	FIXENDIAN(OriginalSize); 	FIXENDIAN(CompressedSize);
        FIXENDIAN(FileCheck);	FIXENDIAN(HdrCheck); 		FIXENDIAN(FileType);
        FIXENDIAN(AudioStart);	FIXENDIAN(PostAudioStart);
#undef FIXENDIAN
#endif

        // Ok now, we know the HeaderLength and have the FileHeader structure properly populated...
#if 0
        // debug, display header...
        printf("Flags %lx  OrignalSize %ld  CompressedSize %ld\n", FileHeader->Flags, FileHeader->OriginalSize, FileHeader->CompressedSize);
        printf("FileCheck %lx  HdrCheck %lx  ProgVersionNeeded %d\n", FileHeader->FileCheck, FileHeader->HdrCheck, FileHeader->ProgVersionNeeded);
        printf("AudioStart %ld  PostAudioStart %ld  Orginal filename %s\n", FileHeader->AudioStart, FileHeader->PostAudioStart, FileHeader->FileName);
#endif
        *(uint32_t *)(HdrBuf+HEADER_HDRCHECK_POS) = 0;			// Zero-out the HeaderChecksum position in the buffer
        CalcHdrCheck = adler32(0, HdrBuf, HeaderLen);		// and recalculate the header checksum
        HdrCheckDone = true;
        if (CalcHdrCheck == FileHeader->HdrCheck)  break;		// Check passed: Yes, we've found the header!
    }

    // When we get here, see what happened:
    if (SigFound && HdrCheckDone && CalcHdrCheck == FileHeader->HdrCheck)	// Everything Ok!  File is V2 and valid
        ; 							// Fall through to below (everything else is an error)
    else if (SeemsV1)					// Seems to be a sfArkV1 file
    {
        sprintf(MsgTxt, "This file was created with sfArk V1, and this program only handles sfArk V2+ files.  Unfortunately sfArk V1 uses a proprietary compression algorithm for the non-audio metadata, so we cannot really support that. You might try running the Windows sfArk program from http://melodymachine.com/sfark.htm under Wine.");
        msg(MsgTxt, MSG_PopUp);
        return (GlobalErrorFlag = SFARKLIB_ERR_INCOMPATIBLE);
    }
    else if (SigFound)					// Apparently a corrupt sfArk file (well, it had "sfArk" in it!)
    {
        sprintf(MsgTxt, "File Header fails checksum!%s", CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        return (GlobalErrorFlag = SFARKLIB_ERR_HEADERCHECK);
    }
    else							// Either very corrupted, or not a sfArk file
    {
        sprintf(MsgTxt, "This does not appear to be a sfArk file!");
        msg(MsgTxt, MSG_PopUp);
        return (GlobalErrorFlag = SFARKLIB_ERR_SIGNATURE);
    }

    // Get CreatedBy program name and version number (need null-terminated strings)...
    strncpy(CreatedByProg, FileHeader->ProgName, HDR_NAME_LEN);			// Copy program name
    CreatedByProg[HDR_NAME_LEN] = 0;																		// Terminate string
    strncpy(CreatedByVersion, FileHeader->ProgVersion, HDR_VERS_LEN);		// Copy version string
    CreatedByVersion[HDR_VERS_LEN] = 0;																	// Terminate string

    // Check for compatible version...
    if (FileHeader->ProgVersionNeeded > ProgVersionMaj)
    {
        sprintf(MsgTxt, "You need %s version %2.1f (or higher) to decompress this file (your version is %s) %s",
                ProgName, (float)FileHeader->ProgVersionNeeded/10, ProgVersion, UpgradeMsg);
        msg(MsgTxt, MSG_PopUp);
        return (GlobalErrorFlag = SFARKLIB_ERR_INCOMPATIBLE);
    }

    // Warn if file was created by a newer version than this version...
    float fProgVersion = (float) atof(ProgVersion);
    float fCreatedByVersion = (float) atof(CreatedByVersion);
    if (fCreatedByVersion > fProgVersion)
    {
        sprintf(MsgTxt, "This file was created with %s %s.  Your version of %s (%s) can uncompress this file, "
                        "but you might like to obtain the latest version.  %s",
                CreatedByProg, CreatedByVersion, ProgName, ProgVersion, UpgradeMsg);
        msg(MsgTxt, MSG_PopUp);
    }

    SetInputFilePosition(HdrOffset + HeaderLen);	// re-wind file to start of post-header data
    RETURN_ON_ERROR();
    return SFARKLIB_SUCCESS;
}

void SfArkExtractor2::InitFilenames(const char *OrigFileName, const char * /*InFileName*/, const char *ReqOutFileName)
{
    if (ReqOutFileName)
    {
        ChangeFileExt(ReqOutFileName, SfExt, _outFileNameMain, sizeof(_outFileNameMain));
        ChangeFileExt(ReqOutFileName, NotesExt, _outFileNameNotes, sizeof(_outFileNameNotes));
        ChangeFileExt(ReqOutFileName, LicenseExt, _outFileNameLicense, sizeof(_outFileNameLicense));
    }
    else
    {
        if (strchr(OrigFileName, '/') || strchr(OrigFileName, '\\'))
        {
            msg("Please run Decode again and provide an output filename. We cannot use the original filename because"
                " it contains a disallowed character.", MSG_PopUp);
        }
        else
        {
            ChangeFileExt(OrigFileName, SfExt, _outFileNameMain, sizeof(_outFileNameMain));
            ChangeFileExt(OrigFileName, NotesExt, _outFileNameNotes, sizeof(_outFileNameNotes));
            ChangeFileExt(OrigFileName, LicenseExt, _outFileNameLicense, sizeof(_outFileNameLicense));
        }
    }

    if (!_outFileNameMain[0] || !_outFileNameNotes[0] || !_outFileNameLicense[0])
    {
        msg("Could not get output path (path too long?)", MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_FILEIO;
    }
}

bool SfArkExtractor2::InvalidEncodeCount(int EncodeCount, int MaxLoops)
{
    if (EncodeCount < 0  ||  EncodeCount > MaxLoops)	// EncodeCount out of range?
    {
        sprintf(MsgTxt, "ERROR - Invalid EncodeCount (apparently %d) %s", EncodeCount, CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        return true;
    }
    else
        return false;
}

int SfArkExtractor2::DecompressTurbo(BLOCK_DATA *Blk, quint16 NumWords)
{
    int EncodeCount = InputDiff(Blk->PrevEncodeCount);
    if (InvalidEncodeCount(EncodeCount, Blk->MaxLoops))  return (GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
    Blk->PrevEncodeCount = EncodeCount;

    int UnCrunchResult = UnCrunchWin(Blk->SrcBuf, NumWords, 8*OPTWINSIZE);
    if (UnCrunchResult < 0)
    {
        sprintf(MsgTxt, "ERROR - UnCrunchWin returned: %d %s", UnCrunchResult, CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        return (GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
    }

    for (int j = EncodeCount-1; j >= 0; j--)
    {
        if (j == 0)  Blk->FileCheck = (Blk->FileCheck << 1) + BufSum(Blk->SrcBuf, NumWords);
        UnBufDif2(Blk->DstBuf, Blk->SrcBuf, NumWords, &(Blk->PrevIn[j]));
        qint16 *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
    }
    return SFARKLIB_SUCCESS;
}

bool SfArkExtractor2::CheckShift(short *ShiftVal, quint16 NumWords, short *PrevShift, short *PrevUsedShift)
// Here we look to see if the current buffer has been rightshifted
// There is a flag for the whole buffer to show if any shifted data exists,
// and if so there further flags to show if the Shift value changes within each sub block
// of ShiftWin words.
// For any changes, then follows the sub-block number where change occurs, then the new
// ShiftValue.  The latter is given either as absolute value (if shift was previously
// non-zero) or relative change from previously used value (if shift was previously zero)
// For non-zero shift value, we then need to leftshift the data by that number of bits.
{
#define		ShiftWin	64		// Size of window for Shift

    bool UsingShift = BioReadFlag();		// Read flag to see if using any shift
    if (UsingShift)											// and if so...
    {
        int MaxShifts = (NumWords+ShiftWin-1) / ShiftWin;		// Number of ShiftWin sized sub-blocks in this block
        int ChangePos = 0;																	// Init. position of last change

        int p = 0;
        while (BioReadFlag())		// Read flag to see if there is a (further) change of shift value
        {
            // Read position of new shift value...
            int nb = GetNBits(MaxShifts - ChangePos -1);	// number of possible bits for ChangePos
            ChangePos = BioRead(nb) + ChangePos;          // Get position of next change of shift value

            // Read value of new shift...
            short	NewShift;
            if (*PrevShift == 0)													// If previous shift was 0
            {
                NewShift = InputDiff(*PrevUsedShift);				// Get new shift as diff from last used shift
                *PrevUsedShift = NewShift;									// Update PrevUsedShift
            }
            else                                          // Else
                NewShift = InputDiff(0);                    // Get new shift as difference from 0

            // Update all ShiftVal[] data prior to change...
            if (ChangePos > MaxShifts)										// Corrupt data?
            {
                sprintf(MsgTxt, "ERROR - Invalid Shift ChangePos (apparently %d) %s", ChangePos, CorruptedMsg);
                msg(MsgTxt, MSG_PopUp);
                GlobalErrorFlag = SFARKLIB_ERR_CORRUPT;
                return false;
            }

            for ( ; p < ChangePos; p++) ShiftVal[p] = *PrevShift;
            *PrevShift = NewShift;                        // Update prev shift
        }

        for ( ; p < MaxShifts; p++)	ShiftVal[p] = *PrevShift;		// Fill array to end with final shift value

    }
    return UsingShift;
}

int SfArkExtractor2::DecompressFast(BLOCK_DATA *Blk, quint16 NumWords)
{
    int	i, EncodeCount;
    short	ShiftVal[NSHIFTS];						// Shift values (one per SHIFTWIN words)
    quint16	Method[MAX_DIFF_LOOPS];				// Block processing methods used per iteration

#if	DB_BLOCKCHECK											// If debug mode block check enabled
    quint32 BlockCheck = BioRead(16);				// Read block check bits
#endif

    bool UsingShift = CheckShift(ShiftVal, NumWords, &Blk->PrevShift, &Blk->PrevUsedShift);
    bool UsingBD4 = BioReadFlag();			// See if using BD4

    if (UsingBD4)
    {
        EncodeCount = InputDiff(Blk->BD4PrevEncodeCount);
        if (InvalidEncodeCount(EncodeCount, Blk->MaxBD4Loops))  return(GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
        Blk->BD4PrevEncodeCount = EncodeCount;
    }
    else	// Using BD2/3
    {
        EncodeCount = InputDiff(Blk->PrevEncodeCount);
        if (InvalidEncodeCount(EncodeCount, Blk->MaxLoops))  return(GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
        Blk->PrevEncodeCount = EncodeCount;

        for(i = 0; i < EncodeCount; i++)
            Method[i] = BioReadFlag();			// Read flags for BD2/3
    }

    // If using LPC, check for and read flags...
    quint32 LPCflags;
    bool UsingLPC = (Blk->FileHeader.CompMethod != COMPRESSION_v2Fast);
    if (UsingLPC)
    {
        if (BioReadFlag())	// Any flags?
            LPCflags = BioRead(16) | (BioRead(16) << 16);	// Then read them (32 bits)
        else																						// else
            LPCflags = 0;
    }

    // Read the file and unpack the bitstream into buffer at Buf1p...
    if (int UnCrunchResult = UnCrunchWin(Blk->SrcBuf, NumWords, OPTWINSIZE) < 0)		// failed?
    {
        sprintf(MsgTxt, "ERROR - UnCrunchWin returned: %d %s", UnCrunchResult, CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        return(GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
    }

    if (UsingLPC)
    {
        UnLPC(Blk->DstBuf, Blk->SrcBuf, NumWords, Blk->nc, &LPCflags);
        qint16 *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
    }

    if (UsingBD4)
    {
        for (i = EncodeCount-1; i >= 0; i--)
        {
            UnBufDif4(Blk->DstBuf, Blk->SrcBuf, NumWords, &(Blk->PrevIn[i]));
            qint16 *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
        }
    }
    else
    {
        for (i = EncodeCount-1; i >= 0; i--)
        {
            switch (Method[i])
            {
            case 0: UnBufDif2(Blk->DstBuf, Blk->SrcBuf, NumWords, &(Blk->PrevIn[i])); break;
            case 1: UnBufDif3(Blk->DstBuf, Blk->SrcBuf, NumWords, &(Blk->PrevIn[i])); break;
            }
            qint16 *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
        }
    }

    if (UsingShift)  UnBufShift(Blk->SrcBuf, NumWords, ShiftVal);

#if	DB_BLOCKCHECK											// If debug mode block check enabled
    quint32 CalcBlockCheck = adler32(0, (const quint8 *) Blk->SrcBuf, 2*NumWords) & 0xffff;
    //quint32 CalcBlockCheck = Blk->FileCheck & 0xffff;
    //printf("Audio Block Checks Read: %ld, Calc %ld  Length=%d\n", BlockCheck, CalcBlockCheck, 2*NumWords);
    //getc(stdin);
    if (BlockCheck != CalcBlockCheck)			// Compare to calculated cheksum
    {
        msg("*** Audio Block check FAIL");
    }
    //else
    //  printf("Audio Block check Ok\n");
#endif

    Blk->FileCheck = 2 * Blk->FileCheck + BufSum(Blk->SrcBuf, NumWords);
    return SFARKLIB_SUCCESS;
}

int SfArkExtractor2::ProcessNextBlock(BLOCK_DATA *Blk)
{
    //int	TotBytesRead = 0;						// Total bytes read in file
    int	NumWords;							//

    uint32_t	n, m;							// NB: Must be 32-bit integer

#define	AWBYTES	(sizeof(qint16))
    quint8 *zSrcBuf = (quint8 *) Blk->SrcBuf;
    quint8 *zDstBuf = (quint8 *) Blk->DstBuf;

    switch (Blk->FileSection)
    {
    case AUDIO:
    {
        NumWords = Blk->ReadSize;					// Number of words we will read in this block
        n = NumWords * AWBYTES;						// ... and number of bytes

        if (Blk->TotBytesWritten + n >= Blk->FileHeader.PostAudioStart)			// Short block? (near end of file)
        {
            n = Blk->FileHeader.PostAudioStart - Blk->TotBytesWritten;				// Get exact length in bytes
            NumWords = n / AWBYTES;																						// ... and words
            Blk->FileSection = POST_AUDIO;		// End of Audio -- PostAudio section is next
        }

        //printf("AUDIO, read %ld bytes\n", n);

        if (Blk->FileHeader.CompMethod == COMPRESSION_v2Turbo)						// If using Turbo compression
            DecompressTurbo(Blk, NumWords);									// Decompress
        else																															// For all other methods
            DecompressFast(Blk, NumWords);										// Decompress

        //printf("B4 WriteOutputFile: %ld\n", adler32(0, (const quint8 *) Blk->SrcBuf, n) & 0xffff);
#if BYTE_ORDER == BIG_ENDIAN
#define	WFIX(I)		s = bp[I+0]; bp[I+0] = bp[I+1]; bp[I+1] = s;
        quint8 *bp = (quint8 *) Blk->SrcBuf; quint8 *ep = bp + n;
        do {
            quint8 s;
            WFIX(0); WFIX(2); WFIX(4); WFIX(6);
            WFIX(8); WFIX(10); WFIX(12); WFIX(14);
            bp += 16;
        } while (bp < ep);
#undef WFIX
#endif

        WriteOutputFile((const quint8 *)Blk->SrcBuf, n);										// Write to output file
        Blk->TotBytesWritten += n;																				// Accumulate total bytes written
        break;
    }

    case PRE_AUDIO: case POST_AUDIO: case NON_AUDIO:
    {
        BioReadBuf((quint8 *) &n, sizeof(n));
        FixEndian(&n, sizeof(n));
        //printf("Reading PRE/POST AUDIO block, compressed %ld bytes\n", n);
        if (n > ZBUF_SIZE)	// Check for valid block length
        {
            sprintf(MsgTxt, "ERROR - Invalid length for Non-audio Block (apparently %d bytes) %s", n, CorruptedMsg);
            msg(MsgTxt, MSG_PopUp);
            return (GlobalErrorFlag = SFARKLIB_ERR_CORRUPT);
        }

        BioReadBuf(zSrcBuf, n);		// Read the block
        m = UnMemcomp(zSrcBuf, n, zDstBuf, ZBUF_SIZE);											//printf("PRE/POST AUDIO block, compressed %ld bytes, uncompressed %ld bytes\n", n, m);

        // Uncompress
        if (GlobalErrorFlag != SFARKLIB_SUCCESS)  return(GlobalErrorFlag);
        if (m <= ZBUF_SIZE)														// Uncompressed ok & size is valid?
        {
            //printf("writing uncompressed block %ld bytes\n", m);
            Blk->FileCheck = adler32(Blk->FileCheck, zDstBuf, m);	// Accumulate checksum
            WriteOutputFile(zDstBuf, m);				// and write to output file
            Blk->TotBytesWritten += m;				// Accumulate byte count
        }
        else
            return SFARKLIB_ERR_CORRUPT;

#if	DB_BLOCKCHECK					// If debug mode block check enabled
        quint32 BlockCheck = BioRead(16);				// Read block check bits
        FixEndian(&BlockCheck, sizeof(Blockcheck));
        quint32 CalcBlockCheck = adler32(0, zDstBuf, m) & 0xFFFF;
        printf("NonAudio Block Checks Read: %ld, Calc %ld Length=%d\n", BlockCheck, CalcBlockCheck, m);
        if (BlockCheck != CalcBlockCheck)			// Compare to calculated cheksum
        {
            printf("*** NonAudio Block check FAIL\n");
        }
        else
            printf("NonAudio Block check Ok\n");
#endif

        //printf("PRE/POST AUDIO, read %ld bytes, writing %ld bytes...", n, m);

        if (Blk->TotBytesWritten >= Blk->FileHeader.OriginalSize)	// Have we finished the file?
            Blk->FileSection = FINISHED;
        else if (Blk->FileSection == PRE_AUDIO && Blk->TotBytesWritten >= Blk->FileHeader.AudioStart)	// finished Pre-Audio?
            Blk->FileSection = AUDIO;					// .. then next section is Audio

        break;
    } // case
    } //switch

    //sprintf(MsgTxt, "BytesWritten: %ld of %ld", Blk->TotBytesWritten, Blk->FileHeader.OriginalSize);
    //msg(MsgTxt, 0);
    return SFARKLIB_SUCCESS;
}

int	SfArkExtractor2::EndProcess(int ErrorNum)
{
    CloseInputFile();
    CloseOutputFile();
    return ErrorNum;
}

// Extract License & Notes files
// These are stored as 4-bytes length, followed by length-bytes of compressed data
bool SfArkExtractor2::ExtractTextFile(BLOCK_DATA *Blk, quint32 FileType)
{
    quint32 n, m;
    quint8 *zSrcBuf = (quint8 *) Blk->SrcBuf;
    quint8 *zDstBuf = (quint8 *) Blk->DstBuf;

    const char *OutFileName = 0;
    if (FileType == FLAGS_License)
        OutFileName = _outFileNameLicense;
    else if (FileType == FLAGS_Notes)
        OutFileName = _outFileNameNotes;
    else
        return false;

    // NB:Can't use BioReadBuf... aligment problems? Yet it works for ProcessNextBlock() !!??
    // Ok, can use ReadInputFile here cause everythjing is whole no. of bytes...

    //BioReadBuf((quint8 *)&n, sizeof(n));					// Read length of block from file
    ReadInputFile((quint8 *)&n, sizeof(n));					// Read length of block from file
    FixEndian(&n, sizeof(n));										// Fix endian

    if (n <= 0  ||  n > ZBUF_SIZE)								// Check for valid block length
    {
        sprintf(MsgTxt, "ERROR - Invalid length for %s file (apparently %d bytes) %s", OutFileName, n, CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_CORRUPT;
        return false;
    }

    //BioReadBuf(zSrcBuf, n);																					// Read the block
    ReadInputFile((quint8 *)zSrcBuf, n);																// Read the block
    m = UnMemcomp(zSrcBuf, n, zDstBuf, ZBUF_SIZE);										// Uncompress
    Blk->FileCheck = adler32(Blk->FileCheck, zDstBuf, m);	   					// Accumulate checksum
    if (GlobalErrorFlag  ||  m > ZBUF_SIZE)														// Uncompressed ok & size is valid?
        return false;

    OpenOutputFile(OutFileName);	// Create notes / license file
    WriteOutputFile(zDstBuf, m);																			// and write to output file
    CloseOutputFile();
    if (FileType == FLAGS_License)
    {
        sprintf(MsgTxt, "Created license file: %s", OutFileName);
        msg(MsgTxt, 0);
        if (GetLicenseAgreement((const char *)zDstBuf, OutFileName) == false)
        {
            GlobalErrorFlag = SFARKLIB_ERR_LICENSE;
            return EndProcess(false);
        }
    }
    else if (FileType == FLAGS_Notes)
    {
        sprintf(MsgTxt, "Created notes file: %s", OutFileName);
        msg(MsgTxt, 0);
        DisplayNotes((const char *)zDstBuf, OutFileName);
    }

    return true;
}

int SfArkExtractor2::Decode(const char *InFileName, const char *ReqOutFileName)
{
    int	NumLoops;			// Number of loops before screen update etc.

    BLOCK_DATA	Blk;
    memset(&Blk, 0, sizeof(Blk));
    V2_FILEHEADER	*FileHeader = &Blk.FileHeader;

    // NB: We keep 2 buffers with pointers in Blk->SrcBuf and Blk->DstBuf
    // Generally we process from SrcBuf to DstBuf then swap the pointers,
    // so that current data is always at SrcBuf

    // NB: On MacOS/GNU C, the following allocation of Zbuf1 and Zbuf2 causes "segmentation fault"
    // quint8	Zbuf1[ZBUF_SIZE];					// Buffer1
    // quint8	Zbuf2[ZBUF_SIZE];					// Buffer2
    // so instead...

    if (_zbuf1 == NULL)
        _zbuf1 = (quint8 *) calloc(ZBUF_SIZE, sizeof(quint8));
    if (_zbuf2 == NULL)
        _zbuf2 = (quint8 *) calloc(ZBUF_SIZE, sizeof(quint8));
    if (_zbuf1 == NULL  ||  _zbuf2 == NULL)
        return EndProcess(SFARKLIB_ERR_MALLOC);

    Blk.SrcBuf = (qint16 *) _zbuf1;					// Point to Zbuf1
    Blk.DstBuf = (qint16 *) _zbuf2;					// Point to Zbuf2

    // Initialisation...
    BioDecompInit();						// Initialise bit i/o
    LPCinit();							// Init LPC
    GlobalErrorFlag = SFARKLIB_SUCCESS;

    // Open input (.sfArk) file and read the header...
    OpenInputFile(InFileName);
    if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);				// Something went wrong?
    ReadHeader(FileHeader, _zbuf1, ZBUF_SIZE);

    if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);				// Something went wrong?

    InitFilenames(FileHeader->FileName, InFileName, ReqOutFileName);
    if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);				// Something went wrong?

    if ((FileHeader->Flags & FLAGS_License) != 0)		// License file exists?
    {
        if (ExtractTextFile(&Blk, FLAGS_License) == false)
            return EndProcess(GlobalErrorFlag);
    }

    if ((FileHeader->Flags & FLAGS_Notes) != 0)		// Notes file exists?
    {
        if (ExtractTextFile(&Blk, FLAGS_Notes) == false)
            return EndProcess(GlobalErrorFlag);
    }

    OpenOutputFile(_outFileNameMain);					// Create the main output file...

    // Set the decompression parameters...
    switch (FileHeader->CompMethod)		// Depending on compression method that was used...
    {
    case COMPRESSION_v2Max:
    {
        //msg("Compression: Max\n");
        Blk.ReadSize = 4096;
        Blk.MaxLoops = 3;
        Blk.MaxBD4Loops = 5;
        Blk.nc = 128;
        Blk.WinSize = OPTWINSIZE;
        NumLoops = 2 * 4096 / Blk.ReadSize;
        break;
    }
    case COMPRESSION_v2Standard:
    {
        //printf("Compression: Standard\n");
        Blk.MaxLoops = 3;
        Blk.MaxBD4Loops = 3;
        Blk.ReadSize = 4096;
        Blk.nc = 8;
        Blk.WinSize = OPTWINSIZE;
        NumLoops = 50 * 4096 / Blk.ReadSize;
        break;
    }
    case COMPRESSION_v2Fast:
    {
        //printf("Compression: Fast\n");
        Blk.MaxLoops = 20;
        Blk.MaxBD4Loops = 20;
        Blk.ReadSize = 1024;
        Blk.WinSize = OPTWINSIZE;
        NumLoops = 300 * 4096 / Blk.ReadSize;
        break;
    }
    case COMPRESSION_v2Turbo:
    {
        //printf("Compression: Turbo\n");
        Blk.MaxLoops = 3;
        Blk.MaxBD4Loops = 0;
        Blk.ReadSize = 4096;
        Blk.WinSize = OPTWINSIZE << 3;
        NumLoops = 400 * 4096 / Blk.ReadSize;
        break;
    }
    default:
    {
        sprintf(MsgTxt, "Unknown Compression Method: %d%s", FileHeader->CompMethod, CorruptedMsg);
        GlobalErrorFlag = SFARKLIB_ERR_INCOMPATIBLE;
        msg(MsgTxt, MSG_PopUp);
        return EndProcess(GlobalErrorFlag);
    }
    }

    // Process the main file...
    Blk.FileSection = PRE_AUDIO;		// We start with pre-audio data

    quint32 ProgressUpdateInterval = Blk.FileHeader.OriginalSize / 100; // Calculate progress update
    quint32 NextProgressUpdate = ProgressUpdateInterval;
    int ProgressPercent = 0;
    UpdateProgress(0);

    //int BlockNum = 0;
    for (Blk.FileSection = PRE_AUDIO; Blk.FileSection != FINISHED; )
    {
        for (int BlockCount = 0; BlockCount < NumLoops  &&  Blk.FileSection != FINISHED; BlockCount++)
        {
            //printf("Block: %d\n", ++BlockNum);
            ProcessNextBlock(&Blk);

            while (Blk.TotBytesWritten >= NextProgressUpdate)  	// Further 1% done since last UpdateProgress?
            {
                UpdateProgress(++ProgressPercent);
                NextProgressUpdate += ProgressUpdateInterval;
            }
            if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);
        }
        // CheckForCancel();
        if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);
    }

    if (ProgressPercent != 100)  UpdateProgress(100);

    // Check the CheckSum...
    if (Blk.FileCheck != FileHeader->FileCheck)
    {
        sprintf(MsgTxt, "CheckSum Fail!%s",CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        //sprintf(MsgTxt, "Calc check %lx", Blk.FileCheck);
        //msg(MsgTxt, MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_FILECHECK;
        return EndProcess(GlobalErrorFlag);
    }

    sprintf(MsgTxt, "Created %s (%d KiB) successfully.", _outFileNameMain, Blk.TotBytesWritten/1024);
    msg(MsgTxt, 0);

    return EndProcess(GlobalErrorFlag);
}

// Adjust integer held at *num for this machine's endian system
#if BYTE_ORDER == BIG_ENDIAN
void SfArkExtractor2::FixEndian(void *num, int nsize)
{
    int i;
    quint8 bb[4];
    for (i = 0; i < nsize; i++)  bb[i] = ((quint8 *) num)[i];
    for (i = 0; i < nsize; i++)  ((quint8 *) num)[i] = bb[nsize-1-i];
}
#else
void SfArkExtractor2::FixEndian(void * /*num*/, int /*nsize*/) {}
#endif

// ==============================================================
// SFKL ZIP
// ==============================================================

quint32 SfArkExtractor2::UnMemcomp(const quint8 *InBuf, int InBytes, quint8 *OutBuf, int OutBufLen)
{
    // Uncompress buffer using ZLIBs uncompress function...
    unsigned long OutBytes = OutBufLen;
    int Result = uncompress(OutBuf, &OutBytes, InBuf, InBytes);
    if (Result != Z_OK)				// uncompress failed?
    {
        //sprintf(MsgTxt, "ZLIB uncompress failed: %d", Result);
        //msg(MsgTxt, MSG_PopUp);
        OutBytes = 0;
        if (Result == Z_MEM_ERROR)
            GlobalErrorFlag = SFARKLIB_ERR_MALLOC;
        else
            GlobalErrorFlag = SFARKLIB_ERR_CORRUPT;
    }

    return OutBytes;
}

// ==============================================================
// SFKL STRING
// ==============================================================

char * SfArkExtractor2::GetFileExt(char *FileName)
{
    // return pointer to (final) file extension including the dot e.g. '.txt'
    // returns NULL if extension is not found
    char *p;

    for(p = FileName + strlen(FileName); p > FileName; p--)
    {
        if (*p == '.')
        {
            return p;
        }
        else if (*p == '/' || *p == '\\')
        {
            return NULL;
        }
    }

    return NULL;
}

char * SfArkExtractor2::StrncpyEnsureNul(char *destination, const char *source, int num)
{
    if (num <= 0 || !destination)
    {
        return destination;
    }
    else if (!source)
    {
        destination[0] = '\0';
        return destination;
    }
    else
    {
        // same as strncpy, except it ensures the result is nul-terminated.
        char *ret = strncpy(destination, source, num - 1);
        destination[num - 1] = 0;
        return ret;
    }
}

void SfArkExtractor2::ChangeFileExt(const char *path, const char *newExtension, char *out, int outSize)
{
    if (strlen(path) >= MAX_FILEPATH)
    {
        // path is too long for our buffer
        out[0] = 0;
        return;
    }

    char pathWithoutExtension[MAX_FILEPATH] = "";
    StrncpyEnsureNul(pathWithoutExtension, path, sizeof(pathWithoutExtension));
    char *p = GetFileExt(pathWithoutExtension);
    if (p)
    {
        // add null-terminator where the '.' used to be.
        *p = 0;
    }

    // if there's no extension in the input, we'll simply append the new extension.
    int ret = snprintf(out, outSize, "%s%s", pathWithoutExtension, newExtension);
    if (ret < 0 || ret >= outSize)
    {
        out[0] = 0;
        return;
    }
}


// ==============================================================
// SFKL LPC
// ==============================================================

LPC_CORR SfArkExtractor2::schur(             // returns the minimum mean square error
                                             LPC_CORR const * ac,    //  in: [0...p] autocorrelation values
                                             int nc,                 //  in: number of ref. coeff
                                             LPC_PRAM        * ref)  // out: [0...p-1] reflection coefficients
{
    int i, m;

    LPC_CORR2 error, r;
    LPC_CORR2 Gen0[PMAX], Gen1[PMAX];

    if (ac[0] == 0)
    {
        for (i = 0; i < nc; i++) ref[i] = 0;
        return 0;
    }

    // Initialize the rows of the generator matrix G to ac[1...p].
    error = ac[0];

    for (i = 0; i < nc; i++)
        Gen0[i] = Gen1[i] = ac[i + 1];

    for (i = 0;;)
    {
        // Calculate this iteration's reflection coefficient and error.
        r = - (LPN) ((XPN)Gen1[0] / (XPN)error);
        error = (LPN) ( (XPN)error  +  ((XPN)Gen1[0] * (XPN)r) );

#if REFINT == 1
        ref[i] = (LPC_PRAM) ( (LPN)((XPN)r * ISCALE)) ;    // Scale-up to avoid loss of precision
#else
        ref[i] = r;
#endif

        if (++i >= nc)   break;

        // Update the generator matrix.
        for (m = 0; m < nc - i; m++)
        {
            //        Gen1[m] = (XPN) Gen1[m + 1]  +  ((XPN) r * (XPN) Gen0[m]);
            //        Gen0[m] = (XPN) Gen0[m]      +  ((XPN) r * (XPN) Gen1[m + 1]);
            Gen1[m] = (LPN) ( (XPN) Gen1[m + 1]  +  ((XPN) r * (XPN) Gen0[m]) );
            Gen0[m] = (LPN) ( (XPN) Gen0[m]      +  ((XPN) r * (XPN) Gen1[m + 1]) );
        }
    }

    //    blk++;
    return error;
}

// Compute the autocorrelation
void SfArkExtractor2::autocorrelation(int n, qint32 const *ibuf, int nc, LPC_CORR *ac)
{
    int i;

    // Copy integer data to float -- speeds things up slightly...
    LPC_FLOAT buf[ZWINMAX];
    for (i = 0; i < n ; i++)  buf[i] = (LPC_FLOAT) ibuf[i];

    while (nc--)
    {
        LPC_CORR c = 0;
        LPC_FLOAT const *lbuf = buf + nc;          // Points to current sample + nc

#define CI(I)  ( ((XPN)buf[I] * (XPN)lbuf[I]) )
        //#define CI(I)  	(buf[I] * lbuf[I])

        int istop = n - nc - 15;                   // Process 16 steps at a time for speed...
        for (i = 0;  i < istop; i += 16)
            c = (LPN) ((XPN) c 	+ CI(i+0) + CI(i+1) + CI(i+2) + CI(i+3) + CI(i+4) + CI(i+5) + CI(i+6) + CI(i+7)
                       + CI(i+8) + CI(i+9) + CI(i+10)+ CI(i+11)+ CI(i+12)+ CI(i+13)+ CI(i+14)+ CI(i+15) );

        istop = n - nc;                    // Process any remainder, one step at a time...
        for (; i < istop; i++)
            c = (LPN) ( (XPN) c  +  CI(i) );

        ac[nc] = c;
#undef CI
    }
}

// Add the autocorrelation for the end section of previous Window / start of current window
void SfArkExtractor2::AddAC (qint32 const *hbuf, qint32 const *ibuf, int nc, LPC_CORR *ac)
{
    int i;

    // Copy integer data to float -- speeds things up slightly...
    LPC_FLOAT buf[PMAX*2];

    int n = nc-1;                            // Number of samples is always one less than nc value

    for (i = 0; i < n ; i++)
    {
        buf[i] = (LPC_FLOAT) hbuf[i];
        buf[i + n] = (LPC_FLOAT) ibuf[i];
    }

    while (nc-- > 1)
    {
        LPC_CORR c = 0;
        LPC_FLOAT const *lbuf = buf + nc;      // Points to current sample + nc

        int istop;

#define CI(I)  ( ((XPN)buf[I] * (XPN)lbuf[I]) )
        //#define CI(I)  	(buf[I] * lbuf[I])

        istop = n - 15;                     // Process 16 steps at a time for speed...
        i = n - nc;

        for (;  i < istop; i += 16)
            c = (LPN) ( (XPN) c 	+ CI(i+0) + CI(i+1) + CI(i+2) + CI(i+3) + CI(i+4) + CI(i+5) + CI(i+6) + CI(i+7)
                        + CI(i+8) + CI(i+9) + CI(i+10)+ CI(i+11)+ CI(i+12)+ CI(i+13)+ CI(i+14)+ CI(i+15) );

        istop = n;                    // Process any remainder, one step at a time...
        for (; i < istop; i++)
            c = (LPN) ( (XPN) c  +  (XPN) CI(i) );

        ac[nc] = (LPN) ( (XPN) ac[nc] + (XPN) c );
#undef CI
    }
}

void SfArkExtractor2::LPCdecode(
        LPC_PRAM const *ref,    //  in: [0...p-1] reflection coefficients
        int            nc,      //  in: number of coefficients
        int            n,       //      # of samples
        qint32 const *in,     //      [0...n-1] residual input
        qint32       *out)    // out: [0...n-1] short-term signal

{
    qint32 s;
    int i;

    if (in == LAW_NULL)     // Initialise?
    {
        for (i = 0; i < PMAX; i++)  LPCdecode_u[i] = 0;
        return;
    }

    while (n--)
    {
        s = *in++;

#if REFINT == 1   //22.4 8gm
        // ------------------------------------------------------
#define LPC_AN1(I)                                      \
    s    -= SDIV(refp[I] * up[I], ISCALE_BITS);       \
    up[I+1] = up[I] + SDIV(refp[I] * s, ISCALE_BITS); \
    // ------------------------------------------------------

        LPC_PRAM const *refp = ref+nc-1;
        qint32 *up = LPCdecode_u+nc-1;

        while(refp >= ref)
        {
            LPC_AN1(0) ; LPC_AN1(-1); LPC_AN1(-2); LPC_AN1(-3);
            LPC_AN1(-4); LPC_AN1(-5); LPC_AN1(-6); LPC_AN1(-7);
            up -= 8; refp -= 8;
        }
#undef LPC_AN1

#else
        for (i = nc; i--;)
        {
#if REFINT == 1
#if 1   // Use SDIV
            long m;
            m = ( ref[i] * u[i] );
            s = ( s    - SDIV(m, ISCALE_BITS) );
            m = ( ref[i] * s );
            u[i+1] = ( u[i] + SDIV(m, ISCALE_BITS) );
#else
            s      = s    - ref[i] * u[i] / ISCALE;
            u[i+1] = u[i] + (ref[i] * s / ISCALE);
#endif
#else // Use floating point, convert to int...
            s      = s -    (int) (ref[i] * u[i]);
            u[i+1] = u[i] + (int) (ref[i] * s);
#endif
        }
#endif

        *out++ = LPCdecode_u[0] = s;
    }
}

// UnLPC2() is called by UnLPC() -- process one LPCWIN sized chunk
long SfArkExtractor2::UnLPC2(qint32 *OutBuf, qint32 *InBuf, short bufsize, short nc, quint32 *Flags)
{
    LPC_PRAM ref[PMAX];
    LPC_CORR ac[PMAX+1];

    int i, k;
    quint32 FlagMask = 1;
    int zwin = ZWINMIN;
    if (nc > zwin)  zwin = ZWINMAX;

    if (InBuf == LAW_NULL)  // Initialise?
    {
        UnLPC2_HistNum = 0;
        for (i = 0; i < nc; i++)  ref[i] = 0;
        for (i = 0; i < PMAX*2; i++)  UnLPC2_HistBuf[i] = 0;
        for (i = 0; i < PMAX+1; i++)
            for (int j = 0; j < HISTSIZE; j++)
                UnLPC2_AcHist[j][i] = 0;
        //      LPCdecode(LAW_NULL, nc, 0, LAW_NULL, LAW_NULL);
        LPCdecode(NULL, nc, 0, NULL, NULL);
        return 0;
    }

    //if ((bufsize % zwin) != 0)  return -3;

    for (i = 0; i < bufsize; i += zwin)
    {
#if HISTSIZE == 4
        for (k = 0; k < nc+1; k++)
            ac[k] = (XPN)UnLPC2_AcHist[0][k] + (XPN)UnLPC2_AcHist[1][k] + (XPN)UnLPC2_AcHist[2][k] + (XPN)UnLPC2_AcHist[3][k];
#else
        for (k = 0; k < nc+1; k++)
        {
            ac[k] = 0;
            for (int h = 0; h < HISTSIZE; h++)
                ac[k] = (XPN)ac[k] + (XPN)AcHist[h][k];
        }
#endif

        // Decode...

        if ((*Flags & FlagMask) == 0)
        {
            schur(ac, nc, ref);
            LPCdecode(ref, nc, zwin, InBuf+i, OutBuf+i);
        }
        else
        {
            LPCinit();                                                  // Re-initialise
            for (int j = 0; j < zwin; j++)  OutBuf[i+j] = InBuf[i+j];   // Copy input to output
        }
        FlagMask <<= 1;

        // Update the AutoCorrelation history data...
        AddAC(UnLPC2_HistBuf, OutBuf+i, nc+1, UnLPC2_AcHist[UnLPC2_HistNum]);           // Process overlap of prev. & current buffer
        if (++UnLPC2_HistNum == HISTSIZE)  UnLPC2_HistNum = 0;                   // Increment History counter, wrap-around if needed

        autocorrelation(zwin, OutBuf+i, nc+1, UnLPC2_AcHist[UnLPC2_HistNum]);    // Update AcHist with current buffer
        for (k = 0; k < nc; k++)  UnLPC2_HistBuf[k] = OutBuf[i+k];        // Store beginning of current buffer for next AddAC()
    }

    return 0;
}

void SfArkExtractor2::LPCinit()
{
    UnLPC2(LAW_NULL, LAW_NULL, 0, 0, (quint32 *)0);
}

long SfArkExtractor2::UnLPC(qint16 *OutBuf, qint16 *InBuf, short bufsize, short nc, quint32 *Flags)
{
    qint32  lInBuf[MAX_BUFSIZE], lOutBuf[MAX_BUFSIZE];
    qint32  *inp = lInBuf, *bufend = inp + bufsize, *outp = lOutBuf;

    int i;

    // Copy 16 bit data to 32 bits...
    while (inp < bufend)	*inp++ = *InBuf++;

    inp = lInBuf;

    short WinSize = LPCWIN;
    if (WinSize > bufsize)  WinSize = bufsize;

    int WordsLeft = bufsize;
    do {
        if (WordsLeft < ZWINMIN)
        {
            for (i = 0; i < WordsLeft; i++)
                outp[i] = inp[i];
        }
        else
        {
            long LPCout = UnLPC2(outp, inp, WinSize, nc, Flags);
            if (LPCout < 0)  return LPCout;
        }

        inp += WinSize;
        outp += WinSize;
        WordsLeft -= WinSize;

    } while (inp < bufend);

    // Copy 32 bit data to 16 bits...
    outp = lOutBuf; bufend = outp + bufsize;
    while (outp < bufend)
        *OutBuf++ = (qint16) *outp++;

    return 0;
}

// ==============================================================
// SFKL DIFF
// ==============================================================

void SfArkExtractor2::UnBufDif2(qint16 *OutBuf, const qint16 *InBuf, quint16 bufsize, qint16 *prev)
{
    // NB: This code is optimised for speed - do not change order of operations or data types!!!
    // -----------------------------------------------------------------------------------------
    const qint16 *bufend1 = OutBuf + bufsize;

    //for (int i = 0; i < bufsize; i++)  OutBuf[i] = InBuf[i];
    memcpy(OutBuf, InBuf, bufsize*sizeof(qint16));

    // Process start of buffer...
    *OutBuf++ += *prev;

    // Process rest of buffer...
#define STEPSIZE 8
#define DO_ONE(N)     OutBuf[N] += OutBuf[N-1];
    // ----------------------------------------------------

    const qint16 *bufend2 = bufend1 - STEPSIZE;
    while (OutBuf < bufend2)
    {
        DO_ONE(0); DO_ONE(1); DO_ONE(2); DO_ONE(3);
        DO_ONE(4); DO_ONE(5); DO_ONE(6); DO_ONE(7);
        OutBuf += STEPSIZE;
    }

    // Do final few...
    while (OutBuf < bufend1)  {
        *OutBuf += OutBuf[-1];
        OutBuf++;
    };

    *prev = OutBuf[-1];

    return;

#undef DO_ONE
#undef STEPSIZE
}

void SfArkExtractor2::old_UnBufDif3(qint16 *OutBuf, const qint16 *InBuf, quint16 bufsize, qint16 *prev)
{
    // Decoding is done from the END of the buffer, backwards...

    const qint16 *bufstart = InBuf;

    InBuf += bufsize-1;
    OutBuf+= bufsize-1;

    // Process end of buffer...
    *OutBuf-- = *InBuf--;

    //-----------------------------------------
#define DO_ONE(N)  OutBuf[N] = InBuf[N] + NSDIV(InBuf[N-1] + OutBuf[N+1], 1);
    // Process rest of buffer...
    while (InBuf > bufstart) { DO_ONE(0); InBuf--; OutBuf--; }

    // Final word
    *OutBuf = *InBuf + NSDIV(OutBuf[1], 1);

    *prev = OutBuf[bufsize-1];
    return;
#undef DO_ONE
}

void SfArkExtractor2::UnBufDif3(qint16 *OutBuf, const qint16 *InBuf, quint16 bufsize, qint16 *prev)
{
    // Decoding is done from the END of the buffer, backwards...

#define STEPSIZE 8
    const qint16 *bufstart = InBuf, *bufstart1 = InBuf+STEPSIZE;

    InBuf += bufsize-1;
    OutBuf+= bufsize-1;

    // Process end of buffer...
    *OutBuf-- = *InBuf--;

    // Process rest of buffer...
    //-----------------------------------------
#define DO_ONE(N)  OutBuf[N] = InBuf[N] + NSDIV(InBuf[N-1] + OutBuf[N+1], 1);
    //-----------------------------------------

    // Process most of buffer...
    while (InBuf > bufstart1)
    {
        DO_ONE(0); DO_ONE(-1);DO_ONE(-2);DO_ONE(-3);
        DO_ONE(-4);DO_ONE(-5);DO_ONE(-6);DO_ONE(-7);
        InBuf-=STEPSIZE; OutBuf-=STEPSIZE;
    }

    // Process rest of buffer...
    while (InBuf > bufstart) { DO_ONE(0); InBuf--; OutBuf--; }

    // Final word
    *OutBuf = *InBuf + NSDIV(OutBuf[1], 1);

    *prev = OutBuf[bufsize-1];
    return;
#undef DO_ONE
#undef STEPSIZE
}

void SfArkExtractor2::UnBufDif4(qint16 *OutBuf, const qint16 *InBuf, quint16 bufsize, qint16 *prev)
{
    qint16 avg = *prev;
    const qint16 *bufend1 = InBuf + bufsize;

    // Process rest of buffer...
    //-----------------------------------------
#define STEPSIZE    8
    const qint16 *bufend2 = bufend1 - STEPSIZE;

#define DO_ONE(N)                     \
    OutBuf[N] = InBuf[N] + avg;         \
    avg += SDIV(InBuf[N], 1);          \

    //-----------------------------------------

    for (; InBuf < bufend2; InBuf += STEPSIZE, OutBuf += STEPSIZE)
    {
        DO_ONE(0); DO_ONE(1); DO_ONE(2); DO_ONE(3);
        DO_ONE(4); DO_ONE(5); DO_ONE(6); DO_ONE(7);
    }
    for (; InBuf < bufend1; ++InBuf, ++OutBuf)
    {
        DO_ONE(0);
    }

    *prev = avg;
    return;

#undef DO_ONE
#undef STEPSIZE
}

long SfArkExtractor2::BufSum(const qint16 *buf, quint16 bufsize)
{
    // --- Optimised! ---
    long Total = 0;

    const qint16 *endp1 = buf + bufsize - 7;
    const qint16 *endp2 = buf + bufsize;

    for (; buf < endp1; buf += 8)
    {
        Total += QUICKABS2(buf[0]) + QUICKABS2(buf[1]) + QUICKABS2(buf[2]) + QUICKABS2(buf[3])
                + QUICKABS2(buf[4]) + QUICKABS2(buf[5]) + QUICKABS2(buf[6]) + QUICKABS2(buf[7]);
    }

    for ( ; buf < endp2; ++buf)
        Total += QUICKABS2(*buf);

    return Total;
}

void SfArkExtractor2::UnBufShift1(qint16 *InBuf, quint16 bufsize, short Shift)
{
#define STEPSIZE 8

    // Ok, now we have a shift value - perform the shift...
    qint16 *bps = InBuf, *bufend1s = bps + bufsize;
    qint16 *bufend2s = bufend1s - STEPSIZE;

#define DO_ONE(I)     bps[I] <<= Shift
    while (bps < bufend2s)
    {
        DO_ONE(0); DO_ONE(1); DO_ONE(2); DO_ONE(3);
        DO_ONE(4); DO_ONE(5); DO_ONE(6); DO_ONE(7);
        bps += STEPSIZE;
    }

    // Do final few...
    while (bps < bufend1s)  { DO_ONE(0); bps++; };

    return;

#undef DO_ONE
#undef STEPSIZE
}

void SfArkExtractor2::UnBufShift(qint16 *InBuf, quint16 bufsize, short *shift)
{
    for(qint16 *endp = InBuf + bufsize; InBuf < endp; InBuf+= SHIFTWIN)
    {
        short s = *shift++;
        if (s != 0)  UnBufShift1(InBuf, SHIFTWIN, s);
    }
}
