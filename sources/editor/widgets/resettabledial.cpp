/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "resettabledial.h"
#include <QMouseEvent>

// Number of pixels for covering the whole range, multiplied by 10 with Shift for a fine adjustment
static const double s_dragLength = 200.0;

ResettableDial::ResettableDial(QWidget * parent) : QDial(parent),
    _defaultValue(0),
    _dragValue(0)
{}

void ResettableDial::mousePressEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mousePressEvent(event);
        return;
    }

    // Unlike QDial, the value doesn't jump to the position of the mouse
    event->accept();
    _dragValue = this->value();
    _lastPosition = event->position();
    this->setSliderDown(true);
}

void ResettableDial::mouseMoveEvent(QMouseEvent * event)
{
    // Not after a double click, the button being held without having been pressed
    if (!this->isSliderDown())
    {
        event->ignore();
        return;
    }

    // The value increases upward and to the right
    QPointF delta = event->position() - _lastPosition;
    _lastPosition = event->position();
    double length = (event->modifiers() & Qt::ShiftModifier) ? 10.0 * s_dragLength : s_dragLength;
    _dragValue += (delta.x() - delta.y()) * (this->maximum() - this->minimum()) / length;
    _dragValue = qBound(static_cast<double>(this->minimum()), _dragValue, static_cast<double>(this->maximum()));
    this->setSliderPosition(qRound(_dragValue));
}

void ResettableDial::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mouseReleaseEvent(event);
        return;
    }

    // QDial would take the value under the mouse
    this->setSliderDown(false);
}

void ResettableDial::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QDial::mouseDoubleClickEvent(event);
        return;
    }

    // The base implementation is not called since it would process the second click as a press
    event->accept();
    this->setValue(_defaultValue);
}
