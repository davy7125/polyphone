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

#include "graphicslegenditem2.h"
#include <QPainter>
#include "contextmanager.h"

const int GraphicsLegendItem2::s_border = 5;

GraphicsLegendItem2::GraphicsLegendItem2(QString fontFamily, QGraphicsItem *parent) : QGraphicsItem(parent),
    _font(fontFamily, 8),
    _offsetY(0)
{
    // Colors
    QColor color = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    _borderPen = QPen(color, 1, Qt::SolidLine);
    color.setAlpha(200);
    _foregroundBrush = QBrush(color);
    _textPen = QPen(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT), 1, Qt::SolidLine);

    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setLeft(true);
}

void GraphicsLegendItem2::setLeft(bool isLeft)
{
    if (isLeft)
        _alignment = Qt::AlignLeft | Qt::AlignTop;
    else
        _alignment = Qt::AlignRight | Qt::AlignTop;
}

void GraphicsLegendItem2::setNewValues(int minKey, int maxKey, int minVel, int maxVel)
{
    _text.clear();
    if (minKey != -1)
    {
        _text << QObject::tr("Key range:") + " " +
                 ContextManager::keyName()->getKeyName(static_cast<unsigned int>(minKey)) + " - " +
                 ContextManager::keyName()->getKeyName(static_cast<unsigned int>(maxKey))
              << QObject::tr("Velocity range:") + " " +
                 QString::number(minVel) + " - " +
                 QString::number(maxVel);
    }
}

QRectF GraphicsLegendItem2::boundingRect() const
{
    QSizeF size = getTextSize();
    return QRectF(QPointF(dx(size), dy(size)), size);
}

void GraphicsLegendItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (_text.isEmpty())
        return;

    // Translation of the painter
    QSizeF size = getTextSize();
    painter->translate(dx(size), dy(size));

    // Paint foreground and border
    painter->setBrush(_foregroundBrush);
    painter->setPen(_borderPen);
    painter->drawRect(QRectF(QPoint(0, 0), size));

    // Paint text
    painter->setPen(_textPen);
    painter->setFont(_font);
    QFontMetrics fm(_font);
    for (int i = 0; i < _text.count(); i++)
        painter->drawText(QPoint(s_border, fm.height() * (i + 1)), _text.at(i));
}

QSizeF GraphicsLegendItem2::getTextSize() const
{
    if (_text.isEmpty())
        return QSizeF(0, 0);

    QFontMetrics fm(_font);

    int height = _text.count() * fm.height() + 2 * s_border;
    int width = 0;
    foreach (QString line, _text)
        width = qMax(width, fm.horizontalAdvance(line));
    width += 2 * s_border;

    return QSizeF(width, height);
}

qreal GraphicsLegendItem2::dx(QSizeF size) const
{
    if (_alignment & Qt::AlignHCenter)
        return -size.width() / 2;
    else if (_alignment & Qt::AlignRight)
        return -size.width();
    return 0;
}

qreal GraphicsLegendItem2::dy(QSizeF size) const
{
    double value = _offsetY;

    if (_alignment & Qt::AlignVCenter)
        value -= size.height() / 2;
    else if (_alignment & Qt::AlignBottom)
        value -= size.height();

    return value;
}
