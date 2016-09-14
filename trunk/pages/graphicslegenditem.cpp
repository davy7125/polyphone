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

#include "graphicslegenditem.h"
#include "keynamemanager.h"
#include <QPen>
#include <QPainter>
#include <QApplication>

Pile_sf2 *   GraphicsLegendItem::s_sf2 = NULL;
const int    GraphicsLegendItem::s_border = 5;

GraphicsLegendItem::GraphicsLegendItem(QString fontFamily, QGraphicsItem * parent) : QGraphicsItem(parent),
    _font(fontFamily, 8),
    _smallFont(fontFamily, 6)
{
    // Colors
    QColor color = QApplication::palette().color(QPalette::Highlight);
    _borderPen = QPen(color, 1, Qt::SolidLine);
    color.setAlpha(200);
    _foregroundBrush = QBrush(color);
    _textPen = QPen(QApplication::palette().color(QPalette::HighlightedText), 1, Qt::SolidLine);

    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setLeft(true);
}

void GraphicsLegendItem::setLeft(bool isLeft)
{
    if (isLeft)
        _alignment = Qt::AlignLeft | Qt::AlignTop;
    else
        _alignment = Qt::AlignRight | Qt::AlignTop;
}

bool GraphicsLegendItem::isLeft()
{
    return (_alignment & Qt::AlignLeft) != 0;
}

void GraphicsLegendItem::setIds(QList<EltID> ids, int selectionIndex, int selectionNumber)
{
    _selectionIndex = selectionIndex;
    _selectionNumber = selectionNumber;

    _text.clear();
    if (!ids.isEmpty())
    {
        // First id and global division
        EltID id = ids.first();
        EltID idGlobal = id;
        if (idGlobal.typeElement == elementInstSmpl)
            idGlobal.typeElement = elementInst;
        else
            idGlobal.typeElement = elementPrst;

        // Name of the divisions
        foreach (EltID id, ids)
        {
            EltID idLink = id;
            if (id.typeElement == elementInstSmpl)
            {
                idLink.typeElement = elementSmpl;
                idLink.indexElt = s_sf2->get(id, champ_sampleID).wValue;
            }
            else
            {
                idLink.typeElement = elementInst;
                idLink.indexElt = s_sf2->get(id, champ_instrument).wValue;
            }
            _text << s_sf2->getQstr(idLink, champ_name);
        }

        // Key range
        int minKey = 0;
        int maxKey = 127;
        if (s_sf2->isSet(id, champ_keyRange))
        {
            rangesType range = s_sf2->get(id, champ_keyRange).rValue;
            minKey = range.byLo;
            maxKey = range.byHi;
        }
        else if (s_sf2->isSet(idGlobal, champ_keyRange))
        {
            rangesType range = s_sf2->get(idGlobal, champ_keyRange).rValue;
            minKey = range.byLo;
            maxKey = range.byHi;
        }
        _text << QObject::trUtf8("Étendue note :") + " " +
                 KeyNameManager::getInstance()->getKeyName(minKey) + " - " +
                 KeyNameManager::getInstance()->getKeyName(maxKey);

        // Velocity range
        int minVel = 0;
        int maxVel = 127;
        if (s_sf2->isSet(id, champ_velRange))
        {
            rangesType range = s_sf2->get(id, champ_velRange).rValue;
            minVel = range.byLo;
            maxVel = range.byHi;
        }
        else if (s_sf2->isSet(idGlobal, champ_velRange))
        {
            rangesType range = s_sf2->get(idGlobal, champ_velRange).rValue;
            minVel = range.byLo;
            maxVel = range.byHi;
        }
        _text << QObject::trUtf8("Étendue vélocité :") + " " +
                 QString::number(minVel) + " - " +
                 QString::number(maxVel);
    }
}

QRectF GraphicsLegendItem::boundingRect() const
{
    QSizeF size = getTextSize();
    return QRectF(QPointF(dx(size), dy(size)), size);
}

void GraphicsLegendItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (_text.isEmpty())
        return;

    // Translation of the painter
    QSizeF size = getTextSize();
    painter->translate(dx(size), dy(size));

    // Paint foreground and border
    painter->setBrush(_foregroundBrush);
    painter->setPen(_borderPen);
    painter->drawRect(QRectF(QPoint(0, 0), size));

    // Paint text
    painter->setPen(_textPen);
    painter->setFont(_font);
    QFontMetrics fm(_font);
    for (int i = 0; i < _text.count(); i++)
        painter->drawText(QPoint(s_border, fm.height() * (i + 1)), _text.at(i));

    if (_selectionNumber > 1)
    {
        painter->setFont(_smallFont);
        QString text = QString::number(_selectionIndex + 1) + " / " + QString::number(_selectionNumber);
        double posY = fm.height() * (_text.count() + 1) + 3;
        double posX = 0.5 * (size.width() - QFontMetrics(_smallFont).width(text));
        painter->drawText(QPointF(posX, posY), text);
    }
}

QSizeF GraphicsLegendItem::getTextSize() const
{
    if (_text.isEmpty())
        return QSizeF(0, 0);

    QFontMetrics fm(_font);

    int height = _text.count() * fm.height() + 2 * s_border;
    int width = 0;
    foreach (QString line, _text)
        width = qMax(width, fm.width(line));
    width += 2 * s_border;

    if (_selectionNumber > 1)
        height += fm.height();

    return QSizeF(width, height);
}

qreal GraphicsLegendItem::dx(QSizeF size) const
{
    if (_alignment & Qt::AlignHCenter)
        return -size.width() / 2;
    else if (_alignment & Qt::AlignRight)
        return -size.width();
    return 0;
}

qreal GraphicsLegendItem::dy(QSizeF size) const
{
    if (_alignment & Qt::AlignVCenter)
        return -size.height() / 2;
    else if (_alignment & Qt::AlignBottom)
        return -size.height();
    return 0;
}
