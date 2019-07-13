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

#include "styledslider.h"
#include "contextmanager.h"
#include <QPainter>

StyledSlider::StyledSlider(QWidget *parent) : QSlider(parent),
    _fromMiddle(false),
    _middleValue(-1),
    _pressed(true),
    _fast(false)
{
    // Colors
    _highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::BUTTON_BACKGROUND);
    QColor borderColor = _backgroundColor.darker(140);

    this->setStyleSheet(QString("QSlider::groove:horizontal {" )+
                        "border: 1px solid " + borderColor.name() + ";border-radius: 3px;" +
                        "height: 6px; " +
                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #22000000, stop:1 #00000000);}" +
                        "QSlider::handle:horizontal {" +
                        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 " +
                        _backgroundColor.light().name() + ", stop:1 " + _backgroundColor.name() + ");" +
                        "border: 1px solid " + borderColor.name() + ";" +
                        "width: 18px;margin: -4px 0;border-radius: 3px;}"
                        );

    // Timer for the animation
    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    connect(this, SIGNAL(sliderPressed()), this, SLOT(onSliderPressed()));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
}


void StyledSlider::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    // Set the background
    int x1 = 0;
    double length = this->width();
    int height = 6;
    int y1 = (this->height() - height) / 2;
    p.fillRect(QRect(x1, y1, static_cast<int>(length), height), _backgroundColor.darker(110));

    // And over it a highlight
    if (_fromMiddle)
    {
        x1 = this->width() / 2;
        length = static_cast<double>(this->value() - this->maximum() / 2) / this->maximum() * this->width();
        length += (length < 0 ? 5 : -5);
    }
    else
    {
        x1 = 0;
        length = (this->width() * this->value()) / this->maximum();
    }
    p.fillRect(QRect(x1, y1, static_cast<int>(length), height), _highlightColor);

    // Paint the widget
    QSlider::paintEvent(event);
}

void StyledSlider::onTimeout()
{
    if (_pressed)
        return;

    int delta = _middleValue - this->value();
    if (delta != 0)
    {
        _fast = true; // In the animation
        int move = delta / 8;
        if (move == 0)
            move = delta < 0 ? -1 : 1;
        this->setValue(this->value() + move);
    }

    // Now restore the pause
    _fast = false;
}

void StyledSlider::onValueChanged(int value)
{
    if (!_pressed && _middleValue != -1 && value != _middleValue)
        _timer.start(_fast ? 20 : 500); // Possibly add a pause
}
