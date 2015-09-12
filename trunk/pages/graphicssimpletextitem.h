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

#ifndef GRAPHICSSIMPLETEXTITEM_H
#define GRAPHICSSIMPLETEXTITEM_H

#include <QGraphicsSimpleTextItem>

// QGraphicsSimpleTextItem with alignment and ignoring transformations
class GraphicsSimpleTextItem : public QGraphicsSimpleTextItem
{
public:
    GraphicsSimpleTextItem(int alignment, QGraphicsItem* parent = 0) : QGraphicsSimpleTextItem(parent),
        _alignment(alignment)
    {
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    }

    QRectF boundingRect() const
    {
        QRectF br = QGraphicsSimpleTextItem::boundingRect();
        return br.translated(dx(br), dy(br));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0)
    {
        painter->translate(dx(boundingRect()), dy(boundingRect()));
        QGraphicsSimpleTextItem::paint(painter, option, widget);
    }

private:
    qreal dx(QRectF br) const
    {
        double width = br.width();
        if (_alignment & Qt::AlignHCenter)
            return -width / 2;
        else if (_alignment & Qt::AlignRight)
            return -width;
        return 0;
    }

    qreal dy(QRectF br) const
    {
        double height = br.height() - 3;
        if (_alignment & Qt::AlignVCenter)
            return -height / 2;
        else if (_alignment & Qt::AlignBottom)
            return -height;
        return 0;
    }

    int _alignment;
};

#endif // GRAPHICSSIMPLETEXTITEM_H
