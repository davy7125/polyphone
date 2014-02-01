// sfArkLib file i/o
// copyright 1998-2000, Andy Inman
// Contact via: http://netgenius.co.uk or http://melodymachine.com

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


// NB: When sfArkLib is used with SDL, these functions are replaced
// by equvalents in sdl.cpp because the linker finds those first.

#include <stdio.h>
#include <string.h>

#include "wcc.h"
//#include "zlib.h"	// only needed for debug printf

#ifdef TARGET_WIN32
// Windows32 target
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "windows.h"
#undef 	WIN32_LEAN_AND_MEAN

#define	FILE_SHARE_NONE			0
#define NO_SECURITY			((LPSECURITY_ATTRIBUTES) 0)
#define NOT_OVERLAPPED			((LPOVERLAPPED) 0)
#define	NO_TEMPLATE			((HANDLE) 0)

#define	CREATEFILE(FileName) 	CreateFile((LPCTSTR)FileName, GENERIC_WRITE, FILE_SHARE_NONE, NO_SECURITY, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN, NO_TEMPLATE)
#define OPENFILE(FileName)	CreateFile((LPCTSTR)FileName, GENERIC_READ,  FILE_SHARE_READ, NO_SECURITY, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN, NO_TEMPLATE)
#define READFILE(fh, Buf, BytesToRead, pBytesRead)	ReadFile(fh, Buf, BytesToRead, pBytesRead, NOT_OVERLAPPED)
#define WRITEFILE(fh, Buf, BytesToWrite, pBytesWritten)	ReadFile(fh, Buf, BytesToWrite, pBytesWritten, NOT_OVERLAPPED)
#define	SETFILEPOINTER(fh, NewPos)	(SetFilePointer(fh, NewPos, 0, FILE_BEGIN))
#define CLOSEFILE(fh)		(CloseHandle(fh))
#define GETLASTERROR()		(GetLastError())

// Mac, Linux target
#else
typedef FILE* HANDLE;

#define INVALID_HANDLE_VALUE	(NULL)

#define	CREATEFILE(filename) 	fopen(filename, "wb")
#define OPENFILE(filename)	fopen(filename, "rb")
#define READFILE(fh, Buf, BytesToRead, pBytesRead)		*pBytesRead = fread(Buf, 1, BytesToRead, fh)
#define WRITEFILE(fh, Buf, BytesToWrite, pBytesWritten)	*pBytesWritten = fwrite(Buf, 1, BytesToWrite, fh)
#define SETFILEPOINTER(fh, NewPos)	(fseek(fh, NewPos, SEEK_SET))
#define CLOSEFILE(fh)		(fclose(fh) == 0)
#define GETLASTERROR()		(-1)
#endif


// Static data to track current input and output file...
HANDLE	InputFileHandle = INVALID_HANDLE_VALUE;		// current input file handle
QDataStream	* OutputFileHandle = NULL;	// ... output file handle
char	InFileName[SFARKLIB_MAX_FILEPATH];							// current input file name

// Local prototypes...
int	ChkErr(const char *message, const char *filename);

void OpenInputFile(const char *FileName)
{
    strncpy(InFileName, FileName, sizeof(InFileName));
    InputFileHandle = OPENFILE(FileName);
    if (InputFileHandle == INVALID_HANDLE_VALUE)
        ChkErr("open", InFileName);
}

void setOutputFileHandle(QDataStream * stream)
{
    OutputFileHandle = stream;
}

// =================================================================================

int ReadInputFile(BYTE *Buf, int BytesToRead)
{
    int	BytesRead;

    READFILE(InputFileHandle, Buf, BytesToRead, &BytesRead);
    if (BytesRead < 0)
    {
        ChkErr("read from", InFileName);
        BytesRead = 0;
    }
    return BytesRead;
}
// =================================================================================

int WriteOutputFile(const BYTE *Buf, int BytesToWrite)
{
    OutputFileHandle->writeRawData((char *)Buf, BytesToWrite);
    return BytesToWrite;
}

// =================================================================================

bool SetInputFilePosition(int NewPos)
{
    if (SETFILEPOINTER(InputFileHandle, NewPos) != 0)
    {
        ChkErr("SetInputFilePosition", InFileName);
        return false;
    }
    //else printf("SetInputFilePosition Ok: Requested %d Actual %ld\n", NewPos, ftell(InputFileHandle));
    return true;
}
// =================================================================================

void CloseInputFile(void)
{
    if (InputFileHandle != INVALID_HANDLE_VALUE && CLOSEFILE(InputFileHandle) == 0)
        ChkErr("Close input file: ", InFileName);
    InputFileHandle = INVALID_HANDLE_VALUE;
    return;
}
// =================================================================================

int ChkErr(const char *ErrorMsg, const char *FileName)
{
    int		ErrCode;
    char	ErrDesc[MAX_MSGTEXT];

    if (~GlobalErrorFlag)		// Prevent multiple error messages
    {
        ErrCode = GETLASTERROR();
        sprintf(ErrDesc, "OS ERROR %d - Failed to %s: %s", ErrCode, ErrorMsg, FileName);
        msg(ErrDesc, MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_FILEIO;
    }
    return(GlobalErrorFlag);
}

// =================================================================================
// eof
