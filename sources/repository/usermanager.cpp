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

#include "usermanager.h"
#include "contextmanager.h"
#include "repositorymanager.h"
#include "urlreaderjson.h"
#include <QUrl>

UserManager * UserManager::s_instance = nullptr;

UserManager * UserManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new UserManager();
    return s_instance;
}

void UserManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

UserManager::UserManager() : QObject(),
    _connectionState(DISCONNECTED),
    _userReaderJson(new UrlReaderJson(RepositoryManager::BASE_URL + "login"))
{
    connect(_userReaderJson, SIGNAL(downloadCompleted(QString)), this, SLOT(userDataAvailable(QString)));
}

UserManager::~UserManager()
{
    delete _userReaderJson;
}

void UserManager::login()
{
    // Get username / password
    QString username = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString();
    QString password = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString();
    if (username.isEmpty() || password.isEmpty())
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "auto_connect", false);
        _connectionState = DISCONNECTED;
        emit(connectionStateChanged(_connectionState));
        return;
    }

    // State is now pending
    _connectionState = PENDING;
    emit(connectionStateChanged(_connectionState));

    // Prepare the query for login
    _userReaderJson->clearArguments();
    _userReaderJson->addArgument("user", username);
    _userReaderJson->addArgument("pass", password);

    // Send the query
    _userReaderJson->download();
}

void UserManager::logout()
{
    _connectionState = DISCONNECTED;
    emit(connectionStateChanged(_connectionState));
}

UserManager::ConnectionState UserManager::getConnectionState()
{
    ConnectionState result;
    _mutex.lock();
    result = _connectionState;
    _mutex.unlock();

    return result;
}

QString UserManager::error()
{
    return _error;
}

void UserManager::userDataAvailable(QString error)
{
    if (error.isEmpty())
    {
        QJsonObject data = _userReaderJson->getData();
        if (data.contains("status"))
        {
            switch (data.value("status").toInt(-1))
            {
            case 0:
                _error = trUtf8("Server error");
                _connectionState = FAILED;
                break;
            case 1:
                _error = trUtf8("Wrong username or password");
                _connectionState = FAILED;

                // Stop the auto-connect
                ContextManager::configuration()->setValue(ConfManager::SECTION_REPOSITORY, "autoconnect", false);
                break;
            case 2:
                _error = "";
                if (data.contains("isPremium") && data.value("isPremium").toBool())
                    _connectionState = CONNECTED_PREMIUM;
                else
                    _connectionState = CONNECTED;
                if (data.contains("username"))
                    _username = data.value("username").toString();
                else
                    _username = "";
                break;
            case 3:
                _error = "";
                _connectionState = BANNED;
                break;
            default:
                _error = trUtf8("Server error") + QString(" (status '%1')").arg(data.value("status").toString());
                _connectionState = FAILED;
            }
        }
        else
        {
            _error = trUtf8("Server error");
            _connectionState = FAILED;
        }
    }
    else
    {
        _error = error;
        _connectionState = FAILED;
    }

    emit(connectionStateChanged(_connectionState));
}
