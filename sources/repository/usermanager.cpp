#include "usermanager.h"
#include "contextmanager.h"

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
    _connectionState(DISCONNECTED)
{

}

UserManager::~UserManager()
{

}

void UserManager::login()
{
    // Get username / password
    QString username = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString();
    QString hashedPassword = ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString();
    if (username.isEmpty() || hashedPassword.isEmpty())
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
