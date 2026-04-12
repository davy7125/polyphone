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

DirectoryElementListView::DirectoryElementListView(QWidget *parent) : QListView(parent),
    _path(""),
    _type(elementUnknown)
{
    _model = new QStandardItemModel(this);
    _proxy = new QSortFilterProxyModel(this);
    _proxy->setSourceModel(_model);
    _proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    _proxy->setFilterRole(Qt::DisplayRole);
    this->setModel(_proxy);

    this->setItemDelegate(new DirectoryElementListDelegate(this));
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

        _model->appendRow(item);
    }
}

void DirectoryElementListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && _type != elementUnknown)
        emit itemDoubleClicked(_path, EltID(_type, -1, index.data(Qt::UserRole + 3).toInt()));
    QListView::mouseDoubleClickEvent(event);
}

void DirectoryElementListView::setFilter(QString filter)
{
    _proxy->setFilterFixedString(filter);
}