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

#include "graphicsrectangleitem.h"
#include <QGraphicsScene>
#include "config.h"

QPen   GraphicsRectangleItem::s_penBorderThin         = QPen  (QColor(100, 170, 140, 180), 1);
QPen   GraphicsRectangleItem::s_penBorderFat          = QPen  (QColor(100, 170, 140, 255), 3);
QBrush GraphicsRectangleItem::s_brushRectangle        = QBrush(QColor(  0, 200, 100,  60));
QBrush GraphicsRectangleItem::s_brushRectangleHovered = QBrush(QColor( 70, 255, 100, 120));
Pile_sf2 * GraphicsRectangleItem::s_sf2 = NULL;

GraphicsRectangleItem::GraphicsRectangleItem(EltID id, QGraphicsItem *parent) : QGraphicsRectItem(parent),
    _id(id),
    _editingMode(NONE)
{
    initialize(id);
    s_penBorderThin.setCosmetic(true);
    s_penBorderFat.setCosmetic(true);

    this->setRect(getRectF());
}

void GraphicsRectangleItem::initialize(EltID id)
{
    // Default values
    _minKey = 0, _maxKey = 127;
    _minVel = 0, _maxVel = 127;

    // Global division
    EltID idGlobal = id;
    if (id.typeElement == elementInstSmpl)
        idGlobal.typeElement = elementInst;
    else
        idGlobal.typeElement = elementPrst;

    // Key range
    if (s_sf2->isSet(id, champ_keyRange))
    {
        rangesType range = s_sf2->get(id, champ_keyRange).rValue;
        _minKey = range.byLo;
        _maxKey = range.byHi;
    }
    else if (s_sf2->isSet(idGlobal, champ_keyRange))
    {
        rangesType range = s_sf2->get(idGlobal, champ_keyRange).rValue;
        _minKey = range.byLo;
        _maxKey = range.byHi;
    }

    // Velocity range
    if (s_sf2->isSet(id, champ_velRange))
    {
        rangesType range = s_sf2->get(id, champ_velRange).rValue;
        _minVel = range.byLo;
        _maxVel = range.byHi;
    }
    else if (s_sf2->isSet(idGlobal, champ_velRange))
    {
        rangesType range = s_sf2->get(idGlobal, champ_velRange).rValue;
        _minVel = range.byLo;
        _maxVel = range.byHi;
    }

    _minKeyInit = _minKey;
    _maxKeyInit = _maxKey;
    _minVelInit = _minVel;
    _maxVelInit = _maxVel;
}

QRectF GraphicsRectangleItem::getRectF() const
{
    return QRectF((float)_minKey - 0.5, 126.5 - (float)_maxVel,
                  (float)_maxKey - (float)_minKey + 1, (float)_maxVel - (float)_minVel + 1);
}

bool GraphicsRectangleItem::contains(const QPointF &point) const
{
    QRectF rectF = getRectF();
    return rectF.left() <= point.x() && point.x() < rectF.right() &&
            rectF.top() < point.y() && point.y() < rectF.bottom();
}

void GraphicsRectangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    // Draw base rectangle with a background color and a thin border
    if (_editingMode == NONE)
        this->setBrush(s_brushRectangle);
    else
        this->setBrush(s_brushRectangleHovered);
    this->setPen(s_penBorderThin);
    QGraphicsRectItem::paint(painter, option, widget);

    QRectF rectF = this->rect();
    switch (_editingMode)
    {
    case NONE:
        // Nothing else
        break;
    case MOVE_ALL:
        painter->setPen(s_penBorderFat);
        painter->drawLine(rectF.topRight(), rectF.bottomRight());
        painter->drawLine(rectF.topLeft(), rectF.bottomLeft());
        painter->drawLine(rectF.topLeft(), rectF.topRight());
        painter->drawLine(rectF.bottomLeft(), rectF.bottomRight());
        break;
    case MOVE_RIGHT:
        painter->setPen(s_penBorderFat);
        painter->drawLine(rectF.topRight(), rectF.bottomRight());
        break;
    case MOVE_LEFT:
        painter->setPen(s_penBorderFat);
        painter->drawLine(rectF.topLeft(), rectF.bottomLeft());
        break;
    case MOVE_TOP:
        painter->setPen(s_penBorderFat);
        painter->drawLine(rectF.topLeft(), rectF.topRight());
        break;
    case MOVE_BOTTOM:
        painter->setPen(s_penBorderFat);
        painter->drawLine(rectF.bottomLeft(), rectF.bottomRight());
        break;
    }
}

