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

#ifndef GRAPHICSLEGENDITEM2_H
#define GRAPHICSLEGENDITEM2_H

#include <QGraphicsItem>
#include <QBrush>
#include <QFont>

class GraphicsLegendItem2 : public QGraphicsItem
{
public:
    GraphicsLegendItem2(QString fontFamily, QGraphicsItem *parent = NULL);

    void setNewValues(int minKey, int maxKey, int minVel, int maxVel);
    void setLeft(bool isLeft);
    void setOffsetY(double offsetY) { _offsetY = offsetY; }
    bool isLeft();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);

private:
    static const int s_border;
    static const QBrush s_foregroundBrush;
    static const QPen s_borderPen;
    static const QPen s_textPen;

    const QFont _font;
    QStringList _text;
    int _alignment;
    double _offsetY;

    QSizeF getTextSize() const;
    qreal dx(QSizeF size) const;
    qreal dy(QSizeF size) const;
};

#endif // GRAPHICSLEGENDITEM2_H
