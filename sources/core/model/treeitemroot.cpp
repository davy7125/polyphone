#include "treeitemroot.h"

TreeItemRoot::TreeItemRoot(EltID id) : TreeItem(id, nullptr)
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
    return nullptr;
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

void TreeItemRoot::removeChild(TreeItem * treeItem)
{
    _children.removeAll(treeItem);
}

int TreeItemRoot::indexOfId(int id)
{
    for (int i = 0; i < _children.count(); i++)
        if ((int)_children[i]->getId().typeElement == id)
            return i;
    return -1;
}
