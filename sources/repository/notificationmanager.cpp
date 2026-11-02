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

#include "notificationmanager.h"
#include "urlreaderjson.h"
#include "usermanager.h"
#include "contextmanager.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDate>
#include <QLocale>
#include <QRegularExpression>

//const QString NotificationManager::WEBSITE_URL = "http://www.polyphone.local";
const QString NotificationManager::WEBSITE_URL = "https://www.polyphone.io";
const int NotificationManager::DAYS_BETWEEN_NOTIFICATIONS = 7;
NotificationManager * NotificationManager::s_instance = nullptr;

NotificationManager * NotificationManager::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new NotificationManager();
    return s_instance;
}

void NotificationManager::kill()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

NotificationManager::NotificationManager() : QObject(),
    _urlReaderJson(new UrlReaderJson(WEBSITE_URL + "/api/notifications/list")),
    _downloadStarted(false),
    _currentIndex(-1)
{
    connect(_urlReaderJson, SIGNAL(downloadCompleted(QString)), this, SLOT(notificationListAvailable(QString)));

    // An appeal is not shown to a Premium member
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)), this, SLOT(updateCurrentNotification()));
}

NotificationManager::~NotificationManager()
{
    delete _urlReaderJson;
}

void NotificationManager::initialize()
{
    // Days on which the software has been opened, for the appeals
    QString today = QDate::currentDate().toString(Qt::ISODate);
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "last_day_used", "").toString() != today)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_NOTIFICATIONS, "days_used",
                                                  ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "days_used", 0).toInt() + 1);
        ContextManager::configuration()->setValue(ConfManager::SECTION_NOTIFICATIONS, "last_day_used", today);
    }

    if (isEnabled())
        download();
}

bool NotificationManager::isEnabled()
{
    return ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "enabled", true).toBool();
}

void NotificationManager::setEnabled(bool isEnabled)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_NOTIFICATIONS, "enabled", isEnabled);
    if (isEnabled)
        download();
    updateCurrentNotification();
}

const NotificationManager::Notification * NotificationManager::getCurrentNotification()
{
    return _currentIndex >= 0 ? &_notifications.at(_currentIndex) : nullptr;
}

void NotificationManager::closeCurrentNotification()
{
    if (_currentIndex < 0)
        return;

    QStringList closedIds = ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "closed_ids", QStringList()).toStringList();
    closedIds << QString::number(_notifications.at(_currentIndex).id);
    ContextManager::configuration()->setValue(ConfManager::SECTION_NOTIFICATIONS, "closed_ids", closedIds);

    // Start of the delay before the next notification
    ContextManager::configuration()->setValue(ConfManager::SECTION_NOTIFICATIONS, "last_closing", QDate::currentDate().toString(Qt::ISODate));

    updateCurrentNotification();
}

void NotificationManager::download()
{
    // Once per session
    if (_downloadStarted)
        return;
    _downloadStarted = true;

    // Nothing else than the language is sent (the texts are in English if the website doesn't have it)
    _urlReaderJson->clearArguments();
    _urlReaderJson->addArgument("language", QLocale().name().section('_', 0, 0));
    _urlReaderJson->download();
}

