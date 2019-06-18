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

#include "toprightwidget.h"
#include "ui_toprightwidget.h"
#include "mainmenu.h"
#include "contextmanager.h"
#include "usermanager.h"
#include "downloadmanager.h"

TopRightWidget::TopRightWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TopRightWidget)
{
    ui->setupUi(this);

    // Style
    if (ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT))
    {
        ui->toolButton->setIcon(ContextManager::theme()->getColoredSvg(":/icons/menu.svg", QSize(28, 28), ThemeManager::WINDOW_TEXT));
        ui->pushUser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(22, 22), ThemeManager::WINDOW_TEXT));
    }
    else
    {
        ui->toolButton->setIcon(ContextManager::theme()->getColoredSvg(":/icons/menu.svg", QSize(28, 28), ThemeManager::WINDOW_BACKGROUND));
        ui->pushUser->setIcon(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(22, 22), ThemeManager::WINDOW_BACKGROUND));
    }

    // Red color for the warning icon
    _colorReplacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::WINDOW_BACKGROUND).name();

    ui->toolButton->setStyleSheet("QToolButton::menu-indicator{width:0px;} QToolButton{margin: 3px;padding: 3px;background-color:#000}");
    ui->toolButtonDownload->setStyleSheet("QToolButton::menu-indicator{width:0px;} QToolButton{border:0;margin: 3px;padding: 3px;}");
    ui->toolButtonDownload->hide();

    // Add the main menu
    _menu = new MainMenu(ui->toolButton);
    _menu->setFullScreen(parent->windowState() & Qt::WindowFullScreen);
    ui->toolButton->setMenu(_menu);

    // Connections
    connect(_menu, SIGNAL(newClicked()), this, SIGNAL(newClicked()));
    connect(_menu, SIGNAL(openClicked()), this, SIGNAL(openClicked()));
    connect(_menu, SIGNAL(openSettingsClicked()), this, SIGNAL(openSettingsClicked()));
    connect(_menu, SIGNAL(onlineHelpClicked()), this, SIGNAL(onlineHelpClicked()));
    connect(_menu, SIGNAL(aboutClicked()), this, SIGNAL(aboutClicked()));
    connect(_menu, SIGNAL(closeFileClicked()), this, SIGNAL(closeFileClicked()));
    connect(_menu, SIGNAL(closeClicked()), this, SIGNAL(closeClicked()));
    connect(_menu, SIGNAL(save()), this, SIGNAL(save()));
    connect(_menu, SIGNAL(saveAs()), this, SIGNAL(saveAs()));
    connect(_menu, SIGNAL(fullScreenTriggered()), this, SIGNAL(fullScreenTriggered()));
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)),
            this, SLOT(userStateChanged(UserManager::ConnectionState)));
    connect(DownloadManager::getInstance(), SIGNAL(progressChanged(int,int,QString,QString)), this, SLOT(progressChanged(int,int,QString,QString)),
            Qt::QueuedConnection);
    connect(DownloadManager::getInstance(), SIGNAL(downloadCanceled(int)), this, SLOT(downloadCanceled(int)));
    connect(ui->toolButtonDownload, SIGNAL(cleared()), this, SLOT(downloadCleared()));

    userStateChanged(UserManager::DISCONNECTED);
}

TopRightWidget::~TopRightWidget()
{
    delete ui;
}

void TopRightWidget::onEditorOpen(bool isOpen)
{
    _menu->onEditorOpen(isOpen);
}

void TopRightWidget::userStateChanged(UserManager::ConnectionState state)
{
    switch (state)
    {
    case UserManager::DISCONNECTED:
        // Hide everything
        ui->labelWarning->hide();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::PENDING:
        // Show the rotating spinner
        ui->labelWarning->hide();
        ui->pushUser->hide();
        ui->spinner->show();
        ui->spinner->startAnimation();
        break;
    case UserManager::CONNECTED:
        // Show a warning "not premium"
        ui->labelWarning->setToolTip(trUtf8("Subscribe to a Premium account to get all the features!"));
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::CONNECTED_PREMIUM:
        // Show the user button
        ui->labelWarning->hide();
        ui->pushUser->show();
        ui->pushUser->setToolTip(trUtf8("Welcome %1").arg(UserManager::getInstance()->getUsername()));
        ui->spinner->hide();
        break;
    case UserManager::BANNED:
        // Show an error "banned account"
        ui->labelWarning->setToolTip(trUtf8("Your account has been banned."));
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/skull.svg", QSize(20, 20), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    case UserManager::FAILED:
        // Show the connection error
        ui->labelWarning->setToolTip(UserManager::getInstance()->error());
        ui->labelWarning->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/warning.svg", QSize(16, 16), _colorReplacement));
        ui->labelWarning->show();
        ui->pushUser->hide();
        ui->spinner->hide();
        break;
    }
}

void TopRightWidget::on_pushUser_clicked()
{
    emit(userClicked());
}

void TopRightWidget::progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName)
{
    ui->toolButtonDownload->progressChanged(percent, soundfontId, soundfontName, finalFileName);
    ui->toolButtonDownload->show();
}

void TopRightWidget::downloadCanceled(int soundfontId)
{
    ui->toolButtonDownload->downloadCanceled(soundfontId);
}

void TopRightWidget::downloadCleared()
{
    ui->toolButtonDownload->hide();
}
