#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    static DownloadManager * getInstance();
    static void kill();

    /// Ask for downloading a soundfont
    void download(int id);

private:
    DownloadManager();
    ~DownloadManager();

    static DownloadManager * s_instance;

};

#endif // DOWNLOADMANAGER_H
