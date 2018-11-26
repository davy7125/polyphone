#include "urlreader.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QMutex>
#include <QUrlQuery>

const int UrlReader::TIMEOUT_MS = 5000;

UrlReader::UrlReader(QString url) :
    _url(url),
    _webCtrl(new QNetworkAccessManager(this)),
    _timer(new QTimer(this)),
    _reply(nullptr),
    _mutex(new QMutex())
{
    // Connect the network access manager
    connect(_webCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)), Qt::DirectConnection);

    // Prepare the timer
    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    _timer->setSingleShot(true);
}

UrlReader::~UrlReader()
{
    _timer->stop();
    delete _timer;
    if (_reply != nullptr)
        _reply->deleteLater();
    delete _webCtrl;
    delete _mutex;
}

void UrlReader::addArgument(QString key, QString value)
{
    _arguments[key] = QUrl::toPercentEncoding(value);
}

void UrlReader::download()
{
    _queryProcessed = false;

    // Prepare the url
    QUrl url(_url);

    QUrlQuery query;
    foreach (QString key, _arguments.keys())
        query.addQueryItem(key, _arguments[key]);
    url.setQuery(query.query());

    // Send the query
    if (_reply != nullptr)
        _reply->deleteLater();
    _reply = _webCtrl->get(QNetworkRequest(url));
    connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgressed(qint64, qint64)));
    _timer->start(TIMEOUT_MS);
}

void UrlReader::fileDownloaded(QNetworkReply * pReply)
{
    Q_UNUSED(pReply)

    // Return if already processed by the timer
    _mutex->lock();
    if (_queryProcessed)
    {
        _mutex->unlock();
        return;
    }
    _queryProcessed = true;
    _mutex->unlock();

    QString error = "";
    _timer->stop();

    if (_reply->error() == 0)
    {
        int v = _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (v >= 200 && v < 300)
        {
            // Downloaded data
            _data = _reply->readAll();

            // Process data
            this->processData();
        }
        else
        {
            error = "Error " + QString::number(v);
        }
    }
    else
    {
        error = _reply->errorString();
    }

    // Emit a signal
    emit(downloadCompleted(error));
}

void UrlReader::onTimeout()
{
    // Return if already processed by the query
    _mutex->lock();
    if (_queryProcessed)
    {
        _mutex->unlock();
        return;
    }
    _queryProcessed = true;
    _mutex->unlock();

    // Timeout
    disconnect(_webCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
    _reply->abort();

    emit(downloadCompleted("timeout"));
}

void UrlReader::downloadProgressed(qint64 bytesReceived, qint64 bytesTotal)
{
    _timer->start(TIMEOUT_MS);
    if (bytesTotal != 0)
        emit(progressChanged(100 * bytesReceived / bytesTotal));
}
