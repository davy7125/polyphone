/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QAbstractItemView>
#include <QMouseEvent>
#include "sf2_types.h"

// Classe ComboBox pour destination et source
class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QWidget* parent = 0) : QComboBox(parent)
    {
        this->limite = 0;
        this->view()->viewport()->installEventFilter(this);
    }

    bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
        {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            int index = view()->indexAt(mouseEvent->pos()).row();
            if (index >= this->limite)
            {
                // index du modulateur pointé
                index = this->itemText(index).split("#").last().toInt();
                index += 32767; // 32768 - 1
            }
            emit(this->clicked(index));
        }
        return false;
    }

    void setLimite(int lim) { this->limite = lim; }

    void removeItemsAfterLim()
    {
        for (int i = this->count(); i >= this->limite; i--)
            this->removeItem(i);
    }

    void selectIndex(quint16 index, quint16 numChamp)
    {
        if (index > 99)
        {
            // On cherche le modulateur numChamp
            numChamp -= 32768;
            int iVal = this->findText(trUtf8("Modulateur") + ": #" + QString::number(numChamp + 1));
            if (iVal != -1)
                this->setCurrentIndex(iVal);
            else
                this->setCurrentIndex(0);
        }
        else
        {
            // Sélection de l'index pointé par index
            this->setCurrentIndex(index);
        }
    }

    void selectIndex(quint16 index)
    {
        this->setCurrentIndex(index);
    }

private:
    int limite;

signals:
    void clicked(int index);
};

#endif // COMBOBOX_H
