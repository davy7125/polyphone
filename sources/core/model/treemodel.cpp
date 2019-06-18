/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "treemodel.h"
#include "treeitem.h"
#include "soundfontmanager.h"

TreeModel::TreeModel(TreeItem * rootItem) : QAbstractItemModel(),
    _rootItem(rootItem)
{

}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem *parentItem = nullptr;
    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if (parentItem != nullptr)
        return parentItem->childCount();
    return 0;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != nullptr)
            return item->display();
    }
    else if (role == Qt::UserRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != nullptr)
            return QVariant::fromValue(item->getId());
    }
    else if (role == Qt::UserRole + 1)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != nullptr)
            return item->isHidden();
    }
    if (role == Qt::UserRole + 2)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != nullptr)
            return item->sortText();
    }

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = nullptr;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if (parentItem != nullptr && parentItem->childCount() > row)
        return createIndex(row, column, parentItem->child(row));

    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    if (childItem != nullptr)
    {
        TreeItem *parentItem = childItem->parent();
        if (parentItem == _rootItem)
            return QModelIndex();
        if (parentItem != nullptr)
            return createIndex(parentItem->row(), 0, parentItem);
    }

    return QModelIndex();
}

void TreeModel::elementBeingAdded(EltID id)
{
    int position;
    QModelIndex index = getParentIndexWithPosition(id, position);
    emit(saveExpandedState());
    emit(beginInsertRows(index, position, position));
}

void TreeModel::endOfAddition()
{
    emit(endInsertRows());
    emit(restoreExpandedState());
}

void TreeModel::elementUpdated(EltID id)
{
    int position;
    QModelIndex index = getParentIndexWithPosition(id, position);
    index = this->index(position, 0, index);

    emit(dataChanged(index, index));

    // Possibly update the name of the linked elements
    if (id.typeElement == elementSmpl)
    {
        SoundfontManager * sf2 = SoundfontManager::getInstance();

        EltID id2 = id;
        id2.typeElement = elementInst;
        foreach (int i, sf2->getSiblings(id2)) // Scan all instruments
        {
            id2.indexElt = i;
            EltID id3 = id2;
            id3.typeElement = elementInstSmpl;
            foreach (int j, sf2->getSiblings(id3)) // Scan all samples linked
            {
                id3.indexElt2 = j;
                if (sf2->get(id3, champ_sampleID).wValue == id.indexElt)
                {
                    index = getParentIndexWithPosition(id3, position);
                    index = this->index(position, 0, index);
                    emit(dataChanged(index, index));
                }
            }
        }
    }
    else if (id.typeElement == elementInst)
    {
        SoundfontManager * sf2 = SoundfontManager::getInstance();

        EltID id2 = id;
        id2.typeElement = elementPrst;
        foreach (int i, sf2->getSiblings(id2)) // Scan all presets
        {
            id2.indexElt = i;
            EltID id3 = id2;
            id3.typeElement = elementPrstInst;
            foreach (int j, sf2->getSiblings(id3)) // Scan all instruments linked
            {
                id3.indexElt2 = j;
                if (sf2->get(id3, champ_instrument).wValue == id.indexElt)
                {
                    index = getParentIndexWithPosition(id3, position);
                    index = this->index(position, 0, index);
                    emit(dataChanged(index, index));
                }
            }
        }
    }
}

void TreeModel::elementBeingDeleted(EltID id, bool storeExpandedState)
{
    int position;
    QModelIndex index = getParentIndexWithPosition(id, position);

    if (storeExpandedState)
        emit(saveExpandedState());
    emit(beginRemoveRows(index, position, position));
}

void TreeModel::endOfDeletion()
{
    emit(endRemoveRows());
    emit(restoreExpandedState());
}

void TreeModel::visibilityChanged(EltID id)
{
    int position;
    QModelIndex indexParent = getParentIndexWithPosition(id, position);
    QModelIndex index = this->index(position, 0, indexParent);
    emit(dataChanged(index, index));
}

QModelIndex TreeModel::getParentIndexWithPosition(EltID id, int &position)
{
    // Find the corresponding parent index of an id
    QModelIndex index = QModelIndex();
    TreeItem * item = nullptr;
    switch (id.typeElement)
    {
    case elementSmpl:
        index = this->index(1, 0);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt);
        break;
    case elementInst:
        index = this->index(2, 0);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt);
        break;
    case elementPrst:
        index = this->index(3, 0);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt);
        break;
    case elementInstSmpl:
        index = this->index(2, 0);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt);
        index = this->index(position, 0, index);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt2);
        break;
    case elementPrstInst:
        index = this->index(3, 0);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt);
        index = this->index(position, 0, index);
        item = (TreeItem*)index.internalPointer();
        position = item->indexOfId(id.indexElt2);
        break;
    case elementSf2: case elementRootSmpl: case elementRootInst: case elementRootPrst:
        position = _rootItem->indexOfId((int)id.typeElement);
        break;
    default:
        return QModelIndex();
    }

    // If -1, the element is going to be added at the end
    if (position == -1 && item != nullptr)
        position = item->childCount();

    return index;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    if (childItem != nullptr)
    {
        EltID id = childItem->getId();
        switch (id.typeElement)
        {
        case elementSmpl:
            return Qt::ItemIsDragEnabled | defaultFlags;
        case elementInst: case elementInstSmpl:
        case elementPrst: case elementPrstInst:
            return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
        default:
            break;
        }
    }

    return defaultFlags;
}
