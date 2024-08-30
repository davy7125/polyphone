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

#include "comboboxloopmode.h"
#include "contextmanager.h"
#include <QKeyEvent>

ComboBoxLoopMode::ComboBoxLoopMode(QWidget *parent) : QComboBox(parent),
    _ignoreFirstHide(true),
    _currentIndex(0)
{
    this->setView(new ComboView());
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    // Different loop possibilities
    this->addItem("", -1);
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::ColorContext::DISABLED).name();
    this->addItem(ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_off.svg", QSize(48, 48), replacement), "", 0);
    this->addItem(ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on.svg", QSize(48, 48), replacement), "", 1);
    this->addItem(ContextManager::theme()->getColoredSvg(":/icons/sample_mode_release.svg", QSize(48, 48), replacement), "", 2);
    this->addItem(ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on_end.svg", QSize(48, 48), replacement), "", 3);

    QObject::connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
    QObject::connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onActivated(int)));
}

void ComboBoxLoopMode::showEvent(QShowEvent * event)
{
    QComboBox::showEvent(event);
    this->showPopup();
}

void ComboBoxLoopMode::hidePopup()
{
    _currentIndex = this->currentIndex();
    if (_ignoreFirstHide)
        _ignoreFirstHide = false;
    else
    {
        if (this->hasFocus())
        {
            QComboBox::hidePopup();
            this->setDisabled(true);
        }
    }
}

void ComboBoxLoopMode::onActivated(int index)
{
    Q_UNUSED(index)

    // Close the popup
    _ignoreFirstHide = false;
    hidePopup();

    // End the editor
    this->setDisabled(true);
}

#if QT_VERSION >= 0x060000
void ComboView::initViewItemOption(QStyleOptionViewItem *option) const
{
    QListView::initViewItemOption(option);
#else
QStyleOptionViewItem ComboView::viewOptions() const
{
    QStyleOptionViewItem optionObject = QListView::viewOptions();
    QStyleOptionViewItem * option = &optionObject;
#endif

    // Set icon on the top and center of combo box item
    option->decorationSize = QSize(37, 37);

#if QT_VERSION < 0x060000
    return optionObject;
#endif
}

