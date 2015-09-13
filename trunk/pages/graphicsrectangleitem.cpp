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
#include <QApplication>
#include <QGraphicsScene>
#include "config.h"

QPen GraphicsRectangleItem::s_penRectangle = QPen(QColor(100, 170, 140, 255), 1);
QBrush GraphicsRectangleItem::s_brushRectangle = QBrush(QColor(70, 180, 100, 50));
QBrush GraphicsRectangleItem::s_brushRectangleHovered = QBrush(QColor(0, 250, 70, 60));
Pile_sf2 * GraphicsRectangleItem::s_sf2 = NULL;

GraphicsRectangleItem::GraphicsRectangleItem(EltID id, QGraphicsItem *parent) :
    QGraphicsRectItem(getRectInit(id), parent),
    _id(id)
{
    s_penRectangle.setCosmetic(true);
    this->setPen(s_penRectangle);
    this->setBrush(s_brushRectangle);

    this->setFlag(QGraphicsItem::ItemIsSelectable);
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant GraphicsRectangleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        // Offset made by integers
        QPointF offset = value.toPointF();
        offset.setX(qRound(offset.x()));
        offset.setY(qRound(offset.y()));

        // Keep item in scene rect
        offset.setX(qMax(offset.x(), scene()->sceneRect().left() - rect().left()));
        offset.setX(qMin(offset.x(), scene()->sceneRect().right() - rect().right()));
        offset.setY(qMax(offset.y(), scene()->sceneRect().top() - rect().top()));
        offset.setY(qMin(offset.y(), scene()->sceneRect().bottom() - rect().bottom()));

        return offset;
    }

    return QGraphicsItem::itemChange(change, value);
}

QRectF GraphicsRectangleItem::getRectInit(EltID id)
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

    return getRect();
}

QRectF GraphicsRectangleItem::getRect()
{
    return QRectF((float)_minKey - 0.5, 126.5 - (float)_maxVel,
                  (float)_maxKey - (float)_minKey + 1, (float)_maxVel - (float)_minVel + 1);
}

bool GraphicsRectangleItem::contains(const QPointF &point) const
{
    return (float)_minKey - 0.5 < point.x() &&
            point.x() < (float)_maxKey + 0.5 &&
            126.5 - (float)_maxVel < point.y() &&
            point.y() < 127.5 - (float)_minVel;
}

void GraphicsRectangleItem::setHover(bool isHovered)
{
    if (isHovered)
        this->setBrush(s_brushRectangleHovered);
    else
        this->setBrush(s_brushRectangle);
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
            bool numBrothers = 0;
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
    _minKey = limit(_minKeyInit + nKey);
    _maxKey = limit(_maxKeyInit + nKey);
    _minVel = limit(_minVelInit + nVel);
    _maxVel = limit(_maxVelInit + nVel);

    this->setRect(getRect());
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


int GraphicsRectangleItem::limit(int value)
{
    if (value < 0)
        return 0;
    else if (value > 127)
        return 127;
    else
        return value;
}
