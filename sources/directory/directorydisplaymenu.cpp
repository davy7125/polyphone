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
#include "contextmanager.h"
#include <QMenu>
#include <QMouseEvent>
#include "directorylistview.h"

// Preset summary and columns of the list, details of the elements
const int DirectoryDisplayMenu::LIST_OPTIONS = (0x01 << DirectoryListView::COLUMN_NUMBER) - 1;
const int DirectoryDisplayMenu::DETAILS_SHIFT = DirectoryListView::COLUMN_NUMBER;
const int DirectoryDisplayMenu::DETAILS_OPTIONS = SAMPLE_DETAILS | INSTRUMENT_DETAILS | PRESET_DETAILS;
static_assert(DirectoryDisplayMenu::SAMPLE_DETAILS == (0x01 << DirectoryListView::COLUMN_NUMBER),
              "the details follow the columns");

DirectoryDisplayMenu::DirectoryDisplayMenu(QWidget *parent) : StyledAction(parent)
{
    this->initialize(tr("Display"), ":/icons/eye.svg");

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
    menu->setStyleSheet(ContextManager::theme()->getMenuTheme());

    // Bit 0 for the preset summary, bit i for the column i, then the details of the elements
    // (saved under another key so that they are displayed by default with an existing configuration)
    _displayOptions =
        (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_display", LIST_OPTIONS).toInt() & LIST_OPTIONS) |
        ((ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_details", DETAILS_OPTIONS >> DETAILS_SHIFT).toInt()
          << DETAILS_SHIFT) & DETAILS_OPTIONS);
    addOption(menu, tr("Preset summary"), PRESET_SUMMARY);
    menu->addSeparator();
    addOption(menu, tr("Size"), 0x01 << DirectoryListView::COLUMN_SIZE);
    addOption(menu, tr("Date"), 0x01 << DirectoryListView::COLUMN_DATE);
    addOption(menu, tr("Samples"), 0x01 << DirectoryListView::COLUMN_SAMPLES);
    addOption(menu, tr("Instruments"), 0x01 << DirectoryListView::COLUMN_INSTRUMENTS);
    addOption(menu, tr("Presets"), 0x01 << DirectoryListView::COLUMN_PRESETS);
    menu->addSeparator();
    addOption(menu, tr("Sample details"), SAMPLE_DETAILS);
    addOption(menu, tr("Instrument details"), INSTRUMENT_DETAILS);
    addOption(menu, tr("Preset details"), PRESET_DETAILS);

    this->setDropDownMenu(menu);
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
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_display", _displayOptions & LIST_OPTIONS);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_details", (_displayOptions & DETAILS_OPTIONS) >> DETAILS_SHIFT);
    emit displayOptionsChanged(_displayOptions);
}

bool DirectoryDisplayMenu::eventFilter(QObject *obj, QEvent *event)
{
    QMenu * menu = qobject_cast<QMenu *>(obj);
    if (!menu)
        return StyledAction::eventFilter(obj, event);

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

    return StyledAction::eventFilter(obj, event);
}