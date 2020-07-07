/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "tableheaderviewv.h"
#include <QPainter>

const int TableHeaderViewV::MARGIN = 2;

TableHeaderViewV::TableHeaderViewV(QWidget *parent) : QHeaderView(Qt::Vertical, parent)
{
    this->setSectionResizeMode(QHeaderView::Fixed);
    this->setSectionsClickable(true);
    this->setHighlightSections(true);
}

void TableHeaderViewV::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // Get the text and icon to display
    QString text = this->model()->headerData(logicalIndex, this->orientation(), Qt::DisplayRole).toString();
    QPixmap icon = this->model()->headerData(logicalIndex, this->orientation(), Qt::DecorationRole).value<QPixmap>();

    // Icon and text rect
    QRect iconRect = icon.rect();
    QRect textRect = rect;
    textRect.setWidth(textRect.width() - 2 * MARGIN);
    if (iconRect.width() > 0)
        textRect.setWidth(textRect.width() - iconRect.width() - MARGIN);
    textRect.translate(MARGIN, 0);

    // Elide the text
    QFontMetrics fm(this->font());
    QString adaptedText = fm.elidedText(text, Qt::ElideRight, textRect.width());

    // First draw the cell without text or icon for the background and border
    this->model()->blockSignals(true);
    this->model()->setHeaderData(logicalIndex, this->orientation(), "", Qt::DisplayRole);
    this->model()->setHeaderData(logicalIndex, this->orientation(), QVariant(), Qt::DecorationRole);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    this->model()->setHeaderData(logicalIndex, this->orientation(), text, Qt::DisplayRole);
    this->model()->setHeaderData(logicalIndex, this->orientation(), icon, Qt::DecorationRole);
    this->model()->blockSignals(false);

    // Then draw the text
    QVariant foregroundBrush = model()->headerData(logicalIndex, this->orientation(), Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
        painter->setPen(foregroundBrush.value<QBrush>().color());
    painter->setClipRect(rect);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, adaptedText);

    // Finally draw the icon on the right and vertically centered
    int iconHeight = iconRect.height();
    int headerHeight = rect.height();
    int offsetY = iconHeight < headerHeight ? (headerHeight - iconHeight) / 2 : 0;
    painter->drawPixmap(textRect.right() + MARGIN, rect.top() + offsetY, iconRect.width(), iconRect.height(), icon);
}
