/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef RECTANGLEITEM_H
#define RECTANGLEITEM_H

#include <QGraphicsItem>
#include "pile_sf2.h"

class GraphicsRectangleItem : public QGraphicsRectItem
{
public:
    enum EditingMode
    {
        NONE,
        MOVE_ALL,
        MOVE_RIGHT,
        MOVE_LEFT,
        MOVE_TOP,
        MOVE_BOTTOM
    };

    GraphicsRectangleItem(EltID id, QGraphicsItem *parent = 0);
    static void init(Pile_sf2 * sf2) { s_sf2 = sf2; }

    EltID getID() { return _id; }
    EltID findBrother();
    EditingMode setHover(bool isHovered, const QPoint &point = QPoint());

    QRectF getRectF() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);
    bool contains(const QPointF &point) const;

    void computeNewRange(const QPointF &pointInit, const QPointF &pointFinal);
    void saveChanges();

    bool operator==(const GraphicsRectangleItem& other) { return (_id == other._id); }
    bool operator==(const EltID &id) { return _id == id; }

    int currentMinKey() { return _minKey; }
    int currentMaxKey() { return _maxKey; }
    int currentMinVel() { return _minVel; }
    int currentMaxVel() { return _maxVel; }

private:

    static QPen s_penBorderThin;
    static QPen s_penBorderFat;
    static QBrush s_brushRectangle;
    static QBrush s_brushRectangleHovered;
    static Pile_sf2 * s_sf2;

    EltID _id;
    int _minKeyInit, _maxKeyInit, _minVelInit, _maxVelInit;
    int _minKey, _maxKey, _minVel, _maxVel;
    EditingMode _editingMode;

    void initialize(EltID id);
    EditingMode getEditingMode(const QPoint &point);
    int limit(int value, int min, int max);
};

#endif // RECTANGLEITEM_H
