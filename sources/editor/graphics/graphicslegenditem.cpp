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

#include "graphicslegenditem.h"
#include "contextmanager.h"
#include <QPen>
#include <QPainter>
#include <QApplication>
#include "soundfontmanager.h"

const int GraphicsLegendItem::s_border = 5;
static const QChar unicodeArrow[] = { 0xfeff, 0x279c };
const QString GraphicsLegendItem::s_rightArrow = QString::fromRawData(unicodeArrow, 2);

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

void GraphicsLegendItem::setIds(QList<EltID> ids, QList<int> highlightedIds, int selectionIndex, int selectionNumber)
{
    _selectionIndex = selectionIndex;
    _selectionNumber = selectionNumber;

    _textTop.clear();
    _textBottom.clear();
    _selectedLinesInTextTop = highlightedIds;
    if (!ids.isEmpty())
    {
        // First id and global division
        EltID id = ids.first();
        if (!highlightedIds.empty() && highlightedIds[0] < ids.count() && highlightedIds[0] >= 0)
            id = ids[highlightedIds[0]]; // Anyone of the highlighted ids have the same range
        EltID idGlobal = id;
        if (idGlobal.typeElement == elementInstSmpl)
            idGlobal.typeElement = elementInst;
        else
            idGlobal.typeElement = elementPrst;

        // Name of the divisions
        SoundfontManager * sm = SoundfontManager::getInstance();
        foreach (EltID id, ids)
        {
            EltID idLink = id;
            if (id.typeElement == elementInstSmpl)
            {
                idLink.typeElement = elementSmpl;
                idLink.indexElt = sm->get(id, champ_sampleID).wValue;
            }
            else
            {
                idLink.typeElement = elementInst;
                idLink.indexElt = sm->get(id, champ_instrument).wValue;
            }
            _textTop << sm->getQstr(idLink, champ_name);
        }

        // Key range
        unsigned int minKey = 0;
        unsigned int maxKey = 127;
        if (sm->isSet(id, champ_keyRange))
        {
            RangesType range = sm->get(id, champ_keyRange).rValue;
            minKey = range.byLo;
            maxKey = range.byHi;
        }
        else if (sm->isSet(idGlobal, champ_keyRange))
        {
            RangesType range = sm->get(idGlobal, champ_keyRange).rValue;
            minKey = range.byLo;
            maxKey = range.byHi;
        }
        _textBottom << QObject::trUtf8("Key range:") + " " +
                       ContextManager::keyName()->getKeyName(minKey) + " - " +
                       ContextManager::keyName()->getKeyName(maxKey);

        // Velocity range
        int minVel = 0;
        int maxVel = 127;
        if (sm->isSet(id, champ_velRange))
        {
            RangesType range = sm->get(id, champ_velRange).rValue;
            minVel = range.byLo;
            maxVel = range.byHi;
        }
        else if (sm->isSet(idGlobal, champ_velRange))
        {
            RangesType range = sm->get(idGlobal, champ_velRange).rValue;
            minVel = range.byLo;
            maxVel = range.byHi;
        }
        _textBottom << QObject::trUtf8("Velocity range:") + " " +
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
    if (_textTop.isEmpty())
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
    for (int i = 0; i < _textTop.count(); i++)
    {
        if (_selectedLinesInTextTop.contains(i))
            painter->drawText(QPoint(s_border, fm.height() * (i + 1)), s_rightArrow);
        painter->drawText(QPoint(static_cast<int>(1.5 * fm.width(s_rightArrow) + s_border),
                                 fm.height() * (i + 1)), _textTop.at(i));
    }
    for (int i = 0; i < _textBottom.count(); i++)
        painter->drawText(QPoint(s_border, static_cast<int>(fm.height() * (1.5 + _textTop.count() + i))), _textBottom.at(i));

    if (_selectionNumber > 1)
    {
        painter->setFont(_smallFont);
        QString text = QString::number(_selectionIndex + 1) + " / " + QString::number(_selectionNumber);
        double posY = fm.height() * (1.5 + _textTop.count() + _textBottom.count()) + 3;
        double posX = 0.5 * (size.width() - QFontMetrics(_smallFont).width(text));
        painter->drawText(QPointF(posX, posY), text);
    }
}

QSizeF GraphicsLegendItem::getTextSize() const
{
    if (_textTop.isEmpty())
        return QSizeF(0, 0);

    QFontMetrics fm(_font);

    double height = (0.5 + _textTop.count() + _textBottom.count()) * fm.height() + 2 * s_border;
    double width = 0;
    foreach (QString line, _textTop)
        width = qMax(width, 1.5 * fm.width(s_rightArrow) + fm.width(line));
    foreach (QString line, _textBottom)
        width = qMax(width, static_cast<double>(fm.width(line)));
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
