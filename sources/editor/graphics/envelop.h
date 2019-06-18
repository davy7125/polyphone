/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef ENVELOP_H
#define ENVELOP_H

#include <QMap>
#include <QColor>
class QCPGraph;

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

    Envelop(QCPGraph * graph1, QCPGraph *graph2);

    // Set a property of the envelop
    void set(ValueType type, double value, bool defined);

    // Set the color of the envelop
    void setColor(QColor color) { _color = color; }

    // True if the envelop is thick
    void setThick(bool isThick) { _isThick = isThick; }

    // Get the graph used by the envelop
    QCPGraph * graph1() { return _graph1; }
    QCPGraph * graph2() { return _graph2; }

    // Get the attack duration (delay + attack + hold + decay)
    double getAttackDuration();

    // Get the release duration
    double getReleaseDuration();

    // Draw the envelop on the graph
    void draw(double triggeredKeyDuration, double releasedKeyDuration);

private:
    double getValueForKey(double value, double keyModifier, int key);
    QMap<ValueType, double> _values;
    QMap<ValueType, bool> _defined;
    QColor _color;
    bool _isThick;
    QCPGraph * _graph1, * _graph2;
};

#endif // ENVELOP_H
