/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "scrollbarcornerfiller.h"
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QEvent>

void ScrollBarCornerFiller::install(QAbstractScrollArea * area)
{
    // The containers stay the same if the scroll bars are replaced
    QWidget * verticalContainer = area->verticalScrollBar()->parentWidget();
    QWidget * horizontalContainer = area->horizontalScrollBar()->parentWidget();
    if (verticalContainer == nullptr || horizontalContainer == nullptr ||
        verticalContainer == area || horizontalContainer == area)
        return;
    new ScrollBarCornerFiller(area, verticalContainer, horizontalContainer);
}

ScrollBarCornerFiller::ScrollBarCornerFiller(QAbstractScrollArea * area, QWidget * verticalContainer, QWidget * horizontalContainer) :
    QObject(area),
    _verticalContainer(verticalContainer),
    _horizontalContainer(horizontalContainer),
    _adjusting(false)
{
    verticalContainer->installEventFilter(this);
    horizontalContainer->installEventFilter(this);
    adjust();
}

bool ScrollBarCornerFiller::eventFilter(QObject * watched, QEvent * event)
{
    Q_UNUSED(watched)
    switch (event->type())
    {
    case QEvent::Resize: case QEvent::Move: case QEvent::Show: case QEvent::Hide:
        adjust();
        break;
    default:
        break;
    }
    return false;
}

void ScrollBarCornerFiller::adjust()
{
    if (_adjusting || _verticalContainer->isHidden() || _horizontalContainer->isHidden())
        return;

    // Corner: the horizontal scroll bar goes lower than the vertical one, without being under it
    QRect vertical = _verticalContainer->geometry();
    QRect horizontal = _horizontalContainer->geometry();
    if (vertical.bottom() >= horizontal.bottom() || (horizontal.right() >= vertical.left() && horizontal.left() <= vertical.right()))
        return;

    _adjusting = true;
    vertical.setBottom(horizontal.bottom());
    _verticalContainer->setGeometry(vertical);
    _adjusting = false;
}
