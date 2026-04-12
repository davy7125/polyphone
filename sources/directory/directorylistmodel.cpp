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

#include "directorylistmodel.h"

DirectoryListModel::DirectoryListModel(QObject *parent) : QAbstractListModel(parent) {}

DirectoryListModel::~DirectoryListModel()
{
    qDeleteAll(_files);
}

void DirectoryListModel::addFile(DirectoryFileData * fd)
{
    QString path = fd->getPath();
    if (_files.contains(path))
    {
        // Update it instead
        updateFile(fd);
    }
    else
    {
        beginInsertRows(QModelIndex(), _files.count(), _files.count());
        _files[path] = fd;
        _filePaths.append(path);
        endInsertRows();

        connect(fd, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));
        fd->process();
    }
}

void DirectoryListModel::removeFile(QString filePath)
{
    if (_files.contains(filePath))
    {
        int row = _filePaths.indexOf(filePath);
        beginRemoveRows(QModelIndex(), row, row);
        delete _files.take(filePath);
        _filePaths.removeAll(filePath);
        endRemoveRows();
    }
}

void DirectoryListModel::updateFile(DirectoryFileData * fd)
{
    // Find the corresponding soundfont
    QString path = fd->getPath();
    if (_files.contains(path))
    {
        delete _files[path];
        _files[path] = fd;

        // Notify the change
        int row = _filePaths.indexOf(path);
        QModelIndex topLeft = index(row, 0);
        QModelIndex bottomRight = index(row, 0);
        emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole, Qt::UserRole});

        connect(fd, SIGNAL(processFinished()), this, SLOT(onProcessFinished()));
        fd->process();
    }
    else
    {
        // Add it instead
        addFile(fd);
    }
}

void DirectoryListModel::onProcessFinished()
{
    // Row with complete data
    DirectoryFileData * fd = dynamic_cast<DirectoryFileData *>(QObject::sender());
    QString path = fd->getPath();
    int row = _filePaths.indexOf(path);

    // Notify the change
    QModelIndex topLeft = index(row, 0);
    QModelIndex bottomRight = index(row, 0);
    emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole, Qt::UserRole});
}

int DirectoryListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _filePaths.count();
}

QVariant DirectoryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _files.count())
        return QVariant();

    switch (role)
    {
    case Qt::UserRole:
        return QVariant::fromValue(_files[_filePaths[index.row()]]);
    default:
        break;
    }

    return QVariant();
}
