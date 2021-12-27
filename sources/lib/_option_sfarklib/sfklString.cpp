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
#include <stdio.h>
#include "wcc.h"

char *GetFileExt(char *FileName)
{
  // return pointer to (final) file extension including the dot e.g. '.txt'
  // returns NULL if extension is not found
  char *p;

  for(p = FileName + strlen(FileName); p > FileName; p--)
  {
    if (*p == '.')
    {
      return p;
    }
    else if (*p == '/' || *p == '\\')
    {
      return NULL;
    }
  }

  return NULL;
}

char *StrncpyEnsureNul(char *destination, const char *source, int num)
{
  if (num <= 0 || !destination)
  {
    return destination;
  }
  else if (!source)
  {
    destination[0] = '\0';
    return destination;
  }
  else
  {
    // same as strncpy, except it ensures the result is nul-terminated.
    char *ret = strncpy(destination, source, num - 1);
    destination[num - 1] = 0;
    return ret;
  }
}

void ChangeFileExt(const char *path, const char *newExtension, char *out, int outSize)
{
  if (strlen(path) >= MAX_FILEPATH)
  {
    // path is too long for our buffer
    out[0] = 0;
    return;
  }

  char pathWithoutExtension[MAX_FILEPATH] = "";
  StrncpyEnsureNul(pathWithoutExtension, path, sizeof(pathWithoutExtension));
  char *p = GetFileExt(pathWithoutExtension);
  if (p)
  {
    // add null-terminator where the '.' used to be.
    *p = 0;
  }

  // if there's no extension in the input, we'll simply append the new extension.
  int ret = snprintf(out, outSize, "%s%s", pathWithoutExtension, newExtension);
  if (ret < 0 || ret >= outSize)
  {
    out[0] = 0;
    return;
  }
}
