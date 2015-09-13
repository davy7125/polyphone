#include "graphicszoomline.h"
#include <QPainter>

const QPen GraphicsZoomLine::s_penLine = QPen(QColor(220, 30, 30), 2, Qt::DotLine);

GraphicsZoomLine::GraphicsZoomLine(QGraphicsItem *parent)  : QGraphicsItem(parent),
    _x(0),
    _y(0)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void GraphicsZoomLine::setSize(double x, double y)
{
    _x = x;
    _y = y;
}

QRectF GraphicsZoomLine::boundingRect() const
{
    return QRectF(0, 0, _x, _y);
}

void GraphicsZoomLine::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (_x != 0 || _y != 0)
    {
        painter->setPen(s_penLine);
        painter->drawLine(QPointF(0, 0), QPointF(_x, _y));
    }
}
