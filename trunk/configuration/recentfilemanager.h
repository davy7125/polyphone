/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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

#ifndef RECENTFILEMANAGER_H
#define RECENTFILEMANAGER_H

#include <QStringList>

class RecentFileManager
{
public:
    enum FileType
    {
        FILE_TYPE_SF2,
        FILE_TYPE_SAMPLE,
        FILE_TYPE_RECORD,
        FILE_TYPE_SOUNDFONT,
        FILE_TYPE_EXPORT,
        FILE_TYPE_FREQUENCIES
    };

    /// Get the instance
    static RecentFileManager * getInstance()
    {
        if (_instance == NULL)
            _instance = new RecentFileManager();
        return _instance;
    }

    /// Destroy the singleton
    void kill()
    {
        delete _instance;
        _instance = NULL;
    }

    /// Get a recent file, possibly in a specific position (for sf2 only)
    QString getLastFile(FileType fileType, int num=0);

    /// Get the last directory for a specific type of file
    QString getLastDirectory(FileType fileType);

    /// Add a recent file
    void addRecentFile(FileType fileType, QString filePath);

private:
    RecentFileManager();

    static RecentFileManager * _instance;
    QStringList _listFiles;
    QString _recordFile, _sampleFile, _exportFile, _importFile, _pngFile;
};

#endif // RECENTFILEMANAGER_H
