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

#include "envelop.h"
#include <QPainter>

Envelop::Envelop() :
    _vect1(8),
    _vect2(8)
{
    // Default values
    _values[DELAY] = 0;
    _values[ATTACK] = 0;
    _values[HOLD] = 0;
    _values[DECAY] = 0;
    _values[SUSTAIN] = 0;
    _values[RELEASE] = 0;
    _values[KEYNUM_TO_HOLD] = 0;
    _values[KEYNUM_TO_DECAY] = 0;
    _values[KEY_MIN] = 0;
    _values[KEY_MAX] = 0;

    _defined[DELAY] = false;
    _defined[ATTACK] = false;
    _defined[HOLD] = false;
    _defined[DECAY] = false;
    _defined[SUSTAIN] = false;
    _defined[RELEASE] = false;
    _defined[KEYNUM_TO_HOLD] = false;
    _defined[KEYNUM_TO_DECAY] = false;
    _values[KEY_MIN] = false;
    _values[KEY_MAX] = false;
}

void Envelop::set(ValueType type, double value, bool defined)
{
    _values[type] = value;
    _defined[type] = defined;
}

double Envelop::getAttackDuration()
{
    return _values[DELAY] + _values[ATTACK] +
            qMax(getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MIN]),
                 getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MAX])) +
            qMax(getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MIN]),
                 getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MAX]));
}

double Envelop::getReleaseDuration()
{
    return _values[RELEASE];
}

void Envelop::computePoints(double triggeredKeyDuration, double releasedKeyDuration)
{
    // Compute data
    double position = 0, position2;

    // First point: origin
    _vect1[0] = _vect2[0] = QPointF(0, 0);

    // Second point: after delay
    position += _values[DELAY];
    _vect1[1] = _vect2[1] = QPointF(position, 0);

    // Third point: after attack
    position += _values[ATTACK];
    position2 = position;
    _vect1[2] = _vect2[2] = QPointF(position, 1);

    // Fourth point: after hold
    position += getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MIN]);
    position2 += getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MAX]);
    _vect1[3] = QPointF(position, 1);
    _vect2[3] = QPointF(position2, 1);

    // Fifth point: after decay
    position += getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MIN]);
    position2 += getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MAX]);
    _vect1[4] = QPointF(position, _values[SUSTAIN]);
    _vect2[4] = QPointF(position2, _values[SUSTAIN]);

    // Sixth point: after sustain
    position = triggeredKeyDuration;
    _vect1[5] = _vect2[5] = QPointF(position, _values[SUSTAIN]);

    // Seventh point: after release
    position += _values[RELEASE];
    _vect1[6] = _vect2[6] = QPointF(position, 0);

    // Heighth point: end
    position = triggeredKeyDuration + releasedKeyDuration;
    _vect1[7] = _vect2[7] = QPointF(position, 0);

    // Polygon to fill between the curves
    _polygon << _vect1[2] << _vect1[3] << _vect1[4] << _vect1[5] << _vect2[4] << _vect2[3] << _vect1[2];
}

double Envelop::getValueForKey(double value, double keyModifier, int key)
{
    return value * pow(2, (double)(keyModifier / 100) * ((60. - (double)key) / 12));
}

void Envelop::draw(QPainter * painter, const int width, const int height, const double xRange, const double xOffset)
{
    // Compute the coordinates
    QVector<QPointF> vect1(_vect1.size()), vect2(_vect2.size());
    QPolygonF polygon(_polygon.size());
    for (int i = 0; i < _vect1.size(); i++)
    {
        vect1[i] = coordToPixels(_vect1[i], width, height, xRange, xOffset);
        vect2[i] = coordToPixels(_vect2[i], width, height, xRange, xOffset);
    }
    for (int i = 0; i < _polygon.size(); i++)
        polygon[i] = coordToPixels(_polygon[i], width, height, xRange, xOffset);

    // Fill the space between the lines
    _color.setAlpha(100);
    painter->setBrush(_color);
    painter->setPen(Qt::transparent);
    painter->drawPolygon(polygon);

    // Draw the lines
    _color.setAlpha(255);
    painter->setPen(QPen(_color, _isThick ? 3 : 1, Qt::SolidLine));
    painter->drawLine(vect1[0], vect1[1]); // Delay
    painter->drawLine(vect1[1], vect1[2]); // Attack
    painter->drawLine(vect1[2], QPointF(qMax(vect1[3].x(), vect2[3].x()), vect1[3].y())); // Hold
    painter->drawLine(vect1[3], vect1[4]); // Decay1
    painter->drawLine(vect2[3], vect2[4]); // Decay2

    // Sustain: dots in the middle
    double xCommon = qMax(vect1[4].x(), vect2[4].x());
    double sustainLength = vect1[5].x() - xCommon;
    painter->drawLine(QPointF(qMin(vect1[4].x(), vect2[4].x()), vect1[4].y()), QPointF(xCommon, vect1[4].y()));
    painter->drawLine(QPointF(xCommon, vect1[4].y()), QPointF(xCommon + 2 * sustainLength / 5, vect1[4].y()));
    painter->setPen(QPen(_color, _isThick ? 3 : 1, Qt::DotLine));
    painter->drawLine(QPointF(xCommon + 2 * sustainLength / 5, vect1[4].y()), QPointF(xCommon + 3 * sustainLength / 5, vect1[4].y()));
    painter->setPen(QPen(_color, _isThick ? 3 : 1, Qt::SolidLine));
    painter->drawLine(QPointF(xCommon + 3 * sustainLength / 5, vect1[4].y()), vect1[5]);

    painter->drawLine(vect1[5], vect1[6]); // Release
    painter->drawLine(vect1[6], vect1[7]); // End

    // Draw circles
    painter->setBrush(_color);
    for (int i = 1; i < vect1.size() - 1; i++)
    {
        painter->drawEllipse(vect1[i], 5, 5);
        painter->drawEllipse(vect2[i], 5, 5);
    }
}

QPointF Envelop::coordToPixels(QPointF pointF, const int width, const int height, const double xRange, const double xOffset)
{
    return QPointF((pointF.x() - xOffset) / xRange * width + 1, 69 + (1.0 - pointF.y()) * (height - 70));
}
