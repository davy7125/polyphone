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

#ifndef GRAPHICSLEGENDITEM_H
#define GRAPHICSLEGENDITEM_H

#include <QGraphicsItem>
#include <QBrush>
#include <QFont>
#include "pile_sf2.h"

class GraphicsLegendItem : public QGraphicsItem
{
public:
    GraphicsLegendItem(QString fontFamily, QGraphicsItem *parent = NULL);
    static void initSf2(Pile_sf2 * sf2) { s_sf2 = sf2; }

    void setIds(QList<EltID> ids, int selectionIndex, int selectionNumber);
    void setLeft(bool isLeft);
    bool isLeft();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);

private:
    static Pile_sf2 * s_sf2;
    static const int s_border;
    static const QBrush s_foregroundBrush;
    static const QPen s_borderPen;
    static const QPen s_textPen;

    const QFont _font;
    const QFont _smallFont;
    QStringList _text;
    int _selectionIndex;
    int _selectionNumber;
    int _alignment;

    QSizeF getTextSize() const;
    qreal dx(QSizeF size) const;
    qreal dy(QSizeF size) const;
};

#endif // GRAPHICSLEGENDITEM_H
