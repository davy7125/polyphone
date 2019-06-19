//sfArkLib Coding
//Copyright 1998-2000 Andy Inman
//Contact via: http://netgenius.co.uk or http://melodymachine.com

// This file is part of sfArkLib.
//
// sfArkLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// sfArkLib is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sfArkLib.  If not, see <http://www.gnu.org/licenses/>.


// V2.11	Initial release (with SDL 0.07)
// V2.12	Bug fix in UnMemcomp (OutBytes was uninitialised causing crash for some files)
//				Used for SDL 0.08.
// V2.13	Bug fix, previously file written was corrupt due to wrong buffer pointers
// V2.14	Now using UpdateProgress function (supplied by application)
// V2.15	Now using GetLicenseAgreement & DisplayNotes functions (app-supplied)

// 2.20		Version bumped to 2.20 - First version for MacOS support
//		Major re-write to ReadHeader function, supports any size of self-extraction code in sfArk.exe files.
// 2.21	25-09-02 Fixed byte order on output samples (for MacOS).

// 2.22 27-09-03 Fixed bug in call to UpdateProgress which reset to zero with large files when bytes
//		written greater than (1<<31)/100 (approx 20MB) -- was due to 32-bit integer overflow
//		Added more floating-point precision conversions in LPC to tighten-up on Mac compatibility issues
//		Avoid potential bug in WriteOutputFile, now test BytesWritten != BytesToWrite instead of <= 0
//		Fixed rare bug when POST_AUDIO is more than ZBUF_SIZE (256kb) uncompressed (affected RealFont_2_1.sfArk)

// 2.23 02-10-03 LPC Speeded up, now double the speed of earlier versions (on Intel at least)

// CLIB headers...
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdint.h>

// sfArk specific headers...
#define		SFARKLIB_GLOBAL 
#include	"wcc.h"
#include	"zlib.h"

// Set to 1 for special debug mode (needs enabled version of compressor)
#define		DB_BLOCKCHECK 0

// FileSection equates...
#define		RIFF_HEADER	0
#define		RIFF_INFO	1
#define		INFO_LIST	2
#define		SDTA_LIST	3
#define		AUDIO_START	4
#define		AUDIO		5
#define		PRE_AUDIO	6
#define		POST_AUDIO	7
#define		NON_AUDIO	8
#define		FINISHED	9

// Misc equates...
#define		OPTWINSIZE	32			// Default window size used by CrunchWin()
#define		ZBUF_SIZE     (256 * 1024)		// Size of buffer used for MemComp (do not change!)
#define		NSHIFTS	(MAX_BUFSIZE / SHIFTWIN)	// Max number of shift values per block (MaxBuf/SHIFTWIN = 4096/64)
#define		MAX_DIFF_LOOPS	20			// Max number of BufDif loops

// --- Version 2 File Header Structure ---

#define	HDR_NAME_LEN	5
#define	HDR_VERS_LEN	5

typedef struct
        /*
         NB: For compatibilty with sfArk V1, we must store "sfArk" at offset 27 (base 1) in
         5 characters and the compression method as one byte immediately afterwards at offset 32.
         This will allow sfArk V1 to recoginse this as a .sfArk file (though not decompress it!)
        */
{
    uint32_t	Flags;				// 0-3		Bits 0 & 1 used to indicate presence of Notes and License files
    uint32_t	OriginalSize;			// 4-7		Uncompressed file size
    uint32_t	CompressedSize;			// 8-11		Compressed file size (including header)
    uint32_t	FileCheck;			// 12-15	File Checksum
    uint32_t	HdrCheck;			// 16-19	Header Checksum
    uint8_t		ProgVersionNeeded;		// 20		SfArk version needed to unpack this file (20 = Version 2.0x, etc.)
    char		ProgVersion[HDR_NAME_LEN];	// 21-25	Version string (nn.nn) that created this file (NOT terminated)
    char		ProgName[HDR_VERS_LEN];		// 26-30	Signature "sfArk" (not terminated)
    uint8_t		CompMethod;			// 31		Compression Method
    uint16_t	FileType;			// 32-33	Currently always 0 (for SF2)
    uint32_t	AudioStart;			// 34-37	Position in original file of start of audio data
    uint32_t	PostAudioStart;			// 38-41	Position in original file of start any data after audio data (e.g. SF2 parameters)
    char		FileName[MAX_FILENAME];		// 42-297	Original filename, no path (stored variable length, null terminated)
} V2_FILEHEADER;

