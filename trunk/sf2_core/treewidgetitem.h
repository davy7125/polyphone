#ifndef TREEWIDGETITEM_H
#define TREEWIDGETITEM_H

#include <QTreeWidgetItem>

class TreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit TreeWidgetItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    explicit TreeWidgetItem(QTreeWidget *view, int type = Type) : QTreeWidgetItem(view, type) {}

private:
    bool operator < (const QTreeWidgetItem &other)const;
    bool compareNames(const QString& s1,const QString& s2);
};

#endif // TREEWIDGETITEM_H
