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

#include "tablepageoverview.h"
#include "contextmanager.h"
#include <QHeaderView>
#include <QPainter>

TablePageOverview::TablePageOverview(QWidget *parent) : QTableWidget(parent)
{
    // Custom header
    setHorizontalHeader(new OverviewTableHeaderView(this));

    // Style of the table content
    this->setAlternatingRowColors(true);
    this->setStyleSheet("QTableWidget{border:1px solid " +
                        this->palette().dark().color().name() +
                        ";border-top:0;border-left:0;border-right:0" +
                        ";gridline-color: " + this->palette().dark().color().name() + "}");
}

OverviewTableHeaderView::OverviewTableHeaderView(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    this->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    this->setResizeMode(QHeaderView::ResizeToContents);
#endif

    // Text in bold
    this->setHighlightSections(false);
    QFont font = this->font();
    font.setPointSize(10);
    font.setBold(true);
    this->setFont(font);

    // Height of the header
    QFontMetrics fm(this->font());
    _height = fm.height() * 2 + 8;

    // Pixmap, text color for painting a section
    _arrowDown = ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(12, 12), ThemeManager::LIST_TEXT);
    _arrowUp = ContextManager::theme()->getColoredSvg(":/icons/arrow_up.svg", QSize(12, 12), ThemeManager::LIST_TEXT);
    _textColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT);

    // Style
    this->setStyleSheet("QHeaderView::section{border:1px solid " + this->palette().dark().color().name() +
                        ";border-top: 0; border-left: 0}" +
                        "QHeaderView::down-arrow {image: url(:/icons/empty)}" +
                        "QHeaderView::up-arrow {image: url(:/icons/empty)}");

    // The sections can be clicked for sorting rows
    this->setSectionsClickable(true);
}

const int OverviewTableHeaderView::MARGIN = 2;

QSize OverviewTableHeaderView::sizeHint() const
{
    // Override the height with a custom value
    QSize size = QHeaderView::sizeHint();
    size.setHeight(_height);
    return size;
}

void OverviewTableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // Get the text and icon to display
    QString text = this->model()->headerData(logicalIndex, this->orientation(), Qt::DisplayRole).toString();

    // Sort icon?
    QPixmap icon;
    if (this->sortIndicatorSection() == logicalIndex)
       icon = this->sortIndicatorOrder() == 0 ? _arrowDown : _arrowUp;

    // Icon and text rect
    QRect iconRect = icon.rect();
    QRect textRect = rect;
    textRect.setWidth(textRect.width() - 2 * MARGIN);
    if (iconRect.width() > 0)
        textRect.setWidth(textRect.width() - iconRect.width() - MARGIN);
    textRect.translate(MARGIN, 0);

    // Adapt the text
    QString adaptedText = text;
    int lengthLine1 = text.length();
    QFontMetrics fm(this->font());
    while (fm.horizontalAdvance(text.left(lengthLine1)) > textRect.width() && lengthLine1 > 0)
        lengthLine1--;
    if (lengthLine1 < text.length())
        adaptedText = text.left(lengthLine1) + "\n" + fm.elidedText(text.mid(lengthLine1), Qt::ElideRight, textRect.width());

    // First draw the cell without text for the background and border
    this->model()->blockSignals(true);
    this->model()->setHeaderData(logicalIndex, this->orientation(), "", Qt::DisplayRole);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    this->model()->setHeaderData(logicalIndex, this->orientation(), text, Qt::DisplayRole);
    this->model()->blockSignals(false);

    // Then draw the text
    painter->setPen(_textColor);
    painter->setClipRect(rect);
    painter->drawText(textRect, Qt::AlignCenter, text);

    // Finally draw the icon on the right and vertically centered
    int iconHeight = iconRect.height();
    int headerHeight = rect.height();
    int offsetY = iconHeight < headerHeight ? (headerHeight - iconHeight) / 2 : 0;
    painter->drawPixmap(textRect.right() + MARGIN, rect.top() + offsetY, iconRect.width(), iconRect.height(), icon);
}
