/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#ifndef RECTANGLEITEM_H
#define RECTANGLEITEM_H

#include <QGraphicsItem>
#include <QPen>
#include "basetypes.h"

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

    GraphicsRectangleItem(EltID id, QGraphicsItem *parent = nullptr);

    EltID getID() { return _id; }
    void setHover(bool isHovered, const QPoint &point = QPoint());
    bool isHovered() { return _editingMode != EditingMode::NONE; }
    EditingMode getHoverType() { return _editingMode; }
    static void syncHover(bool isSync) { s_synchronizeEditingMode = isSync; }
    void setSelected(bool isSelected) { _isSelected = isSelected; }
    bool isSelected() { return _isSelected; }

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;
    bool contains(const QPointF &point) const override;

    void computeNewRange(const QPointF &pointInit, const QPointF &pointFinal);
    bool saveChanges(); // Return true if changes have been made
    void cancelChanges();

    bool operator==(const GraphicsRectangleItem& other) { return (_id == other._id); }
    bool operator==(const EltID &id) { return _id == id; }

    int currentMinKey() { return _minKey; }
    int currentMaxKey() { return _maxKey; }
    int currentMinVel() { return _minVel; }
    int currentMaxVel() { return _maxVel; }

private:
    void initialize(EltID id);
    EditingMode getEditingMode(const QPoint &point);
    int limit(int value, int min, int max);
    QRectF getRectF() const;

    QPen _penBorderThin;
    QPen _penBorderFat;
    QBrush _brushRectangle;
    QBrush _brushRectangleSelected;

    EltID _id;
    int _minKeyInit, _maxKeyInit, _minVelInit, _maxVelInit;
    int _minKey, _maxKey, _minVel, _maxVel;
    EditingMode _editingMode;
    bool _isSelected;
    static EditingMode s_globalEditingMode;
    static bool s_synchronizeEditingMode;
};

#endif // RECTANGLEITEM_H
