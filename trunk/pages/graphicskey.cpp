/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
