#ifndef TREEITEMDELEGATE_H
#define TREEITEMDELEGATE_H

#include <QStyledItemDelegate>

class TreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    TreeItemDelegate(QObject *parent);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    bool helpEvent(QHelpEvent* e, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index);

private:
    void drawRoot(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, bool expandable) const;
    void drawElement(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, QString iconName, bool expandable, bool withIndent) const;

    class Icons
    {
    public:
        QPixmap getPixmap(QString name, int colorType);

    private:
        QMap<QString, QPixmap> _pixmaps;
    };

    class Colors
    {
    public:
        Colors();

        QColor _borderColor;
        QColor _alternateColor;
        QColor _textColor;
        QColor _highlightColorBackground;
        QColor _highlightColorText;
    };

    static Icons * s_icons;
    static Colors * s_colors;
    static const int MARGIN;
};

#endif // TREEITEMDELEGATE_H
