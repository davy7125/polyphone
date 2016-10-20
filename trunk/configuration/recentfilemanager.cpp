/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "recentfilemanager.h"
#include "confmanager.h"
#include <QFile>
#include <QDir>
#include <QDesktopServices>

RecentFileManager * RecentFileManager::_instance = NULL;

RecentFileManager::RecentFileManager()
{
    // Initialize the list
    for (int i = 0; i < 5; i++)
        _listFiles.append("");

    // Load recent files
    _recordFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "record", "").toString();
    _sampleFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "sample", "").toString();
    _exportFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "export", "").toString();
    _importFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "import", "").toString();
    _pngFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "frequencies", "").toString();
    _executableFile = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "executable", "").toString();

    int j = 0;
    QString strTmp;
    for (int i = 0; i < 5; i++)
    {
        // Test if still valid
        strTmp = ConfManager::getInstance()->getValue(ConfManager::SECTION_RECENT_FILES, "file_" + QString::number(i), "").toString();
        QFile file(strTmp);
        if (file.exists())
        {
            ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_" + QString::number(i), strTmp);
            _listFiles[j++] = strTmp;
        }
    }
    for (int i = j; i < 5; i++)
    {
        _listFiles[i] = "";
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_" + QString::number(i), "");
    }
}

void RecentFileManager::addRecentFile(FileType fileType, QString filePath)
{
    filePath = filePath.replace("\\", "/");

    // Change recent file
    switch (fileType)
    {
    case FILE_TYPE_RECORD:
        _recordFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "record", _recordFile);
        break;
    case FILE_TYPE_SAMPLE:
        _sampleFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "sample", _sampleFile);
        break;
    case FILE_TYPE_EXPORT:
        _exportFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "export", _exportFile);
        break;
    case FILE_TYPE_SOUNDFONT:
        _importFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "import", _importFile);
        break;
    case FILE_TYPE_FREQUENCIES:
        _pngFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "frequencies", _pngFile);
        break;
    case FILE_TYPE_EXECUTABLE:
        _executableFile = filePath;
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "executable", _executableFile);
        break;
    case FILE_TYPE_SF2:{
        int n = 4;
        if (filePath.compare(_listFiles.at(0)) == 0)
            n = 0;
        else if (filePath.compare(_listFiles.at(1)) == 0)
            n = 1;
        else if (filePath.compare(_listFiles.at(2)) == 0)
            n = 2;
        else if (filePath.compare(_listFiles.at(3)) == 0)
            n = 3;
        else if (filePath.compare(_listFiles.at(4)) == 0)
            n = 4;
        for (int i = n-1; i >= 0; i--)
            _listFiles[i+1] = _listFiles.at(i);
        _listFiles[0] = filePath;

        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_0", _listFiles.at(0));
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_1", _listFiles.at(1));
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_2", _listFiles.at(2));
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_3", _listFiles.at(3));
        ConfManager::getInstance()->setValue(ConfManager::SECTION_RECENT_FILES, "file_4", _listFiles.at(4));
    }break;
    }
}

QString RecentFileManager::getLastFile(FileType fileType, int num)
{
    QString lastFile;
    switch (fileType)
    {
    case FILE_TYPE_RECORD:
        lastFile = _recordFile;
        break;
    case FILE_TYPE_SAMPLE:
        lastFile = _sampleFile;
        break;
    case FILE_TYPE_SF2:
        if (num >= 0 && num < 5)
            lastFile = _listFiles.at(num);
        break;
    case FILE_TYPE_EXPORT:
        lastFile = _exportFile;
        break;
    case FILE_TYPE_SOUNDFONT:
        lastFile = _importFile;
        break;
    case FILE_TYPE_FREQUENCIES:
        lastFile = _pngFile;
        break;
    case FILE_TYPE_EXECUTABLE:
        lastFile = _executableFile;
        break;
    }

    return lastFile;
}

QString RecentFileManager::getLastDirectory(FileType fileType)
{
    QString lastDir = getLastFile(fileType);
    if (!lastDir.isEmpty())
        lastDir = QFileInfo(lastDir).dir().path();

    if (lastDir.isEmpty() || !QDir(lastDir).exists())
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        lastDir =  QDir::home().path();
#else
        lastDir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

    return lastDir;
}
