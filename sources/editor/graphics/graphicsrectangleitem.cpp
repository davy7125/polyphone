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

#include "graphicsrectangleitem.h"
#include <QGraphicsScene>
#include <QApplication>
#include <QPainter>
#include <QGraphicsView>
#include "contextmanager.h"
#include "soundfontmanager.h"

GraphicsRectangleItem::EditingMode GraphicsRectangleItem::s_globalEditingMode = GraphicsRectangleItem::NONE;
bool GraphicsRectangleItem::s_synchronizeEditingMode = false;

GraphicsRectangleItem::GraphicsRectangleItem(EltID id, QGraphicsItem *parent) : QGraphicsRectItem(parent),
    _id(id),
    _editingMode(NONE),
    _isSelected(false)
{
    // Initialize pens and brushes
    QColor color = QApplication::palette().color(QPalette::Highlight);
    color.setAlpha(180);
    _penBorderThin = QPen(color, 1);
    color.setAlpha(255);
    _penBorderFat = QPen(color, 3);
    color.setAlpha(60);
    _brushRectangle = QBrush(color);
    color.setAlpha(140);
    _brushRectangleSelected = QBrush(color, Qt::DiagCrossPattern);
    _brushRectangleSelected.setMatrix(QMatrix(1,0,1,0,0,0)); // Don't scale the pattern

    initialize(id);
    _penBorderThin.setCosmetic(true);
    _penBorderFat.setCosmetic(true);

    this->setRect(getRectF());
}

