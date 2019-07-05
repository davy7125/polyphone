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

#ifndef GRAPHICSLEGENDITEM2_H
#define GRAPHICSLEGENDITEM2_H

#include <QGraphicsItem>
#include <QBrush>
#include <QFont>
#include <QPen>

class GraphicsLegendItem2 : public QGraphicsItem
{
public:
    GraphicsLegendItem2(QString fontFamily, QGraphicsItem *parent = nullptr);

    void setNewValues(int minKey, int maxKey, int minVel, int maxVel);
    void setLeft(bool isLeft);
    void setOffsetY(double offsetY) { _offsetY = offsetY; }
    bool isLeft();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr);

private:
    static const int s_border;
    QBrush _foregroundBrush;
    QPen _borderPen;
    QPen _textPen;

    const QFont _font;
    QStringList _text;
    int _alignment;
    double _offsetY;

    QSizeF getTextSize() const;
    qreal dx(QSizeF size) const;
    qreal dy(QSizeF size) const;
};

#endif // GRAPHICSLEGENDITEM2_H
