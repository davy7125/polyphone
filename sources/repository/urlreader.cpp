#include "urlreader.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QMutex>

UrlReader::UrlReader(QString url) :
    _url(url),
    _webCtrl(new QNetworkAccessManager(this)),
    _timer(new QTimer(this)),
    _reply(NULL),
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
    delete _webCtrl;
    if (_reply != NULL)
        _reply->deleteLater();
}

void UrlReader::download()
{
    _queryProcessed = false;
    _reply = _webCtrl->get(QNetworkRequest(_url));
    _timer->start(5000);
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
