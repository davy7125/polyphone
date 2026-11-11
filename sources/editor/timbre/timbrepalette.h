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


#ifndef TIMBREPALETTE_H
#define TIMBREPALETTE_H

#include <QColor>
#include <QPainter>

// Colors of the timbre view: perceptually uniform palettes readable by color blind people, following the theme
class TimbrePalette
{
public:
    TimbrePalette();

    // Sequential palette (viridis) for t in [0, 1]
    QColor sequential(double t) const;

    // Diverging palette (blue for negative values, red for positive ones) for t in [-1, 1], neutral at 0
    QColor diverging(double t) const;

    // Colors of the theme
    QColor background() const { return _background; }
    QColor text() const { return _text; }
    QColor neutral() const { return _neutral; }
    QColor grid() const { return _grid; }
    QColor highlight() const { return _highlight; }
    QColor uncovered() const { return _uncovered; }
    QColor hatch() const { return _hatch; }
    bool isDark() const { return _dark; }

    // Colors of the labels (shape and color)
    QColor conform() const { return _conform; }
    QColor redundant() const { return _redundant; }
    QColor toCorrect() const { return _toCorrect; }
    QColor tooDifferent() const { return _tooDifferent; }
    QColor breakColor() const { return _break; }
    QColor levelColor() const { return _level; }

    // Shapes of the labels drawn in a square
    enum Shape
    {
        SHAPE_CHECK,
        SHAPE_MINUS,
        SHAPE_TILDE,
        SHAPE_CROSS,
        SHAPE_PLUS,
        SHAPE_UP,
        SHAPE_DOWN
    };
    static void drawShape(QPainter &painter, Shape shape, QRectF rect, QColor color);

private:
    QColor _background, _text, _neutral, _grid, _highlight, _uncovered, _hatch;
    QColor _conform, _redundant, _toCorrect, _tooDifferent, _break, _level;
    bool _dark;
};

#endif // TIMBREPALETTE_H
