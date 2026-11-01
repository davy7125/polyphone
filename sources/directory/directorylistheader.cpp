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

#include "directorylistheader.h"
#include "directorylistdelegate.h"
#include "contextmanager.h"
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>

DirectoryListHeader::DirectoryListHeader(QWidget *parent) : QWidget(parent),
    _listView(nullptr),
    _hoveredColumn(-1),
    _pressedColumn(-1)
{
    this->setMouseTracking(true);
    this->setFixedHeight(this->fontMetrics().height() + 2 * DirectoryListDelegate::MARGIN);

    // Colors
    _backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
    _hoveredBackgroundColor = ThemeManager::mix(_backgroundColor, ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.1);
    _textColor = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), _backgroundColor, 0.3);
    _borderColor = ContextManager::theme()->getColor(ThemeManager::BORDER);

    // Labels: texts or icons
    _texts[DirectoryListView::COLUMN_NAME] = tr("Name");
    _texts[DirectoryListView::COLUMN_SIZE] = tr("Size");
    _texts[DirectoryListView::COLUMN_DATE] = tr("Date");

    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::YELLOW, ThemeManager::LIST_BACKGROUND).name();
    _icons[DirectoryListView::COLUMN_SAMPLES] = ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(16, 16), replacement);
    _toolTips[DirectoryListView::COLUMN_SAMPLES] = tr("Number of samples");
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::BLUE, ThemeManager::LIST_BACKGROUND).name();
    _icons[DirectoryListView::COLUMN_INSTRUMENTS] = ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(16, 16), replacement);
    _toolTips[DirectoryListView::COLUMN_INSTRUMENTS] = tr("Number of instruments");
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND).name();
    _icons[DirectoryListView::COLUMN_PRESETS] = ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(16, 16), replacement);
    _toolTips[DirectoryListView::COLUMN_PRESETS] = tr("Number of presets");

    // Sort arrows
    replacement["currentColor"] = _textColor.name();
    _arrowUp = ContextManager::theme()->getColoredSvg(":/icons/arrow_up.svg", QSize(12, 12), replacement);
    _arrowDown = ContextManager::theme()->getColoredSvg(":/icons/arrow_down.svg", QSize(12, 12), replacement);
}

void DirectoryListHeader::setListView(DirectoryListView * listView)
{
    _listView = listView;
    connect(_listView, SIGNAL(columnsChanged()), this, SLOT(update()));

    // Each column is wide enough for its label and the sort arrow
    for (int column = 0; column < DirectoryListView::COLUMN_NUMBER; column++)
        _listView->setMinimumColumnWidth(column, getLabelWidth(column) + DirectoryListDelegate::MARGIN / 2 + _arrowUp.width());
}

int DirectoryListHeader::getTotalWidth() const
{
    // Same width as the rows, the scroll bar being excluded
    return _listView->isHidden() ? this->width() : _listView->viewport()->width();
}

int DirectoryListHeader::getLabelWidth(int column) const
{
    return _icons[column].isNull() ? this->fontMetrics().horizontalAdvance(_texts[column]) : _icons[column].width();
}

void DirectoryListHeader::getSectionLimits(int column, int &left, int &right) const
{
    // Sections are delimited in the middle of the space between two columns
    int totalWidth = getTotalWidth();
    int columnLeft, columnRight, otherLeft, otherRight;
    _listView->getColumnLimits(column, totalWidth, columnLeft, columnRight);

    left = 0;
    for (int previous = column - 1; previous >= 0; previous--)
    {
        if (_listView->isColumnDisplayed(previous))
        {
            _listView->getColumnLimits(previous, totalWidth, otherLeft, otherRight);
            left = (otherRight + columnLeft) / 2;
            break;
        }
    }

    right = totalWidth;
    for (int next = column + 1; next < DirectoryListView::COLUMN_NUMBER; next++)
    {
        if (_listView->isColumnDisplayed(next))
        {
            _listView->getColumnLimits(next, totalWidth, otherLeft, otherRight);
            right = (columnRight + otherLeft) / 2;
            break;
        }
    }
}

