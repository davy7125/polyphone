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

#include "topbackground.h"
#include "contextmanager.h"
#include <QPainter>
#include <QDateTime>

TopBackground::TopBackground(QWidget *parent) : QWidget(parent),
    _pixmapBack(nullptr),
    _pixmapFront(nullptr)
{
    // Background
    _pixmapBack = ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT) ?
                      new QPixmap(":/misc/background_dark.png") : new QPixmap(":/misc/background.png");

    QDate today = QDateTime::currentDateTime().date();
    if (today.month() == 12 && today.day() > 24) // Between christmas and new year
        _pixmapFront = new QPixmap(":/banners/christmas.png");
    else if (today.month() == 10 && today.day() == 31) // Halloween
        _pixmapFront = new QPixmap(":/banners/halloween.png");
    else if (today.month() == 2 && today.day() == 14) // Valentine's day
        _pixmapFront = new QPixmap(":/banners/valentine.png");
    else if (today.month() == 6 && today.day() == 21) // Summer
        _pixmapFront = new QPixmap(":/banners/music.png");

    // Color of the bottom border
    _bottomBorderColor = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT),
        ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT) ? 0.2 : 0.7);
}

TopBackground::~TopBackground()
{
    delete _pixmapBack;
    delete _pixmapFront;
}

void TopBackground::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    // Background
    for (int i = 0; i < this->width() / _pixmapBack->width() + 1; i++)
        for (int j = 0; j < this->height() / _pixmapBack->height() + 1; j++)
            p.drawPixmap(i * _pixmapBack->width(), j * _pixmapBack->height(), *_pixmapBack);

    // Possible pattern on top of it
    if (_pixmapFront)
    {
        for (int i = 0; i < this->width() / _pixmapFront->width() + 1; i++)
            p.drawPixmap(i * _pixmapFront->width(), 0, *_pixmapFront);
    }

    // Bottom border
    p.setPen(QPen(_bottomBorderColor, 1));
    p.drawLine(0, this->height(), this->width(), this->height());

    QWidget::paintEvent(event);
}
