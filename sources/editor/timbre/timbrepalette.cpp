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

static double relativeLuminance(const QColor &color)
{
    auto linear = [](double value) {
        return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * linear(color.redF()) + 0.7152 * linear(color.greenF()) + 0.0722 * linear(color.blueF());
}

static double contrastRatio(const QColor &color1, const QColor &color2)
{
    double l1 = relativeLuminance(color1);
    double l2 = relativeLuminance(color2);
    return (qMax(l1, l2) + 0.05) / (qMin(l1, l2) + 0.05);
}

TimbrePalette::TimbrePalette(Surface surface)
{
    ThemeManager * theme = ContextManager::theme();
    ThemeManager::ColorType backgroundType = surface == SURFACE_WINDOW ?
                ThemeManager::WINDOW_BACKGROUND : ThemeManager::LIST_BACKGROUND;
    ThemeManager::ColorType textType = surface == SURFACE_WINDOW ? ThemeManager::WINDOW_TEXT : ThemeManager::LIST_TEXT;
    _background = theme->getColor(backgroundType);
    _text = theme->getColor(textType);
    _dark = theme->isDark(backgroundType, textType);
    _neutral = ThemeManager::mix(_background, _text, 0.1);
    _grid = ThemeManager::mix(_background, _text, 0.25);
    _uncovered = ThemeManager::mix(_background, _text, 0.04);
    _hatch = ThemeManager::mix(_background, _text, 0.45);
    _highlight = theme->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Deviations (RdBu): from the neutral color to colors more and more contrasted with the background, darker on a
    // light background and lighter on a dark one (as the shades of grey)
    if (_dark)
    {
        _negativeStops = { _neutral, QColor(40, 70, 120), QColor(60, 130, 210), QColor(150, 200, 255) };
        _positiveStops = { _neutral, QColor(120, 45, 40), QColor(220, 70, 60), QColor(255, 165, 145) };
    }
    else
    {
        _negativeStops = { _neutral, QColor(103, 169, 207), QColor(33, 102, 172), QColor(5, 48, 97) };
        _positiveStops = { _neutral, QColor(239, 138, 98), QColor(178, 24, 43), QColor(103, 0, 31) };
    }

    // Labels, drawn as thin shapes: a light red less visible than the red on both backgrounds
    _grey = ThemeManager::mix(_background, _text, 0.5);
    _lightRed = _dark ? QColor(200, 110, 95) : QColor(239, 138, 98);
    _red = _dark ? QColor(255, 105, 85) : QColor(178, 24, 43);
    _blue = _dark ? QColor(110, 170, 240) : QColor(33, 102, 172);
}

QColor TimbrePalette::magnitude(double t) const
{
    return ThemeManager::mix(_background, _text, 0.1 + 0.75 * qBound(0.0, t, 1.0));
}

QColor TimbrePalette::diverging(double t) const
{
    // Blue - neutral - red (RdBu), the neutral color being close to the background of the theme
    t = qBound(-1.0, t, 1.0);
    return t < 0 ? interpolate(_negativeStops, -t) : interpolate(_positiveStops, t);
}

QColor TimbrePalette::readable(QColor color, double minimumContrast) const
{
    QColor limit = _dark ? QColor(Qt::white) : QColor(Qt::black);
    QColor result = color;
    for (int step = 1; step <= 20 && contrastRatio(result, _background) < minimumContrast; step++)
        result = ThemeManager::mix(color, limit, 0.05 * step);
    return result;
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
