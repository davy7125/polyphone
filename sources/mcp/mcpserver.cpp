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

#include "mcpserver.h"
#include "mcpfunctions.h"
#include "contextmanager.h"
#include "soundfontmanager.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QUrl>

McpServer * McpServer::s_instance = nullptr;
const quint16 McpServer::DEFAULT_PORT = 8642;

static const int MAX_BODY_SIZE = 32 * 1024 * 1024;
static const char * INSTRUCTIONS =
    "Polyphone is a soundfont editor (sf2, sf3, also opening sfz, dls, GrandOrgue and sfArk files). This server gives "
    "access to the soundfonts open in Polyphone: what you change is immediately visible in the editor.\n"
    "Structure: a soundfont contains samples (audio data with a root key and loop points), instruments (divisions "
    "linking a sample to key / velocity ranges with parameters) and presets (divisions linking instruments, with "
    "parameters that are added to or multiply the instrument values). Instruments and presets also have global "
    "parameters, applied to the divisions that don't define them.\n"
    "Elements are identified by strings: \"sf0\", \"sf0/smpl3\", \"sf0/inst2\", \"sf0/inst2/div5\", \"sf0/prst1\", "
    "\"sf0/prst1/div0\", \"sf0/inst2/mod1\". Keys are MIDI numbers or note names, 60 = \"C4\". Parameters are named "
    "in real units (seconds, dB, Hz, cents): call list_parameters once to know them.\n"
    "Start with get_selection (what the user is looking at) or list_soundfonts, then get_soundfont / get_element. "
    "Each call changing something is one undoable step (undo / redo functions, also Ctrl+Z in Polyphone). Nothing is "
    "written on the disk until save_soundfont is called: save only if the user asks for it.\n"
    "You cannot hear: use the analysis functions (analyze_sample, analyze_instrument, render_note, compare_samples, "
    "get_spectrum, get_loop_analysis) to measure the sound before and after an edit, and to check the consistency of "
    "the samples across the keyboard. Prefer gradual changes verified by measures.";

McpServer * McpServer::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new McpServer();
    return s_instance;
}

void McpServer::kill()
{
    delete s_instance;
    s_instance = nullptr;
}

McpServer::McpServer() : QObject(),
    _server(new QTcpServer(this)),
    _busy(false)
{
    connect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

McpServer::~McpServer()
{
    _server->close();
}

void McpServer::applyConfiguration()
{
    bool enabled = ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "enabled", false).toBool();
    quint16 port = static_cast<quint16>(
        ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "port", DEFAULT_PORT).toUInt());

    if (!enabled)
    {
        _server->close();
        _error.clear();
    }
    else if (!_server->isListening() || _server->serverPort() != port)
    {
        _server->close();
        if (_server->listen(QHostAddress::LocalHost, port))
            _error.clear();
        else
            _error = _server->errorString();
    }

    emit stateChanged();
}

bool McpServer::isListening()
{
    return _server->isListening();
}

quint16 McpServer::getPort()
{
    return _server->isListening() ? _server->serverPort() : static_cast<quint16>(
        ContextManager::configuration()->getValue(ConfManager::SECTION_MCP, "port", DEFAULT_PORT).toUInt());
}

QString McpServer::getUrl()
{
    return QString("http://127.0.0.1:%1/mcp").arg(getPort());
}

void McpServer::onNewConnection()
{
    while (_server->hasPendingConnections())
    {
        QTcpSocket * socket = _server->nextPendingConnection();
        _buffers[socket] = QByteArray();
        connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    }
}

void McpServer::onDisconnected()
{
    QTcpSocket * socket = dynamic_cast<QTcpSocket *>(QObject::sender());
    if (socket == nullptr)
        return;
    _buffers.remove(socket);
    socket->deleteLater();
}

void McpServer::onReadyRead()
{
    QTcpSocket * socket = dynamic_cast<QTcpSocket *>(QObject::sender());
    if (socket == nullptr)
        return;

    // A request may arrive while another one is processed (dialog with its own event loop for instance)
    if (_busy)
    {
        if (!_pendingSockets.contains(socket))
            _pendingSockets << socket;
        return;
    }

    processSocket(socket);
}

