#ifndef URLREADER_H
#define URLREADER_H

#include <QObject>
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QMutex;

// Base class for downloading data
class UrlReader: public QObject
{
    Q_OBJECT

public:
    // Constructor, destructor
    explicit UrlReader(QString url);
    ~UrlReader();

    // Start the download. When this is finished, the signal "downloaded"
    void download();

    // Get raw data
    QByteArray getRawData() { return _data; }

signals:
    // Emitted when the download is complete
    // The download is successful when the error is empty
    void downloadCompleted(QString error);

protected:
    // This will be specific for each specialized url reader
    virtual void processData() {}

private slots:
    void fileDownloaded(QNetworkReply* pReply);
    void onTimeout();

private:
    QString _url;
    QNetworkAccessManager * _webCtrl;
    QByteArray _data;
    QTimer * _timer;
    QNetworkReply* _reply;
    bool _queryProcessed;
    QMutex * _mutex;
};

#endif // URLREADER_H