GraphicsRectangleItem::EditingMode GraphicsRectangleItem::getEditingMode(const QPoint &point)
{
    /*    _____________
     *    |\         /|     The central area is for a global move
     *    | \ _____ / |
     *    |  |     |  |     The other areas have a constant width and only move a side of the rectangle
     *    |  |     |  |     Width = 30% of the minimal size (width or length), maximum 10 pixels
     *    |  |     |  |
     *    |  |     |  |     Top or bottom have the priority in case of a conflict with right or left
     *    |  |     |  |
     *    |  |     |  |
     *    |  |_____|  |
     *    | /       \ |
     *    |/_________\|
     */

    // Size in pixels
    QRectF rectF = getRectF();
    QGraphicsView *view = scene()->views().first();
    QPoint topLeftPx = view->mapFromScene(rectF.topLeft());
    QPoint bottomRightPx = view->mapFromScene(rectF.bottomRight());

    // Width of the external areas
    int width = qMin(0.3 * qMin(bottomRightPx.x() - topLeftPx.x(), bottomRightPx.y() - topLeftPx.y()), 10.);

    // Minimal distance from a border
    EditingMode mode = MOVE_TOP;
    int minDist = point.y() - topLeftPx.y();
    if (bottomRightPx.y() - point.y() < minDist)
    {
        minDist = bottomRightPx.y() - point.y();
        mode = MOVE_BOTTOM;
    }
    if (point.x() - topLeftPx.x() < minDist)
    {
        minDist = point.x() - topLeftPx.x();
        mode = MOVE_LEFT;
    }
    if (bottomRightPx.x() - point.x() < minDist)
    {
        minDist = bottomRightPx.x() - point.x();
        mode = MOVE_RIGHT;
    }

    if (minDist < 0)
        mode = NONE;
    else if (minDist > width)
        mode = MOVE_ALL;

    return mode;
}

GraphicsRectangleItem::EditingMode GraphicsRectangleItem::setHover(bool isHovered, const QPoint &point)
{
    if (isHovered)
    {
        _editingMode = getEditingMode(point);
        this->setZValue(51);
    }
    else
    {
        _editingMode = NONE;
        this->setZValue(50);
    }

    return _editingMode;
}

