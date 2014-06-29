/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

void TableWidgetMod::clear()
{
    for (int i = 0; i < this->columnCount(); i++)
        for (int j = 0; j < this->rowCount(); j++)
            delete this->item(j, i);
    this->setRowCount(0);
}

void TableWidgetMod::addRow(int row)
{
    // Ajout d'une ligne
    this->insertRow(row);

    // Création d'éléments
    for (int i = 0; i < this->columnCount(); i++)
        this->setItem(row, i, new QTableWidgetItem);
}

void TableWidgetMod::setID(EltID id, int row)
{
    char T[20];
    switch(id.typeElement)
    {
    case elementInstMod: case elementInst: strcpy(T, "Inst"); break;
    case elementInstSmplMod: case elementInstSmpl: strcpy(T, "InstSmpl"); break;
    case elementPrstMod: case elementPrst: strcpy(T, "Prst"); break;
    case elementPrstInstMod: case elementPrstInst: strcpy(T, "PrstInst"); break;
    default: break;
    }
    this->item(row, 0)->setText(T);
    sprintf(T, "%d", id.indexSf2);
    this->item(row, 1)->setText(T);
    sprintf(T, "%d", id.indexElt);
    this->item(row, 2)->setText(T);
    sprintf(T, "%d", id.indexElt2);
    this->item(row, 3)->setText(T);
    sprintf(T, "%d", id.indexMod);
    this->item(row, 4)->setText(T);
}

EltID TableWidgetMod::getID(int row)
{
    EltID id;
    if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "Inst") == 0)
        id.typeElement = elementInstMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "InstSmpl") == 0)
        id.typeElement = elementInstSmplMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "Prst") == 0)
        id.typeElement = elementPrstMod;
    else if (strcmp(this->item(row, 0)->text().toStdString().c_str(), "PrstInst") == 0)
        id.typeElement = elementPrstInstMod;

    sscanf(this->item(row, 1)->text().toStdString().c_str(), "%d", &id.indexSf2);
    sscanf(this->item(row, 2)->text().toStdString().c_str(), "%d", &id.indexElt);
    sscanf(this->item(row, 3)->text().toStdString().c_str(), "%d", &id.indexElt2);
    sscanf(this->item(row, 4)->text().toStdString().c_str(), "%d", &id.indexMod);

    return id;
}

EltID TableWidgetMod::getID()
{
    EltID id(elementUnknown, 0, 0, 0, 0);
    if (this->selectedItems().count())
        id = getID(this->selectedItems().at(0)->row());
    return id;
}