// Some extras re. Header structure...
#define V2_FILEHEADER_SIZE	298		// *Actual* size of header data, may be less than sizeof(V2_FILEHEADER)
#define HEADER_HDRCHECK_POS	16		// Byte position of HeaderCheck field in header
#define HEADER_SIG_POS		26		// Byte position of "sfArk" signautre in header
#define HEADER_MAX_OFFSET	(128*1024) 	// Maximum search extent: maximum expected size of "self-extractor" code.

// FileHeader Flags...
#define	FLAGS_Notes		(1 << 0)	// Notes file included
#define	FLAGS_License		(1 << 1)	// License file included

const char	LicenseExt[] = ".license.txt";		// File extension for license file
const char	NotesExt[] = ".txt";			// File extension for notes file

static	BYTE *Zbuf1 = NULL, *Zbuf2 = nullptr;

// Data per block, passed to ProcessNextBlock()
typedef	struct
{
    V2_FILEHEADER	FileHeader;			// FileHeader structre
    int				FileSection;	// Track current "file section"

    int	ReadSize;	// Number of words to read per block
    int	MaxLoops;	// Max loops for reduction with BufDiff2/3
    int	MaxBD4Loops;	// Max loops for reduction with BufDiff4
    int	nc;		// Number of LPC parameters
    int	WinSize;	// Window size for CrunchWin

    AWORD	*SrcBuf;	// Address of source buffer
    AWORD	*DstBuf;	// Address of destination buffer

    ULONG	TotBytesWritten;	// Total bytes written in file
    ULONG	FileCheck;		// File checksum (accumulated)
    AWORD	PrevIn[MAX_DIFF_LOOPS];	// Previous values (per loop)

    USHORT	PrevEncodeCount;	// Previous number of loops used
    USHORT	BD4PrevEncodeCount;	// Previous number of loops used for BD4
    short	PrevShift;		// Previous Shift value
    short	PrevUsedShift;		// Previously used (non zero) Shift value
} BLOCK_DATA;

// Messages...
const char	CorruptedMsg[]	= "- This file appears to be corrupted.";
const char	UpgradeMsg[]		= "Please see Help/About for information on how to obtain an update.";

// ==============================================================
USHORT	GetsfArkLibVersion(void)
{
    return (ProgVersionMaj * 10) + ProgVersionMin/10;
}
// ==============================================================

char *ChangeFileExt(char *OutFileName, const char *NewExt, int OutFileNameSize)
{
    int n = strlen(OutFileName);
    char *p;

    for (p = OutFileName+n; *p != '.'; p--)
    {
        if (*p == '\\'  ||  p <= OutFileName)	// No extension found?
        {
            p = OutFileName + n;
            break;
        }
    }

    n = p - OutFileName;	// Length of filename without extension
    strncpy(p, NewExt, OutFileNameSize-1 - n);
    return OutFileName;
}
// ==============================================================

// Read the File Header....
int ReadHeader(V2_FILEHEADER *FileHeader, BYTE *fbuf, int bufsize)
{
    int HeaderLen = 0, HdrOffset;
    char	CreatedByProg[HDR_NAME_LEN +1],  CreatedByVersion[HDR_VERS_LEN +1];
    ULONG	CalcHdrCheck = 0;
    BYTE *HdrBuf, *bpFileHeader = (BYTE *) FileHeader;

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

        BYTE *sigpos = fbuf + HdrOffset + HEADER_SIG_POS;
        if (*sigpos != 's'  ||  memcmp(sigpos, "sfArk", 5) != 0)  continue;

        SigFound = true;					// Set a flag to remember that we at least got this far
        HdrBuf = fbuf + HdrOffset;

        if (V2_FILEHEADER_SIZE != sizeof(V2_FILEHEADER))	// Compare structure size to real size
        {
            // The compiler has messed with structure (alignment), so copy the data to the structure byte by byte...
            BYTE *bptr = HdrBuf;		// Point to start
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
#ifdef	__BIG_ENDIAN__
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

// ==============================================================
bool InvalidEncodeCount(int EncodeCount, int MaxLoops)
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

// ==============================================================
int DecompressTurbo(BLOCK_DATA *Blk, USHORT NumWords)
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
        AWORD *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
    }
    return SFARKLIB_SUCCESS;
}

// ==============================================================
bool CheckShift(short *ShiftVal, USHORT NumWords, short *PrevShift, short *PrevUsedShift)
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

// ==============================================================

