#include "treeitemroot.h"

TreeItemRoot::TreeItemRoot(EltID id) : TreeItem(id, NULL)
{

}

int TreeItemRoot::childCount() const
{
    return _children.count();
}

TreeItem * TreeItemRoot::child(int row)
{
    if (row < _children.count())
        return _children[row];
    return NULL;
}

QString TreeItemRoot::display()
{
    return "";
}

int TreeItemRoot::row()
{
    return 0;
}

int TreeItemRoot::addChild(TreeItem * treeItem)
{
    _children.append(treeItem);
    return _children.count() - 1;
}

int TreeItemRoot::indexOfId(int id)
{
    Q_UNUSED(id)
    return -1;
}
