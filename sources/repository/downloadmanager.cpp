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

#include "downloadmanager.h"
#include "repositorymanager.h"
#include "urlreader.h"
#include "contextmanager.h"
#include "usermanager.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>

DownloadManager * DownloadManager::s_instance = nullptr;

DownloadManager * DownloadManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new DownloadManager();
    return s_instance;
}

void DownloadManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

DownloadManager::DownloadManager() : QObject(),
    _reader(new UrlReader(RepositoryManager::BASE_URL + "download")),
    _currentDownloadId(-1)
{
    // Downloader
    connect(_reader, SIGNAL(downloadCompleted(QString)), this, SLOT(fileDownloaded(QString)), Qt::QueuedConnection);
    connect(_reader, SIGNAL(progressChanged(int)), this, SLOT(progressChanged(int)));
}

DownloadManager::~DownloadManager()
{
    delete _reader;
}

void DownloadManager::download(int id, QString soundfontName)
{
    // Add a url to download
    _mutex.lock();
    _fileNames[id] = soundfontName;
    if (!_filesToDownload.contains(id))
    {
        _filesToDownload << id;
        emit(progressChanged(0, id, soundfontName, ""));
    }
    _mutex.unlock();

    processOne();
}

void DownloadManager::processOne()
{
    QMutexLocker locker(&_mutex);
    if (_currentDownloadId != -1 || _filesToDownload.empty())
        return;
    _currentDownloadId = _filesToDownload.takeFirst();

    // Build a url to download
    _reader->clearArguments();
    _reader->addArgument("id", QString::number(_currentDownloadId));
    if (UserManager::getInstance()->getConnectionState() == UserManager::CONNECTED_PREMIUM)
    {
        _reader->addArgument("user", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString());
        _reader->addArgument("pass", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString());
    }
    _reader->download();
}

void DownloadManager::fileDownloaded(QString error)
{
    _mutex.lock();
    if (_currentDownloadId == -1)
    {
        _mutex.unlock();
        return;
    }

    QString currentDownload = _fileNames.contains(_currentDownloadId) ? _fileNames[_currentDownloadId] : trUtf8("untitled");
    if (error.isEmpty())
    {
        // Download directory
        QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        if (downloadDir.isEmpty())
            downloadDir = QDir::homePath();
        downloadDir = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "directory", downloadDir).toString();

        // Find an available file name
        QFileInfo info(downloadDir + "/" + currentDownload);
        QString extension = info.suffix();
        QString pathWithoutExtension = info.dir().path() + "/" + info.completeBaseName();
        if (QFile(pathWithoutExtension + "." + extension).exists())
        {
            int suffix = 1;
            while (QFile(pathWithoutExtension + "-" + QString::number(suffix) + "." + extension).exists())
            {
                suffix++;
            }
            pathWithoutExtension += "-" + QString::number(suffix);
        }

        // Save the file
        QFile file(pathWithoutExtension + "." + extension);
        file.open(QIODevice::WriteOnly);
        file.write(_reader->getRawData());
        file.close();

        // The download is complete, notify it
        emit(progressChanged(100, _currentDownloadId, currentDownload, pathWithoutExtension + "." + extension));
    }
    else
    {
        QMessageBox::warning(QApplication::activeWindow(), trUtf8("Warning"),
                             trUtf8("Couldn't download file \"%1\": %2").arg(currentDownload)
                             .arg(error));
    }
    _currentDownloadId = -1;
    _mutex.unlock();

    // Download the next file
    this->processOne();
}

void DownloadManager::progressChanged(int percent)
{
    _mutex.lock();
    QString currentDownload = _fileNames.contains(_currentDownloadId) ? _fileNames[_currentDownloadId] : trUtf8("untitled");
    _mutex.unlock();

    if (_currentDownloadId != -1)
        emit(progressChanged(percent, _currentDownloadId, currentDownload, ""));
}

void DownloadManager::cancel(int soundfontId)
{
    _mutex.lock();
    _filesToDownload.removeAll(soundfontId);
    _fileNames.remove(soundfontId);
    if (_currentDownloadId == soundfontId)
    {
        _currentDownloadId = -1;
        _mutex.unlock();

        _reader->stop();
        processOne();
    }
    else
        _mutex.unlock();
    emit(downloadCanceled(soundfontId));
}
