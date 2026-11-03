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

#include <QHeaderView>
#include <QMouseEvent>
#include <QTimer>
#include "directorylistview.h"
#include "directorylistdelegate.h"
#include "directorylistmodel.h"
#include "directorysortproxymodel.h"
#include "contextmanager.h"

const int DirectoryListView::COLUMN_SPACING = 2 * DirectoryListDelegate::MARGIN;

DirectoryListView::DirectoryListView(QWidget * parent) : QListView(parent),
    _model(new DirectoryListModel(this)),
    _delegate(new DirectoryListDelegate(this)),
    _timerContentWidths(new QTimer(this)),
    _displayOptions(0x3F)
{
    // Custom types, configuration
    qRegisterMetaType<const DirectoryFileData*>();
    setMouseTracking(true);
    this->viewport()->installEventFilter(_delegate); // Tooltips

    // Proxy / model
    _proxy = new DirectorySortProxyModel(this);
    _proxy->setSourceModel(_model);
    this->setModel(_proxy);
    _proxy->sort(0);

    // Custom delegate
    this->setItemDelegate(_delegate);
    connect(_delegate, SIGNAL(renameRequested(QString)), this, SIGNAL(renameRequested(QString)));
    connect(_delegate, SIGNAL(deleteRequested(QString)), this, SIGNAL(deleteRequested(QString)));

    // Column widths, computed at most every 100 ms while the files are being analyzed
    for (int i = 0; i < COLUMN_NUMBER; i++)
    {
        _minimumWidths[i] = 0;
        _contentWidths[i] = 0;
    }
    _timerContentWidths->setSingleShot(true);
    _timerContentWidths->setInterval(100);
    connect(_timerContentWidths, SIGNAL(timeout()), this, SLOT(updateContentWidths()));
    connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onModelChanged()));
    connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(onModelChanged()));
    connect(_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelChanged()));

    // Initial sort (formerly a single choice: name, newest first, largest first, smallest first)
    int sortColumn = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort_column", -1).toInt();
    bool sortDescending = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort_descending", false).toBool();
    if (sortColumn < 0 || sortColumn >= COLUMN_NUMBER)
    {
        switch (ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort", 0).toInt())
        {
        case 1:
            sortColumn = COLUMN_DATE;
            sortDescending = true;
            break;
        case 2:
            sortColumn = COLUMN_SIZE;
            sortDescending = true;
            break;
        case 3:
            sortColumn = COLUMN_SIZE;
            sortDescending = false;
            break;
        default:
            sortColumn = COLUMN_NAME;
            sortDescending = false;
            break;
        }
    }
    setSort(sortColumn, sortDescending);
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

bool DirectoryListView::isPresetSummaryDisplayed() const
{
    return _displayOptions & 0x01;
}

bool DirectoryListView::isColumnDisplayed(int column) const
{
    return column == COLUMN_NAME || (_displayOptions & (0x01 << column));
}

void DirectoryListView::setDisplayOptions(int displayOptions)
{
    if (displayOptions == _displayOptions)
        return;

    // The row height depends on the preset summary
    bool presetSummaryChanged = (displayOptions & 0x01) != (_displayOptions & 0x01);
    _displayOptions = displayOptions;
    if (presetSummaryChanged)
        this->scheduleDelayedItemsLayout();
    this->viewport()->update();
    emit columnsChanged();
}

void DirectoryListView::getColumnLimits(int column, int totalWidth, int &left, int &right) const
{
    right = totalWidth - DirectoryListDelegate::MARGIN;
    for (int i = COLUMN_NUMBER - 1; i > COLUMN_NAME; i--)
    {
        if (!isColumnDisplayed(i))
            continue;

        left = right - qMax(_minimumWidths[i], _contentWidths[i]);
        if (i == column)
            return;
        right = left - COLUMN_SPACING;
    }

    left = DirectoryListDelegate::MARGIN;
    if (right < left)
        right = left;
}

void DirectoryListView::setMinimumColumnWidth(int column, int width)
{
    _minimumWidths[column] = width;
    this->viewport()->update();
    emit columnsChanged();
}

int DirectoryListView::getSortColumn() const
{
    return _proxy->getSortColumn();
}

bool DirectoryListView::isSortDescending() const
{
    return _proxy->sortOrder() == Qt::DescendingOrder;
}

void DirectoryListView::toggleSort(int column)
{
    if (column == getSortColumn())
        setSort(column, !isSortDescending());
    else
        setSort(column, column != COLUMN_NAME); // The largest sizes / numbers or the newest files first

    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort_column", column);
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_sort_descending", isSortDescending());

    // Keep the current file visible
    if (currentIndex().isValid())
        this->scrollTo(currentIndex());
}

void DirectoryListView::setSort(int column, bool descending)
{
    _proxy->setSortColumn(column, descending);
    emit columnsChanged();
}

void DirectoryListView::setFilter(QString filter)
{
    _proxy->setFilter(filter);
    emit contentChanged();
}

void DirectoryListView::onModelChanged()
{
    if (!_timerContentWidths->isActive())
        _timerContentWidths->start();
}

void DirectoryListView::updateContentWidths()
{
    int contentWidths[COLUMN_NUMBER];
    for (int i = 0; i < COLUMN_NUMBER; i++)
        contentWidths[i] = 0;

    QFontMetrics fm(this->font());
    for (int row = 0; row < _model->rowCount(); row++)
    {
        const DirectoryFileData * d = _model->index(row, 0).data(Qt::UserRole).value<const DirectoryFileData *>();
        if (d == nullptr)
            continue;
        for (int i = COLUMN_NAME + 1; i < COLUMN_NUMBER; i++)
            contentWidths[i] = qMax(contentWidths[i], fm.horizontalAdvance(_delegate->getText(d, i)));
    }

    bool changed = false;
    for (int i = 0; i < COLUMN_NUMBER; i++)
    {
        if (contentWidths[i] != _contentWidths[i])
        {
            _contentWidths[i] = contentWidths[i];
            changed = true;
        }
    }

    if (changed)
    {
        this->viewport()->update();
        emit columnsChanged();
    }
}

void DirectoryListView::resizeEvent(QResizeEvent *event)
{
    // Resize of the viewport, including when the scroll bar appears or disappears
    QListView::resizeEvent(event);
    emit columnsChanged();
}

void DirectoryListView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F2 || event->key() == Qt::Key_Delete)
    {
        QModelIndex index = currentIndex();
        if (index.isValid())
        {
            const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
            if (d != nullptr)
            {
                switch (event->key())
                {
                case Qt::Key_F2:
                    emit renameRequested(d->getPath());
                    return;
                case Qt::Key_Delete:
                    emit deleteRequested(d->getPath());
                    return;
                default:
                    break;
                }
            }
        }
    }

    QListView::keyPressEvent(event);
}
