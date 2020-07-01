/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "transparentframe.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

TransparentFrame::TransparentFrame(QWidget *parent) : QFrame(parent)
{
    _opacity = new QGraphicsOpacityEffect(this);
    _opacity->setOpacity(0.4);
    this->setGraphicsEffect(_opacity);

    _animation = new QPropertyAnimation(_opacity, "opacity");
}

TransparentFrame::~TransparentFrame()
{
    delete _animation;
    delete _opacity;
}

void TransparentFrame::enterEvent(QEvent * event)
{
    QFrame::enterEvent(event);

    _animation->stop();
    _animation->setStartValue(_opacity->opacity());
    _animation->setEndValue(1);
    _animation->setDuration(150);
    _animation->start();
}
void TransparentFrame::leaveEvent(QEvent * event)
{
    QFrame::leaveEvent(event);

    _animation->stop();
    _animation->setStartValue(_opacity->opacity());
    _animation->setEndValue(0.4);
    _animation->setDuration(300);
    _animation->start();
}