EltID GraphicsRectangleItem::findBrother()
{
    if (Config::getInstance()->getRepercussionStereo() && _id.typeElement == elementInstSmpl)
    {
        // Sample linked to the division
        EltID idSmpl = _id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = s_sf2->get(_id, champ_sampleID).wValue;

        // Sample linked to another one?
        SFSampleLink typeLink = s_sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLink == rightSample || typeLink == leftSample || typeLink == linkedSample ||
                typeLink == RomRightSample || typeLink == RomLeftSample || typeLink == RomLinkedSample)
        {
            // Characteristics to find in the other divisions
            int numSmpl2 = s_sf2->get(idSmpl, champ_wSampleLink).wValue;
            rangesType keyRange = s_sf2->get(_id, champ_keyRange).rValue;
            rangesType velRange = s_sf2->get(_id, champ_velRange).rValue;

            // Search the brother
            int numBrothers = 0;
            EltID idBrother = _id;
            EltID id2 = _id;
            for (int i = 0; i < s_sf2->count(_id); i++)
            {
                id2.indexElt2 = i;
                if (!s_sf2->get(id2, champ_hidden).bValue && i != _id.indexElt2)
                {
                    rangesType keyRange2 = s_sf2->get(id2, champ_keyRange).rValue;
                    rangesType velRange2 = s_sf2->get(id2, champ_velRange).rValue;
                    if (keyRange2.byLo == keyRange.byLo && keyRange2.byHi == keyRange.byHi &&
                            velRange2.byLo == velRange.byLo && velRange2.byHi == velRange.byHi)
                    {
                        int iTmp = s_sf2->get(id2, champ_sampleID).wValue;
                        if (iTmp == idSmpl.indexElt)
                            numBrothers = 2; // ambiguity, another sample is like id
                        else if (iTmp == numSmpl2)
                        {
                            numBrothers++;
                            idBrother.indexElt2 = i;
                        }
                    }
                }
            }

            // Return the brother if there is only one (no ambiguity)
            if (numBrothers == 1)
                return idBrother;
            else
                return EltID(elementUnknown, 0, 0, 0, 0);
        }
        else
            return EltID(elementUnknown, 0, 0, 0, 0);
    }
    else
        return EltID(elementUnknown, 0, 0, 0, 0);
}

void GraphicsRectangleItem::computeNewRange(const QPointF &pointInit, const QPointF &pointFinal)
{
    // Move
    int nKey = qRound(pointFinal.x() - pointInit.x());
    int nVel = qRound(pointInit.y() - pointFinal.y());

    if (_editingMode != NONE)
    {
        switch (_editingMode)
        {
        case MOVE_ALL:
            _minKey = limit(_minKeyInit + nKey, 0, 127);
            _maxKey = limit(_maxKeyInit + nKey, 0, 127);
            _minVel = limit(_minVelInit + nVel, 0, 127);
            _maxVel = limit(_maxVelInit + nVel, 0, 127);
            break;
        case MOVE_RIGHT:
            _maxKey = limit(_maxKeyInit + nKey, _minKey, 127);
            break;
        case MOVE_LEFT:
            _minKey = limit(_minKeyInit + nKey, 0, _maxKey);
            break;
        case MOVE_TOP:
            _maxVel = limit(_maxVelInit + nVel, _minVel, 127);
            break;
        case MOVE_BOTTOM:
            _minVel = limit(_minVelInit + nVel, 0, _maxVel);
            break;
        default:
            break;
        }
    }

    this->setRect(getRectF());
}

void GraphicsRectangleItem::saveChanges()
{
    EltID idGlobal = _id;
    if (_id.typeElement == elementInstSmpl)
        idGlobal.typeElement = elementInst;
    else
        idGlobal.typeElement = elementPrst;

    // Store the key range
    if (_minKey != _minKeyInit || _maxKey != _maxKeyInit)
    {
        if (_minKey == 0 && _maxKey == 127 && !s_sf2->isSet(idGlobal, champ_keyRange))
            s_sf2->reset(_id, champ_keyRange);
        else
        {
            Valeur value;
            value.rValue.byLo = _minKey;
            value.rValue.byHi = _maxKey;
            s_sf2->set(_id, champ_keyRange, value);
        }
    }

    // Store the velocity range
    if (_minVel != _minVelInit || _maxVel != _maxVelInit)
    {
        if (_minVel == 0 && _maxVel == 127 && !s_sf2->isSet(idGlobal, champ_velRange))
            s_sf2->reset(_id, champ_velRange);
        else
        {
            Valeur value;
            value.rValue.byLo = _minVel;
            value.rValue.byHi = _maxVel;
            s_sf2->set(_id, champ_velRange, value);
        }
    }

    _minKeyInit = _minKey;
    _maxKeyInit = _maxKey;
    _minVelInit = _minVel;
    _maxVelInit = _maxVel;
}


int GraphicsRectangleItem::limit(int value, int min, int max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}
