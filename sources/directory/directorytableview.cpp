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

#include "directorytableview.h"
#include "directorytabledelegate.h"
#include "directorytablemodel.h"
#include <QHeaderView>
#include "directorysortproxymodel.h"

DirectoryTableView::DirectoryTableView(QWidget * parent) : QTableView(parent),
    _model(new DirectoryTableModel(this)),
    _delegate(new DirectoryTableDelegate(this))
{
    // Custom types
    qRegisterMetaType<const DirectoryFileData*>();

    // Model
    _proxy = new DirectorySortProxyModel(this);
    _proxy->setSourceModel(_model);
    this->setModel(_proxy);

    // Configuration
    this->horizontalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setSectionsClickable(false);
    this->verticalHeader()->hide();

    // Custom delegate
    //this->setItemDelegateForColumn(1, _delegate);
    //this->setItemDelegateForColumn(2, _delegate);
    //this->setItemDelegateForColumn(3, _delegate);
    connect(_delegate, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));

    // Initialize display
    this->resizeRowsToContents();
}

void DirectoryTableView::addFile(DirectoryFileData * fileData)
{
    _model->addFile(fileData);
    this->resizeRowsToContents();
    emit contentChanged();
}

void DirectoryTableView::removeFile(QString filePath)
{
    _model->removeFile(filePath);
    this->resizeRowsToContents();
    emit contentChanged();
}

void DirectoryTableView::updateFile(DirectoryFileData * fileData)
{
    _model->updateFile(fileData);
    this->resizeRowsToContents();
}

void DirectoryTableView::setDisplayOptions(int displayOptions)
{
    _delegate->setProperty("displayOptions", displayOptions);
    this->viewport()->update();
    this->resizeRowsToContents();
}

void DirectoryTableView::setSortType(int sortType)
{
    _proxy->setSortMode((DirectorySortProxyModel::SortMode)sortType);
}

void DirectoryTableView::setFilter(QString filter)
{
    _proxy->setFilter(filter);
    emit contentChanged();
}
