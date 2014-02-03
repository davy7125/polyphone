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

// Static data to track current input and output file...
QDataStream	* InputFileHandle = NULL;	// current input file handle
QDataStream	* OutputFileHandle = NULL;	// ... output file handle

void setInputFileHandle(QDataStream * stream)
{
    InputFileHandle = stream;
}

void setOutputFileHandle(QDataStream * stream)
{
    OutputFileHandle = stream;
}

int ReadInputFile(BYTE *Buf, int BytesToRead)
{
    return InputFileHandle->readRawData((char *)Buf, BytesToRead);
}

int WriteOutputFile(const BYTE *Buf, int BytesToWrite)
{
    return OutputFileHandle->writeRawData((char *)Buf, BytesToWrite);
}

bool SetInputFilePosition(int NewPos)
{
    QIODevice * device = InputFileHandle->device();
    return device->seek(NewPos);
}
