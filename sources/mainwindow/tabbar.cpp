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

#include "tabbar.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionTab>

TabBar::TabBar(QWidget *parent) : QTabBar(parent)
{
    this->setDrawBase(false);
}

void TabBar::mousePressEvent(QMouseEvent *event)
{
    // Current tab
    int tabIndex = this->tabAt(event->pos());

    // Position in the current tab
    _leftOffset = event->x() - this->tabRect(tabIndex).left() + 2;
    _rightOffset = this->tabRect(tabIndex).right() - event->x() + 1;

    // All tabs movable except the first one
    setMovable(tabIndex != 0);
    QTabBar::mousePressEvent(event);
}

void TabBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->x() - _leftOffset < this->tabRect(0).right())
    {
        // Too far on the left
        event->ignore();
        QMouseEvent newEvent(event->type(),
                             QPointF(this->tabRect(0).right() + _leftOffset, event->y()),
                             event->button(), event->buttons(), event->modifiers());
        QTabBar::mouseMoveEvent(&newEvent);
    }
    else if (event->x() + _rightOffset > this->tabRect(this->count() - 1).right())
    {
        // Too far on the right
        event->ignore();
        QMouseEvent newEvent(event->type(),
                             QPointF(this->tabRect(this->count() - 1).right() - _rightOffset, event->y()),
                             event->button(), event->buttons(), event->modifiers());
        QTabBar::mouseMoveEvent(&newEvent);
    }
    else
        QTabBar::mouseMoveEvent(event);
}

void TabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOptionTab opt;
    QStylePainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (int i = 0;i < count();i++)
    {
        initStyleOption(&opt, i);
        painter.drawControl(QStyle::CE_TabBarTab, opt);
        //painter.setPen(Qt::white);
        //painter.drawText(opt.rect, Qt::AlignVCenter | Qt::AlignHCenter, opt.text);
    }
    //QTabBar::paintEvent(event);
}
