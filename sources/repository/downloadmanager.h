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

signals:
    void progressChanged(int percent, QString soundfontName, QString finalFileName);

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
    int _currentDownload;
    QMap<int, QString> _fileNames;

    static DownloadManager * s_instance;
};

#endif // DOWNLOADMANAGER_H
