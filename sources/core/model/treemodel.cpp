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

int	TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    TreeItem *parentItem = NULL;
    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if (parentItem != NULL)
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
        if (item != NULL)
            return item->display();
    }
    else if (role == Qt::UserRole)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != NULL)
            return QVariant::fromValue(item->getId());
    }
    else if (role == Qt::UserRole + 1)
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item != NULL)
            return item->isHidden();
    }

    return QVariant();
}

QModelIndex	TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = NULL;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if (parentItem != NULL && parentItem->childCount() > row)
        return createIndex(row, column, parentItem->child(row));

    return QModelIndex();
}

QModelIndex	TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    if (childItem != NULL)
    {
        TreeItem *parentItem = childItem->parent();
        if (parentItem != NULL)
            return createIndex(parentItem->row(), 0, parentItem);
    }

    return QModelIndex();
}

void TreeModel::elementAdded(EltID id)
{
    int position;
    QModelIndex index = getParentIndexWithPosition(id, position);

    //emit(saveExpandedState());
    emit(beginInsertRows(index, position, position));
    emit(endInsertRows());
    //emit(restoreExpandedState());
}

void TreeModel::elementUpdated(EltID id)
{
    int position;
    QModelIndex index = getParentIndexWithPosition(id, position);
    index = this->index(position, 0, index);

    _indexToChange << index;

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
                    _indexToChange << index;
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
                    _indexToChange << index;
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
    _indexToChange << index << indexParent;
}

QModelIndex TreeModel::getParentIndexWithPosition(EltID id, int &position)
{
    // Find the corresponding parent index of an id
    QModelIndex index = QModelIndex();
    TreeItem * item;
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
    default:
        return QModelIndex();
    }

    return index;
}

void TreeModel::triggerUpdate()
{
    if (!_indexToChange.empty())
    {
        emit(saveExpandedState());
        while (!_indexToChange.empty())
        {
            QModelIndex index = _indexToChange.takeFirst();
            emit(dataChanged(index, index));
        }
        emit(restoreExpandedState());
    }
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    if (childItem != NULL)
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
