/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef GRAPHICSVIEWENVELOPS_H
#define GRAPHICSVIEWENVELOPS_H

#include <QGraphicsView>
#include "pile_sf2.h"
#include "envelop.h"

class GraphicsViewEnvelop : public QGraphicsView
{
    Q_OBJECT

public:
    explicit GraphicsViewEnvelop(QWidget *parent = 0);
    ~GraphicsViewEnvelop();

    // Clear all envelops
    void clearEnvelops();

    // Prepare an envelop (-1 is automatically added)
    void addEnvelop(int index);

    // Set the value of an envelop
    void setValue(int index, Envelop::ValueType type, double value, bool isDefined = true);

    // Set the selected envelops
    void setSelection(QList<int> index);

signals:
    void valueChanged(int index, int type, double value, bool isDefined);

protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *) {}
    void scrollContentsBy(int dx, int dy);

private:
    QMap<int, Envelop *> _envelops;
    bool _dontRememberScroll;

    // Graphics items
    QGraphicsScene * _scene;
};

#endif // GRAPHICSVIEWENVELOPS_H
