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
    int currentSort = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "repository_sort", 0).toInt();
    if (currentSort < SORT_DATE || currentSort > SORT_SIZE)
        currentSort = SORT_DATE;
    QMenu * menu = new QMenu(this);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuClosed()));
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());
    addElement(menu, tr("Date"), SORT_DATE, currentSort);
    addElement(menu, tr("Trend"), SORT_TRENDING, currentSort);
    addElement(menu, tr("Downloads"), SORT_DOWNLOADS, currentSort);
    addElement(menu, tr("Rating score"), SORT_RATING, currentSort);
    addElement(menu, tr("Title (A→Z)"), SORT_TITLE, currentSort);
    addElement(menu, tr("Size"), SORT_SIZE, currentSort);
    ui->pushButton->setMenu(menu);
}

void BrowserSortMenu::addElement(QMenu * menu, QString text, SortType sortType, int currentSort)
{
    QAction * action = menu->addAction(text);
    action->setData(sortType);
    if (currentSort == sortType)
    {
        action->setIcon(_checkIcon);
        ui->pushButton->setText(action->text());
    }
    connect(action, &QAction::triggered, this, [this, sortType]() { elementClicked(sortType); });
}

BrowserSortMenu::~BrowserSortMenu()
{
    delete ui;
}

BrowserSortMenu::SortType BrowserSortMenu::currentSort()
{
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    foreach (QAction * action, actions)
    {
        if (!action->icon().isNull())
            return static_cast<SortType>(action->data().toInt());
    }
    return SORT_DATE;
}

void BrowserSortMenu::elementClicked(SortType sortType)
{
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "repository_sort", sortType);
    QList<QAction *> actions = ui->pushButton->menu()->actions();
    foreach (QAction * action, actions)
    {
        if (action->data().toInt() == sortType)
        {
            ui->pushButton->setText(action->text());
            action->setIcon(_checkIcon);
        }
        else
            action->setIcon(QIcon());
    }
    emit sortChanged();
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
