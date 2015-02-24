/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "rectangleitem.h"
#include <QApplication>
#include <QGraphicsScene>

RectangleItem::RectangleItem(const QRectF &rect, QGraphicsItem *parent) :
    QGraphicsRectItem(rect, parent)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable);
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant RectangleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // Offset made by integers
        QPointF offset = value.toPointF();
        offset.setX(qRound(offset.x()));
        offset.setY(qRound(offset.y()));

        // Keep item in scene rect
        offset.setX(qMax(offset.x(), scene()->sceneRect().left() - rect().left()));
        offset.setX(qMin(offset.x(), scene()->sceneRect().right() - rect().right()));
        offset.setY(qMax(offset.y(), scene()->sceneRect().top() - rect().top()));
        offset.setY(qMin(offset.y(), scene()->sceneRect().bottom() - rect().bottom()));

        return offset;
    }

    return QGraphicsItem::itemChange(change, value);
}
