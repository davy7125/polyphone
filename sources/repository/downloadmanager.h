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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMap>
class UrlReader;

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    static DownloadManager * getInstance();
    static void kill();

    /// Ask for downloading a soundfont
    void download(int id, QString soundfontName);

    /// Stop a download
    void cancel(int soundfontId);

signals:
    void progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName);
    void downloadCanceled(int soundfontId);

private slots:
    void fileDownloaded(QString error);
    void progressChanged(int percent);

private:
    DownloadManager();
    ~DownloadManager();
    void processOne();

    UrlReader * _reader;
    QList<int> _filesToDownload;
    QMutex _mutex;
    int _currentDownloadId;
    QMap<int, QString> _fileNames;

    static DownloadManager * s_instance;
};

#endif // DOWNLOADMANAGER_H
