#ifndef TREEITEMROOT_H
#define TREEITEMROOT_H

#include "treeitem.h"
#include "sf2_types.h"
#include <QList>
class TreeModel;

class TreeItemRoot: public TreeItem
{
public:
    TreeItemRoot(EltID id);
    virtual ~TreeItemRoot() {}

    int childCount() const override;
    TreeItem * child(int row) override;
    QString display() override;
    int row() override;
    int indexOfId(int id) override;

    int addChild(TreeItem * treeItem);
    void attachModel(TreeModel * model) { this->_model = model; }

private:
    QList<TreeItem * > _children;
};

#endif // TREEITEMROOT_H