void GraphicsRectangleItem::initialize(EltID id)
{
    // Default values
    _minKey = 0;
    _maxKey = 127;
    _minVel = 0;
    _maxVel = 127;

    // Global division
    EltID idGlobal = id;
    if (id.typeElement == elementInstSmpl)
        idGlobal.typeElement = elementInst;
    else
        idGlobal.typeElement = elementPrst;

    // Key range
    SoundfontManager * sm = SoundfontManager::getInstance();
    if (sm->isSet(id, champ_keyRange))
    {
        RangesType range = sm->get(id, champ_keyRange).rValue;
        _minKey = range.byLo;
        _maxKey = range.byHi;
    }
    else if (sm->isSet(idGlobal, champ_keyRange))
    {
        RangesType range = sm->get(idGlobal, champ_keyRange).rValue;
        _minKey = range.byLo;
        _maxKey = range.byHi;
    }

    // Velocity range
    if (sm->isSet(id, champ_velRange))
    {
        RangesType range = sm->get(id, champ_velRange).rValue;
        _minVel = range.byLo;
        _maxVel = range.byHi;
    }
    else if (sm->isSet(idGlobal, champ_velRange))
    {
        RangesType range = sm->get(idGlobal, champ_velRange).rValue;
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
    return QRectF(-0.5 + _minKey, 126.5 - _maxVel, 1.0 + _maxKey - _minKey, 1.0 + _maxVel - _minVel);
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
    this->setBrush(_isSelected ? _brushRectangleSelected : _brushRectangle);
    this->setPen(_penBorderThin);
    QGraphicsRectItem::paint(painter, option, widget);

    QRectF rectF = this->rect();
    switch (_isSelected && s_synchronizeEditingMode ? s_globalEditingMode : _editingMode)
    {
    case NONE:
        // Nothing else
        break;
    case MOVE_ALL:
        painter->setPen(_penBorderFat);
        painter->drawLine(rectF.topRight(), rectF.bottomRight());
        painter->drawLine(rectF.topLeft(), rectF.bottomLeft());
        painter->drawLine(rectF.topLeft(), rectF.topRight());
        painter->drawLine(rectF.bottomLeft(), rectF.bottomRight());
        break;
    case MOVE_RIGHT:
        painter->setPen(_penBorderFat);
        painter->drawLine(rectF.topRight(), rectF.bottomRight());
        break;
    case MOVE_LEFT:
        painter->setPen(_penBorderFat);
        painter->drawLine(rectF.topLeft(), rectF.bottomLeft());
        break;
    case MOVE_TOP:
        painter->setPen(_penBorderFat);
        painter->drawLine(rectF.topLeft(), rectF.topRight());
        break;
    case MOVE_BOTTOM:
        painter->setPen(_penBorderFat);
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
    int width = static_cast<int>(qMin(0.3 * qMin(bottomRightPx.x() - topLeftPx.x(),
                                                 bottomRightPx.y() - topLeftPx.y()), 10.));

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
        s_globalEditingMode = _editingMode;
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
    if (ContextManager::configuration()->getValue(ConfManager::SECTION_NONE, "stereo_modification", false).toBool() &&
            _id.typeElement == elementInstSmpl)
    {
        // Sample linked to the division
        SoundfontManager * sm = SoundfontManager::getInstance();
        EltID idSmpl = _id;
        idSmpl.typeElement = elementSmpl;
        idSmpl.indexElt = sm->get(_id, champ_sampleID).wValue;

        // Sample linked to another one?
        SFSampleLink typeLink = sm->get(idSmpl, champ_sfSampleType).sfLinkValue;
        if (typeLink == rightSample || typeLink == leftSample || typeLink == linkedSample ||
                typeLink == RomRightSample || typeLink == RomLeftSample || typeLink == RomLinkedSample)
        {
            // Characteristics to find in the other divisions
            int numSmpl2 = sm->get(idSmpl, champ_wSampleLink).wValue;
            RangesType keyRange = sm->get(_id, champ_keyRange).rValue;
            RangesType velRange = sm->get(_id, champ_velRange).rValue;

            // Search the brother
            int numBrothers = 0;
            EltID idBrother = _id;
            EltID id2 = _id;
            foreach (int i, sm->getSiblings(_id))
            {
                id2.indexElt2 = i;
                if (i != _id.indexElt2)
                {
                    RangesType keyRange2 = sm->get(id2, champ_keyRange).rValue;
                    RangesType velRange2 = sm->get(id2, champ_velRange).rValue;
                    if (keyRange2.byLo == keyRange.byLo && keyRange2.byHi == keyRange.byHi &&
                            velRange2.byLo == velRange.byLo && velRange2.byHi == velRange.byHi)
                    {
                        int iTmp = sm->get(id2, champ_sampleID).wValue;
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

    switch (s_globalEditingMode)
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

    this->setRect(getRectF());
}

bool GraphicsRectangleItem::saveChanges()
{
    EltID idGlobal = _id;
    if (_id.typeElement == elementInstSmpl)
        idGlobal.typeElement = elementInst;
    else
        idGlobal.typeElement = elementPrst;

    // Store the key range
    SoundfontManager * sm = SoundfontManager::getInstance();
    bool changes = false;
    if (_minKey != _minKeyInit || _maxKey != _maxKeyInit)
    {
        changes = true;
        if (_minKey == 0 && _maxKey == 127 && !sm->isSet(idGlobal, champ_keyRange))
            sm->reset(_id, champ_keyRange);
        else
        {
            AttributeValue value;
            value.rValue.byLo = static_cast<unsigned char>(_minKey);
            value.rValue.byHi = static_cast<unsigned char>(_maxKey);
            sm->set(_id, champ_keyRange, value);
        }
    }

    // Store the velocity range
    if (_minVel != _minVelInit || _maxVel != _maxVelInit)
    {
        changes = true;
        if (_minVel == 0 && _maxVel == 127 && !sm->isSet(idGlobal, champ_velRange))
            sm->reset(_id, champ_velRange);
        else
        {
            AttributeValue value;
            value.rValue.byLo = static_cast<unsigned char>(_minVel);
            value.rValue.byHi = static_cast<unsigned char>(_maxVel);
            sm->set(_id, champ_velRange, value);
        }
    }

    _minKeyInit = _minKey;
    _maxKeyInit = _maxKey;
    _minVelInit = _minVel;
    _maxVelInit = _maxVel;

    return changes;
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
