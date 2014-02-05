// Misc C stuff for sfArk/daArk
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


#include "wcc.h"
#include <stdio.h>
#include <string.h>

void UnBufDif2(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
    const AWORD *bufend1 = OutBuf + bufsize;

    for (int i = 0; i < bufsize; i++)
        OutBuf[i] = InBuf[i];

    // Process start of buffer...
    *OutBuf++ += *prev;

    while (OutBuf < bufend1)
    {
        *OutBuf += OutBuf[-1];
        OutBuf++;
    };

    *prev = OutBuf[-1];
}

void UnBufDif3(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
    // Decoding is done from the END of the buffer, backwards...

    const AWORD *bufstart = InBuf;

    InBuf += bufsize-1;
    OutBuf+= bufsize-1;

    // Process end of buffer...
    *OutBuf-- = *InBuf--;

    while (InBuf > bufstart)
    {
        OutBuf[0] = InBuf[0] + ((InBuf[-1] + OutBuf[1]) >> 1);
        InBuf--; OutBuf--;
    }

    // Final word
    *OutBuf = *InBuf + (OutBuf[1] >> 1);

    *prev = OutBuf[bufsize-1];
}

void UnBufDif4(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
    AWORD avg = *prev;

    const AWORD *bufend = InBuf + bufsize;
    for (; InBuf < bufend; ++InBuf, ++OutBuf)
    {
        OutBuf[0] = InBuf[0] + avg;
        avg += InBuf[0] / 2;
    }

    *prev = avg;
}

long BufSum(const AWORD *buf, USHORT bufsize)
{
    long Total = 0;

    const AWORD *endp = buf + bufsize;
    for ( ; buf < endp; ++buf)
        Total += QUICKABS2(*buf);

    return Total;
}

void UnBufShift1(AWORD *InBuf, USHORT bufsize, short Shift)
{
    // Now we have a shift value - perform the shift...
    AWORD *bps = InBuf, *bufend1s = bps + bufsize;

    while (bps < bufend1s)
    {
        bps[0] = (bps[0] << Shift);
        bps++;
    };
}

void UnBufShift(AWORD *InBuf, USHORT bufsize, short *shift)
{
    for(AWORD *endp = InBuf + bufsize; InBuf < endp; InBuf+= SHIFTWIN)
    {
        short s = *shift++;
        if (s != 0)
            UnBufShift1(InBuf, SHIFTWIN, s);
    }
}