int DecompressFast(BLOCK_DATA *Blk, USHORT NumWords)
{
    int	i, EncodeCount;
    short	ShiftVal[NSHIFTS];						// Shift values (one per SHIFTWIN words)
    USHORT	Method[MAX_DIFF_LOOPS];				// Block processing methods used per iteration

#if	DB_BLOCKCHECK											// If debug mode block check enabled
    ULONG BlockCheck = BioRead(16);				// Read block check bits
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
    ULONG LPCflags;
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
        AWORD *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
    }
    
    if (UsingBD4)
    {
        for (i = EncodeCount-1; i >= 0; i--)
        {
            UnBufDif4(Blk->DstBuf, Blk->SrcBuf, NumWords, &(Blk->PrevIn[i]));
            AWORD *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
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
            AWORD *SwapBuf = Blk->SrcBuf;  Blk->SrcBuf = Blk->DstBuf; Blk->DstBuf = SwapBuf;
        }
    }

    if (UsingShift)  UnBufShift(Blk->SrcBuf, NumWords, ShiftVal);

#if	DB_BLOCKCHECK											// If debug mode block check enabled
    ULONG CalcBlockCheck = adler32(0, (const BYTE *) Blk->SrcBuf, 2*NumWords) & 0xffff;
    //ULONG CalcBlockCheck = Blk->FileCheck & 0xffff;
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
// ==============================================================
int ProcessNextBlock(BLOCK_DATA *Blk)
{
    //int	TotBytesRead = 0;						// Total bytes read in file
    int	NumWords;							//

    uint32_t	n, m;							// NB: Must be 32-bit integer

#define	AWBYTES	(sizeof(AWORD))
    BYTE *zSrcBuf = (BYTE *) Blk->SrcBuf;
    BYTE *zDstBuf = (BYTE *) Blk->DstBuf;

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

        //printf("B4 WriteOutputFile: %ld\n", adler32(0, (const BYTE *) Blk->SrcBuf, n) & 0xffff);
#ifdef __BIG_ENDIAN__
#define	WFIX(I)		s = bp[I+0]; bp[I+0] = bp[I+1]; bp[I+1] = s;
        BYTE *bp = (BYTE *) Blk->SrcBuf; BYTE *ep = bp + n;
        do {
            BYTE s;
            WFIX(0); WFIX(2); WFIX(4); WFIX(6);
            WFIX(8); WFIX(10); WFIX(12); WFIX(14);
            bp += 16;
        } while (bp < ep);
#undef WFIX
#endif

        WriteOutputFile((const BYTE *)Blk->SrcBuf, n);										// Write to output file
        Blk->TotBytesWritten += n;																				// Accumulate total bytes written
        break;
    }

    case PRE_AUDIO: case POST_AUDIO: case NON_AUDIO:
    {
        BioReadBuf((BYTE *) &n, sizeof(n));
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
        ULONG BlockCheck = BioRead(16);				// Read block check bits
        FixEndian(&BlockCheck, sizeof(Blockcheck));
        ULONG CalcBlockCheck = adler32(0, zDstBuf, m) & 0xFFFF;
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

// ==============================================================
int	EndProcess(int ErrorNum)
{
    CloseInputFile();
    CloseOutputFile();
    return ErrorNum;
}

// ==============================================================
// Extract License & Notes files
// These are stored as 4-bytes length, followed by length-bytes of compressed data
bool	ExtractTextFile(BLOCK_DATA *Blk, ULONG FileType)
{
    ULONG n, m;
    BYTE *zSrcBuf = (BYTE *) Blk->SrcBuf;
    BYTE *zDstBuf = (BYTE *) Blk->DstBuf;

    const char *FileExt;
    if (FileType == FLAGS_License)
        FileExt = LicenseExt;
    else if (FileType == FLAGS_Notes)
        FileExt = NotesExt;
    else
        return false;

    // NB:Can't use BioReadBuf... aligment problems? Yet it works for ProcessNextBlock() !!??
    // Ok, can use ReadInputFile here cause everythjing is whole no. of bytes...

    //BioReadBuf((BYTE *)&n, sizeof(n));					// Read length of block from file
    ReadInputFile((BYTE *)&n, sizeof(n));					// Read length of block from file
    FixEndian(&n, sizeof(n));										// Fix endian

    if (n <= 0  ||  n > ZBUF_SIZE)								// Check for valid block length
    {
        sprintf(MsgTxt, "ERROR - Invalid length for %s file (apparently %ld bytes) %s", FileExt, n, CorruptedMsg);
        msg(MsgTxt, MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_CORRUPT;
        return false;
    }

    //BioReadBuf(zSrcBuf, n);																					// Read the block
    ReadInputFile((BYTE *)zSrcBuf, n);																// Read the block
    m = UnMemcomp(zSrcBuf, n, zDstBuf, ZBUF_SIZE);										// Uncompress
    Blk->FileCheck = adler32(Blk->FileCheck, zDstBuf, m);	   					// Accumulate checksum
    if (GlobalErrorFlag  ||  m > ZBUF_SIZE)														// Uncompressed ok & size is valid?
        return false;

    // Write file - Use original file name plus specified extension for OutFileName...
    char OutFileName[MAX_FILENAME];
    strncpy(OutFileName, Blk->FileHeader.FileName, sizeof(OutFileName));	// copy output filename
    ChangeFileExt(OutFileName, FileExt, sizeof(OutFileName));
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

// ==============================================================

int Decode(const char *InFileName, const char *ReqOutFileName)
{
    char	OutFileName[MAX_FILEPATH];	// File name for current output file
    int	NumLoops;			// Number of loops before screen update etc.

    BLOCK_DATA	Blk;
    memset(&Blk, 0, sizeof(Blk));
    V2_FILEHEADER	*FileHeader = &Blk.FileHeader;

    // NB: We keep 2 buffers with pointers in Blk->SrcBuf and Blk->DstBuf
    // Generally we process from SrcBuf to DstBuf then swap the pointers,
    // so that current data is always at SrcBuf

    // NB: On MacOS/GNU C, the following allocation of Zbuf1 and Zbuf2 causes "segmentation fault"
    // BYTE	Zbuf1[ZBUF_SIZE];					// Buffer1
    // BYTE	Zbuf2[ZBUF_SIZE];					// Buffer2
    // so instead...

#if 0
    static BYTE	Zbuf1[ZBUF_SIZE];				// Buffer1
    static BYTE	Zbuf2[ZBUF_SIZE];				// Buffer2

#else
    if (Zbuf1 == NULL) Zbuf1 = (BYTE *) calloc(ZBUF_SIZE, sizeof(BYTE));
    if (Zbuf2 == NULL) Zbuf2 = (BYTE *) calloc(ZBUF_SIZE, sizeof(BYTE));

    if (Zbuf1 == NULL  ||  Zbuf2 == NULL)
        return EndProcess(SFARKLIB_ERR_MALLOC);
#endif

    Blk.SrcBuf = (AWORD *) Zbuf1;					// Point to Zbuf1
    Blk.DstBuf = (AWORD *) Zbuf2;					// Point to Zbuf2

    // Initialisation...
    BioDecompInit();						// Initialise bit i/o
    LPCinit();							// Init LPC
    GlobalErrorFlag = SFARKLIB_SUCCESS;

    // Open input (.sfArk) file and read the header...
    OpenInputFile(InFileName);
    if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);				// Something went wrong?
    ReadHeader(FileHeader, Zbuf1, ZBUF_SIZE);

    if (GlobalErrorFlag)  return EndProcess(GlobalErrorFlag);				// Something went wrong?

    if (ReqOutFileName == NULL)							// If no output filename requested
        ReqOutFileName = FileHeader->FileName;

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

    // Use original file extension for OutFileName...
    strncpy(OutFileName, ReqOutFileName, sizeof(OutFileName));			// Copy output filename
    OpenOutputFile(OutFileName);																		// Create the main output file...

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

    ULONG ProgressUpdateInterval = Blk.FileHeader.OriginalSize / 100; // Calculate progress update
    ULONG NextProgressUpdate = ProgressUpdateInterval;
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

    sprintf(MsgTxt, "Created %s (%ld kb) successfully.", ReqOutFileName, Blk.TotBytesWritten/1024);
    msg(MsgTxt, 0);
    
    return EndProcess(GlobalErrorFlag);
}

// ==============================================================

// Adjust integer held at *num for this machine's endian system
void FixEndian(void *num, int nsize)
{
#ifdef	__BIG_ENDIAN__
    int i;
    BYTE bb[4];
    for (i = 0; i < nsize; i++)  bb[i] = ((BYTE *) num)[i];
    for (i = 0; i < nsize; i++)  ((BYTE *) num)[i] = bb[nsize-1-i];
#else 
#ifndef __LITTLE_ENDIAN__
#error ENDIAN system undefined
#endif
#endif
}

// ==============================================================
