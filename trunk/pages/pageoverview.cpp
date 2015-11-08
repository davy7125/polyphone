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

#ifdef Q_OS_MAC
    ui->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = ui->table->font();
    font.setPixelSize(10);
    ui->table->setFont(font);
#endif
    ui->table->verticalHeader()->setDefaultSectionSize(QFontMetrics(ui->table->font()).height() + 8);
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
    ui->table->reset();
    ui->table->setColumnCount(0);
    ui->table->setRowCount(0);

    // Number of columns (first is for sorting), header
    QStringList hHeader = getHorizontalHeader();
    hHeader.prepend(trUtf8("Nom"));
    hHeader.prepend("id");
    ui->table->setColumnCount(hHeader.count());
    ui->table->setHorizontalHeaderLabels(hHeader);
    ui->table->setColumnHidden(0, true);

    // Number of rows
    EltID id = this->_tree->getFirstID();
    id.typeElement = _typeElement;
    ui->table->setRowCount(_sf2->count(id, false));

    // Preparation (if needed by the overview page)
    this->prepare(id);

    // Fill each row, retrieving the name in the same time
    int nbElts = _sf2->count(id);
    int row = 0;
    for (int i = 0; i < nbElts; i++)
    {
        id.indexElt = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            QString name = fillInformation(id, row);
            QString idStr = QString::number(id.typeElement) + ":" +
                    QString::number(id.indexSf2) + ":" +
                    QString::number(id.indexElt);
            ui->table->setItem(row, 0, new QTableWidgetItem(idStr));
            ui->table->setItem(row, 1, new QTableWidgetItem(name));
            row++;
        }
    }

    // Vertical header
    ui->table->sortByColumn(1, Qt::AscendingOrder);

    // Colors
    ui->table->colorRows();

    // Switch page
    this->_qStackedWidget->setCurrentWidget(this);
}

QString PageOverview::fillInformation(EltID id, int row)
{
    QStringList info = getInformation(id);

    for (int i = 0; i < info.count(); i++)
    {
        QTableWidgetItem * item = new QTableWidgetItem(info[i]);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        ui->table->setItem(row, i + 2, item);
    }

    // Return the name
    return _sf2->getQstr(id, champ_name);
}

QString PageOverview::getRange(EltID id, Champ champ)
{
    // Global value
    int globalValue = 0;
    if (_sf2->isSet(id, champ))
        globalValue = _sf2->get(id, champ).shValue;

    // Attenuation per division
    int min = -1;
    int max = -1;
    if (id.typeElement == elementInst)
        id.typeElement = elementInstSmpl;
    else
        id.typeElement = elementPrstInst;
    int nbElt = _sf2->count(id);
    for (int i = 0; i < nbElt; i++)
    {
        id.indexElt2 = i;
        if (!_sf2->get(id, champ_hidden).bValue)
        {
            int value = globalValue;
            if (_sf2->isSet(id, champ))
                value = _sf2->get(id, champ).shValue;

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

void PageOverview::on_table_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    QString idStr = ui->table->item(row, 0)->text();
    QStringList listTmp = idStr.split(':');
    EltID id((ElementType)listTmp[0].toInt(), listTmp[1].toInt(), listTmp[2].toInt(), 0, 0);
    _tree->selectNone();
    _tree->select(id, true);
}
