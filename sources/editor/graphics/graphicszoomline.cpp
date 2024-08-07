/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "graphicszoomline.h"
#include <QPainter>
#include <QApplication>
#include <QPalette>
#include "contextmanager.h"

GraphicsZoomLine::GraphicsZoomLine(QGraphicsItem *parent)  : QGraphicsItem(parent),
    _x(0),
    _y(0)
{
    // Pen
    _penLine = QPen(ContextManager::theme()->getFixedColor(ThemeManager::RED, true), 2, Qt::DotLine);

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
        painter->setPen(_penLine);
        painter->drawLine(QPointF(0, 0), QPointF(_x, _y));
    }
}
