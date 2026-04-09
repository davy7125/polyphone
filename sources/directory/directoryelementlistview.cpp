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
#include <QStringListModel>
#include <QMouseEvent>

DirectoryElementListView::DirectoryElementListView(QWidget *parent) : QListView(parent),
    _path(""),
    _type(elementUnknown)
{
    this->setModel(new QStringListModel());
}

void DirectoryElementListView::clear()
{
    QStringListModel * model = (QStringListModel *)this->model();
    model->setStringList(QStringList());
    _path = "";
    _type = elementUnknown;
}

void DirectoryElementListView::setData(DirectoryFileData::DetailsData data, QString path, ElementType type)
{
    _path = path;
    _type = type;
    QStringListModel * model = (QStringListModel *)this->model();
    model->setStringList(data.names);
    _values = data.values;
}

void DirectoryElementListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && _type != elementUnknown && index.row() < _values.count())
        emit itemDoubleClicked(_path, EltID(_type, -1, _values[index.row()]));
    QListView::mouseDoubleClickEvent(event);
}