void McpServer::processPendingSockets()
{
    while (!_pendingSockets.isEmpty() && !_busy)
    {
        QPointer<QTcpSocket> socket = _pendingSockets.takeFirst();
        if (socket != nullptr)
            processSocket(socket);
    }
}

void McpServer::processSocket(QTcpSocket * socket)
{
    if (!_buffers.contains(socket))
        return;
    _buffers[socket].append(socket->readAll());
    if (_buffers[socket].size() > MAX_BODY_SIZE + 65536)
    {
        sendResponse(socket, 413, QByteArray(), QByteArray(), true);
        return;
    }

    // Process all complete requests
    _busy = true;
    HttpRequest request;
    bool isValid;
    while (_buffers.contains(socket) && extractRequest(_buffers[socket], request, isValid))
    {
        if (isValid)
            processHttpRequest(socket, request);
        else
        {
            sendResponse(socket, 400, QByteArray(), QByteArray(), true);
            break;
        }
    }
    _busy = false;

    if (!_pendingSockets.isEmpty())
        QTimer::singleShot(0, this, SLOT(processPendingSockets()));
}

bool McpServer::extractRequest(QByteArray &buffer, HttpRequest &request, bool &isValid)
{
    isValid = true;
    int headerEnd = buffer.indexOf("\r\n\r\n");
    if (headerEnd < 0)
    {
        isValid = buffer.size() < 65536;
        return !isValid;
    }

    // Request line and headers
    QList<QByteArray> lines = buffer.left(headerEnd).split('\n');
    QList<QByteArray> requestLine = lines.takeFirst().trimmed().split(' ');
    if (requestLine.count() != 3)
    {
        isValid = false;
        return true;
    }
    request.method = requestLine[0];
    request.path = requestLine[1];
    request.headers.clear();
    foreach (QByteArray line, lines)
    {
        int colon = line.indexOf(':');
        if (colon > 0)
            request.headers[line.left(colon).trimmed().toLower()] = line.mid(colon + 1).trimmed();
    }

    // Body
    int bodyStart = headerEnd + 4;
    if (request.headers.value("transfer-encoding").toLower().contains("chunked"))
    {
        QByteArray body;
        int position = bodyStart;
        while (true)
        {
            int lineEnd = buffer.indexOf("\r\n", position);
            if (lineEnd < 0)
                return false; // Incomplete
            bool ok;
            int chunkSize = buffer.mid(position, lineEnd - position).split(';')[0].trimmed().toInt(&ok, 16);
            if (!ok || chunkSize < 0 || body.size() + chunkSize > MAX_BODY_SIZE)
            {
                isValid = false;
                return true;
            }
            if (chunkSize == 0)
            {
                int end = buffer.indexOf("\r\n\r\n", lineEnd);
                if (end < 0)
                    return false; // Trailer incomplete
                request.body = body;
                buffer.remove(0, end + 4);
                return true;
            }
            if (buffer.size() < lineEnd + 2 + chunkSize + 2)
                return false; // Incomplete
            body.append(buffer.mid(lineEnd + 2, chunkSize));
            position = lineEnd + 2 + chunkSize + 2;
        }
    }

    bool ok = true;
    int contentLength = request.headers.contains("content-length") ? request.headers["content-length"].toInt(&ok) : 0;
    if (!ok || contentLength < 0 || contentLength > MAX_BODY_SIZE)
    {
        isValid = false;
        return true;
    }
    if (buffer.size() < bodyStart + contentLength)
        return false; // Incomplete
    request.body = buffer.mid(bodyStart, contentLength);
    buffer.remove(0, bodyStart + contentLength);
    return true;
}

bool McpServer::isLocalHost(QByteArray host)
{
    // Host without the port
    QString hostName = QString::fromLatin1(host).toLower();
    if (hostName.startsWith("["))
        hostName = hostName.mid(1, hostName.indexOf(']') - 1);
    else if (hostName.contains(':'))
        hostName = hostName.left(hostName.indexOf(':'));
    return hostName == "127.0.0.1" || hostName == "localhost" || hostName == "::1";
}

