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

#include "browsersortmenu.h"
#include "ui_browsersortmenu.h"
#include "contextmanager.h"
#include <QMenu>
#include <QMouseEvent>

BrowserSortMenu::BrowserSortMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserSortMenu)
{
    ui->setupUi(this);

    // Style
    _textColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    _hoveredBackgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->labelSort->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/sort.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    this->setStyle(false);

    // Icon
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::LIST_TEXT), QIcon::Normal);
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Active);

    // Add a menu to the button
    QMenu * menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuClosed()));
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());
    QAction * action = menu->addAction(tr("Date"));
    action->setIcon(_checkIcon);
    ui->pushButton->setText(action->text());
    connect(action, SIGNAL(triggered()), this, SLOT(element1Clicked()));
    action = menu->addAction(tr("Downloads"));
    connect(action, SIGNAL(triggered()), this, SLOT(element2Clicked()));
    action = menu->addAction(tr("Rating score"));
    connect(action, SIGNAL(triggered()), this, SLOT(element3Clicked()));
    action = menu->addAction(tr("Title (A→Z)"));
    connect(action, SIGNAL(triggered()), this, SLOT(element4Clicked()));
    ui->pushButton->setMenu(menu);
}

BrowserSortMenu::~BrowserSortMenu()
{
    delete ui;
}

int BrowserSortMenu::currentIndex()
{
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (!actions[i]->icon().isNull())
            return i;
    }
    return 0;
}

void BrowserSortMenu::element1Clicked()
{
    elementClicked(0);
}

void BrowserSortMenu::element2Clicked()
{
    elementClicked(1);
}

void BrowserSortMenu::element3Clicked()
{
    elementClicked(2);
}

void BrowserSortMenu::element4Clicked()
{
    elementClicked(3);
}

void BrowserSortMenu::elementClicked(int index)
{
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        QAction * action = actions[i];
        if (i == index)
        {
            ui->pushButton->setText(action->text());
            action->setIcon(_checkIcon);
        }
        else
            action->setIcon(QIcon());
    }
    emit currentIndexChanged(index);
}

#if QT_VERSION >= 0x060000
void BrowserSortMenu::enterEvent(QEnterEvent *event)
#else
void BrowserSortMenu::enterEvent(QEvent *event)
#endif
{
    setStyle(true);
    QWidget::enterEvent(event);
}

void BrowserSortMenu::leaveEvent(QEvent *event)
{
    setStyle(false);
    QWidget::leaveEvent(event);
}

void BrowserSortMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        ui->pushButton->menu()->exec();
    QWidget::mousePressEvent(event);
}

void BrowserSortMenu::onMenuClosed()
{
    setStyle(false);
}

void BrowserSortMenu::setStyle(bool hovered)
{
    ui->pushButton->setStyleSheet("QPushButton{border: 0; padding:0 5px 0 5px;color:" + _textColor + ";background-color:" +
                                  (hovered ? _hoveredBackgroundColor : _backgroundColor) + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                  "QPushButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}");
    ui->labelSort->setStyleSheet("QLabel{border-top-left-radius:2px;border-bottom-left-radius:2px;padding: 0 5px;background-color:" +
                                 (hovered ? _hoveredBackgroundColor : _backgroundColor) + "}");
}
