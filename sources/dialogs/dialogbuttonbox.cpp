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

#include "dialogbuttonbox.h"
#include <QShortcut>
#include <QKeyEvent>

DialogButtonBox::DialogButtonBox(QWidget * parent) : QDialogButtonBox(parent)
{
    // Ctrl+Enter from the main keyboard or the keypad (Cmd+Enter on macOS)
    // A shortcut is used so that it also works in widgets using Enter, such as text editors
    QList<QKeySequence> keySequences;
    keySequences << QKeySequence(Qt::CTRL | Qt::Key_Return) << QKeySequence(Qt::CTRL | Qt::Key_Enter);
    foreach (QKeySequence keySequence, keySequences)
    {
        QShortcut * shortcut = new QShortcut(keySequence, this);
        connect(shortcut, SIGNAL(activated()), this, SLOT(onValidationShortcut()));
    }
}

bool DialogButtonBox::event(QEvent * event)
{
    // The window is known once shown (the guis of the tools are inserted in a dialog after their creation)
    if (event->type() == QEvent::Show)
        this->window()->installEventFilter(this);
    return QDialogButtonBox::event(event);
}

bool DialogButtonBox::eventFilter(QObject * watched, QEvent * event)
{
    // Escape reaching the window, not used by its widgets (same condition as for rejecting a QDialog)
    if (watched == this->window() && event->type() == QEvent::KeyPress &&
        static_cast<QKeyEvent *>(event)->matches(QKeySequence::Cancel))
    {
        QAbstractButton * button = getSingleButton(QList<ButtonRole>() << RejectRole);
        if (button != nullptr)
        {
            button->click(); // Nothing happens if the button is disabled
            return true;
        }
    }

    return QDialogButtonBox::eventFilter(watched, event);
}

void DialogButtonBox::onValidationShortcut()
{
    QAbstractButton * button = getSingleButton(QList<ButtonRole>() << AcceptRole << YesRole);
    if (button != nullptr)
        button->click(); // Nothing happens if the button is disabled
}

QAbstractButton * DialogButtonBox::getSingleButton(QList<ButtonRole> roles)
{
    QAbstractButton * result = nullptr;
    foreach (QAbstractButton * button, this->buttons())
    {
        if (!button->isHidden() && roles.contains(this->buttonRole(button)))
        {
            if (result != nullptr)
                return nullptr; // Several buttons
            result = button;
        }
    }
    return result;
}
