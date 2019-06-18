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

#include "rotatingspinner.h"
#include "contextmanager.h"
#include <QPropertyAnimation>
#include <QPainter>

RotatingSpinner::RotatingSpinner(QWidget *parent) : QWidget(parent),
    _animation(new QPropertyAnimation(this))
{
    // Configure the animation
    _animation->setTargetObject(this);
    _animation->setStartValue(0);
    _animation->setEndValue(360);
    _animation->setDuration(1500);

    connect(_animation, SIGNAL(valueChanged(const QVariant&)), SLOT(update()));
    connect(_animation, SIGNAL(finished()), _animation, SLOT(start()));
}

RotatingSpinner::~RotatingSpinner()
{
    _animation->stop();
    delete _animation;
}

void RotatingSpinner::startAnimation()
{
    _animation->setCurrentTime(0);
    _animation->start();
}

void RotatingSpinner::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    // Get the image and scale it if not already done or if the size changes
    if (_pixmap.isNull() || _size != this->size())
    {
        _pixmap = ContextManager::theme()->getColoredSvg(":/misc/spinner.svg", this->size(), ThemeManager::WINDOW_TEXT);
        _size = this->size();
    }

    // Rotation
    QTransform transform;
    transform.rotate(_animation->currentValue().toDouble());
    QPixmap pix = _pixmap.transformed(transform, Qt::SmoothTransformation);

    // Draw the spinner
    QPainter painter(this);
    double x = ((double)(this->width() - pix.width())) / 2.0;
    double y = ((double)(this->height() - pix.height())) / 2.0;
    painter.drawPixmap(QPointF(x, y), pix);
}
