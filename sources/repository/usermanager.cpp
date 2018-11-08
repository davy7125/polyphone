#include "usermanager.h"

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
    _connectionState = CONNECTED_PREMIUM;
    emit(connectionStateChanged(_connectionState));
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
