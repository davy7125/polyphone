#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QMutex>

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

signals:
    void connectionStateChanged(UserManager::ConnectionState state);

private:
    UserManager();
    ~UserManager();

    ConnectionState _connectionState;
    QString _error;
    QMutex _mutex;
    static UserManager * s_instance;
};

#endif // USERMANAGER_H
