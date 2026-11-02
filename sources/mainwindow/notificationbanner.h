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

#ifndef NOTIFICATIONBANNER_H
#define NOTIFICATIONBANNER_H

#include <QWidget>

namespace Ui {
class NotificationBanner;
}

// Banner of the home screen showing the notification chosen by NotificationManager, hidden if none
class NotificationBanner : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationBanner(QWidget *parent = nullptr);
    ~NotificationBanner() override;

public slots:
    void updateNotification();

private slots:
    void on_pushLink_clicked();
    void on_pushClose_clicked();

private:
    Ui::NotificationBanner *ui;
    QString _url;
};

#endif // NOTIFICATIONBANNER_H
