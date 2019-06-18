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
                error = trUtf8("Subscribe to a Premium account to get all the features!");
                break;
            case 2:
                error = trUtf8("Server error");
                break;
            default:
                error = trUtf8("Server error") + QString(" (status '%1')").arg(data.value("status").toString());
                break;
            }
        }
        else
        {
            error = trUtf8("Server error");
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
