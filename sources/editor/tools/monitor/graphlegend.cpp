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

#include "graphlegend.h"
#include <QPainter>
#include <QPaintEvent>

const int GraphLegend::MARGIN = 5;

GraphLegend::GraphLegend(QWidget *parent) : QWidget(parent) {}

void GraphLegend::plot(QColor color, int thickness)
{
    _pen = QPen(color, thickness, Qt::SolidLine, Qt::RoundCap);
}

void GraphLegend::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = event->rect();
    painter.setPen(_pen);
    int y = (rect.top() + rect.bottom()) / 2;
    painter.drawLine(rect.left() + MARGIN, y, rect.right() - MARGIN, y);
}
