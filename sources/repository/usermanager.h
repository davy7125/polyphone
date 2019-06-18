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

#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QMutex>
class UrlReaderJson;

class UserManager: public QObject
{
    Q_OBJECT

public:
    enum ConnectionState
    {
        DISCONNECTED,
        PENDING,
        CONNECTED,
        CONNECTED_PREMIUM,
        BANNED,
        FAILED
    };

    // Singleton: get the instance of the object and finally kill it
    static UserManager * getInstance();
    static void kill();

    /// Initialize the connection
    void login();

    /// Cut the connection
    void logout();

    /// Get the current state
    ConnectionState getConnectionState();

    /// Get the error if the state is FAILED
    QString error();

    /// Get the current user name
    QString getUsername() { return _username; }

signals:
    void connectionStateChanged(UserManager::ConnectionState state);

private slots:
    void userDataAvailable(QString error);

private:
    UserManager();
    ~UserManager();

    ConnectionState _connectionState;
    QString _error;
    QMutex _mutex;
    UrlReaderJson * _userReaderJson;
    QString _username;
    static UserManager * s_instance;
};

#endif // USERMANAGER_H
