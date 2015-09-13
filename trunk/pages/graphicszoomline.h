#ifndef GRAPHICSZOOMLINE_H
#define GRAPHICSZOOMLINE_H

#include <QGraphicsItem>
#include <QPen>

class GraphicsZoomLine : public QGraphicsItem
{
public:
    GraphicsZoomLine(QGraphicsItem *parent = NULL);

    void setSize(double x, double y);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);

private:
    double _x, _y;
    static const QPen s_penLine;
};

#endif // GRAPHICSZOOMLINE_H
