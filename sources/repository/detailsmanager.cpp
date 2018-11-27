#include "detailsmanager.h"
#include "soundfontdetails.h"
#include "urlreaderjson.h"
#include "repositorymanager.h"
#include "contextmanager.h"
#include "usermanager.h"
#include <QVariant>
#include <QUrl>

DetailsManager * DetailsManager::s_instance = nullptr;


DetailsManager * DetailsManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new DetailsManager();
    return s_instance;
}

void DetailsManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

DetailsManager::DetailsManager() :
    _urlReaderDetails(new UrlReaderJson(RepositoryManager::BASE_URL + "details")),
    _currentReaderDetailId(-1)
{
    connect(_urlReaderDetails, SIGNAL(downloadCompleted(QString)), this, SLOT(downloadCompleted(QString)), Qt::QueuedConnection);
}

DetailsManager::~DetailsManager()
{
    _mutex.lock();
    QList<int> keys = _soundfontDetails.keys();
    foreach (int key, keys)
    {
        delete _soundfontDetails.take(key);
    }
    delete _urlReaderDetails;
    _mutex.unlock();
}

void DetailsManager::askForSoundfontDetails(int id)
{
    _mutex.lock();
    if (!_stack.contains(id))
        _stack << id;
    _mutex.unlock();

    this->run();
}

void DetailsManager::run()
{
    _mutex.lock();

    // Remove from the stack all ids that are already available
    QList<int> availableIds;
    for (int i = _stack.count() - 1; i >= 0; i--)
        if (_soundfontDetails.contains(_stack[i]))
            availableIds << _stack.takeAt(i);

    // Send a query if something is to be processed and if no urlReaders are in use
    if (!_stack.isEmpty() && _currentReaderDetailId == -1)
    {
        _currentReaderDetailId = _stack.takeLast();
        _urlReaderDetails->clearArguments();
        _urlReaderDetails->addArgument("id", QString::number(_currentReaderDetailId));
        if (UserManager::getInstance()->getConnectionState() == UserManager::CONNECTED_PREMIUM)
        {
            _urlReaderDetails->addArgument("user", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "username", "").toString());
            _urlReaderDetails->addArgument("pass", ContextManager::configuration()->getValue(ConfManager::SECTION_REPOSITORY, "password", "").toString());
        }
        _urlReaderDetails->download();
    }

    _mutex.unlock();

    foreach (int availableId, availableIds)
        emit(detailsReady(availableId));
}

void DetailsManager::downloadCompleted(QString error)
{
    _mutex.lock();

    // Get the current id while the mutex is locked
    int currentId = _currentReaderDetailId;
    _currentReaderDetailId = -1;

    // Check if there is an error
    if (error.isEmpty() && !_urlReaderDetails->getErrorCode().isEmpty())
        error = _urlReaderDetails->getErrorMessage() + " (" + _urlReaderDetails->getErrorCode() + ")";

    // If success, process data
    if (error.isEmpty())
    {
        QJsonObject data = _urlReaderDetails->getData();
        if (data.contains("status"))
        {
            switch (data.value("status").toInt(-1))
            {
            case 0:
                _soundfontDetails[currentId] = new SoundfontDetails(_urlReaderDetails->getData());
                break;
            case 1:
                error = trUtf8("Souscrivez à un compte Premium pour bénéficier de toutes les fonctionnalités !");
                break;
            case 2:
                error = trUtf8("Problème serveur");
                break;
            default:
                error = trUtf8("Problème serveur") + QString(" (status '%1')").arg(data.value("status").toString());
                break;
            }
        }
        else
        {
            error = trUtf8("Problème serveur");
        }
    }

    _mutex.unlock();

    // Notify the details are ready
    if (error.isEmpty())
        emit(detailsReady(currentId));
    else
        emit(detailsFailed(currentId, error));

    // Check if something else is to be processed
    this->run();
}

SoundfontDetails * DetailsManager::getDetails(int id)
{
    SoundfontDetails * result = nullptr;
    _mutex.lock();
    if (_soundfontDetails.contains(id))
        result = _soundfontDetails[id];
    _mutex.unlock();

    return result;
}
