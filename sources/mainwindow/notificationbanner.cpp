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

#include "notificationbanner.h"
#include "ui_notificationbanner.h"
#include "notificationmanager.h"
#include "contextmanager.h"
#include <QDesktopServices>
#include <QUrl>

NotificationBanner::NotificationBanner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotificationBanner)
{
    ui->setupUi(this);

    // Colors of the selection
    QString highlightedBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("NotificationBanner{background-color:" + highlightedBackground + "}" +
                        "QLabel{color:" + highlightedText + "}" +
                        "QPushButton#pushLink{background-color:" + highlightedText + ";color:" + highlightedBackground +
                        ";border:0;border-radius:2px;padding:5px 10px}" +
                        "QPushButton#pushLink:hover{background-color:" +
                        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name() + "}" +
                        "QPushButton#pushClose{background-color:transparent;border:0;border-radius:2px;padding:5px}" +
                        "QPushButton#pushClose:hover{background-color:" +
                        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED).name() + "}");
    ui->pushClose->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(14, 14), ThemeManager::HIGHLIGHTED_TEXT));

    this->hide();
}

NotificationBanner::~NotificationBanner()
{
    delete ui;
}

void NotificationBanner::updateNotification()
{
    const NotificationManager::Notification * notification = NotificationManager::getInstance()->getCurrentNotification();
    if (notification == nullptr)
    {
        this->hide();
        return;
    }

    // Icon and default label of the button depending on the type
    QString iconPath;
    QString defaultButtonLabel;
    switch (notification->type)
    {
    case NotificationManager::TYPE_ALERT:
        iconPath = ":/icons/warning.svg";
        defaultButtonLabel = tr("More information");
        break;
    case NotificationManager::TYPE_UPDATE:
        iconPath = ":/icons/download.svg";
        defaultButtonLabel = tr("Download");
        break;
    case NotificationManager::TYPE_ANNOUNCEMENT:
        iconPath = ":/icons/info.svg";
        defaultButtonLabel = tr("More information");
        break;
    case NotificationManager::TYPE_APPEAL:
        iconPath = ":/icons/heart.svg";
        defaultButtonLabel = tr("Support Polyphone");
        break;
    }
    ui->labelIcon->setPixmap(ContextManager::theme()->getColoredSvg(iconPath, QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT));

    // Texts
    ui->labelTitle->setText(notification->title);
    ui->labelText->setText(notification->text);
    ui->labelText->setVisible(!notification->text.isEmpty());

    // Link
    _url = notification->url;
    ui->pushLink->setText(notification->buttonLabel.isEmpty() ? defaultButtonLabel : notification->buttonLabel);
    ui->pushLink->setVisible(!_url.isEmpty());

    this->show();
}

void NotificationBanner::on_pushLink_clicked()
{
    // The notification has been read
    QDesktopServices::openUrl(QUrl(_url));
    NotificationManager::getInstance()->closeCurrentNotification();
}

void NotificationBanner::on_pushClose_clicked()
{
    NotificationManager::getInstance()->closeCurrentNotification();
}
