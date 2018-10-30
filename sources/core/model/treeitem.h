#ifndef TREEITEM_H
#define TREEITEM_H

#include <QString>
#include "basetypes.h"
class TreeModel;

class TreeItem
{
public:
    TreeItem(EltID id, TreeItem * parent);
    virtual ~TreeItem();

    // Parent of the item
    TreeItem * parent() { return _parent; }

    // Number of children
    virtual int childCount() const = 0;

    // Find a child
    virtual TreeItem * child(int row) = 0;

    // Current row of the item
    virtual int row() = 0;

    // Position of an id
    virtual int indexOfId(int id) = 0;

    // Data associated to the item
    virtual QString display() = 0;
    EltID getId() { return _id; }
    void setHidden(bool isHidden);
    bool isHidden() { return _isHidden; }

    // Trigger signals for updating the view
    void notifyCreated();
    void notifyRename();
    void notifyDeletion(bool storeExpandedState = true);

protected:
    TreeModel * _model;
    EltID _id;
    TreeItem * _parent;
    bool _isHidden;
};

#endif // TREEITEM_H
