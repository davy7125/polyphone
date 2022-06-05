/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SFARKEXTRACTOR2_H
#define SFARKEXTRACTOR2_H

#include "abstractextractor.h"

// Set to 1 for special debug mode (needs enabled version of compressor)
#define		DB_BLOCKCHECK 0
#define		SFARKLIB_GLOBAL
#define USE_MANUAL_ENDIANNESS
#define MANUAL_LITTLE_ENDIAN

#if defined(USE_MANUAL_ENDIANNESS) && !defined(LITTLE_ENDIAN) && \
    (defined(MANUAL_LITTLE_ENDIAN) || defined(MANUAL_BIG_ENDIAN))
#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN	4321
#ifdef MANUAL_LITTLE_ENDIAN
#define BYTE_ORDER	LITTLE_ENDIAN
#else
#define BYTE_ORDER	BIG_ENDIAN
#endif
#elif defined(USE_SYS_ENDIAN_H)
/* BSD */
#include <sys/endian.h>
#elif defined(USE_ENDIAN_H)
/* GNU */
#include <endian.h>
#endif

#if !defined(LITTLE_ENDIAN) || !defined(BIG_ENDIAN) || !defined(BYTE_ORDER)
#error ENDIAN system undefined
#endif

#if (BYTE_ORDER != LITTLE_ENDIAN) && (BYTE_ORDER != BIG_ENDIAN)
/* PDP endian or something */
#error only big or little endian systems are supported
#endif

// ------------------------------------------------------------------------------------
// The following are now defined in sfarklib.h ... redefined here for compatibility...
#define	MAX_FILENAME	SFARKLIB_MAX_FILENAME
#define	MAX_FILEPATH	SFARKLIB_MAX_FILEPATH
#define	MAX_MSGTEXT	SFARKLIB_MAX_MSGTEXT
#define	MSG_SameLine	SFARKLIB_MSG_SameLine
#define	MSG_AppendLine	SFARKLIB_MSG_AppendLine
#define	MSG_PopUp	SFARKLIB_MSG_PopUp

#define	msg(a, b)		sfkl_msg(a, b)			// Message display function
#define UpdateProgress(a)	sfkl_UpdateProgress(a)		// Progress indication
#define GetLicenseAgreement(a, b) sfkl_GetLicenseAgreement(a, b)// Display/confirm license
#define DisplayNotes(a, b)	sfkl_DisplayNotes(a, b)		// Display notes text file
#define GetsfArkLibVersion(a)	sfkl_GetVersion(a)
#define Decode(a, b)		sfkl_Decode(a, b)
// ------------------------------------------------------------------------------------

// ----- typdefs -----
typedef unsigned short		USHORT;
typedef unsigned char		BYTE;
typedef unsigned int		ULONG;

typedef short			AWORD;		// Audio word, i.e. 16-bit audio
typedef unsigned short		UAWORD;
typedef int			LAWORD;		// "long" audio word, i.e. 32 bits
typedef unsigned int		ULAWORD;

// Types used by Bit I/O (BIO) routines...
typedef USHORT					BIOWORD;
typedef ULONG					BIOWORD2;


// ----- Constants -----
#ifndef true
#define true    1
#define false	0
#endif

#define SHIFTWIN 64       // window size used for shift operations

// Just for readability...
#define SIGNBIT  1
#define FLAGBIT  1

// Some max sizes...
#define MAX_BUFSIZE		4096		// Largest buffer size
#define MAX_AWORD   ( (1 << (AWORD_BITS-1)) -1 )    // 32767 for 16-Bit AWORD
#define MAX_UAWORD  ( (1 << AWORD_BITS)-1 )         // 65535 for 16-Bit AWORD

// Typed NULLs ...
#define FL_NULL					((float *) 0)
#define AW_NULL					((AWORD *) 0)
#define LAW_NULL				((LAWORD *) 0)

