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

#ifndef RECENTFILEMANAGER_H
#define RECENTFILEMANAGER_H

#include <QStringList>
#include <QObject>
#include <QDateTime>
class ConfManager;

class RecentFileManager: public QObject
{
    Q_OBJECT

public:
    static int MAX_SF2_FILES;

    enum FileType
    {
        FILE_TYPE_SOUNDFONT,
        FILE_TYPE_SAMPLE,
        FILE_TYPE_RECORD,
        FILE_TYPE_EXPORT,
        FILE_TYPE_FREQUENCIES,
        FILE_TYPE_EXECUTABLE
    };

    RecentFileManager(ConfManager * configuration);
    virtual ~RecentFileManager() {}

    /// Get a recent file, possibly in a specific position (for sf2 only)
    QString getLastFile(FileType fileType, int num = 0);

    /// Get the access data time of a sf2 file (sf2 only for now)
    QDateTime getLastFileDateTime(FileType fileType, int num = 0);

    /// Get the last directory for a specific type of file
    QString getLastDirectory(FileType fileType);

    /// Add a recent file
    void addRecentFile(FileType fileType, QString filePath);

signals:
    void recentSf2Changed();

private:
    ConfManager * _configuration;
    QStringList _listFiles;
    QList<QDateTime> _listDateTimes;
    QString _recordFile, _sampleFile, _exportFile, _pngFile, _executableFile;
};

#endif // RECENTFILEMANAGER_H
