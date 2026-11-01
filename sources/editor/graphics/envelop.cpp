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
#include <cmath>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

static const double RULER_HALF_LENGTH = 16; // Pixels
static const int RULER_SAMPLES = 24;
static const double RULER_ZONE_MARGIN = 8;
static const double DISC_RADIUS = 5;
static const double DISC_RADIUS_HOVERED = 7;
static const double DIAMOND_RADIUS = 6;
static const double DIAMOND_RADIUS_HOVERED = 8;
static const double HANDLE_HIT_DISTANCE = 10;
static const double HANDLE_MERGE_DISTANCE = 2.5;

Envelop::Envelop() :
    _isThick(false),
    _isVolume(true),
    _isEditable(false),
    _vect1(8),
    _vect2(8),
    _width(1),
    _height(1),
    _xRange(1),
    _xOffset(0),
    _hoveredHandle(HANDLE_NONE)
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
    _defined[KEY_MIN] = false;
    _defined[KEY_MAX] = false;

    _rulerVisible[0] = _rulerVisible[1] = false;
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
    _polygon.clear();
    _polygon << _vect1[2] << _vect1[3] << _vect1[4] << _vect1[5] << _vect2[4] << _vect2[3] << _vect1[2];
}

double Envelop::getValueForKey(double value, double keyModifier, double key)
{
    return value * pow(2, (double)(keyModifier / 100) * ((60. - key) / 12));
}

double Envelop::getKeyCenter()
{
    return 0.5 * (_values[KEY_MIN] + _values[KEY_MAX]);
}

bool Envelop::hasRulers()
{
    return _values[KEY_MAX] > _values[KEY_MIN];
}

QPointF Envelop::getHandleCoordinates(HandleType handle)
{
    double key = getKeyCenter();
    if (handle == HANDLE_HOLD_KEY_MAX || handle == HANDLE_DECAY_KEY_MAX)
        key = _values[KEY_MAX];
    else if (handle == HANDLE_HOLD_KEY_MIN || handle == HANDLE_DECAY_KEY_MIN)
        key = _values[KEY_MIN];
    double holdEnd = _values[DELAY] + _values[ATTACK] + getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], key);

    switch (handle)
    {
    case HANDLE_DELAY:
        return _vect1[1];
    case HANDLE_ATTACK:
        return _vect1[2];
    case HANDLE_HOLD: case HANDLE_HOLD_KEY_MAX: case HANDLE_HOLD_KEY_MIN:
        return QPointF(holdEnd, 1);
    case HANDLE_DECAY: case HANDLE_DECAY_KEY_MAX: case HANDLE_DECAY_KEY_MIN:
        return QPointF(holdEnd + getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], key), _values[SUSTAIN]);
    case HANDLE_SUSTAIN:
        return _vect1[5];
    case HANDLE_RELEASE:
        return _vect1[6];
    default:
        break;
    }
    return QPointF();
}

void Envelop::computePixels(const int width, const int height, const double xRange, const double xOffset)
{
    _width = width;
    _height = height;
    _xRange = xRange;
    _xOffset = xOffset;

    _handlePixels.clear();
    if (!_isEditable)
        return;

    QList<HandleType> discs;
    discs << HANDLE_DELAY << HANDLE_ATTACK << HANDLE_HOLD << HANDLE_DECAY << HANDLE_SUSTAIN << HANDLE_RELEASE;
    foreach (HandleType handle, discs)
        _handlePixels[handle] = coordToPixels(getHandleCoordinates(handle));

    // Rulers: highest key on top, lowest key at the bottom
    if (hasRulers())
    {
        _rulers[0] = computeRuler(HOLD, _handlePixels[HANDLE_HOLD].y());
        _rulers[1] = computeRuler(DECAY, _handlePixels[HANDLE_DECAY].y());
        _handlePixels[HANDLE_HOLD_KEY_MAX] = _rulers[0].first();
        _handlePixels[HANDLE_HOLD_KEY_MIN] = _rulers[0].last();
        _handlePixels[HANDLE_DECAY_KEY_MAX] = _rulers[1].first();
        _handlePixels[HANDLE_DECAY_KEY_MIN] = _rulers[1].last();
        for (int i = 0; i < 2; i++)
            _rulerZones[i] = _rulers[i].boundingRect().adjusted(-RULER_ZONE_MARGIN, -RULER_ZONE_MARGIN,
                                                                RULER_ZONE_MARGIN, RULER_ZONE_MARGIN);
    }
}

QPolygonF Envelop::computeRuler(ValueType type, double y)
{
    // Vertical position, staying in the area
    double top = qMax(2., y - RULER_HALF_LENGTH);
    double bottom = qMin(_height - 2., top + 2 * RULER_HALF_LENGTH);
    top = bottom - 2 * RULER_HALF_LENGTH;

    // Duration for each key, from the highest to the lowest
    QPolygonF ruler;
    for (int i = 0; i <= RULER_SAMPLES; i++)
    {
        double key = _values[KEY_MAX] - (_values[KEY_MAX] - _values[KEY_MIN]) * i / RULER_SAMPLES;
        double position = _values[DELAY] + _values[ATTACK] + getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], key);
        if (type == DECAY)
            position += getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], key);
        ruler << QPointF(coordToPixels(QPointF(position, 0)).x(), top + (bottom - top) * i / RULER_SAMPLES);
    }
    return ruler;
}

