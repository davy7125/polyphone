//sfArkLib String functions
// copyright 1998-2000 Andy Inman
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

#include <string.h>

const char *GetFileExt(const char *FileName)
{
  // return pointer to (final) file extension including the dot e.g. '.txt'
  const char *p;

  for(p = FileName + strlen(FileName); p > FileName &&  *p != '.'; p--) {}
  if (*p == '.')  p = FileName + strlen(FileName);
	return p;
}

