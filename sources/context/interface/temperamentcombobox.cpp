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

#include "temperamentcombobox.h"
#include "contextmanager.h"
#include "tuningprogrammanager.h"
#include "temperamentsettings.h"
#include <QMenu>
#include <QScreen>

TemperamentComboBox::TemperamentComboBox(QWidget * parent) : QComboBox(parent) {}

void TemperamentComboBox::fillTemperaments()
{
    int program = getProgram();
    this->blockSignals(true);
    this->clear();
    QMap<int, TuningProgram> * programs = TuningProgramManager::getInstance()->getAllPrograms();
    for (auto i = programs->cbegin(), end = programs->cend(); i != end; ++i)
    {
        this->addItem(i.value()._name, i.key());
        this->setItemData(this->count() - 1, i.value()._category, CATEGORY_ROLE);
    }
    this->addItem(TemperamentSettings::getCustomName(), TemperamentSettings::CUSTOM);
    this->blockSignals(false);
    selectProgram(program);
}

int TemperamentComboBox::getProgram() const
{
    return this->currentIndex() < 0 ? 0 : this->currentData().toInt();
}

void TemperamentComboBox::selectProgram(int program)
{
    int index = this->findData(program);
    this->blockSignals(true);
    this->setCurrentIndex(index < 0 ? 0 : index);
    this->blockSignals(false);
}

void TemperamentComboBox::showPopup()
{
    // Long menus (irregular temperaments) scrolled in a single column instead of being split into columns, with a
    // margin since the screen area is not always known (panels of the desktop under Wayland)
    QMenu menu(this);
    menu.setStyleSheet(ContextManager::theme()->getMenuTheme() + "QMenu{menu-scrollable:1}");
    menu.setMaximumHeight(this->screen()->availableGeometry().height() * 4 / 5);
    menu.setMinimumWidth(this->width());
    QMap<QString, QMenu *> menus;
    QFont boldFont = menu.font();
    boldFont.setBold(true);

    bool previousIsTopLevel = true;
    for (int i = 0; i < this->count(); i++)
    {
        // Separator between the items of the top level and the categories
        QStringList category = this->itemData(i, CATEGORY_ROLE).toStringList();
        if (i > 0 && category.isEmpty() != previousIsTopLevel)
            menu.addSeparator();
        previousIsTopLevel = category.isEmpty();

        // Item in its category, the current one being checked and its categories in bold
        QAction * action = getMenu(&menu, menus, category)->addAction(this->itemText(i));
        action->setData(i);
        action->setCheckable(true);
        if (i == this->currentIndex())
        {
            action->setChecked(true);
            for (int level = 1; level <= category.size(); level++)
                menus[category.mid(0, level).join("/")]->menuAction()->setFont(boldFont);
        }
    }

    QAction * action = menu.exec(this->mapToGlobal(QPoint(0, this->height())));
    if (action != nullptr)
        this->setCurrentIndex(action->data().toInt());
}

QMenu * TemperamentComboBox::getMenu(QMenu * rootMenu, QMap<QString, QMenu *> &menus, const QStringList &category)
{
    // Submenus created when they are first needed, so that they follow the order of the items
    QMenu * menu = rootMenu;
    for (int level = 1; level <= category.size(); level++)
    {
        QString path = category.mid(0, level).join("/");
        if (!menus.contains(path))
        {
            QMenu * subMenu = menu->addMenu(category[level - 1]);
            subMenu->setStyleSheet(rootMenu->styleSheet());
            subMenu->setMaximumHeight(rootMenu->maximumHeight());
            menus[path] = subMenu;
        }
        menu = menus[path];
    }
    return menu;
}
