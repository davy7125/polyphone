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

#ifdef TARGET_WIN32
// Windows32 target
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "windows.h"
#undef 	WIN32_LEAN_AND_MEAN

#define	FILE_SHARE_NONE			0
#define NO_SECURITY			((LPSECURITY_ATTRIBUTES) 0)
#define NOT_OVERLAPPED			((LPOVERLAPPED) 0)
#define	NO_TEMPLATE			((HANDLE) 0)

#define GETLASTERROR()		(GetLastError())

// Mac, Linux target
#else

#define INVALID_HANDLE_VALUE	(NULL)
#define GETLASTERROR()		(-1)
#endif


// Static data to track current input and output file...
QDataStream	* InputFileHandle = NULL;	// current input file handle
QDataStream	* OutputFileHandle = NULL;	// ... output file handle

// Local prototypes...
int	ChkErr(const char *message, const char *filename);

void setInputFileHandle(QDataStream * stream)
{
    InputFileHandle = stream;
}
void setOutputFileHandle(QDataStream * stream)
{
    OutputFileHandle = stream;
}

// =================================================================================

int ReadInputFile(BYTE *Buf, int BytesToRead)
{
    return InputFileHandle->readRawData((char *)Buf, BytesToRead);
}
// =================================================================================

int WriteOutputFile(const BYTE *Buf, int BytesToWrite)
{
    return OutputFileHandle->writeRawData((char *)Buf, BytesToWrite);
}

// =================================================================================

bool SetInputFilePosition(int NewPos)
{
    QIODevice * device = InputFileHandle->device();
    return device->seek(NewPos);
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
