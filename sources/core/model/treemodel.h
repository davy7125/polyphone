#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include "basetypes.h"
class TreeItem;

class TreeModel: public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(TreeItem * rootItem);

    int columnCount(const QModelIndex &parent) const override;
    int	rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    /// Drag & drop
    Qt::DropActions supportedDropActions() const override { return Qt::MoveAction; }
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /// Tree update
    void elementBeingAdded(EltID id);
    void endOfAddition();
    void elementUpdated(EltID id);
    void elementBeingDeleted(EltID id, bool storeExpandedState);
    void endOfDeletion();
    void visibilityChanged(EltID id);

signals:
    void saveExpandedState();
    void restoreExpandedState();

private:
    QModelIndex getParentIndexWithPosition(EltID id, int &position);
    TreeItem * _rootItem;
};

#endif // TREEMODEL_H
