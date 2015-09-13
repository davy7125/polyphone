#include "graphicskey.h"
#include <QPainter>

const int GraphicsKey::RADIUS = 5;
const QBrush GraphicsKey::BRUSH = QBrush(QColor(220, 30, 30));
const QPen GraphicsKey::PEN = QPen(QColor(50, 10, 10));

GraphicsKey::GraphicsKey(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

QRectF GraphicsKey::boundingRect() const
{
    return QRectF(-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
}

void GraphicsKey::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setBrush(BRUSH);
    painter->setPen(PEN);
    painter->drawEllipse(boundingRect());
}
