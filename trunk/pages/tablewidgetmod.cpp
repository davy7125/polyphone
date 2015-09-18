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

#include "tablewidgetmod.h"
#include <QKeyEvent>

void TableWidgetMod::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    this->setRowCount(0);
}

void TableWidgetMod::addRow(int row, EltID id)
{
    // Ajout d'une ligne
    this->insertRow(row);

    // Création d'éléments
    for (int i = 0; i < this->columnCount(); i++)
        this->setItem(row, i, new QTableWidgetItem);

    QString str;
    switch (id.typeElement)
    {
    case elementInstMod: case elementInst:          str = "Inst";       break;
    case elementInstSmplMod: case elementInstSmpl:  str = "InstSmpl";   break;
    case elementPrstMod: case elementPrst:          str = "Prst";       break;
    case elementPrstInstMod: case elementPrstInst:  str = "PrstInst";   break;
    default: break;
    }

    this->item(row, 0)->setText(str);
    this->item(row, 1)->setText(QString::number(id.indexSf2));
    this->item(row, 2)->setText(QString::number(id.indexElt));
    this->item(row, 3)->setText(QString::number(id.indexElt2));
    this->item(row, 4)->setText(QString::number(id.indexMod));
}

EltID TableWidgetMod::getID(int row)
{
    EltID id;
    if (this->item(row, 0)->text() == "Inst")
        id.typeElement = elementInstMod;
    else if (this->item(row, 0)->text() == "InstSmpl")
        id.typeElement = elementInstSmplMod;
    else if (this->item(row, 0)->text() == "Prst")
        id.typeElement = elementPrstMod;
    else if (this->item(row, 0)->text() == "PrstInst")
        id.typeElement = elementPrstInstMod;

    id.indexSf2 = this->item(row, 1)->text().toInt();
    id.indexElt = this->item(row, 2)->text().toInt();
    id.indexElt2 = this->item(row, 3)->text().toInt();
    id.indexMod = this->item(row, 4)->text().toInt();

    return id;
}

EltID TableWidgetMod::getID()
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->currentRow() != -1 && this->selectedItems().count() == 4) // because 4 visible columns
        id = getID(this->currentRow());
    return id;
}

QList<EltID> TableWidgetMod::getSelectedIDs()
{
    QMap <int, EltID> mapID;
    foreach (QTableWidgetItem * item, this->selectedItems())
    {
        int row = item->row();
        if (!mapID.keys().contains(row))
            mapID[row] = getID(row);
    }
    return mapID.values();
}

void TableWidgetMod::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Delete)
        emit(deleteModPressed());
    else if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
        emit(copyAsked());
    else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
        emit(pasteAsked());
}
