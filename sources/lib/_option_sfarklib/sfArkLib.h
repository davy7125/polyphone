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

																	
// Future SfArkLib versions may include additional further error codes,
// so, any other negative value is "unknown error"