// Compression methods...
#define COMPRESSION_v1            0       // sfArk V1 compression
#define COMPRESSION_v1Fast        1       // sfArk V1.1 "Fast" compression
#define COMPRESSION_v1Full        2       // sfArk V1.1 "Full" compression

#define	COMPRESSION_v2						3				// Lowest value for V2 compression

#define COMPRESSION_v2NonAudio    3       // sfArk V2 "NonAudio" (uses ZLib MemComp)
#define COMPRESSION_v2Turbo       4       // sfArk V2 "Turbo" compression (BD2 only, maxloops 3)
#define COMPRESSION_v2Fast        5       // sfArk V2 "Fast" compression (BD2,3,4, maxloops 15)
#define COMPRESSION_v2Standard    6       // sfArk V2 "Standard" compression (BD2 + LPC, 8 LPC parameters)
#define COMPRESSION_v2Max         7       // sfArk V2 "Max" compression (BD2, 3, 4 + LPC with 128(?) LPC parameters)
#define COMPRESSION_v2Custom      8       // sfArk V2 "Custom" compression (user choices)
#define COMPRESSION_NotV1         254     // Returned by ReadV1Header() for non-V1 files
#define COMPRESSION_Error         255     // Returned by ReadV1Header() for unrecognised compression type

// ----------------------

// ------ Macros -------
#define	RETURN_ON_ERROR()	if (GlobalErrorFlag != SFARKLIB_SUCCESS)  return(GlobalErrorFlag)
#define	JUMP_ON_ERROR(label)	if (GlobalErrorFlag != SFARKLIB_SUCCESS)  goto label

#if BYTE_ORDER == BIG_ENDIAN
    #define FIX_ENDIAN16(w)	((((BYTE) w) << 8) | (((USHORT)w) >> 8))
#else
    #define FIX_ENDIAN16(w)	(w)
#endif

#define BIT_SIZEOF(x)   (sizeof(x) * 8)
#define AWORD_BITS      BIT_SIZEOF(AWORD)	// Number of bits in Audio Word

#define ABS(x)          ( ((x) > 0) ? (x) : -(x))

#define MIN(x, y)       ( ( (x) < (y) )?  (x) : (y) )
#define MAX(x, y)       ( ( (x) > (y) )?  (x) : (y) )
#define NZSIGN(w)       ((w) != 0)          // Used to add 1 for SignBit if value is non-zero
#define LOWBITS(w, n)   ( (w) & ((1 << (n))-1) )


#if 0   // Safer methods (use in case of compile compatibility problems, etc)...
#define SIGN(x)      ( ((x) >= 0) ? 0 : 1)
#define QUICKABS(x)  ABS(x)
#define QUICKABS2(x) ( ABS(x) - SIGN(x) )

#else
// Quick "absolute value" for (much faster than using conditional) ...
// if x is positive QUICKABS2(x) = abs(x) ... but...
// if x is negative QUICKABS2(x) = abs(x)-1 -- wrong! but suits many of our purposes here!
#define SIGN(x)      ( (((x)) >> (BIT_SIZEOF(x)-1)) & 1 )				// Fast!  Returns 0 for positive, 1 for negative
#define QUICKABS2(x) ( (x) ^ ( (x) >> (BIT_SIZEOF(x)-1) ) )			// Fast version (inaccurate for negative value)
#define QUICKABS(x)  ( QUICKABS2(x) + SIGN(x) )									// Correct ABS value

#endif

// Fast division using Shift for Unsigned numbers (or where innacuracy isn't a problem)
// Innacurate on negative odd numbers, e.g. NSDIV(-100, 1) gives -51 instead of -50.  (Use SDIV instead).
#define NSDIV(x, y)  (  (x) >> (y)  )

// Fast division using Shift for Signed numbers
#define SDIV(x, y)      ( ((x) >= 0)? (x) >> (y) : -((-(x)) >> (y)) )


