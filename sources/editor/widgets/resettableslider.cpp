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

#include "resettableslider.h"
#include <QMouseEvent>

ResettableSlider::ResettableSlider(QWidget * parent) : QSlider(parent),
    _defaultValue(0)
{}

void ResettableSlider::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() != Qt::LeftButton)
    {
        QSlider::mouseDoubleClickEvent(event);
        return;
    }

    // The base implementation is not called since it would process the second click as a press
    event->accept();
    this->setValue(_defaultValue);
}
