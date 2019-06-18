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

#include "animatedbackground.h"
#include <QPainter>
#include <QDateTime>

AnimatedBackground::AnimatedBackground(QWidget *parent) : QWidget(parent),
    _pixmap(nullptr)
{
    QDate today = QDateTime::currentDateTime().date();

    if (today.month() == 12 && today.day() > 24) // Between christmas and new year
        _pixmap = new QPixmap(":/banners/christmas.png");
    else if (today.month() == 10 && today.day() == 31) // Halloween
        _pixmap = new QPixmap(":/banners/halloween.png");
    else if (today.month() == 2 && today.day() == 14) // Valentine's day
        _pixmap = new QPixmap(":/banners/valentine.png");
    else if (today.month() == 6 && today.day() == 21) // Summer
        _pixmap = new QPixmap(":/banners/music.png");
}

AnimatedBackground::~AnimatedBackground()
{
    delete _pixmap;
}

void AnimatedBackground::paintEvent(QPaintEvent *event)
{
    if (_pixmap)
    {
        QPainter p(this);
        for (int i = 0; i < this->width() / _pixmap->width() + 1; i++)
            p.drawPixmap(i * _pixmap->width(), 0, *_pixmap);
    }

    QWidget::paintEvent(event);
}
