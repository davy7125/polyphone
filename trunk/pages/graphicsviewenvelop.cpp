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

#include "graphicsviewenvelop.h"
#include <QDebug>
#include <QResizeEvent>

GraphicsViewEnvelop::GraphicsViewEnvelop(QWidget *parent) : QGraphicsView(parent),
    _dontRememberScroll(false),
    _scene(new QGraphicsScene())
{
    // Configuration
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setMouseTracking(true);

    // Preparation of the graphics
    this->setScene(_scene);
}

GraphicsViewEnvelop::~GraphicsViewEnvelop()
{
    clearEnvelops();
}

void GraphicsViewEnvelop::clearEnvelops()
{
    // Delete all envelops
    while (!_envelops.isEmpty())
        delete _envelops.take(_envelops.keys().first());
}

void GraphicsViewEnvelop::addEnvelop(int index)
{
    Envelop * parent = NULL;
    if (index != -1 && _envelops.keys().contains(-1))
        parent = _envelops[-1];
    _envelops[index] = new Envelop(_scene, parent);
}

void GraphicsViewEnvelop::setValue(int index, Envelop::ValueType type, double value, bool isDefined)
{
    _envelops[index]->set(type, value, isDefined);
}

void GraphicsViewEnvelop::setSelection(QList<int> index)
{
    qDebug() << "selection" << index;
}

void GraphicsViewEnvelop::resizeEvent(QResizeEvent * event)
{
    _dontRememberScroll = true;
    _scene->setSceneRect(0, 0, event->size().width(), event->size().height());
    this->fitInView(0, 0, event->size().width(), event->size().height());

    // Adapt the envelops
//    foreach (Envelop * env, _envelops)
//        env->replot(event->size());

    QGraphicsView::resizeEvent(event);
    _dontRememberScroll = false;
}

void GraphicsViewEnvelop::mousePressEvent(QMouseEvent *event)
{

}

void GraphicsViewEnvelop::mouseReleaseEvent(QMouseEvent *event)
{

}

void GraphicsViewEnvelop::mouseMoveEvent(QMouseEvent *event)
{

}

void GraphicsViewEnvelop::wheelEvent(QWheelEvent * event)
{

}

void GraphicsViewEnvelop::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    if (_dontRememberScroll)
        return;

}
