#include "sfarkextractor2.h"
#include "sfArkLib.h"
#include "wcc.h"
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
    _fileManager.clearFiles();
}

void SfArkExtractor2::extract()
{
    int error = sfkl_Decode(_filename.toStdString().c_str(), "output.sf2");
    _error = (error != SFARKLIB_SUCCESS && error != SFARKLIB_ERR_CORRUPT && error != SFARKLIB_ERR_FILECHECK);
}

bool SfArkExtractor2::getData(char *&data, qint32 &size)
{
    if (_error)
        data = NULL;
    else
        data = _fileManager.retrieveData("output.sf2", size);

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
    return true;
}

// Display notes text file
void sfkl_DisplayNotes(const char *NotesText, const char *NotesFileName)
{
    Q_UNUSED(NotesText)
    Q_UNUSED(NotesFileName)
}


//////////////////////
/// INPUT - OUTPUT ///
//////////////////////

int OutputFileHandle = -1;

int ChkErr(const char *ErrorMsg, bool isInput)
{
    char ErrDesc[MAX_MSGTEXT];

    if (~GlobalErrorFlag)		// Prevent multiple error messages
    {
        if (isInput)
            sprintf(ErrDesc, "Input: failed to %s", ErrorMsg);
        else
            sprintf(ErrDesc, "Output: failed to %s", ErrorMsg);
        msg(ErrDesc, MSG_PopUp);
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
    if (!SfArkExtractor2::_fileManager.setPos(OutputFileHandle, NewPos))
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

#include <stdio.h>

#ifdef TARGET_WIN32
// Windows32 target
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "windows.h"
#undef 	WIN32_LEAN_AND_MEAN

#define	FILE_SHARE_NONE		0
#define NO_SECURITY			((LPSECURITY_ATTRIBUTES) 0)
#define NOT_OVERLAPPED		((LPOVERLAPPED) 0)
#define	NO_TEMPLATE			((HANDLE) 0)

#define	CREATEFILE(FileName) 	CreateFile((LPCTSTR)FileName, GENERIC_WRITE, FILE_SHARE_NONE, NO_SECURITY, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN, NO_TEMPLATE)
#define OPENFILE(FileName)	CreateFile((LPCTSTR)FileName, GENERIC_READ,  FILE_SHARE_READ, NO_SECURITY, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN, NO_TEMPLATE)
#define READFILE(fh, Buf, BytesToRead, pBytesRead)	ReadFile(fh, Buf, BytesToRead, pBytesRead, NOT_OVERLAPPED)
#define WRITEFILE(fh, Buf, BytesToWrite, pBytesWritten)	ReadFile(fh, Buf, BytesToWrite, pBytesWritten, NOT_OVERLAPPED)
#define	SETFILEPOINTER(fh, NewPos)	(SetFilePointer(fh, NewPos, 0, FILE_BEGIN))
#define CLOSEFILE(fh)		(CloseHandle(fh))

// Mac, Linux target
#else
typedef FILE* HANDLE;

#define INVALID_HANDLE_VALUE	(NULL)

#define	CREATEFILE(filename) 	fopen(filename, "wb")
#define OPENFILE(filename)	fopen(filename, "rb")
#define READFILE(fh, Buf, BytesToRead, pBytesRead)		*pBytesRead = fread(Buf, 1, BytesToRead, fh)
#define WRITEFILE(fh, Buf, BytesToWrite, pBytesWritten)	*pBytesWritten = fwrite(Buf, 1, BytesToWrite, fh)
#define SETFILEPOINTER(fh, NewPos)	(fseek(fh, NewPos, SEEK_SET))
#define CLOSEFILE(fh)		(fclose(fh) == 0)
#endif

// Static data to track current input and output file...
HANDLE	InputFileHandle = INVALID_HANDLE_VALUE;		// current input file handle

void OpenInputFile(const char *FileName)
{
    InputFileHandle = OPENFILE(FileName);
    if (InputFileHandle == INVALID_HANDLE_VALUE)
        ChkErr("open", true);

    //InputFileHandle = SfArkExtractor2::_fileManager.openReadOnly(FileName);
    //if (InputFileHandle == -1)
    //    ChkErr("open", true);
}

int ReadInputFile(quint8 *Buf, int BytesToRead)
{
    int	BytesRead;

    //BytesRead = SfArkExtractor2::_fileManager.read(InputFileHandle, (char*)Buf, BytesToRead);
    READFILE(InputFileHandle, Buf, BytesToRead, &BytesRead);
    if (BytesRead < 0)
    {
        ChkErr("read", true);
        BytesRead = 0;
    }

    return BytesRead;
}

bool SetInputFilePosition(int NewPos)
{
    //if (!SfArkExtractor2::_fileManager.setPos(InputFileHandle, NewPos))
    if (SETFILEPOINTER(InputFileHandle, NewPos) != 0)
    {
        ChkErr("set position", true);
        return false;
    }

    return true;
}

void CloseInputFile()
{
    if (InputFileHandle != INVALID_HANDLE_VALUE && CLOSEFILE(InputFileHandle) == 0)
        ChkErr("close", true);
    InputFileHandle = INVALID_HANDLE_VALUE;

    //    if (InputFileHandle == -1)
    //        ChkErr("close", true);
    //    else
    //        SfArkExtractor2::_fileManager.close(InputFileHandle);
    //    InputFileHandle = -1;
}
