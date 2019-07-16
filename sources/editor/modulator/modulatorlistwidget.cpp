/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "modulatorlistwidget.h"
#include "modulatorcell.h"
#include <QKeyEvent>

ModulatorListWidget::ModulatorListWidget(QWidget * parent) : QListWidget(parent)
{
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
}

void ModulatorListWidget::onSelectionChanged()
{
    // Browse the list of cells
    for (int i = 0; i < this->count(); i++)
    {
        QListWidgetItem * item = this->item(i);
        ModulatorCell * cell = dynamic_cast<ModulatorCell*>(this->itemWidget(item));

        // Inform them whether they are selected or not
        cell->setSelected(item->isSelected());
    }
}


void ModulatorListWidget::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
        emit(deleted());
    else if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
        emit(copied());
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
        emit(pasted());
}

QList<ModulatorData> ModulatorListWidget::getSelectedModulators()
{
    QList<ModulatorData> result;

    // Browse the list of cells
    for (int i = 0; i < this->count(); i++)
    {
        QListWidgetItem * item = this->item(i);
        ModulatorCell * cell = dynamic_cast<ModulatorCell*>(this->itemWidget(item));

        // Get the modulator data if the cell is selected
        if (item->isSelected())
            result << cell->getModulatorData();
    }

    return result;
}
