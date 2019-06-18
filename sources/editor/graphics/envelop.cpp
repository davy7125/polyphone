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

#include "envelop.h"
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include "graphicsviewenvelop.h"

Envelop::Envelop(QCPGraph * graph1, QCPGraph * graph2) :
    _graph1(graph1),
    _graph2(graph2)
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
    _values[KEYNUM_TO_DECAY] = false;
    _values[KEYNUM_TO_DECAY] = false;

    _graph1->setChannelFillGraph(_graph2);
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

void Envelop::draw(double triggeredKeyDuration, double releasedKeyDuration)
{
    // Style of the line
    _color.setAlpha(255);
    QPen pen(_color, _isThick ? 3 : 1);
    _graph1->setPen(pen);
    _graph2->setPen(pen);

    _color.setAlpha(100);
    _graph1->setBrush(_color);

    // Compute data
    double position = 0, position2;
    QVector<double> vectX1(8), vectY1(8), vectX2(8), vectY2(8);

    // First point: origin
    vectX1[0] = 0;
    vectY1[0] = 0;
    vectX2[0] = 0;
    vectY2[0] = 0;

    // Second point: after delay
    position += _values[DELAY];
    vectX1[1] = position;
    vectY1[1] = 0;
    vectX2[1] = position;
    vectY2[1] = 0;

    // Third point: after attack
    position += _values[ATTACK];
    position2 = position;
    vectX1[2] = position;
    vectY1[2] = 1;
    vectX2[2] = position;
    vectY2[2] = 1;

    // Fourth point: after hold
    position += getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MIN]);
    position2 += getValueForKey(_values[HOLD], _values[KEYNUM_TO_HOLD], _values[KEY_MAX]);
    vectX1[3] = position;
    vectY1[3] = 1;
    vectX2[3] = position2;
    vectY2[3] = 1;

    // Fifth point: after decay
    position += getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MIN]);
    position2 += getValueForKey(_values[DECAY], _values[KEYNUM_TO_DECAY], _values[KEY_MAX]);
    vectX1[4] = position;
    vectY1[4] = _values[SUSTAIN];
    vectX2[4] = position2;
    vectY2[4] = _values[SUSTAIN];

    // Sixth point: after sustain
    position = triggeredKeyDuration;
    vectX1[5] = position;
    vectY1[5] = _values[SUSTAIN];
    vectX2[5] = position;
    vectY2[5] = _values[SUSTAIN];

    // Seventh point: after release
    position += _values[RELEASE];
    vectX1[6] = position;
    vectY1[6] = 0;
    vectX2[6] = position;
    vectY2[6] = 0;

    // Heighth point: end
    position = triggeredKeyDuration + releasedKeyDuration;
    vectX1[7] = position;
    vectY1[7] = 0;
    vectX2[7] = position;
    vectY2[7] = 0;

    _graph1->setData(vectX1, vectY1);
    _graph2->setData(vectX2, vectY2);
}

double Envelop::getValueForKey(double value, double keyModifier, int key)
{
    return value * pow(2, (double)(keyModifier / 100) * ((60. - (double)key) / 12));
}
