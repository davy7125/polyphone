#include "treeitemfirstlevel.h"
#include "treeitemroot.h"
#include <QMap>

TreeItemFirstLevel::TreeItemFirstLevel(QString label, QMap<int, TreeItem *> *children, TreeItemRoot *parent, EltID id) : TreeItem(id, parent),
    _label(label),
    _children(children)
{
    _row = parent->addChild(this);
}

int TreeItemFirstLevel::childCount() const
{
    if (_children == NULL)
        return 0;
    return _children->count();
}

TreeItem * TreeItemFirstLevel::child(int row)
{
    if (_children == NULL || row < _children->count())
        return _children->values()[row];
    return NULL;
}

QString TreeItemFirstLevel::display()
{
    return _label;
}

int TreeItemFirstLevel::row()
{
    return _row;
}

int TreeItemFirstLevel::indexOfId(int id)
{
    if (_children == NULL)
        return -1;
    return _children->keys().indexOf(id);
}
