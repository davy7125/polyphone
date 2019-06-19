/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "recentfilemanager.h"
#include "confmanager.h"
#include <QFile>
#include <QDir>
#include <QDesktopServices>

int RecentFileManager::MAX_SF2_FILES = 10;

RecentFileManager::RecentFileManager(ConfManager * configuration) : QObject(nullptr),
    _configuration(configuration)
{
    // Load recent files
    _recordFile = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "record", "").toString();
    _sampleFile = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "sample", "").toString();
    _exportFile = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "export", "").toString();
    _pngFile = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "frequencies", "").toString();
    _executableFile = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "executable", "").toString();

    // Recent sf2 list
    int j = 0;
    QString strTmp;
    for (int i = 0; i < MAX_SF2_FILES; i++)
    {
        _listFiles.append("");
        _listDateTimes.append(QDateTime());

        // Test if still valid
        strTmp = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "file_" + QString::number(i), "").toString();
        QFile file(strTmp);
        if (file.exists())
        {
            _listFiles[j] = strTmp;

            // Date
            QString dateStr = _configuration->getValue(ConfManager::SECTION_RECENT_FILES, "file_time_" + QString::number(i), "").toString();
            _listDateTimes[j++] = QDateTime::fromString(dateStr, "yyyy/MM/dd HH:mm:ss");
        }
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
        _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "record", _recordFile);
        break;
    case FILE_TYPE_SAMPLE:
        _sampleFile = filePath;
        _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "sample", _sampleFile);
        break;
    case FILE_TYPE_EXPORT:
        _exportFile = filePath;
        _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "export", _exportFile);
        break;
    case FILE_TYPE_FREQUENCIES:
        _pngFile = filePath;
        _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "frequencies", _pngFile);
        break;
    case FILE_TYPE_EXECUTABLE:
        _executableFile = filePath;
        _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "executable", _executableFile);
        break;
    case FILE_TYPE_SOUNDFONT:{
        int position = _listFiles.indexOf(filePath);
        if (position == -1)
            position = MAX_SF2_FILES - 1;
        for (int i = position; i > 0; i--)
        {
            _listFiles[i] = _listFiles.at(i - 1);
            _listDateTimes[i] = _listDateTimes.at(i - 1);
        }
        _listFiles[0] = filePath;
        _listDateTimes[0] = QDateTime::currentDateTime();

        for (int i = 0; i < MAX_SF2_FILES; i++)
        {
            _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "file_" + QString::number(i), _listFiles.at(i));
            _configuration->setValue(ConfManager::SECTION_RECENT_FILES, "file_time_" + QString::number(i),
                                     _listDateTimes.at(i).toString("yyyy/MM/dd HH:mm:ss"));
        }

        emit(recentSf2Changed());
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
    case FILE_TYPE_SOUNDFONT:
        if (num >= 0 && num < _listFiles.count())
            lastFile = _listFiles.at(num);
        break;
    case FILE_TYPE_EXPORT:
        lastFile = _exportFile;
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


QDateTime RecentFileManager::getLastFileDateTime(FileType fileType, int num)
{
    QDateTime datetime;

    switch (fileType)
    {
    case FILE_TYPE_SOUNDFONT:
        if (num >= 0 && num < _listDateTimes.count())
            datetime = _listDateTimes.at(num);
        break;
    default:
        break;
    }

    return datetime;
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
