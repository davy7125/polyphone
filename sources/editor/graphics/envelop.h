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

#ifndef ENVELOP_H
#define ENVELOP_H

#include <QMap>
#include <QColor>
#include <QWidget>

class Envelop
{
public:
    enum ValueType {
        DELAY,
        ATTACK,
        HOLD,
        DECAY,
        SUSTAIN,
        RELEASE,
        KEYNUM_TO_HOLD,
        KEYNUM_TO_DECAY,
        KEY_MIN,
        KEY_MAX
    };

    // Points that can be dragged, in the order of the stages
    // KEY_MAX / KEY_MIN: ends of the ruler showing the duration for the highest / lowest key
    enum HandleType {
        HANDLE_NONE = -1,
        HANDLE_DELAY,
        HANDLE_ATTACK,
        HANDLE_HOLD,
        HANDLE_HOLD_KEY_MAX,
        HANDLE_HOLD_KEY_MIN,
        HANDLE_DECAY,
        HANDLE_DECAY_KEY_MAX,
        HANDLE_DECAY_KEY_MIN,
        HANDLE_SUSTAIN,
        HANDLE_RELEASE
    };

    Envelop();

    // Set / get a property of the envelop
    void set(ValueType type, double value, bool defined);
    double get(ValueType type) { return _values[type]; }

    // Set the color of the envelop
    void setColor(QColor color) { _color = color; }

    // True if the envelop is thick
    void setThick(bool isThick) { _isThick = isThick; }

    // True if the envelop is a volume envelop (otherwise modulation)
    void setVolume(bool isVolume) { _isVolume = isVolume; }
    bool isVolume() { return _isVolume; }

    // True if the points of the envelop can be dragged
    void setEditable(bool isEditable) { _isEditable = isEditable; }
    bool isEditable() { return _isEditable; }

    // Name of the element having the envelop
    void setName(QString name) { _name = name; }
    QString getName() { return _name; }

    // Get the attack duration (delay + attack + hold + decay)
    double getAttackDuration();

    // Get the release duration
    double getReleaseDuration();

    // Compute the points of the envelop
    void computePoints(double triggeredKeyDuration, double releasedKeyDuration);

    // Compute the positions in pixels, before drawing or looking for handles
    void computePixels(const int width, const int height, const double xRange, const double xOffset);

    // Mouse position displaying the rulers of the hovered stages (a position far outside hides them)
    // If showNonZeroRulers is true, rulers are also displayed when the key modifier is not 0
    void setMousePosition(QPointF pos, bool showNonZeroRulers);

    // True if a position is close to a ruler
    bool isInRulerZone(QPointF pos);

    // Visible rulers (bit 0: hold, bit 1: decay)
    int getRulerVisibility() { return (_rulerVisible[0] ? 1 : 0) + (_rulerVisible[1] ? 2 : 0); }

    // Handle under a position in pixels (HANDLE_NONE if none) and its distance to the position
    HandleType getHandleAt(QPointF pos, double &distance);

    // Highlighted handle
    void setHoveredHandle(HandleType handle) { _hoveredHandle = handle; }

    // Position of a handle in pixels, and in seconds / level
    QPointF getHandlePixels(HandleType handle) { return _handlePixels.value(handle); }
    QPointF getHandleCoordinates(HandleType handle);

    // Draw the envelop on the graph
    void draw(QPainter * painter);

    // Value of hold or decay for a key, depending on the key modifier
    static double getValueForKey(double value, double keyModifier, double key);

private:
    struct HandleGroup
    {
        QPointF center;
        bool isDiamond;
        QList<HandleType> handles; // Left to right
    };

    QPointF coordToPixels(QPointF pointF);
    double getKeyCenter();
    bool hasRulers();
    QPolygonF computeRuler(ValueType type, double y);
    void updateHandleGroups();
    void drawGroup(QPainter * painter, const HandleGroup &group);

    QMap<ValueType, double> _values;
    QMap<ValueType, bool> _defined;
    QColor _color;
    bool _isThick;
    bool _isVolume;
    bool _isEditable;
    QString _name;

    QVector<QPointF> _vect1, _vect2;
    QPolygonF _polygon;

    // Pixels
    int _width, _height;
    double _xRange, _xOffset;
    QMap<HandleType, QPointF> _handlePixels;
    QPolygonF _rulers[2]; // Hold, decay
    QRectF _rulerZones[2];
    bool _rulerVisible[2];
    QList<HandleGroup> _handleGroups;
    HandleType _hoveredHandle;
};

#endif // ENVELOP_H
