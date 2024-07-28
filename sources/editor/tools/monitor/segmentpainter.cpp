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

#include "segmentpainter.h"
#include "segment.h"
#include <QPainter>
#include "thememanager.h"
#include <cmath>

const float SegmentPainter::MIN_LOG = 0.0001;

SegmentPainter::SegmentPainter() :
    _isLog(false)
{

}

void SegmentPainter::setData(QList<Segment *> segments)
{
    _segments = segments;
}

void SegmentPainter::paint(QPainter *painter, QRect rect)
{
    float w, x1, x2, negMargin;
    for (int i = 0; i < _segments.count(); i++)
    {
        painter->setPen(QPen(ThemeManager::mix(Qt::white, _color, (double)_segments[i]->_velMax / 127.), 5, Qt::SolidLine, Qt::RoundCap));
        x1 = keyToCoord(_segments[i]->_keyMin, rect, w);
        x2 = keyToCoord(_segments[i]->_keyMax, rect, w);
        if (x1 > x2)
        {
            negMargin = x1;
            x1 = x2;
            x2 = negMargin;
        }
        x1 -= 0.5 * w;
        x2 += 0.5 * w;
        negMargin = qMin(4.f, (float)qAbs(x2 - x1) / 2.f - 2.0f);
        if (negMargin < 0)
            negMargin = 0;
        painter->drawLine(
            x1 + negMargin,
            valueToCoord(_segments[i]->_value, rect),
            x2 - negMargin,
            valueToCoord(_segments[i]->_value, rect));
    }
}

float SegmentPainter::keyToCoord(int key, QRect rect, float &width)
{
    width = static_cast<float>(rect.width()) / (_xMax - _xMin + 2);
    return width * (key - _xMin + 1) + rect.left();
}

int SegmentPainter::coordToKey(int coord, QRect rect)
{
    float width = static_cast<float>(rect.width()) / (_xMax - _xMin + 2);
    int result = (coord - 0.5f * width) / width + _xMin;
    if (result < _xMin)
        return _xMin;
    if (result > _xMax)
        return _xMax;
    return result;
}

float SegmentPainter::valueToCoord(float y, QRect rect)
{
    if (_isLog && y < MIN_LOG)
        return -100; // Out of sight
    float tmp = _isLog ?
        (log(y) - log(_yMin)) / (log(_yMax) - log(_yMin)) :
        (y - _yMin) / (_yMax - _yMin);
    return (1.0 - tmp) * (rect.height() - 1) + rect.top();
}
