// sfArkLib	ZLIB interface
// Copyright 1998-2000 Andy Inman
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

#include	<stdio.h>
#include	<string.h>

#include	"wcc.h"
#include	"zlib.h"

ULONG	UnMemcomp(const BYTE *InBuf, int InBytes, BYTE *OutBuf, int OutBufLen)
{
    // Uncompress buffer using ZLIBs uncompress function...
    ULONG	OutBytes = OutBufLen;
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

