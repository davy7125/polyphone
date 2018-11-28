// Header for sfArkLib

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

/*
Changes since V1.15
===================
All constants now prefixed with SFARKLIB_, and function names prefxied with sfkl_

sfkl_GetLicenseAgreement and sfkl_DisplayNotes functions now both take 2 parameters: the text of the text file
and the name of the text file. This makes it easier for the application to either display the text itself or pass
the filename onto some other program.  Mac/Linux users note: the text is probably in Windows format! 
Future sfArk versions will allow Notes and License files to be of other types than plain text (e.g. HTML, RTF) so
look at the file extenson in order to decide how to handle it.
*/
 
// Some max sizes...
#define	SFARKLIB_MAX_FILENAME	256				// Longest filename handled (or directory name)
#define	SFARKLIB_MAX_FILEPATH	1024				// Longest full path handled
#define	SFARKLIB_MAX_MSGTEXT	(SFARKLIB_MAX_FILEPATH + 1024)	// Longest message we might produce with sfkl_msg()

// Flags used with msg() function...
#define	SFARKLIB_MSG_SameLine	(1 << 0)	// Overwrite previous message
#define	SFARKLIB_MSG_AppendLine	(1 << 1)	// Append to previous message
#define	SFARKLIB_MSG_PopUp	(1 << 2)	// Should "pop up" a dialogue (usually an error message)

// Error codes...
#define SFARKLIB_SUCCESS		 0	// No error
#define	SFARKLIB_ERR_INIT		-1	// Failed to initialize
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
																	
// Future SfArkLib versions may include additional further error codes,
// so, any other negative value is "unknown error"

// Callback functions needed by sfArkLib (must be supplied by Application) ...
extern void sfkl_msg(const char *MessageText, int Flags);	// Message display function
extern void sfkl_UpdateProgress(int ProgressPercent);		// Progress indication
extern bool sfkl_GetLicenseAgreement(const char *LicenseText, const char *LicenseFileName); // Display/confirm license
extern void sfkl_DisplayNotes(const char *NotesText, const char *NotesFileName);	// Display notes text file

// Functions in sfArkLib for use by Application...
extern unsigned short	sfkl_GetVersion(void);
extern int 		sfkl_Decode(const char *InFileName, const char *ReqOutFileName);
