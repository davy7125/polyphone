/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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


#include "timbrepalette.h"
#include "contextmanager.h"
#include <QPainterPath>
#include <cmath>

static QColor interpolate(const QList<QColor> &colors, double t)
{
    t = qBound(0.0, t, 1.0) * (colors.count() - 1);
    int i = qMin(static_cast<int>(t), static_cast<int>(colors.count()) - 2);
    double f = t - i;
    const QColor &a = colors[i], &b = colors[i + 1];
    return QColor::fromRgbF(static_cast<float>(a.redF() + f * (b.redF() - a.redF())),
                            static_cast<float>(a.greenF() + f * (b.greenF() - a.greenF())),
                            static_cast<float>(a.blueF() + f * (b.blueF() - a.blueF())));
}

TimbrePalette::TimbrePalette()
{
    ThemeManager * theme = ContextManager::theme();
    _background = theme->getColor(ThemeManager::LIST_BACKGROUND);
    _text = theme->getColor(ThemeManager::LIST_TEXT);
    _dark = theme->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT);
    _neutral = ThemeManager::mix(_background, _text, 0.1);
    _grid = ThemeManager::mix(_background, _text, 0.25);
    _uncovered = ThemeManager::mix(_background, _text, 0.04);
    _hatch = ThemeManager::mix(_background, _text, 0.45);
    _highlight = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Okabe-Ito colors, readable by color blind people, a bit lighter on a dark background
    _conform = _dark ? QColor(80, 200, 150) : QColor(0, 140, 100);
    _redundant = _dark ? QColor(110, 180, 240) : QColor(0, 114, 178);
    _toCorrect = _dark ? QColor(240, 170, 60) : QColor(220, 130, 0);
    _tooDifferent = _dark ? QColor(240, 110, 70) : QColor(213, 94, 0);
    _break = _dark ? QColor(220, 140, 200) : QColor(170, 70, 150);
    _level = _dark ? QColor(240, 228, 66) : QColor(160, 140, 0);
}

QColor TimbrePalette::sequential(double t) const
{
    static const QList<QColor> viridis = {
        QColor(0x44, 0x01, 0x54), QColor(0x48, 0x28, 0x78), QColor(0x3e, 0x49, 0x89), QColor(0x31, 0x68, 0x8e),
        QColor(0x26, 0x82, 0x8e), QColor(0x1f, 0x9e, 0x89), QColor(0x35, 0xb7, 0x79), QColor(0x6e, 0xce, 0x58),
        QColor(0xb5, 0xde, 0x2b), QColor(0xfd, 0xe7, 0x25) };
    return interpolate(viridis, t);
}

QColor TimbrePalette::diverging(double t) const
{
    // Blue - neutral - red (RdBu), the neutral color being the background of the theme
    QList<QColor> negative = { _neutral, _dark ? QColor(80, 150, 210) : QColor(103, 169, 207), QColor(33, 102, 172), QColor(5, 48, 97) };
    QList<QColor> positive = { _neutral, _dark ? QColor(230, 120, 100) : QColor(239, 138, 98), QColor(178, 24, 43), QColor(103, 0, 31) };
    if (_dark)
    {
        // The darkest colors would vanish in the background
        negative[3] = QColor(140, 200, 255);
        positive[3] = QColor(255, 170, 150);
        negative[2] = QColor(60, 130, 210);
        positive[2] = QColor(220, 70, 60);
    }
    t = qBound(-1.0, t, 1.0);
    return t < 0 ? interpolate(negative, -t) : interpolate(positive, t);
}

void TimbrePalette::drawShape(QPainter &painter, Shape shape, QRectF rect, QColor color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    double size = qMin(rect.width(), rect.height());
    QRectF square(rect.center().x() - size / 2, rect.center().y() - size / 2, size, size);
    square.adjust(size * 0.15, size * 0.15, -size * 0.15, -size * 0.15);
    QPen pen(color, qMax(1.5, size * 0.16), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    switch (shape)
    {
    case SHAPE_CHECK: {
        QPainterPath path;
        path.moveTo(square.left(), square.top() + square.height() * 0.55);
        path.lineTo(square.left() + square.width() * 0.38, square.bottom());
        path.lineTo(square.right(), square.top());
        painter.drawPath(path);
    } break;
    case SHAPE_MINUS:
        painter.drawLine(QPointF(square.left(), square.center().y()), QPointF(square.right(), square.center().y()));
        break;
    case SHAPE_TILDE: {
        QPainterPath path;
        double y = square.center().y(), a = square.height() * 0.25;
        path.moveTo(square.left(), y);
        path.cubicTo(square.left() + square.width() * 0.25, y - 2 * a, square.left() + square.width() * 0.5, y,
                     square.center().x(), y);
        path.cubicTo(square.left() + square.width() * 0.5, y, square.left() + square.width() * 0.75, y + 2 * a,
                     square.right(), y);
        painter.drawPath(path);
    } break;
    case SHAPE_CROSS:
        painter.drawLine(square.topLeft(), square.bottomRight());
        painter.drawLine(square.topRight(), square.bottomLeft());
        break;
    case SHAPE_PLUS:
        painter.drawLine(QPointF(square.left(), square.center().y()), QPointF(square.right(), square.center().y()));
        painter.drawLine(QPointF(square.center().x(), square.top()), QPointF(square.center().x(), square.bottom()));
        break;
    case SHAPE_UP: case SHAPE_DOWN: {
        QPolygonF triangle;
        if (shape == SHAPE_UP)
            triangle << QPointF(square.left(), square.bottom()) << QPointF(square.right(), square.bottom())
                     << QPointF(square.center().x(), square.top());
        else
            triangle << QPointF(square.left(), square.top()) << QPointF(square.right(), square.top())
                     << QPointF(square.center().x(), square.bottom());
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPolygon(triangle);
    } break;
    }
    painter.restore();
}