void McpServer::processHttpRequest(QTcpSocket * socket, const HttpRequest &request)
{
    bool close = request.headers.value("connection").toLower() == "close";

    // Protection against DNS rebinding: a web page cannot use the server
    QByteArray origin = request.headers.value("origin");
    if (!isLocalHost(request.headers.value("host")) ||
        (!origin.isEmpty() && origin != "null" && !isLocalHost(QUrl(QString::fromLatin1(origin)).host().toLatin1())))
    {
        sendResponse(socket, 403, QByteArray(), QByteArray(), close);
        return;
    }

    QByteArray path = request.path.split('?')[0];
    if (path != "/mcp" && path != "/mcp/")
    {
        sendResponse(socket, 404, QByteArray(), QByteArray(), close);
        return;
    }
    if (request.method != "POST")
    {
        // No event stream for messages sent by the server, no sessions
        sendResponse(socket, 405, QByteArray(), "Allow: POST\r\n", close);
        return;
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(request.body, &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        QJsonObject error;
        error["code"] = -32700;
        error["message"] = "Parse error: " + parseError.errorString();
        QJsonObject response;
        response["jsonrpc"] = "2.0";
        response["id"] = QJsonValue();
        response["error"] = error;
        sendResponse(socket, 400, QJsonDocument(response).toJson(QJsonDocument::Compact), QByteArray(), close);
        return;
    }

    // One message or a batch
    QJsonArray responses;
    QJsonObject response;
    if (document.isArray())
    {
        for (const QJsonValue message : document.array())
            if (processMessage(message, response))
                responses.append(response);
    }
    else if (processMessage(document.object(), response))
        responses.append(response);

    if (responses.isEmpty())
        sendResponse(socket, 202, QByteArray(), QByteArray(), close);
    else
        sendResponse(socket, 200, document.isArray() ? QJsonDocument(responses).toJson(QJsonDocument::Compact) :
                                       QJsonDocument(responses[0].toObject()).toJson(QJsonDocument::Compact),
                     QByteArray(), close);
}

void McpServer::sendResponse(QTcpSocket * socket, int status, QByteArray body, QByteArray extraHeaders, bool close)
{
    QByteArray reason;
    switch (status)
    {
    case 200: reason = "OK"; break;
    case 202: reason = "Accepted"; break;
    case 400: reason = "Bad Request"; break;
    case 403: reason = "Forbidden"; break;
    case 404: reason = "Not Found"; break;
    case 405: reason = "Method Not Allowed"; break;
    case 413: reason = "Payload Too Large"; break;
    default: reason = "Error"; break;
    }

    QByteArray response = "HTTP/1.1 " + QByteArray::number(status) + " " + reason + "\r\n";
    if (!body.isEmpty())
        response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    response += extraHeaders;
    if (close)
        response += "Connection: close\r\n";
    response += "\r\n" + body;
    socket->write(response);
    if (close)
    {
        socket->disconnectFromHost();
        _buffers.remove(socket);
    }
}

bool McpServer::processMessage(QJsonValue message, QJsonObject &response)
{
    QJsonObject object = message.toObject();
    response = QJsonObject();
    response["jsonrpc"] = "2.0";

    if (!message.isObject() || object.value("jsonrpc").toString() != "2.0" || !object.value("method").isString())
    {
        // Possibly a response of the client (not expected since the server sends no requests)
        if (message.isObject() && !object.contains("method") && (object.contains("result") || object.contains("error")))
            return false;

        QJsonObject error;
        error["code"] = -32600;
        error["message"] = "Invalid request";
        response["id"] = object.contains("id") ? object.value("id") : QJsonValue();
        response["error"] = error;
        return true;
    }

    // Notifications have no responses
    if (!object.contains("id"))
        return false;
    response["id"] = object.value("id");

    int errorCode = 0;
    QString errorMessage;
    QJsonObject result = processRequest(object.value("method").toString(), object.value("params").toObject(),
                                        errorCode, errorMessage);
    if (errorCode != 0)
    {
        QJsonObject error;
        error["code"] = errorCode;
        error["message"] = errorMessage;
        response["error"] = error;
    }
    else
        response["result"] = result;
    return true;
}

QJsonObject McpServer::processRequest(QString method, QJsonObject params, int &errorCode, QString &errorMessage)
{
    if (method == "initialize")
        return initialize(params);
    if (method == "ping")
        return QJsonObject();
    if (method == "tools/list")
    {
        QJsonArray tools;
        foreach (const McpFunction * function, McpFunctions::getAll())
            tools.append(function->getDefinition());
        QJsonObject result;
        result["tools"] = tools;
        return result;
    }
    if (method == "tools/call")
        return callFunction(params, errorCode, errorMessage);

    errorCode = -32601;
    errorMessage = "Method not found: " + method;
    return QJsonObject();
}

QJsonObject McpServer::initialize(QJsonObject params)
{
    // Same version as the client if known, otherwise the most recent one (the client decides)
    static const QStringList supportedVersions = QStringList() << "2025-11-25" << "2025-06-18" << "2025-03-26" << "2024-11-05";
    QString version = params.value("protocolVersion").toString();
    if (!supportedVersions.contains(version))
        version = supportedVersions[0];

    QJsonObject tools;
    tools["listChanged"] = false;
    QJsonObject capabilities;
    capabilities["tools"] = tools;

    QJsonObject serverInfo;
    serverInfo["name"] = "polyphone";
    serverInfo["title"] = "Polyphone";
    serverInfo["version"] = SOFT_VERSION;

    QJsonObject result;
    result["protocolVersion"] = version;
    result["capabilities"] = capabilities;
    result["serverInfo"] = serverInfo;
    result["instructions"] = INSTRUCTIONS;
    return result;
}

QJsonObject McpServer::callFunction(QJsonObject params, int &errorCode, QString &errorMessage)
{
    QString name = params.value("name").toString();
    const McpFunction * function = McpFunctions::get(name);
    if (function == nullptr)
    {
        errorCode = -32602;
        errorMessage = "Unknown tool: " + name;
        return QJsonObject();
    }

    // Execution, the changes being one undoable step (all changes being canceled in case of error)
    SoundfontManager * sm = SoundfontManager::getInstance();
    McpResult functionResult;
    QString error;
    try
    {
        functionResult = function->call(McpArguments(params.value("arguments").toObject()));
    }
    catch (const McpException &exception)
    {
        error = exception.message();
    }
    catch (const std::exception &exception)
    {
        error = QString("internal error: ") + exception.what();
    }
    catch (...)
    {
        error = "internal error";
    }

    if (!function->isReadOnly())
    {
        if (error.isEmpty())
            sm->endEditing("mcp:" + name);
        else
            sm->revertNewEditing();
    }

    QJsonArray content;
    QJsonObject textContent;
    textContent["type"] = "text";
    if (!error.isEmpty())
        textContent["text"] = "Error: " + error;
    else if (functionResult.data.isString())
        textContent["text"] = functionResult.data.toString();
    else if (functionResult.data.isObject())
        textContent["text"] = QString::fromUtf8(QJsonDocument(functionResult.data.toObject()).toJson(QJsonDocument::Compact));
    else if (functionResult.data.isArray())
        textContent["text"] = QString::fromUtf8(QJsonDocument(functionResult.data.toArray()).toJson(QJsonDocument::Compact));
    else
        textContent["text"] = "Done.";
    content.append(textContent);

    if (error.isEmpty())
    {
        foreach (QByteArray png, functionResult.pngImages)
        {
            QJsonObject imageContent;
            imageContent["type"] = "image";
            imageContent["mimeType"] = "image/png";
            imageContent["data"] = QString::fromLatin1(png.toBase64());
            content.append(imageContent);
        }
    }

    QJsonObject result;
    result["content"] = content;
    result["isError"] = !error.isEmpty();
    return result;
}
