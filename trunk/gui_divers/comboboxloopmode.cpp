/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "comboboxloopmode.h"
#include "thememanager.h"

ComboBoxLoopMode::ComboBoxLoopMode(QWidget *parent) : QComboBox(parent),
    _ignoreFirstHide(true)
{
    this->setView(new ComboView());

    // Different loop possibilities
    if (ThemeManager::getInstance()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
    {
        this->addItem(QIcon(":/icones/loop_off_w.png"), "", 0);
        this->addItem(QIcon(":/icones/loop_on_w.png"), "", 1);
        this->addItem(QIcon(":/icones/loop_on_end_w.png"), "", 3);
    }
    else
    {
        this->addItem(QIcon(":/icones/loop_off.png"), "", 0);
        this->addItem(QIcon(":/icones/loop_on.png"), "", 1);
        this->addItem(QIcon(":/icones/loop_on_end.png"), "", 3);
    }

    QObject::connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
}

void ComboBoxLoopMode::showEvent(QShowEvent * event)
{
    QComboBox::showEvent(event);
    this->showPopup();
}

void ComboBoxLoopMode::hidePopup()
{
    if (_ignoreFirstHide)
        _ignoreFirstHide = false;
    else
    {
        this->blockSignals(true);
        // Remove all elements
        this->clear();

        // End the editor
        this->setDisabled(true);
    }
}

void ComboBoxLoopMode::onActivated(int index)
{
    // Select the right index
    if (this->view()->currentIndex().isValid())
        this->setCurrentIndex(index);

    // End the editor
    this->setDisabled(true);
}
