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

#include "editkey.h"

EditKey::EditKey(int octave, ConfManager::Key key, QWidget *parent) : QLineEdit(parent),
    _octave(octave),
    _key(key)
{
    setReadOnly(true);
    setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    this->setStyleSheet("QLineEdit{border: 0px; padding-left: 2px;}");

}

void EditKey::updateText()
{
    this->setText(ContextManager::configuration()->getMapping(_octave, _key));
}

void EditKey::focusInEvent(QFocusEvent * event)
{
    Q_UNUSED(event)
    this->setText("...");
}

void EditKey::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        processKeyEvent(event);
        event->accept();
    }
}

void EditKey::keyReleaseEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        processKeyEvent(event);
        event->accept();
    }
}

void EditKey::focusOutEvent(QFocusEvent * event)
{
    Q_UNUSED(event)
    if (!this->text().contains("...") && this->text() != "")
        ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD,
                                                  "key_" + QString::number(_octave) + "_" + QString::number(_key),
                                                  this->text());
    updateText();
}

void EditKey::processKeyEvent(QKeyEvent * event)
{
    // Only the modifier "Shift" is allowed
    bool shiftModifier = (event->modifiers() & Qt::ShiftModifier) > 0;

    // Current key
    QString key = QKeySequence(event->key()).toString();
    int keyNum = event->key();

    // New text
    if (key == "")
        this->setText(shiftModifier ? "Shift+..." : "...");
    else
    {
        if (event->key() == Qt::Key_Shift)
            this->setText(shiftModifier ? "Shift+..." : "...");
        else if (keyNum != Qt::Key_Control && keyNum != Qt::Key_Meta && keyNum != Qt::Key_Alt)
        {
            if (keyNum == Qt::Key_Delete)
            {
                // Reset the key
                ContextManager::configuration()->setValue(ConfManager::SECTION_KEYBOARD,
                                                          "key_" + QString::number(_octave) + "_" + QString::number(_key),
                                                          "");
                this->setText("");
            }
            else if (keyNum != Qt::Key_Escape && keyNum != Qt::Key_Return && keyNum != Qt::Key_Enter)
                this->setText((shiftModifier ? "Shift+" : "") + key);
            else
                this->setText("");

            this->clearFocus();
            this->updateText();
        }
    }
}
