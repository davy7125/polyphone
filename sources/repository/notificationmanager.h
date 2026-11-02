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

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QList>
#include <QVersionNumber>
class UrlReaderJson;

// Notifications published on the website (api/notifications/list), downloaded at startup
// The website only gives the contents and the conditions: the software decides which notification is shown,
// one at most on the home screen
class NotificationManager: public QObject
{
    Q_OBJECT

public:
    // Sorted by priority
    enum Type
    {
        TYPE_ALERT,
        TYPE_UPDATE,
        TYPE_ANNOUNCEMENT,
        TYPE_APPEAL
    };

    struct Notification
    {
        int id;
        Type type;
        QString title;
        QString text;
        QString buttonLabel; // Can be empty even if there is a link
        QString url; // Full address of a page of the website, empty if no link
        QVersionNumber versionFrom; // Included, null for no condition
        QVersionNumber versionBelow; // Excluded, null for no condition
        int minDaysUsed;
    };

    // Singleton: get the instance of the object and finally kill it
    static NotificationManager * getInstance();
    static void kill();

    // Count the current day of use and download the notifications if they are enabled
    void initialize();

    // Enable or disable the notifications (enabled by default)
    bool isEnabled();
    void setEnabled(bool isEnabled);

    // Notification to show, nullptr if none
    const Notification * getCurrentNotification();

    // Close the current notification: it will not be shown anymore
    void closeCurrentNotification();

signals:
    // Emitted when the notification to show changed
    void currentNotificationChanged();

private slots:
    void notificationListAvailable(QString error);
    void updateCurrentNotification();

private:
    NotificationManager();
    ~NotificationManager() override;
    void download();
    bool isApplicable(const Notification &notification);
    static bool readVersion(QString text, QVersionNumber &version);

    static NotificationManager * s_instance;
    static const QString WEBSITE_URL;
    static const int DAYS_BETWEEN_NOTIFICATIONS;

    UrlReaderJson * _urlReaderJson;
    bool _downloadStarted;
    QList<Notification> _notifications; // Sorted by priority
    int _currentIndex;
};

#endif // NOTIFICATIONMANAGER_H
