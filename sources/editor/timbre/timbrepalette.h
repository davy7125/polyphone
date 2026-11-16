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
#include <QList>

// Colors of the timbre view, following the theme: shades of grey for a quantity, red or blue for a deviation (red: too
// strong, transposed up, too high; blue: the opposite), readable by color blind people
class TimbrePalette
{
public:
    // Background on which the colors are drawn: a list (map) or the window itself (graphs)
    enum Surface
    {
        SURFACE_LIST,
        SURFACE_WINDOW
    };

    explicit TimbrePalette(Surface surface = SURFACE_LIST);

    // Shades of grey for a quantity, t in [0, 1]: from the neutral color (weakest) to nearly the text color (strongest)
    QColor magnitude(double t) const;

    // Deviation, t in [-1, 1]: neutral at 0, towards blue for negative values and towards red for positive ones, a
    // larger value standing out more from the background
    QColor diverging(double t) const;

    // Colors of the theme
    QColor background() const { return _background; }
    QColor text() const { return _text; }
    QColor axisText() const { QColor color = _text; color.setAlpha(180); return color; } // As the axes of the range editor
    QColor neutral() const { return _neutral; }
    QColor grid() const { return _grid; }
    QColor highlight() const { return _highlight; }
    QColor uncovered() const { return _uncovered; }
    QColor hatch() const { return _hatch; }
    bool isDark() const { return _dark; }

    // Colors of the labels, the shape telling them apart: grey when nothing is needed, red for a problem (light when it
    // can be corrected), red or blue for a level too loud or too soft
    QColor conform() const { return _grey; }
    QColor redundant() const { return _grey; }
    QColor toCorrect() const { return _lightRed; }
    QColor tooDifferent() const { return _red; }
    QColor breakColor() const { return _red; }
    QColor levelColor(bool tooLoud) const { return tooLoud ? _red : _blue; }

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

    // Same color, darker on a light background or lighter on a dark one, until its contrast with the background
    // reaches the minimum (WCAG ratio, from 1 to 21)
    QColor readable(QColor color, double minimumContrast) const;

private:
    QColor _background, _text, _neutral, _grid, _highlight, _uncovered, _hatch;
    QColor _grey, _lightRed, _red, _blue;
    QList<QColor> _negativeStops, _positiveStops;
    bool _dark;
};

#endif // TIMBREPALETTE_H
