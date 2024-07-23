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

#ifndef SEGMENTPAINTER_H
#define SEGMENTPAINTER_H

#include <QList>
#include <QRect>
#include <QColor>
class QPainter;
class Segment;

class SegmentPainter
{
public:
    SegmentPainter();

    void setData(QList<Segment *> segments);
    void setLogarithmicScale(bool isLog) { _isLog = isLog; }
    void setLimits(int xMin, int xMax, float yMin, float yMax)
    {
        _xMin = xMin;
        _xMax = xMax;
        _yMin = yMin;
        _yMax = yMax;
    }

    void setColor(QColor color) { _color = color; }

    // Draw data
    void paint(QPainter *painter, QRect rect);

    // Coordinate conversions
    float keyToCoord(int key, QRect rect, float &width);
    float valueToCoord(float y, QRect rect);

    static const float MIN_LOG;

private:
    QList<Segment *> _segments;
    QColor _color;
    bool _isLog;
    int _xMin, _xMax;
    float _yMin, _yMax;
};

#endif // SEGMENTPAINTER_H
