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

#include "elidedpushbutton.h"

void ElidedPushButton::computeText(int width)
{
    QFontMetrics metrics(this->font());
    QString elidedText = metrics.elidedText(_text, Qt::ElideMiddle, width);
    this->setText(elidedText);
}

void ElidedPushButton::setTextToElide(const QString text, int availableWidth)
{
    _text = text;
    _availableWidth = availableWidth;
}

void ElidedPushButton::setAvailableWidth(int availableWidth)
{
    _availableWidth = availableWidth;
    computeText(availableWidth);
}

void ElidedPushButton::resizeEvent(QResizeEvent * event)
{
    computeText(_availableWidth == -1 ? event->size().width() : _availableWidth);
    QPushButton::resizeEvent(event);
}
