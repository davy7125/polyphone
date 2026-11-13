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
#include "directorylistview.h"
#include "utils.h"

DirectorySortProxyModel::DirectorySortProxyModel(QObject *parent) : QSortFilterProxyModel{parent},
    _column(DirectoryListView::COLUMN_NAME),
    _filter("")
{}

void DirectorySortProxyModel::setSortColumn(int column, bool descending)
{
    _column = column;
    Qt::SortOrder order = descending ? Qt::DescendingOrder : Qt::AscendingOrder;
    if (order == sortOrder())
        invalidate(); // sort() would do nothing
    else
        sort(0, order);
}

bool DirectorySortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const DirectoryFileData * fdL = sourceModel()->data(left, Qt::UserRole).value<const DirectoryFileData *>();
    const DirectoryFileData * dfR = sourceModel()->data(right, Qt::UserRole).value<const DirectoryFileData *>();
    if (fdL == nullptr || dfR == nullptr)
        return false;

    bool ascending = (sortOrder() == Qt::AscendingOrder);
    switch (_column)
    {
    case DirectoryListView::COLUMN_SIZE:
        if (fdL->getFileSize() != dfR->getFileSize())
            return fdL->getFileSize() < dfR->getFileSize();
        break;
    case DirectoryListView::COLUMN_DATE:
        if (fdL->getLastModified() != dfR->getLastModified())
            return fdL->getLastModified() < dfR->getLastModified();
        break;
    case DirectoryListView::COLUMN_SAMPLES:
    case DirectoryListView::COLUMN_INSTRUMENTS:
    case DirectoryListView::COLUMN_PRESETS: {
        // Files without numbers are at the end, whatever the order
        bool hasCountL = (fdL->getStatus() == DirectoryFileData::OK);
        bool hasCountR = (dfR->getStatus() == DirectoryFileData::OK);
        if (hasCountL != hasCountR)
            return ascending ? hasCountL : hasCountR;
        if (hasCountL && getCount(fdL, _column) != getCount(dfR, _column))
            return getCount(fdL, _column) < getCount(dfR, _column);
    } break;
    default:
        break;
    }

    // Sort by name, or names A→Z for equal values whatever the order
    // Extensions are compared last so that "Piano.sf2" is before "Piano (1).sf2"
    int nameOrder = Utils::naturalOrder(fdL->getBaseName(), dfR->getBaseName());
    if (nameOrder == 0)
        nameOrder = Utils::naturalOrder(fdL->getSuffix(), dfR->getSuffix());
    return (_column == DirectoryListView::COLUMN_NAME || ascending) ? nameOrder < 0 : nameOrder > 0;
}

int DirectorySortProxyModel::getCount(const DirectoryFileData * fd, int column)
{
    switch (column)
    {
    case DirectoryListView::COLUMN_SAMPLES:
        return fd->getSampleCount();
    case DirectoryListView::COLUMN_INSTRUMENTS:
        return fd->getInstrumentCount();
    case DirectoryListView::COLUMN_PRESETS:
        return fd->getPresetCount();
    default:
        break;
    }
    return 0;
}

void DirectorySortProxyModel::setFilter(QString filter)
{
#if QT_VERSION >= 0x060900
    beginFilterChange();
#endif
    _filter = Utils::simplifySpaces(filter);

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
