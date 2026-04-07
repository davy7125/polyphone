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

#include "directorytabledelegate.h"
#include "directoryfiledata.h"
#include <QListView>
#include <QApplication>
#include <QStandardItemModel>

void DirectoryTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);
}

QSize DirectoryTableDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(80);
    return s;
}

QWidget * DirectoryTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QListView * editor = new QListView(parent);
    editor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    editor->setModel(new QStandardItemModel(editor));
    connect(editor, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onEditorItemClicked(QModelIndex)));
    return editor;
}

void DirectoryTableDelegate::setEditorData(QWidget * editor, const QModelIndex &index) const
{
    // Editor
    QListView * listView = qobject_cast<QListView *>(editor);
    if (!listView)
        return;

    // Editor model
    QStandardItemModel * listViewModel = qobject_cast<QStandardItemModel*>(listView->model());
    listViewModel->clear();

    // Model data
    const DirectoryFileData * fd = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (!fd)
        return;

    listView->setProperty("column", index.column());
    listView->setProperty("path", fd->getPath());

    // Display options
    int displayOptions = property("displayOptions").toInt();

    DirectoryFileData::DetailsData detailsData;
    switch (index.column())
    {
    case 1:
        detailsData = fd->getSampleDetails(displayOptions);
        break;
    case 2:
        detailsData = fd->getInstrumentDetails(displayOptions);
        break;
    case 3:
        detailsData = fd->getPresetDetails(displayOptions);
        break;
    default:
        break;
    }

    // Fill the model
    for (int i = 0; i < detailsData.texts.size(); ++i)
    {
        QStandardItem * item = new QStandardItem(detailsData.texts[i]);
        item->setData(detailsData.values[i], Qt::UserRole);
        listViewModel->appendRow(item);
    }
}

void DirectoryTableDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void DirectoryTableDelegate::onEditorItemClicked(QModelIndex index)
{
    QListView * listView = qobject_cast<QListView *>(sender());
    if (!listView)
        return;

    int column = listView->property("column").toInt();
    QString path = listView->property("path").toString();
    int id = index.data(Qt::UserRole).toInt();

    emit itemDoubleClicked(path, EltID((ElementType)column, -1, id));
}