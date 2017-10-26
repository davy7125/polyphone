/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef ENVELOP_H
#define ENVELOP_H

#include <QMap>
#include <QSize>
class QGraphicsLineItem;
class QGraphicsScene;

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
        KEYNUM_TO_DECAY
    };

    Envelop(QGraphicsScene * scene, Envelop * parent = NULL);
    ~Envelop();

    void set(ValueType type, double value, bool defined = true);
    bool isDefined(ValueType type);
    double get(ValueType type);

    static void computeLimits();

    void replot(QSize size, double releaseTime, double totalTime);

private:
    QMap<ValueType, double> _values;
    QMap<ValueType, bool> _defined;
    QList<QGraphicsLineItem *> _lines;
    QGraphicsScene * _scene;
    Envelop * _parentEnvelop;

    static QList<Envelop*> s_envelops;
    static double s_releaseTime;
    static double s_totalTime;
};

#endif // ENVELOP_H
