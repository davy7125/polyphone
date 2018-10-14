#ifndef TREEITEMFIRSTLEVEL_H
#define TREEITEMFIRSTLEVEL_H

#include "treeitem.h"
#include "basetypes.h"
class TreeItemRoot;

class TreeItemFirstLevel: public TreeItem
{
public:
    TreeItemFirstLevel(QString label, QMap<int, TreeItem *> * children, TreeItemRoot *parent, EltID id);
    virtual ~TreeItemFirstLevel() {}

    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

private:
    QString _label;
    int _row;
    QMap<int, TreeItem *> * _children;
};

#endif // TREEITEMFIRSTLEVEL_H
