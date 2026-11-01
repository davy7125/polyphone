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

#include "directorydisplaymenu.h"
#include "ui_directorydisplaymenu.h"
#include "contextmanager.h"
#include <QMenu>
#include <QMouseEvent>
#include "directorylistview.h"

DirectoryDisplayMenu::DirectoryDisplayMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectoryDisplayMenu)
{
    ui->setupUi(this);

    // Style
    _textColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    _hoveredBackgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    ui->labelDisplay->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/eye.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    this->setStyle(false);

    // Icons
    QPixmap emptyPixmap(24, 24);
    emptyPixmap.fill(Qt::transparent);
    _emptyIcon.addPixmap(emptyPixmap, QIcon::Normal);
    _emptyIcon.addPixmap(emptyPixmap, QIcon::Active);
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::LIST_TEXT), QIcon::Normal);
    _checkIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Active);

    // Add a menu to the button
    QMenu * menu = new QMenu(this);
    menu->installEventFilter(this);
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(onMenuClosed()));
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());

    // Bit 0 for the preset summary, bit i for the column i
    _displayOptions = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_display", 0x3F).toInt();
    addOption(menu, tr("Preset summary"), 0x01);
    menu->addSeparator();
    addOption(menu, tr("Size"), 0x01 << DirectoryListView::COLUMN_SIZE);
    addOption(menu, tr("Date"), 0x01 << DirectoryListView::COLUMN_DATE);
    addOption(menu, tr("Samples"), 0x01 << DirectoryListView::COLUMN_SAMPLES);
    addOption(menu, tr("Instruments"), 0x01 << DirectoryListView::COLUMN_INSTRUMENTS);
    addOption(menu, tr("Presets"), 0x01 << DirectoryListView::COLUMN_PRESETS);

    ui->pushButton->setMenu(menu);
}

DirectoryDisplayMenu::~DirectoryDisplayMenu()
{
    delete ui;
}

void DirectoryDisplayMenu::addOption(QMenu * menu, QString text, int flag)
{
    QAction * action = menu->addAction(text);
    action->setData(flag);
    action->setIcon((_displayOptions & flag) ? _checkIcon : _emptyIcon);
}

void DirectoryDisplayMenu::toggleOption(QAction * action)
{
    int flag = action->data().toInt();
    _displayOptions ^= flag;
    action->setIcon((_displayOptions & flag) ? _checkIcon : _emptyIcon);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_display", _displayOptions);
    emit displayOptionsChanged(_displayOptions);
}

#if QT_VERSION >= 0x060000
void DirectoryDisplayMenu::enterEvent(QEnterEvent *event)
#else
void DirectoryDisplayMenu::enterEvent(QEvent *event)
#endif
{
    setStyle(true);
    QWidget::enterEvent(event);
}

void DirectoryDisplayMenu::leaveEvent(QEvent *event)
{
    setStyle(false);
    QWidget::leaveEvent(event);
}

void DirectoryDisplayMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        ui->pushButton->showMenu();
    QWidget::mousePressEvent(event);
}

void DirectoryDisplayMenu::onMenuClosed()
{
    setStyle(false);
}

void DirectoryDisplayMenu::setStyle(bool hovered)
{
    ui->pushButton->setStyleSheet("QPushButton{border: 0; padding:0 5px 0 5px;color:" + _textColor + ";background-color:" +
                                  (hovered ? _hoveredBackgroundColor : _backgroundColor) + ";border-top-right-radius:2px;border-bottom-right-radius:2px}" +
                                  "QPushButton::menu-indicator{width:0px;image:url(:/misc/transparent.png)}");
    ui->labelDisplay->setStyleSheet("QLabel{border-top-left-radius:2px;border-bottom-left-radius:2px;padding: 0 5px;background-color:" +
                                 (hovered ? _hoveredBackgroundColor : _backgroundColor) + "}");
}

bool DirectoryDisplayMenu::eventFilter(QObject *obj, QEvent *event)
{
    QMenu * menu = qobject_cast<QMenu *>(obj);
    if (!menu)
        return QWidget::eventFilter(obj, event);

    // Toggle an option without closing the menu, with the mouse or the keyboard
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        QAction * action = menu->actionAt(mouseEvent->pos());
        if (action && !action->isSeparator())
        {
            toggleOption(action);
            return true; // consume the event and don't close the menu
        }
    }
    else if (event->type() == QEvent::KeyPress)
    {
        int key = static_cast<QKeyEvent *>(event)->key();
        QAction * action = menu->activeAction();
        if (action && !action->isSeparator() && (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space))
        {
            toggleOption(action);
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}