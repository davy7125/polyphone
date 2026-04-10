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

#include "directorysortproxymodel.h"
#include "directoryfiledata.h"
#include "utils.h"

DirectorySortProxyModel::DirectorySortProxyModel(QObject *parent) : QSortFilterProxyModel{parent},
    _mode(SortByName),
    _filter("")
{}

void DirectorySortProxyModel::setSortMode(SortMode mode)
{
    _mode = mode;
    invalidate();
}

bool DirectorySortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const DirectoryFileData * fdL = sourceModel()->data(left, Qt::UserRole).value<const DirectoryFileData *>();
    const DirectoryFileData * dfR = sourceModel()->data(right, Qt::UserRole).value<const DirectoryFileData *>();
    if (fdL == nullptr || dfR == nullptr)
        return false;
    switch (_mode)
    {
    case SortByName:
        return Utils::naturalOrder(fdL->getFileName(), dfR->getFileName()) < 0;
    case SortByDateDesc:
        return fdL->getLastModified() > dfR->getLastModified();
    case SortBySizeDesc:
        return fdL->getFileSize() > dfR->getFileSize();
    case SortBySizeAsc:
        return fdL->getFileSize() < dfR->getFileSize();
    }

    return false;
}

void DirectorySortProxyModel::setFilter(QString filter)
{
#if QT_VERSION >= 0x060900
    beginFilterChange();
#endif
    _filter = filter;

#if QT_VERSION >= 0x060900
    endFilterChange();
#else
    invalidateFilter();
#endif
}

bool DirectorySortProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (_filter.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    const DirectoryFileData *fd = sourceModel()->data(index, Qt::UserRole).value<const DirectoryFileData *>();
    if (fd == nullptr)
        return false;

    return fd->getFilterResult(_filter);
}