void NotificationManager::notificationListAvailable(QString error)
{
    // Silent failure: no notifications
    if (!error.isEmpty() || !_urlReaderJson->isValid())
        return;

    QList<Notification> notifications;
    foreach (QJsonValue value, _urlReaderJson->getData().value("notifications").toArray())
    {
        QJsonObject object = value.toObject();
        Notification notification;
        notification.id = object.value("id").toInt(-1);
        notification.title = object.value("title").toString();
        if (notification.id < 0 || notification.title.isEmpty())
            continue;

        // A type unknown by this version is ignored
        QString type = object.value("type").toString();
        if (type == "alert")
            notification.type = TYPE_ALERT;
        else if (type == "update")
            notification.type = TYPE_UPDATE;
        else if (type == "announcement")
            notification.type = TYPE_ANNOUNCEMENT;
        else if (type == "appeal")
            notification.type = TYPE_APPEAL;
        else
            continue;

        notification.text = object.value("text").toString();
        notification.buttonLabel = object.value("button_label").toString();
        notification.minDaysUsed = object.value("min_days_used").toInt(0);
        if (!readVersion(object.value("version_from").toString(), notification.versionFrom) ||
            !readVersion(object.value("version_below").toString(), notification.versionBelow))
            continue;

        // The link can only be a page of the website
        QString path = object.value("url").toString();
        if (path.startsWith("/") && !path.startsWith("//") && !path.startsWith("/\\"))
            notification.url = WEBSITE_URL + path;

        notifications << notification;
    }

    // Priority of the types, then the most recent first
    std::sort(notifications.begin(), notifications.end(), [](const Notification &a, const Notification &b) {
        return a.type != b.type ? a.type < b.type : a.id > b.id;
    });
    _notifications = notifications;

    updateCurrentNotification();
}

bool NotificationManager::readVersion(QString text, QVersionNumber &version)
{
    // An empty text is no condition
    version = QVersionNumber();
    if (text.isEmpty())
        return true;

    if (!QRegularExpression("^\\d+(\\.\\d+)*$").match(text).hasMatch())
        return false;
    version = QVersionNumber::fromString(text);
    return true;
}

bool NotificationManager::isApplicable(const Notification &notification)
{
    QVersionNumber currentVersion = QVersionNumber::fromString(SOFT_VERSION).normalized();
    bool isInVersionRange =
            (notification.versionFrom.isNull() || QVersionNumber::compare(currentVersion, notification.versionFrom.normalized()) >= 0) &&
            (notification.versionBelow.isNull() || QVersionNumber::compare(currentVersion, notification.versionBelow.normalized()) < 0);

    switch (notification.type)
    {
    case TYPE_ALERT: case TYPE_ANNOUNCEMENT:
        return isInVersionRange;
    case TYPE_UPDATE:
#ifdef OFFICIAL_BUILD
        return !notification.versionBelow.isNull() && isInVersionRange;
#else
        // A software installed from the package of a distribution is updated by the distribution
        return false;
#endif
    case TYPE_APPEAL: {
        // Never for a Premium member, so the result of a connection in progress is awaited
        UserManager::ConnectionState state = UserManager::getInstance()->getConnectionState();
        if (state == UserManager::PENDING || state == UserManager::CONNECTED_PREMIUM || state == UserManager::CONNECTED_ADMIN)
            return false;
        return ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "days_used", 0).toInt() >= notification.minDaysUsed;
    }
    }

    return false;
}

void NotificationManager::updateCurrentNotification()
{
    int currentIndex = -1;
    if (isEnabled())
    {
        // An alert is not delayed by the last closing (a closing date in the future is ignored)
        QStringList closedIds = ContextManager::configuration()->getValue(ConfManager::SECTION_NOTIFICATIONS, "closed_ids", QStringList()).toStringList();
        QDate lastClosing = QDate::fromString(ContextManager::configuration()->getValue(
                                                  ConfManager::SECTION_NOTIFICATIONS, "last_closing", "").toString(), Qt::ISODate);
        qint64 daysSinceLastClosing = lastClosing.isValid() ? lastClosing.daysTo(QDate::currentDate()) : -1;
        bool isDelayOver = daysSinceLastClosing < 0 || daysSinceLastClosing >= DAYS_BETWEEN_NOTIFICATIONS;

        for (int i = 0; i < _notifications.count(); i++)
        {
            const Notification &notification = _notifications.at(i);
            if ((isDelayOver || notification.type == TYPE_ALERT) &&
                !closedIds.contains(QString::number(notification.id)) && isApplicable(notification))
            {
                currentIndex = i;
                break;
            }
        }
    }

    if (currentIndex != _currentIndex)
    {
        _currentIndex = currentIndex;
        emit currentNotificationChanged();
    }
}