int DirectoryListHeader::getColumnAt(int x) const
{
    if (_listView == nullptr)
        return -1;

    int left, right;
    for (int column = 0; column < DirectoryListView::COLUMN_NUMBER; column++)
    {
        if (!_listView->isColumnDisplayed(column))
            continue;
        getSectionLimits(column, left, right);
        if (x >= left && x < right)
            return column;
    }
    return -1;
}

void DirectoryListHeader::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(this->rect(), _backgroundColor);

    if (_listView != nullptr)
    {
        int totalWidth = getTotalWidth();
        int sortColumn = _listView->getSortColumn();
        int arrowWidth = DirectoryListDelegate::MARGIN / 2 + _arrowUp.width();
        const QFontMetrics fm = this->fontMetrics();
        painter.setPen(_textColor);

        int left, right;
        for (int column = 0; column < DirectoryListView::COLUMN_NUMBER; column++)
        {
            if (!_listView->isColumnDisplayed(column))
                continue;

            // Background of the hovered section
            if (column == _hoveredColumn)
            {
                getSectionLimits(column, left, right);
                painter.fillRect(left, 0, right - left, this->height() - 1, _hoveredBackgroundColor);
            }

            // Label aligned like the content of the column (left for the name, right otherwise)
            // The sort arrow is on the inner side
            _listView->getColumnLimits(column, totalWidth, left, right);
            int labelWidth = getLabelWidth(column);
            int labelLeft, arrowLeft;
            if (column == DirectoryListView::COLUMN_NAME)
            {
                labelWidth = qMin(labelWidth, right - left - arrowWidth);
                labelLeft = left;
                arrowLeft = left + labelWidth + DirectoryListDelegate::MARGIN / 2;
            }
            else
            {
                labelLeft = right - labelWidth;
                arrowLeft = labelLeft - arrowWidth;
            }

            if (_icons[column].isNull())
            {
                QString text = (labelWidth < getLabelWidth(column)) ? fm.elidedText(_texts[column], Qt::ElideRight, labelWidth) : _texts[column];
                painter.drawText(labelLeft, (this->height() - fm.height()) / 2 + fm.ascent(), text);
            }
            else
                painter.drawPixmap(labelLeft, (this->height() - _icons[column].height()) / 2, _icons[column]);

            if (column == sortColumn)
            {
                const QPixmap &arrow = _listView->isSortDescending() ? _arrowDown : _arrowUp;
                painter.drawPixmap(arrowLeft, (this->height() - arrow.height()) / 2, arrow);
            }
        }
    }

    // Bottom border
    painter.setPen(_borderColor);
    painter.drawLine(0, this->height() - 1, this->width(), this->height() - 1);
}

bool DirectoryListHeader::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
        int column = getColumnAt(helpEvent->pos().x());
        if (column != -1 && !_toolTips[column].isEmpty())
            QToolTip::showText(helpEvent->globalPos(), _toolTips[column], this);
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }

    return QWidget::event(event);
}

void DirectoryListHeader::mouseMoveEvent(QMouseEvent *event)
{
    setHoveredColumn(getColumnAt(event->pos().x()));
    QWidget::mouseMoveEvent(event);
}

void DirectoryListHeader::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        _pressedColumn = getColumnAt(event->pos().x());
    QWidget::mousePressEvent(event);
}

void DirectoryListHeader::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _pressedColumn != -1 && _pressedColumn == getColumnAt(event->pos().x()))
        _listView->toggleSort(_pressedColumn);
    _pressedColumn = -1;
    QWidget::mouseReleaseEvent(event);
}

void DirectoryListHeader::leaveEvent(QEvent *event)
{
    setHoveredColumn(-1);
    QWidget::leaveEvent(event);
}

void DirectoryListHeader::setHoveredColumn(int column)
{
    if (column == _hoveredColumn)
        return;

    _hoveredColumn = column;
    if (column == -1)
        this->unsetCursor();
    else
        this->setCursor(Qt::PointingHandCursor);
    this->update();
}