#define REFINT		1      	// Integers for Reflection Coefficients (faster)
#define LPCWIN      4096
#define PMAX        128              // Max allowed nc
#define ZWINMIN     128              // 128 or 256 best.  Smaller - slightly slower
#define ZWINMAX     ZWINMIN

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

// Some extras re. Header structure...
#define V2_FILEHEADER_SIZE	298		// *Actual* size of header data, may be less than sizeof(V2_FILEHEADER)
#define HEADER_HDRCHECK_POS	16		// Byte position of HeaderCheck field in header
#define HEADER_SIG_POS		26		// Byte position of "sfArk" signautre in header
#define HEADER_MAX_OFFSET	(128*1024) 	// Maximum search extent: maximum expected size of "self-extractor" code.

// FileHeader Flags...
#define	FLAGS_Notes		(1 << 0)	// Notes file included
#define	FLAGS_License		(1 << 1)	// License file included

#define BIO_WBITS     ((signed) (8 * sizeof(BIOWORD)))
#define BIOBUFSIZE    (16 * 1024)	// Disk-read buffer size, bigger may increased speed
#define	HDR_NAME_LEN	5
#define	HDR_VERS_LEN	5

#define	SFARKLIB_MAX_FILENAME	256				// Longest filename handled (or directory name)
#define	SFARKLIB_MAX_FILEPATH	1024				// Longest full path handled
#define	SFARKLIB_MAX_MSGTEXT	(SFARKLIB_MAX_FILEPATH + 1024)	// Longest message we might produce with sfkl_msg()

// Flags used with msg() function...
#define	SFARKLIB_MSG_SameLine	(1 << 0)	// Overwrite previous message
#define	SFARKLIB_MSG_AppendLine	(1 << 1)	// Append to previous message
#define	SFARKLIB_MSG_PopUp	(1 << 2)	// Should "pop up" a dialogue (usually an error message)

// Error codes...
#define SFARKLIB_SUCCESS		 0	// No error
#define	SFARKLIB_ERR_INIT		-1	// Failed to initialise
#define	SFARKLIB_ERR_MALLOC		-2	// Failed to allocate memory
#define	SFARKLIB_ERR_SIGNATURE		-3	// header does not contain "sfArk" signature
#define	SFARKLIB_ERR_HEADERCHECK	-4	// sfArk file has a corrupt header
#define	SFARKLIB_ERR_INCOMPATIBLE	-5	// sfArk file is incompatible (i.e. not sfArk V2.x)
#define	SFARKLIB_ERR_UNSUPPORTED	-6	// sfArk file uses unsupported feature
#define	SFARKLIB_ERR_CORRUPT		-7	// got invalid compressed data (file is corrupted)
#define	SFARKLIB_ERR_FILECHECK		-8	// file checksum failed (file is corrupted)
#define	SFARKLIB_ERR_FILEIO		-9	// File i/o error
#define	SFARKLIB_ERR_LICENSE		-10 	// License included not agreed by user
#define	SFARKLIB_ERR_OTHER		-11 	// Other error (currently unused)

#if (LPCWIN / ZWINMIN) > 32          // Check use of 32bit *Flags
  #error Invalid LPCWIN / ZWINMIN
#endif

// The following parameters determine the history sized used for LPC analysis (little impact on speed)
// The history is the amount of prev
#define HISTSIZE    (4*128/ZWINMIN)		// Multiple of number of ZWINs to use as history size (seems best value)

typedef LAWORD  LPC_WORD;               // LPC_WORD must have (slightly) greater range than AWORD

// There are compatibility issues in floating point calculations between Intel and Mac versions.  Not sure if this
// is a compiler issue or a processor issue -- in short on Windows it seems that calculations performed on float
// datatype get done in double precision.  So, we need to force the correct precision when used on Mac.
// Better longterm solution is to force single precision for all calculations (for the compressor also) but
// that would require a version update.
typedef float  LPC_FLOAT;
typedef double	XPN;			// eXtra PrecisioN during calculations
typedef float	LPN;			// Lower PrecisioN to store results of calculations

