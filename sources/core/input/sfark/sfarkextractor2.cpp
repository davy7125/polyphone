/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "sfarkextractor2.h"
#include "sfArkLib.h"
#include <QDebug>

/////////////////////////////
/// CLASS SfArkExtractor2 ///
/////////////////////////////

SfArkFileManager SfArkExtractor2::_fileManager = SfArkFileManager();


SfArkExtractor2::SfArkExtractor2(const char * fileName) : AbstractExtractor(),
    _filename(fileName)
{
}

SfArkExtractor2::~SfArkExtractor2()
{
    _fileManager.clearData();
}

bool SfArkExtractor2::extract(const char *outputFilePath)
{
    _errorNumber = sfkl_Decode(_filename.toStdString().c_str(), outputFilePath);
    _error = (_errorNumber != SFARKLIB_SUCCESS && _errorNumber != SFARKLIB_ERR_CORRUPT && _errorNumber != SFARKLIB_ERR_FILECHECK);
    return !_error;
}


//////////////////////////
/// SFARKLIB CALLBACKS ///
//////////////////////////

// Message display function
void sfkl_msg(const char *MessageText, int Flags)
{
    qDebug() << MessageText << "(" << Flags << ")";
}

// Progress indication
void sfkl_UpdateProgress(int ProgressPercent)
{
    Q_UNUSED(ProgressPercent)
}

// Display/confirm license
bool sfkl_GetLicenseAgreement(const char *LicenseText, const char *LicenseFileName)
{
    Q_UNUSED(LicenseText)
    Q_UNUSED(LicenseFileName)

    // Delete the file that has been created
    SfArkExtractor2::_fileManager.deleteFile(LicenseFileName);

    return true;
}

// Display notes text file
void sfkl_DisplayNotes(const char *NotesText, const char *NotesFileName)
{
    Q_UNUSED(NotesText)

    // Delete the file that has been created
    SfArkExtractor2::_fileManager.deleteFile(NotesFileName);
}


//////////////////////
/// INPUT - OUTPUT ///
//////////////////////

int OutputFileHandle = -1;

int ChkErr(const char *ErrorMsg, bool isInput)
{
    char ErrDesc[SFARKLIB_MAX_MSGTEXT];
    extern int GlobalErrorFlag;

    if (~GlobalErrorFlag)		// Prevent multiple error messages
    {
        if (isInput)
            sprintf(ErrDesc, "Input: failed to %s", ErrorMsg);
        else
            sprintf(ErrDesc, "Output: failed to %s", ErrorMsg);
        sfkl_msg(ErrDesc, SFARKLIB_MSG_PopUp);
        GlobalErrorFlag = SFARKLIB_ERR_FILEIO;
    }

    return GlobalErrorFlag;
}

void OpenOutputFile(const char *FileName)
{
    OutputFileHandle = SfArkExtractor2::_fileManager.create(FileName);
    if (OutputFileHandle == -1)
        ChkErr("create", false);
}

int WriteOutputFile(const quint8 *Buf, int BytesToWrite)
{
    if (SfArkExtractor2::_fileManager.write(OutputFileHandle, (char *)Buf, BytesToWrite) != BytesToWrite)
    {
        ChkErr("write", false);
        return 0;
    }

    return BytesToWrite;
}

bool SetOutputFilePosition(int NewPos)
{
    if (!SfArkExtractor2::_fileManager.setAbsolutePos(OutputFileHandle, NewPos))
    {
        ChkErr("set position", false);
        return false;
    }

    return true;
}

void CloseOutputFile()
{
    if (OutputFileHandle == -1)
        ChkErr("close", false);
    else
        SfArkExtractor2::_fileManager.close(OutputFileHandle);
    OutputFileHandle = -1;
}

// Static data to track current input and output file...
int	InputFileHandle = -1;		// current input file handle

void OpenInputFile(const char *FileName)
{
    InputFileHandle = SfArkExtractor2::_fileManager.openReadOnly(FileName);
    if (InputFileHandle == -1)
        ChkErr("open", true);
}

int ReadInputFile(quint8 *Buf, int BytesToRead)
{
    int BytesRead;

    BytesRead = SfArkExtractor2::_fileManager.read(InputFileHandle, (char*)Buf, BytesToRead);
    if (BytesRead < 0)
    {
        ChkErr("read", true);
        BytesRead = 0;
    }

    return BytesRead;
}

bool SetInputFilePosition(int NewPos)
{
    if (!SfArkExtractor2::_fileManager.setAbsolutePos(InputFileHandle, NewPos))
    {
        ChkErr("set position", true);
        return false;
    }

    return true;
}

void CloseInputFile()
{
    if (InputFileHandle == -1)
        ChkErr("close", true);
    else
        SfArkExtractor2::_fileManager.close(InputFileHandle);
    InputFileHandle = -1;
}
