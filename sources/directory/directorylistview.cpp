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

#include "directorylistview.h"
#include "directorylistdelegate.h"
#include "directorylistmodel.h"
#include <QHeaderView>
#include "directorysortproxymodel.h"

DirectoryListView::DirectoryListView(QWidget * parent) : QListView(parent),
    _model(new DirectoryListModel(this)),
    _delegate(new DirectoryListDelegate(this))
{
    // Custom types
    qRegisterMetaType<const DirectoryFileData*>();

    // Proxy / model
    _proxy = new DirectorySortProxyModel(this);
    _proxy->setSourceModel(_model);
    _proxy->sort(0);
    this->setModel(_proxy);

    // Custom delegate
    this->setItemDelegate(_delegate);
}

void DirectoryListView::addFile(DirectoryFileData * fileData)
{
    _model->addFile(fileData);
    emit contentChanged();
}

void DirectoryListView::removeFile(QString filePath)
{
    _model->removeFile(filePath);
    emit contentChanged();
}

void DirectoryListView::updateFile(DirectoryFileData * fileData)
{
    _model->updateFile(fileData);
}

void DirectoryListView::setSortType(int sortType)
{
    _proxy->setSortMode((DirectorySortProxyModel::SortMode)sortType);
}

void DirectoryListView::setFilter(QString filter)
{
    _proxy->setFilter(filter);
    emit contentChanged();
}
