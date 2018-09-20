/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "widgetshowhistory.h"
#include "ui_widgetshowhistory.h"
#include "widgetshowhistorycell.h"
#include <QDateTime>

WidgetShowHistory::WidgetShowHistory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetShowHistory)
{
    ui->setupUi(this);
}

WidgetShowHistory::~WidgetShowHistory()
{
    delete ui;
}

void WidgetShowHistory::clear()
{
    ui->listWidget->clear();
}

void WidgetShowHistory::addFile(QString path, QDateTime datetime)
{
    WidgetShowHistoryCell * cell = new WidgetShowHistoryCell(ui->listWidget);
    cell->setLink(path);
    cell->setDateTime(datetime);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(cell->width(), cell->height()));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, cell);
}

void WidgetShowHistory::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // Corresponding widget
    WidgetShowHistoryCell * cell = (WidgetShowHistoryCell*)ui->listWidget->itemWidget(item);
    emit(openFile(cell->getLink()));
}

void WidgetShowHistory::on_listWidget_itemSelectionChanged()
{
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        ((WidgetShowHistoryCell*)ui->listWidget->itemWidget(item))->setActive(item->isSelected());
    }
}
