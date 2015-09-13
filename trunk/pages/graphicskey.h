#ifndef GRAPHICSKEY_H
#define GRAPHICSKEY_H

#include <QGraphicsItem>
#include <QBrush>
#include <QPen>

class GraphicsKey : public QGraphicsItem
{
public:
    GraphicsKey(QGraphicsItem *parent = NULL);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);

private:
    static const int RADIUS;
    static const QBrush BRUSH;
    static const QPen PEN;
};

#endif // GRAPHICSKEY_H
