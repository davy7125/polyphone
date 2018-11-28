// BIO (Bit i/o) routines for sfArk / daArk
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

// Static table to store number of bits per word...
static BYTE nb_init = 0;
static BYTE nb[1 << (AWORD_BITS-1)]; // Array to hold number of bits needed to represent each unsigned short value

// =========================================================================
// Macros for Bit I/O (BIO) ....

// Static buffer and pointers....
  #define BIO_WBITS     ((signed) (8 * sizeof(BIOWORD)))
  #define BIOBUFSIZE    (16 * 1024)	// Disk-read buffer size, bigger may increased speed

  BIOWORD2 bioBits;					// Bits not yet shifted from bioBuf (up to double length of BIOWORD)
  BIOWORD bioBuf[BIOBUFSIZE];		// Buffer

  int    bioP;						// Count of output (index into bioBuf)
  int    bioRemBits;				// Remaining bits left in bioBits
  int    bioWholeBlocks;			// Count blocks read from disk
  short  bioPfb;					// Previous "FixBits" value

// --------------------------------------------------------------------------
#define GRP_INBITS(g)                           \
  /* Get grp value from input by counting number of zeroes before we get a 1 */\
  g = 0;                                        \
  while (bioBits == 0)                          \
  {                                             \
    g += bioRemBits;                            \
    CHECK_INBUFFER                              \
    bioBits = bioBuf[bioP++];                   \
    bioRemBits = BIO_WBITS;                     \
  }                                             \
                                                \
  g += bioRemBits;                              \
  while ((bioBits >> --bioRemBits) != 1);       \
  g -= bioRemBits+1;                            \
  bioBits = LOWBITS(bioBits, bioRemBits)    

// --------------------------------------------------------------------------
#define INBITS(w, n)                            \
  /* Input value w using n bits (n must be <= BIO_WBITS) ... */\
  if (bioRemBits < BIO_WBITS)                   \
  {                                             \
    CHECK_INBUFFER                              \
    bioBits = (bioBits << BIO_WBITS) | bioBuf[bioP++]; \
    bioRemBits += BIO_WBITS;                    \
  }                                             \
  bioRemBits -= n;                              \
  w = (BIOWORD) (bioBits >> bioRemBits);        \
  bioBits = LOWBITS(bioBits, bioRemBits)		
  
// =========================================================================
#ifdef	__BIG_ENDIAN__

#define	WFIX(I)		s = bp[I+0]; bp[I+0] = bp[I+1]; bp[I+1] = s;
// Read from disk if needed, and fix endians
#define CHECK_INBUFFER                          \
  if (bioP == BIOBUFSIZE)                       \
  {                                             \
    bioWholeBlocks++;                           \
    bioP = 0;                                   \
    int ReadLen = ReadInputFile((BYTE *) bioBuf, BIOBUFSIZE * sizeof(BIOWORD)); \
    if (ReadLen <= 0)  return 0;		\
    BYTE *bp = (BYTE *) bioBuf, *ep = (BYTE *) (bioBuf+BIOBUFSIZE); \
    do {					\
        BYTE s;					\
        WFIX(0); WFIX(2); WFIX(4); WFIX(6);	\
        WFIX(8); WFIX(10); WFIX(12); WFIX(14);	\
        bp += 16;				\
    } while (bp < ep);				\
  }

#else
// Read from disk if needed...
#define CHECK_INBUFFER                          \
  if (bioP == BIOBUFSIZE)                       \
  {                                             \
    bioWholeBlocks++;                           \
    bioP = 0;                                   \
    int ReadLen = ReadInputFile((BYTE *) bioBuf, BIOBUFSIZE * sizeof(BIOWORD)); \
    if (ReadLen <= 0)  return 0;				\
  }

#endif

// --------------------------------------------------------------------------
// End of macros
// --------------------------------------------------------------------------


// =========================================================================
void BioDecompInit()
{
  GetNBits(0);									// Initialise nb array
  bioRemBits= 0;
  bioBits	= 0;
  bioP		= BIOBUFSIZE;
  bioPfb	= 8;
	return;
}
// =========================================================================
void BioDecompEnd()
{
  return;
}
// =========================================================================
BIOWORD BioRead(int n)
// Read bits from input, return value
{
  BIOWORD w;
  INBITS(w, n);
  return w;
}

// =========================================================================
bool BioReadFlag(void)
// Read single bit from input, return value as bool
{
  BIOWORD w;
  INBITS(w, 1);
	return (w != 0);
}

// =========================================================================
long BioReadBuf(BYTE *buf, long n)
// Read *bytes* to output, return number of BYTES
{
  int SavebioP = bioP;
  bioWholeBlocks = 0;

  while (n--)
  {
    BIOWORD b;
    INBITS(b, 8);
    *buf++ = (BYTE) b;
  }

  return (bioP - SavebioP + bioWholeBlocks * BIOBUFSIZE) * sizeof(BIOWORD);
}
// =========================================================================
AWORD	InputDiff(AWORD Prev)
// Read a value from input as difference from Previous value, return new value
{
  AWORD x;

  GRP_INBITS(x);
  if (x != 0)								// If non-zero, check sign bit
	{
		int sign;  INBITS(sign, SIGNBIT);
		if (sign != 0)  x = -x;
	}
  return x + Prev;
}

// ==============================================================

long UnCrunch(AWORD *UnCompBuf, USHORT bufsize)
{
  short     FixBits;
  AWORD *bp = UnCompBuf, *endp = UnCompBuf+bufsize;

	FixBits = InputDiff(bioPfb);
  bioPfb = FixBits;

  if (FixBits >= 0  &&  FixBits < 14)
  {
    // Uncompress the block...
    short FSbits = FixBits + SIGNBIT;
    do {
      UAWORD grp, LoBits;
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

// =========================================================================
long UnCrunchWin(AWORD *buf, USHORT bufsize, USHORT winsize)
{
	USHORT finalwinsize = bufsize % winsize;
  AWORD *endp = buf + bufsize - finalwinsize;

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

// =========================================================================
short GetNBits(short w)
 // Get the number of bits used by value w
 // e.g. w=000b, n=0
 //      w=001b, n=1
 //      w=010b, n=2
 //      w=011b, n=2
 //      w=100b, n=3
 //      w=111b, n=3 etc.

{
	// If not initialised (first time), build table in nb[] array...
  if (nb_init == 0)		
  {
    long first = 1, last, i;
    BYTE nbits = 1;
    nb_init = 1;
    nb[0] = 0;

    do {
      last = first + (1 << (nbits-1));
      for (i = first; i < last; i++)  nb[i] = nbits;
      nbits++;
      first = last;
    } while(last <= MAX_AWORD);
  }
  return nb[w];
}

// =========================================================================