void Envelop::setMousePosition(QPointF pos, bool showNonZeroRulers)
{
    // Rulers are visible if the mouse is close to them, or if the key modifier is used
    bool withRulers = _isEditable && hasRulers();
    _rulerVisible[0] = withRulers && ((showNonZeroRulers && _values[KEYNUM_TO_HOLD] != 0.) || _rulerZones[0].contains(pos));
    _rulerVisible[1] = withRulers && ((showNonZeroRulers && _values[KEYNUM_TO_DECAY] != 0.) || _rulerZones[1].contains(pos));

    updateHandleGroups();
}

bool Envelop::isInRulerZone(QPointF pos)
{
    return _isEditable && hasRulers() && (_rulerZones[0].contains(pos) || _rulerZones[1].contains(pos));
}

void Envelop::updateHandleGroups()
{
    // Handles at the same place are gathered, in the order of the stages from left to right
    _handleGroups.clear();
    QList<HandleType> handles = _handlePixels.keys();
    foreach (HandleType handle, handles)
    {
        bool isDiamond = (handle == HANDLE_HOLD_KEY_MAX || handle == HANDLE_HOLD_KEY_MIN ||
                          handle == HANDLE_DECAY_KEY_MAX || handle == HANDLE_DECAY_KEY_MIN);
        if (isDiamond && !_rulerVisible[handle < HANDLE_DECAY ? 0 : 1])
            continue;

        QPointF pos = _handlePixels[handle];
        bool merged = false;
        for (int i = 0; i < _handleGroups.count(); i++)
        {
            if (_handleGroups[i].isDiamond == isDiamond && QLineF(_handleGroups[i].center, pos).length() < HANDLE_MERGE_DISTANCE)
            {
                _handleGroups[i].handles << handle;
                merged = true;
                break;
            }
        }
        if (!merged)
        {
            HandleGroup group;
            group.center = pos;
            group.isDiamond = isDiamond;
            group.handles << handle;
            _handleGroups << group;
        }
    }
}

Envelop::HandleType Envelop::getHandleAt(QPointF pos, double &distance)
{
    // Closest group, the last one being drawn on top
    int groupIndex = -1;
    distance = HANDLE_HIT_DISTANCE;
    for (int i = 0; i < _handleGroups.count(); i++)
    {
        double currentDistance = QLineF(pos, _handleGroups[i].center).length();
        if (currentDistance <= distance)
        {
            distance = currentDistance;
            groupIndex = i;
        }
    }
    if (groupIndex == -1)
        return HANDLE_NONE;

    // Part of the group under the mouse, from left to right
    const HandleGroup &group = _handleGroups[groupIndex];
    int count = group.handles.count();
    if (count == 1)
        return group.handles[0];
    double radius = group.isDiamond ? DIAMOND_RADIUS_HOVERED : DISC_RADIUS_HOVERED;
    int part = qFloor((pos.x() - group.center.x() + radius) * count / (2 * radius));
    return group.handles[qBound(0, part, count - 1)];
}

void Envelop::draw(QPainter * painter)
{
    // Compute the coordinates
    QVector<QPointF> vect1(_vect1.size()), vect2(_vect2.size());
    QPolygonF polygon(_polygon.size());
    for (int i = 0; i < _vect1.size(); i++)
    {
        vect1[i] = coordToPixels(_vect1[i]);
        vect2[i] = coordToPixels(_vect2[i]);
    }
    for (int i = 0; i < _polygon.size(); i++)
        polygon[i] = coordToPixels(_polygon[i]);

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

    if (!_isEditable)
    {
        // Draw circles
        painter->setBrush(_color);
        for (int i = 1; i < vect1.size() - 1; i++)
        {
            painter->drawEllipse(vect1[i], 5, 5);
            painter->drawEllipse(vect2[i], 5, 5);
        }
        return;
    }

    // Rulers
    painter->setPen(QPen(_color, 1, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    for (int i = 0; i < 2; i++)
        if (_rulerVisible[i])
            painter->drawPolyline(_rulers[i]);

    // Handles
    painter->setPen(Qt::NoPen);
    foreach (HandleGroup group, _handleGroups)
        drawGroup(painter, group);
}

void Envelop::drawGroup(QPainter * painter, const HandleGroup &group)
{
    // Shape of the group
    bool isHovered = group.handles.contains(_hoveredHandle);
    double radius = group.isDiamond ? (isHovered ? DIAMOND_RADIUS_HOVERED : DIAMOND_RADIUS) :
                                      (isHovered ? DISC_RADIUS_HOVERED : DISC_RADIUS);
    QPainterPath shape;
    if (group.isDiamond)
    {
        shape.addPolygon(QPolygonF() << group.center + QPointF(0, -radius) << group.center + QPointF(radius, 0)
                         << group.center + QPointF(0, radius) << group.center + QPointF(-radius, 0));
        shape.closeSubpath();
    }
    else
        shape.addEllipse(group.center, radius, radius);

    // One vertical slice per handle, separated by a gap
    int count = group.handles.count();
    for (int i = 0; i < count; i++)
    {
        QPainterPath part = shape;
        if (count > 1)
        {
            double left = group.center.x() - radius + 2 * radius * i / count + (i > 0 ? 1 : 0);
            double right = group.center.x() - radius + 2 * radius * (i + 1) / count - (i < count - 1 ? 1 : 0);
            QPainterPath slice;
            slice.addRect(QRectF(left, group.center.y() - radius, right - left, 2 * radius));
            part = shape.intersected(slice);
        }
        painter->setBrush(group.handles[i] == _hoveredHandle ? _color.lighter(160) : _color);
        painter->drawPath(part);
    }
}

QPointF Envelop::coordToPixels(QPointF pointF)
{
    return QPointF((pointF.x() - _xOffset) / _xRange * _width + 1, 69 + (1.0 - pointF.y()) * (_height - 70));
}
