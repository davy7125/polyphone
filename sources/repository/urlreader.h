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

#ifndef URLREADER_H
#define URLREADER_H

#include <QObject>
class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QMutex;
#include <QMap>

// Base class for downloading data
class UrlReader: public QObject
{
    Q_OBJECT

public:
    // Constructor, destructor
    explicit UrlReader(QString url);
    ~UrlReader();

    // Set the url
    void setUrl(QString url) { _url = url; }
    void clearArguments() { _arguments.clear(); }
    void addArgument(QString key, QString value);

    // Start the download. When this is finished, the signal "downloadCompleted" is emitted
    void download();

    // Stop the download
    void stop();

    // Get raw data
    QByteArray getRawData() { return _data; }

    QString getUrl() { return _url; }

signals:
    // Emitted when the download is complete
    // The download is successful when the error is empty
    void downloadCompleted(QString error);

    // Signal emitted when the download is going on
    void progressChanged(int percent);

protected:
    // This will be specific for each specialized url reader
    virtual void processData() {}

private slots:
    void fileDownloaded(QNetworkReply* pReply);
    void onTimeout();
    void downloadProgressed(qint64 bytesReceived, qint64 bytesTotal);

private:
    QString _url;
    QMap<QString, QString> _arguments;
    QNetworkAccessManager * _webCtrl;
    QByteArray _data;
    QTimer * _timer;
    QNetworkReply* _reply;
    bool _queryProcessed;
    QMutex * _mutex;

    static const int TIMEOUT_MS;
};

#endif // URLREADER_H
