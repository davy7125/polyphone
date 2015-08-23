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

#include "pageoverview.h"
#include "ui_pageoverview.h"

PageOverview::PageOverview(TypePage typePage, ElementType typeElement, QWidget *parent) :
    Page(typePage, parent),
    ui(new Ui::PageOverview),
    _typeElement(typeElement)
{
    ui->setupUi(this);
}

PageOverview::~PageOverview()
{
    delete ui;
}

void PageOverview::afficher()
{
    // Title
    ui->labelTitle->setText(getTitle());

    // Clear the table
    ui->table->clear();

    // Number of columns (first is for sorting), header
    QStringList hHeader = getHorizontalHeader();
    hHeader.prepend("name");
    ui->table->setColumnCount(hHeader.count());
    ui->table->setHorizontalHeaderLabels(hHeader);
    ui->table->setColumnHidden(0, true);

    // Number of rows
    EltID id = this->tree->getFirstID();
    id.typeElement = _typeElement;
    ui->table->setRowCount(sf2->count(id, false));

    // Preparation (if needed by the overview page)
    this->prepare(id);

    // Fill each row, retrieving the name in the same time
    int nbElts = sf2->count(id);
    int row = 0;
    for (int i = 0; i < nbElts; i++)
    {
        id.indexElt = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            QString name = fillInformation(id, row);
            ui->table->setItem(row, 0, new QTableWidgetItem(name));
            row++;
        }
    }

    // Vertical header
    ui->table->sortByColumn(0, Qt::AscendingOrder);
    QStringList vHeader;
    for (int i = 0; i < nbElts; i++)
        vHeader << ui->table->item(i, 0)->text();
    ui->table->setVerticalHeaderLabels(vHeader);

    // Format
    ui->table->resizeColumnsToContents();
    for (int i = 1; i < nbElts; i = i + 2)
        for (int j = 1; j < hHeader.count(); j++)
            ui->table->item(i, j)->setBackgroundColor(QColor(255, 255, 200));

    // Switch page
    this->qStackedWidget->setCurrentWidget(this);
}

QString PageOverview::fillInformation(EltID id, int row)
{
    QStringList info = getInformation(id);

    for (int i = 0; i < info.count(); i++)
    {
        QTableWidgetItem * item = new QTableWidgetItem(info[i]);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        ui->table->setItem(row, i + 1, item);
    }

    // Return the name
    return sf2->getQstr(id, champ_name);
}

QString PageOverview::getRange(EltID id, Champ champ)
{
    // Global value
    int globalValue = 0;
    if (sf2->isSet(id, champ))
        globalValue = sf2->get(id, champ).shValue;

    // Attenuation per division
    int min = -1;
    int max = -1;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    int nbElt = sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!sf2->get(id, champ_hidden).bValue)
        {
            int value = globalValue;
            if (sf2->isSet(id, champ))
                value = sf2->get(id, champ).shValue;

            if (min == -1 || min > value)
                min = value;
            if (max == -1 || max < value)
                max = value;
        }
    }

    if (min == -1)
        return "?";
    else if (min == max)
        return QString::number((double)min / 10, 'f', 1);
    else
        return QString::number((double)min / 10, 'f', 1) + " - " + QString::number((double)max / 10, 'f', 1);
}
