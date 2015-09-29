#ifndef TABLEDELEGATE_H
#define TABLEDELEGATE_H

#include <QStyledItemDelegate>
class QTableWidget;

// Redéfinition des éditeurs au sein de la table
class TableDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TableDelegate(QTableWidget * table, QObject * parent = NULL): QStyledItemDelegate(parent),
        _table(table)
    {}

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void destroyEditor(QWidget * editor, const QModelIndex & index) const;
#endif

private:
    void getType(bool &isNumeric, bool &isKey, int &nbDecimales, int numRow, bool &isLoop) const;
    QTableWidget * _table;

    static const char * DECO_PROPERTY;
};

#endif // TABLEDELEGATE_H