#define ISCALE_BITS     14                  	// Fixed scale seems to work fine in practice
#define ISCALE          (1 << ISCALE_BITS)

#if REFINT == 1
  typedef long LPC_PRAM;
#else
  typedef LPC_FLOAT   LPC_PRAM;
#endif

typedef LPC_FLOAT  LPC_CORR;
typedef LPC_FLOAT  LPC_CORR2;    // Holds LPC_CORR*LPC_CORR

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

class SfArkExtractor2: public AbstractExtractor
{
public:
    SfArkExtractor2(const char * fileName);
    virtual ~SfArkExtractor2();
    bool extract(const char * outputFilePath) override;
    virtual QString getError() override
    {
        QString error = "";
        switch (_errorNumber)
        {
        case 0: error = "No error"; break;
        case -1: error = "Failed to initialize"; break;
        case -2: error = "Failed to allocate memory"; break;
        case -3: error = "header does not contain 'sfArk' signature"; break;
        case -4: error = "sfArk file has a corrupt header"; break;
        case -5: error = "sfArk file is incompatible (i.e. not sfArk V2.x)"; break;
        case -6: error = "sfArk file uses unsupported feature"; break;
        case -7: error = "got invalid compressed data (file is corrupted)"; break;
        case -8: error = "file checksum failed (file is corrupted)"; break;
        case -9: error = "File i/o error"; break;
        case -10: error = "License included not agreed by user"; break;
        case -11: error = "Other error (currently unused)"; break;
        default: error = "Unknown error (" + QString::number(_error) + ")"; break;
        }
        return error;
    }

private:
    void sfkl_msg(const char *MessageText, int Flags);
    void sfkl_UpdateProgress(int ProgressPercent);
    bool sfkl_GetLicenseAgreement(const char *LicenseText, const char *LicenseFileName);
    void sfkl_DisplayNotes(const char *NotesText, const char *NotesFileName);
    int ChkErr(const char *ErrorMsg, bool isInput);
    void OpenOutputFile(const char *FileName);
    int WriteOutputFile(const quint8 *Buf, int BytesToWrite);
    bool SetOutputFilePosition(int NewPos);
    void CloseOutputFile();
    void OpenInputFile(const char *FileName);
    int ReadInputFile(quint8 *Buf, int BytesToRead);
    bool SetInputFilePosition(int NewPos);
    void CloseInputFile();

    void BioDecompInit();
    void BioDecompEnd();
    BIOWORD BioRead(int n);
    bool BioReadFlag(void);
    long BioReadBuf(BYTE *buf, long n);
    AWORD InputDiff(AWORD Prev);
    long UnCrunch(AWORD *UnCompBuf, USHORT bufsize);
    long UnCrunchWin(AWORD *buf, USHORT bufsize, USHORT winsize);
    short GetNBits(short w);

    USHORT GetsfArkLibVersion(void);
    int ReadHeader(V2_FILEHEADER *FileHeader, BYTE *fbuf, int bufsize);
    void InitFilenames(const char *OrigFileName, const char * /*InFileName*/, const char *ReqOutFileName);
    bool InvalidEncodeCount(int EncodeCount, int MaxLoops);
    int DecompressTurbo(BLOCK_DATA *Blk, USHORT NumWords);
    bool CheckShift(short *ShiftVal, USHORT NumWords, short *PrevShift, short *PrevUsedShift);
    int DecompressFast(BLOCK_DATA *Blk, USHORT NumWords);
    int ProcessNextBlock(BLOCK_DATA *Blk);
    int	EndProcess(int ErrorNum);
    bool ExtractTextFile(BLOCK_DATA *Blk, ULONG FileType);
    int Decode(const char *InFileName, const char *ReqOutFileName);
    void FixEndian(void * /*num*/, int /*nsize*/);

