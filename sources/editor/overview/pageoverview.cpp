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

#include "pageoverview.h"
#include "ui_pageoverview.h"
#include "contextmanager.h"
#include "sortedtablewidgetitem.h"

PageOverview::PageOverview(TypePage typePage, ElementType typeElement, QWidget *parent) :
    Page(parent, typePage, typePage == PAGE_SMPL ? "page:ov_smpl" : (typePage == PAGE_INST ? "page:ov_inst" : "page:ov_prst")),
    ui(new Ui::PageOverview),
    _typeElement(typeElement)
{
    ui->setupUi(this);

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");

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

bool PageOverview::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    Q_UNUSED(editingSource)
    Q_UNUSED(displayOption)

    // Clear the table
    ui->table->clear();
    ui->table->reset();
    ui->table->setColumnCount(0);
    ui->table->setRowCount(0);

    // Number of columns (first is for sorting), header
    QStringList hHeader = getHorizontalHeader();
    hHeader.prepend(getTitle());
    hHeader.prepend("id");
    ui->table->setColumnCount(hHeader.count());
    ui->table->setHorizontalHeaderLabels(hHeader);
    ui->table->setColumnHidden(0, true);

    // Number of rows
    if (!selectedIds.isElementUnique(elementSf2))
        return false;

    EltID id = selectedIds.getFirstId(elementSf2);
    id.typeElement = _typeElement;
    QList<int> indexes = _sf2->getSiblings(id);
    ui->table->setRowCount(indexes.count());
    ui->labelInformation->setText(QString::number(indexes.count()) + " " + (indexes.count() > 1 ? trUtf8("elements") : trUtf8("element")));

    // Preparation (if needed by the overview page)
    this->prepare(id);

    // Fill each row, retrieving the name in the same time
    int row = 0;
    foreach (int i, indexes)
    {
        id.indexElt = i;
        QString name = fillInformation(id, row);
        QString idStr = QString::number(id.typeElement) + ":" +
                QString::number(id.indexSf2) + ":" +
                QString::number(id.indexElt);
        ui->table->setItem(row, 0, new QTableWidgetItem(idStr));
        ui->table->setItem(row, 1, new SortedTableWidgetItem(name, name));
        row++;
    }

    // Vertical header
    ui->table->sortByColumn(1, Qt::AscendingOrder);

    // Colors
    ui->table->colorRows();

    return true;
}

QString PageOverview::fillInformation(EltID id, int row)
{
    QStringList info, order;
    getInformation(id, info, order);

    for (int i = 0; i < info.count(); i++)
    {
        SortedTableWidgetItem * item = new SortedTableWidgetItem(info[i], order[i]);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        ui->table->setItem(row, i + 2, item);
    }

    // Return the name
    return _sf2->getQstr(id, champ_name);
}

QString PageOverview::getRange(bool orderMode, EltID id, AttributeType champ)
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
    foreach (int i, _sf2->getSiblings(id))
    {
        id.indexElt2 = i;
        int value = globalValue;
        if (_sf2->isSet(id, champ))
            value = _sf2->get(id, champ).shValue;

        if (min == -1 || min > value)
            min = value;
        if (max == -1 || max < value)
            max = value;
    }

    QString str;
    if (min == -1)
        str = "?";
    else
    {
        if (orderMode)
            str = QString("%1-%2").arg(min, 8, 10, QChar('0')).arg(max, 8, 10, QChar('0'));
        else
        {
            if (min == max)
                str = QString::number((double)min / 10, 'f', 1);
            else
                str = QString::number((double)min / 10, 'f', 1) + " - " + QString::number((double)max / 10, 'f', 1);
        }
    }
    return str;
}

void PageOverview::on_table_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)

    QString idStr = ui->table->item(row, 0)->text();
    QStringList listTmp = idStr.split(':');
    EltID id((ElementType)listTmp[0].toInt(), listTmp[1].toInt(), listTmp[2].toInt(), 0, 0);
    emit(selectedIdsChanged(IdList(id)));
}

void PageOverview::onShow()
{
    // Nothing special
}
