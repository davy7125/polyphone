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

// =========================================================================
 void UnBufDif2(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
// NB: This code is optimised for speed - do not change order of operations or data types!!!
// -----------------------------------------------------------------------------------------

#if 0
  const AWORD *bufend1 = InBuf + bufsize;

  // Process start of buffer...
  *OutBuf++ = *InBuf++ + *prev;

  // Process rest of buffer...
  #define STEPSIZE 8
  #define DO_ONE(N)     OutBuf[N] = InBuf[N] + OutBuf[N-1];
  // ----------------------------------------------------

  const AWORD *bufend2 = bufend1 - STEPSIZE;
  while (InBuf < bufend2)
  {
    DO_ONE(0); DO_ONE(1); DO_ONE(2); DO_ONE(3);
    DO_ONE(4); DO_ONE(5); DO_ONE(6); DO_ONE(7);
    OutBuf += STEPSIZE; InBuf += STEPSIZE;
  }

  // Do final few...
  while (InBuf < bufend1)  { DO_ONE(0); OutBuf++; InBuf++; };

  *prev = OutBuf[-1];

#else
  const AWORD *bufend1 = OutBuf + bufsize;

  //for (int i = 0; i < bufsize; i++)  OutBuf[i] = InBuf[i];
  memcpy(OutBuf, InBuf, bufsize*sizeof(AWORD));  

  // Process start of buffer...
  *OutBuf++ += *prev;

  // Process rest of buffer...
  #define STEPSIZE 8
  #define DO_ONE(N)     OutBuf[N] += OutBuf[N-1]; 
  // ----------------------------------------------------

  const AWORD *bufend2 = bufend1 - STEPSIZE;
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
#endif

  return;

  #undef DO_ONE
  #undef STEPSIZE
}

// =========================================================================
 void old_UnBufDif3(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
  // Decoding is done from the END of the buffer, backwards...

  const AWORD *bufstart = InBuf;

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


 void UnBufDif3(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
  // Decoding is done from the END of the buffer, backwards...

  #define STEPSIZE 8
  const AWORD *bufstart = InBuf, *bufstart1 = InBuf+STEPSIZE;

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

// =========================================================================
 void UnBufDif4(AWORD *OutBuf, const AWORD *InBuf, USHORT bufsize, AWORD *prev)
{
  AWORD avg = *prev;
  const AWORD *bufend1 = InBuf + bufsize;

  // Process rest of buffer...
  //-----------------------------------------
  #define STEPSIZE    8
  const AWORD *bufend2 = bufend1 - STEPSIZE;

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
// =========================================================================
 long BufSum(const AWORD *buf, USHORT bufsize)
{
  // --- Optimised! ---
  long Total = 0;

  const AWORD *endp1 = buf + bufsize - 7;
  const AWORD *endp2 = buf + bufsize;

  for (; buf < endp1; buf += 8)
  {
    Total += QUICKABS2(buf[0]) + QUICKABS2(buf[1]) + QUICKABS2(buf[2]) + QUICKABS2(buf[3])
           + QUICKABS2(buf[4]) + QUICKABS2(buf[5]) + QUICKABS2(buf[6]) + QUICKABS2(buf[7]);
  }

  for ( ; buf < endp2; ++buf)
    Total += QUICKABS2(*buf);

  return Total;
}

// =========================================================================
 void UnBufShift1(AWORD *InBuf, USHORT bufsize, short Shift)
{
  #define STEPSIZE 8

  // Ok, now we have a shift value - perform the shift...
  AWORD *bps = InBuf, *bufend1s = bps + bufsize;
  AWORD *bufend2s = bufend1s - STEPSIZE;

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

// =========================================================================
 void UnBufShift(AWORD *InBuf, USHORT bufsize, short *shift)
{
  for(AWORD *endp = InBuf + bufsize; InBuf < endp; InBuf+= SHIFTWIN)
  {
    short s = *shift++;
    if (s != 0)  UnBufShift1(InBuf, SHIFTWIN, s);
  }

  return;
}
// =========================================================================
