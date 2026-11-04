/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MCPSERVER_H
#define MCPSERVER_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QJsonObject>
class QTcpServer;
class QTcpSocket;

// Server of the Model Context Protocol (MCP), allowing an AI assistant to browse, edit and analyze the open soundfonts
// Transport: "Streamable HTTP" on the loopback interface only, JSON responses (no event streams)
// The requests are processed in the main thread, one at a time
class McpServer : public QObject
{
    Q_OBJECT

public:
    static McpServer * getInstance();
    static void kill();

    // Start or stop the server according to the configuration
    void applyConfiguration();

    bool isListening();
    quint16 getPort();
    QString getUrl();
    QString getError() { return _error; }

    static const quint16 DEFAULT_PORT;

signals:
    void stateChanged();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();
    void processPendingSockets();

private:
    struct HttpRequest
    {
        QByteArray method;
        QByteArray path;
        QMap<QByteArray, QByteArray> headers; // Lower case names
        QByteArray body;
    };

    McpServer();
    ~McpServer() override;
    void processSocket(QTcpSocket * socket);
    bool extractRequest(QByteArray &buffer, HttpRequest &request, bool &isValid);
    void processHttpRequest(QTcpSocket * socket, const HttpRequest &request);
    void sendResponse(QTcpSocket * socket, int status, QByteArray body = QByteArray(),
                      QByteArray extraHeaders = QByteArray(), bool close = false);
    static bool isLocalHost(QByteArray host);

    // JSON-RPC, return false if there is no response (notification or response from the client)
    bool processMessage(QJsonValue message, QJsonObject &response);
    QJsonObject processRequest(QString method, QJsonObject params, int &errorCode, QString &errorMessage);
    QJsonObject initialize(QJsonObject params);
    QJsonObject callFunction(QJsonObject params, int &errorCode, QString &errorMessage);

    static McpServer * s_instance;
    QTcpServer * _server;
    QMap<QTcpSocket *, QByteArray> _buffers;
    QList<QPointer<QTcpSocket> > _pendingSockets;
    bool _busy;
    QString _error;
};

#endif // MCPSERVER_H