    ULONG UnMemcomp(const BYTE *InBuf, int InBytes, BYTE *OutBuf, int OutBufLen);

    char *GetFileExt(char *FileName);
    char *StrncpyEnsureNul(char *destination, const char *source, int num);
    void ChangeFileExt(const char *path, const char *newExtension, char *out, int outSize);

    LPC_CORR schur(             // returns the minimum mean square error
        LPC_CORR const * ac,    //  in: [0...p] autocorrelation values
        int nc,                 //  in: number of ref. coeff
        LPC_PRAM        * ref);
    void autocorrelation(int n, LPC_WORD const *ibuf, int nc, LPC_CORR *ac);
    void AddAC (LPC_WORD const *hbuf, LPC_WORD const *ibuf, int nc, LPC_CORR *ac);
    void LPCdecode(
        LPC_PRAM const *ref,    //  in: [0...p-1] reflection coefficients
        int            nc,      //  in: number of coefficients
        int            n,       //      # of samples
        LPC_WORD const *in,     //      [0...n-1] residual input
        LPC_WORD       *out);
    long UnLPC2(LPC_WORD *OutBuf, LPC_WORD *InBuf, short bufsize, short nc, ULONG *Flags);
    void LPCinit();
    long UnLPC(AWORD *OutBuf, AWORD *InBuf, short bufsize, short nc, ULONG *Flags);

    void UnBufDif2(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev);
    void old_UnBufDif3(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev);
    void UnBufDif3(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev);
    void UnBufDif4(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev);
    long BufSum(const AWORD *buf, USHORT bufsize);
    void UnBufShift1(AWORD *InBuf, USHORT bufsize, short Shift);
    void UnBufShift(AWORD *InBuf, USHORT bufsize, short *shift);

    SfArkFileManager _fileManager;
    bool _error;
    int _errorNumber;
    QString _filename;
    int _outputFileHandle;
    int	_inputFileHandle;

    // Static table to store number of bits per word...
    BYTE _nb_init;
    BYTE _nb[1 << (AWORD_BITS-1)]; // Array to hold number of bits needed to represent each unsigned short value
    BIOWORD2 _bioBits;					// Bits not yet shifted from bioBuf (up to double length of BIOWORD)
    BIOWORD _bioBuf[BIOBUFSIZE];		// Buffer
    int    _bioP;					// Count of output (index into bioBuf)
    int    _bioRemBits;				// Remaining bits left in bioBits
    int    _bioWholeBlocks;			// Count blocks read from disk
    short  _bioPfb;					// Previous "FixBits" value

    BYTE * _zbuf1;
    BYTE * _zbuf2;
    char _outFileNameMain[SFARKLIB_MAX_FILEPATH];
    char _outFileNameNotes[SFARKLIB_MAX_FILEPATH];
    char _outFileNameLicense[SFARKLIB_MAX_FILEPATH];

    int	GlobalErrorFlag;
    char MsgTxt[MAX_MSGTEXT];
    static const char ProgName[];				// e.g. "sfArkLib"
    static const char ProgVersion[];			// e.g."2.10 "
    static const unsigned char ProgVersionMaj;		// 00-255 = V25.5x, etc.
    static const unsigned char ProgVersionMin;		// 00-99 = Vx.x99, etc.
    unsigned SourceFileOffset;			// Set non-zero by app for self-extraction

    static const char LicenseExt[];		// File extension for license file
    static const char NotesExt[] ;			// File extension for notes file
    static const char SfExt[];
    static const char CorruptedMsg[];
    static const char UpgradeMsg[];

    // Static variables in functions...
    LPC_WORD LPCdecode_u[PMAX+1];
    LPC_WORD UnLPC2_HistBuf[PMAX*2];
    LPC_CORR UnLPC2_AcHist[HISTSIZE][PMAX+1];
    int UnLPC2_HistNum;
};

#endif // SFARKEXTRACTOR2_H
