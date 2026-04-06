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

#include "directorysortmenu.h"
#include "ui_directorysortmenu.h"
#include "contextmanager.h"
#include <QMenu>
#include <QMouseEvent>

DirectorySortMenu::DirectorySortMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectorySortMenu)
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
    int currentSort = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort", 0).toInt();
    if (currentSort < 0 || currentSort > 3)
        currentSort = 0;
    QMenu * menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuClosed()));
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());
    QAction * action = menu->addAction(tr("Name (A→Z)"));
    if (currentSort == 0)
    {
        action->setIcon(_checkIcon);
        ui->pushButton->setText(action->text());
    }
    connect(action, SIGNAL(triggered()), this, SLOT(element1Clicked()));
    action = menu->addAction(tr("Newest first"));
    if (currentSort == 1)
    {
        action->setIcon(_checkIcon);
        ui->pushButton->setText(action->text());
    }
    connect(action, SIGNAL(triggered()), this, SLOT(element2Clicked()));
    action = menu->addAction(tr("Largest first"));
    if (currentSort == 2)
    {
        action->setIcon(_checkIcon);
        ui->pushButton->setText(action->text());
    }
    connect(action, SIGNAL(triggered()), this, SLOT(element3Clicked()));
    action = menu->addAction(tr("Smallest first"));
    if (currentSort == 3)
    {
        action->setIcon(_checkIcon);
        ui->pushButton->setText(action->text());
    }
    connect(action, SIGNAL(triggered()), this, SLOT(element4Clicked()));
    ui->pushButton->setMenu(menu);
}

DirectorySortMenu::~DirectorySortMenu()
{
    delete ui;
}

int DirectorySortMenu::currentIndex()
{
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    for (int i = 0; i < actions.count(); i++)
    {
        if (!actions[i]->icon().isNull())
            return i;
    }
    return 0;
}

void DirectorySortMenu::element1Clicked()
{
    elementClicked(0);
}

void DirectorySortMenu::element2Clicked()
{
    elementClicked(1);
}

void DirectorySortMenu::element3Clicked()
{
    elementClicked(2);
}

void DirectorySortMenu::element4Clicked()
{
    elementClicked(3);
}

void DirectorySortMenu::elementClicked(int index)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort", index);
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
void DirectorySortMenu::enterEvent(QEnterEvent *event)
#else
void DirectorySortMenu::enterEvent(QEvent *event)
#endif
{
    setStyle(true);
    QWidget::enterEvent(event);
}

void DirectorySortMenu::leaveEvent(QEvent *event)
{
    setStyle(false);
    QWidget::leaveEvent(event);
}

void DirectorySortMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        ui->pushButton->menu()->exec();
    QWidget::mousePressEvent(event);
}

void DirectorySortMenu::onMenuClosed()
{
    setStyle(false);
}

void DirectorySortMenu::setStyle(bool hovered)
{
    ui->pushButton->setStyleSheet("QPushButton{border: 0; padding:0 5px 0 5px;color:" + _textColor + ";background-color:" +
                                  (hovered ? _hoveredBackgroundColor : _backgroundColor) + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                  "QPushButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}");
    ui->labelSort->setStyleSheet("QLabel{border-top-left-radius:2px;border-bottom-left-radius:2px;padding: 0 5px;background-color:" +
                                 (hovered ? _hoveredBackgroundColor : _backgroundColor) + "}");
}
