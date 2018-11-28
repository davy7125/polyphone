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
#include <QDebug>

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
    _currentDownload(-1)
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
    // Login / password
    QString user = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString();
    QString pass = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString();

    // Add a url to download
    _mutex.lock();
    _fileNames[id] = soundfontName;
    if (!_filesToDownload.contains(id))
        _filesToDownload << id;
    _mutex.unlock();

    processOne();
}

void DownloadManager::processOne()
{
    QMutexLocker locker(&_mutex);
    if (_currentDownload != -1 || _filesToDownload.empty())
        return;
    _currentDownload = _filesToDownload.takeFirst();

    // Build a url to download
    _reader->clearArguments();
    _reader->addArgument("id", QString::number(_currentDownload));
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
    QString currentDownload = _fileNames.contains(_currentDownload) ? _fileNames[_currentDownload] : trUtf8("untitled");
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
        emit(progressChanged(100, currentDownload, pathWithoutExtension + "." + extension));
    }
    else
    {
        QMessageBox::warning(QApplication::activeWindow(), trUtf8("Warning"),
                             trUtf8("Couldn't download file \"%1\": %2").arg(currentDownload)
                             .arg(error));
    }
    _currentDownload = -1;
    _mutex.unlock();

    // Download the next file
    this->processOne();
}

void DownloadManager::progressChanged(int percent)
{
    _mutex.lock();
    QString currentDownload = _fileNames.contains(_currentDownload) ? _fileNames[_currentDownload] : trUtf8("untitled");
    _mutex.unlock();

    emit(progressChanged(percent, currentDownload, ""));
}
