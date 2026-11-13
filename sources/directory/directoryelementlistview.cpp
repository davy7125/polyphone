/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "directoryelementlistview.h"
#include <QMouseEvent>
#include <QStandardItemModel>
#include "directoryelementlistdelegate.h"
#include "utils.h"

DirectoryElementListView::DirectoryElementListView(QWidget *parent) : QListView(parent),
    _delegate(new DirectoryElementListDelegate(this)),
    _detailsDisplayed(true),
    _path(""),
    _type(elementUnknown)
{
    _model = new QStandardItemModel(this);
    _proxy = new QSortFilterProxyModel(this);
    _proxy->setSourceModel(_model);
    _proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxy->setFilterRole(Qt::UserRole + 4);
    this->setModel(_proxy);

    this->setItemDelegate(_delegate);
    connect(this, &QListView::activated, this, &DirectoryElementListView::onActivated);
}

void DirectoryElementListView::clear()
{
    _model->clear();
    _path = "";
    _type = elementUnknown;
}

void DirectoryElementListView::setData(DirectoryFileData::DetailsData data, QString path, ElementType type)
{
    _path = path;
    _type = type;

    _model->clear();
    for (int i = 0; i < data.names.size(); ++i)
    {
        QStandardItem *item = new QStandardItem();
        item->setData(data.names[i], Qt::DisplayRole);

        // Details
        QString tmp = data.details[i];
        item->setData(tmp, Qt::UserRole + 1);
        item->setData(tmp.count('\n') + 1, Qt::UserRole + 2);
        item->setData(data.values[i], Qt::UserRole + 3);

        // Text for the filter, whatever the number of spaces
        item->setData(Utils::simplifySpaces(data.names[i]), Qt::UserRole + 4);

        // Number and name, for the display of the virtual keyboard
        item->setData(data.numbers.value(i), Qt::UserRole + 5);
        item->setData(data.elementNames.value(i), Qt::UserRole + 6);

        _model->appendRow(item);
    }
}

EltID DirectoryElementListView::getSelectedElement()
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.isEmpty() || _type == elementUnknown)
        return EltID(elementUnknown);
    return EltID(_type, -1, indexes[0].data(Qt::UserRole + 3).toInt());
}

int DirectoryElementListView::getSelectedRow()
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.isEmpty() || _type == elementUnknown)
        return -1;
    return indexes[0].row();
}

int DirectoryElementListView::getRowCount()
{
    return _proxy->rowCount();
}

void DirectoryElementListView::getSelectedTexts(QString &number, QString &name)
{
    QModelIndexList indexes = this->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
    {
        number = name = "";
        return;
    }
    number = indexes[0].data(Qt::UserRole + 5).toString();
    name = indexes[0].data(Qt::UserRole + 6).toString();
}

void DirectoryElementListView::selectRow(int row)
{
    QModelIndex index = _proxy->index(row, 0);
    if (!index.isValid())
        return;
    this->setCurrentIndex(index);
    this->scrollTo(index);
}

void DirectoryElementListView::setDetailsDisplayed(bool displayed)
{
    if (displayed == _detailsDisplayed)
        return;

    // The row height depends on the details
    _detailsDisplayed = displayed;
    _delegate->setDetailsDisplayed(displayed);
    this->scheduleDelayedItemsLayout();
    this->viewport()->update();
}

void DirectoryElementListView::onActivated(const QModelIndex &index)
{
    if (index.isValid() && _type != elementUnknown)
        emit itemDoubleClicked(_path, EltID(_type, -1, index.data(Qt::UserRole + 3).toInt()));
}

void DirectoryElementListView::setFilter(QString filter)
{
    _proxy->setFilterFixedString(Utils::simplifySpaces(filter));
}