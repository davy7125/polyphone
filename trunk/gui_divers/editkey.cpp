/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "editkey.h"

EditKey::EditKey(QWidget *parent) : QLineEdit(parent),
    _combinaison("")
{
    setReadOnly(true);
    setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
}

void EditKey::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        QString combinaison = getSequence(event).toString();
        if (combinaison.isEmpty())
            this->setText("...");
        else
        {
            QString key = QKeySequence(event->key()).toString();
            if (key.isEmpty() || event->key() == Qt::Key_Shift)
            {
                this->setText(combinaison);
            }
            else
            {
                int keyNum = event->key();
                if (keyNum == Qt::Key_Delete)
                    _combinaison = "";
                else if (keyNum != Qt::Key_Escape && keyNum != Qt::Key_Return && keyNum != Qt::Key_Enter)
                    _combinaison = combinaison;

                this->clearFocus();
            }
        }
        event->accept();
    }
}

void EditKey::keyReleaseEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        event->ignore();
    else
    {
        QString combinaison = getSequence(event).toString();
        if (combinaison.isEmpty())
        {
            if (this->hasFocus())
                this->setText("...");
            else
                this->setText(_combinaison);
        }
        else
            this->setText(combinaison);
        event->accept();
    }
}

void EditKey::focusInEvent(QFocusEvent *)
{
    this->setText("...");
}

void EditKey::focusOutEvent(QFocusEvent *)
{
    this->setText(_combinaison);
    emit(combinaisonChanged(_combinaison));
}

QKeySequence EditKey::getSequence(QKeyEvent * event)
{
    QString modifier = QString::null;
    // Sensibilité à la touche shift uniquement
    if (event->modifiers() & Qt::ShiftModifier)
        modifier += "Shift+";
    QString key = QKeySequence(event->key()).toString();
    return QKeySequence(modifier + key);
}

void EditKey::setCombinaison(const QString &text)
{
    _combinaison = text;
    QLineEdit::setText(text);
}
