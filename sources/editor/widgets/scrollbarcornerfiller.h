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

#ifndef SCROLLBARCORNERFILLER_H
#define SCROLLBARCORNERFILLER_H

#include <QObject>
class QAbstractScrollArea;
class QWidget;

// Extends the vertical scroll bar of a scroll area down to the bottom, over the empty corner at the right of the
// horizontal scroll bar. Qt places the scroll bars (in containers, the parents of the scroll bars) at each resize or
// range change: the vertical container is extended each time it is placed, or when the horizontal one appears.
class ScrollBarCornerFiller : public QObject
{
    Q_OBJECT

public:
    static void install(QAbstractScrollArea * area);

protected:
    bool eventFilter(QObject * watched, QEvent * event) override;

private:
    ScrollBarCornerFiller(QAbstractScrollArea * area, QWidget * verticalContainer, QWidget * horizontalContainer);
    void adjust();

    QWidget * _verticalContainer;
    QWidget * _horizontalContainer;
    bool _adjusting;
};

#endif // SCROLLBARCORNERFILLER_H
