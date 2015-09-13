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
#include <QTimer>

const int GraphicsKey::s_radius = 5;

GraphicsKey::GraphicsKey(QGraphicsItem *parent) : QObject(NULL), QGraphicsItem(parent),
    _colorBrush(220, 30, 30, 4),
    _colorPen(50, 10, 10, 4)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    QTimer::singleShot(30, this, SLOT(updateColor()));
}

QRectF GraphicsKey::boundingRect() const
{
    return QRectF(-s_radius, -s_radius, 2 * s_radius, 2 * s_radius);
}

void GraphicsKey::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setBrush(QBrush(_colorBrush));
    painter->setPen(QPen(_colorPen));
    painter->drawEllipse(boundingRect());
}

void GraphicsKey::updateColor()
{
    _colorBrush.setAlpha(qMin(255., 5 + 1.5 * _colorBrush.alpha()));
    _colorPen.setAlpha(qMin(255., 5 + 1.5 * _colorPen.alpha()));

    if (_colorBrush.alpha() < 255)
        QTimer::singleShot(30, this, SLOT(updateColor()));

    this->update();
}
