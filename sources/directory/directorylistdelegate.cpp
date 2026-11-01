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

#include "directorylistdelegate.h"
#include "directorylistview.h"
#include "directoryfiledata.h"
#include "utils.h"
#include "contextmanager.h"
#include <QListView>
#include <QPainter>
#include <QApplication>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QToolTip>

const int DirectoryListDelegate::MARGIN = 6;
const int DirectoryListDelegate::ICON_SIZE = 16;
const int DirectoryListDelegate::BUTTON_SIZE = 16;

DirectoryListDelegate::DirectoryListDelegate(DirectoryListView *parent) : QStyledItemDelegate(parent),
    _view(parent)
{
    _dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
    _colorEnabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::NORMAL);
    _colorHighlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::NORMAL);
    _colorDisabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::DISABLED);

    // Icons
    _iconRenameSelected = ContextManager::theme()->getColoredSvg(":/icons/pen.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
    _iconDeleteSelected = ContextManager::theme()->getColoredSvg(":/icons/trash.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
}

QSize DirectoryListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr)
        return QStyledItemDelegate::sizeHint(option, index);

    QFontMetrics fm(option.font);
    int height = fm.height() + 2 * MARGIN;

    // Second line with the preset summary or the status
    if (_view->isPresetSummaryDisplayed() && (d->getStatus() == DirectoryFileData::OK ||
                                              d->getStatus() == DirectoryFileData::NOT_INITIALIZED ||
                                              d->getStatus() == DirectoryFileData::CANNOT_SCAN))
        height += 0.5 * MARGIN + 0.9 * fm.height();

    return QSize(option.rect.width(), height);
}

void DirectoryListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    // Background
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

    // Text color
    bool selected = (option.state & QStyle::State_Selected);
    QColor textColor;
    if (d->getStatus() == DirectoryFileData::NOT_READABLE || d->getStatus() == DirectoryFileData::NOT_OPENABLE)
        textColor = _colorDisabled;
    else
        textColor = selected ?  _colorHighlighted : _colorEnabled;
    painter->setPen(textColor);

    // Rename and delete buttons
    int prefixWidth = 0;
    if (selected)
    {
        QRect btnRect = getRenameButtonRect(option);
        painter->drawPixmap(btnRect, _iconRenameSelected);
        btnRect = getDeleteButtonRect(option);
        painter->drawPixmap(btnRect, _iconDeleteSelected);
        prefixWidth = 2 * (MARGIN + ICON_SIZE);
    }

    // Main font
    QFont font = option.font;
    const QFontMetrics fm(font);
    painter->setFont(font);

    // First line: columns on the right, aligned with the header
    int left, right;
    for (int column = DirectoryListView::COLUMN_NAME + 1; column < DirectoryListView::COLUMN_NUMBER; column++)
    {
        if (!_view->isColumnDisplayed(column))
            continue;
        _view->getColumnLimits(column, option.rect.width(), left, right);
        QString text = getText(d, column);
        painter->drawText(option.rect.left() + right - fm.horizontalAdvance(text), option.rect.top() + MARGIN + fm.ascent(), text);
    }

    // Name and status
    QString fileName = d->getFileName();
    QString statusText = "";
    QString presetSummary = "";
    switch (d->getStatus())
    {
    case DirectoryFileData:: NOT_INITIALIZED:
        statusText = tr("Analyze in progress...");
        break;
    case DirectoryFileData::NOT_READABLE:
        fileName = "[" + tr("NOT READABLE") + "] " + fileName;
        break;
    case DirectoryFileData::NOT_OPENABLE:
        break;
    case DirectoryFileData::NOT_SCANNABLE:
        break;
    case DirectoryFileData::CANNOT_SCAN:
        statusText = tr("Error when reading the file.");
        break;
    case DirectoryFileData::OK:
        if (_view->isPresetSummaryDisplayed())
        {
            DirectoryFileData::DetailsData presets = d->getPresetDetails();
            QStringList listTmp;
            for (const QString &s : presets.names)
                listTmp << s.mid(8);
            presetSummary = listTmp.join(", ");
        }
        break;
    }

    // Second line with the preset summary or the status, otherwise the status follows the name
    QString secondLineText = "";
    if (_view->isPresetSummaryDisplayed())
    {
        secondLineText = statusText.isEmpty() ? presetSummary : statusText;
        statusText = "";
    }

    // First line: name on the left, elided first if a status follows
    _view->getColumnLimits(DirectoryListView::COLUMN_NAME, option.rect.width(), left, right);
    int x = option.rect.left() + left + prefixWidth;
    int availableWidth = right - left - prefixWidth;
    QFont fontBold = font;
    fontBold.setBold(true);
    const QFontMetrics fmBold(fontBold);
    QFont fontItalic = font;
    fontItalic.setItalic(true);
    const QFontMetrics fmItalic(fontItalic);
    int statusWidth = statusText.isEmpty() ? 0 : MARGIN + fmItalic.horizontalAdvance(statusText);
    fileName = fmBold.elidedText(fileName, Qt::ElideMiddle, qMax(availableWidth - statusWidth, availableWidth / 2));
    painter->setFont(fontBold);
    painter->drawText(x, option.rect.top() + MARGIN + fm.ascent(), fileName);
    if (!statusText.isEmpty())
    {
        x += fmBold.horizontalAdvance(fileName) + MARGIN;
        statusText = fmItalic.elidedText(statusText, Qt::ElideRight, option.rect.left() + right - x);
        painter->setFont(fontItalic);
        painter->drawText(x, option.rect.top() + MARGIN + fm.ascent(), statusText);
    }

    // Second line
    if (!secondLineText.isEmpty())
    {
        QFont fontSmall = font;
        fontSmall.setPointSizeF(font.pointSizeF() * 0.9);
        fontSmall.setItalic(true);
        const QFontMetrics fmSmall(fontSmall);
        secondLineText = fmSmall.elidedText(secondLineText, Qt::ElideRight, option.rect.width() - 2 * MARGIN);
        painter->setFont(fontSmall);
        painter->drawText(option.rect.left() + MARGIN, option.rect.top() + MARGIN + fm.height() + 0.5 * MARGIN + fmSmall.ascent(), secondLineText);
    }

    painter->restore();
}

QString DirectoryListDelegate::getText(const DirectoryFileData * d, int column) const
{
    switch (column)
    {
    case DirectoryListView::COLUMN_SIZE:
        return Utils::getFormattedSize(d->getFileSize());
    case DirectoryListView::COLUMN_DATE:
        return d->getLastModified().toString(_dateFormat);
    case DirectoryListView::COLUMN_SAMPLES:
        return d->getStatus() == DirectoryFileData::OK ? QString::number(d->getSampleCount()) : "";
    case DirectoryListView::COLUMN_INSTRUMENTS:
        return d->getStatus() == DirectoryFileData::OK ? QString::number(d->getInstrumentCount()) : "";
    case DirectoryListView::COLUMN_PRESETS:
        return d->getStatus() == DirectoryFileData::OK ? QString::number(d->getPresetCount()) : "";
    default:
        break;
    }
    return "";
}

QRect DirectoryListDelegate::getRenameButtonRect(const QStyleOptionViewItem &option) const
{
    QFont font = option.font;
    const QFontMetrics fm(font);
    int btnY = option.rect.top() + MARGIN + (fm.height() - BUTTON_SIZE) / 2;
    return QRect(option.rect.left() + MARGIN, btnY, BUTTON_SIZE, BUTTON_SIZE);
}

QRect DirectoryListDelegate::getDeleteButtonRect(const QStyleOptionViewItem &option) const
{
    QFont font = option.font;
    const QFontMetrics fm(font);
    int btnY = option.rect.top() + MARGIN + (fm.height() - BUTTON_SIZE) / 2;
    return QRect(option.rect.left() + 2 * MARGIN + BUTTON_SIZE, btnY, BUTTON_SIZE, BUTTON_SIZE);
}

bool DirectoryListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    QAbstractItemView * view = qobject_cast<QAbstractItemView *>(const_cast<QWidget *>(option.widget));
    if (d != nullptr && view != nullptr)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                _pressedIndex = index;
                _pressedWasSelected = view->selectionModel()->isSelected(index);
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (index == _pressedIndex && _pressedWasSelected && mouseEvent->button() == Qt::LeftButton)
            {
                if (getRenameButtonRect(option).contains(mouseEvent->pos()))
                {
                    emit renameRequested(d->getPath());
                    return true;
                }
                if (getDeleteButtonRect(option).contains(mouseEvent->pos()))
                {
                    emit deleteRequested(d->getPath());
                    return true;
                }
            }
            if (view->cursor() != Qt::PointingHandCursor && (
                    getRenameButtonRect(option).contains(mouseEvent->pos()) || getDeleteButtonRect(option).contains(mouseEvent->pos())))
                view->setCursor(Qt::PointingHandCursor);
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
            if (view->selectionModel()->isSelected(index) && (
                    getRenameButtonRect(option).contains(mouseEvent->pos()) || getDeleteButtonRect(option).contains(mouseEvent->pos())))
                view->setCursor(Qt::PointingHandCursor);
            else
                view->unsetCursor();
        }
        else if (event->type() == QEvent::Leave)
        {
            view->unsetCursor();
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool DirectoryListDelegate::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        QAbstractItemView *view = qobject_cast<QAbstractItemView *>(obj->parent());
        if (!view)
            return false;

        QModelIndex index = view->indexAt(helpEvent->pos());
        if (!index.isValid())
            return false;

        QStyleOptionViewItem option;
        option.initFrom(view);
        option.rect = view->visualRect(index);

        if (view->selectionModel()->isSelected(index))
        {
            const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
            if (d != nullptr)
            {
                if (getRenameButtonRect(option).contains(helpEvent->pos()))
                {
                    QToolTip::showText(helpEvent->globalPos(), tr("Rename file \"%1\"").arg(d->getFileName()));
                    return true;
                }
                if (getDeleteButtonRect(option).contains(helpEvent->pos()))
                {
                    QToolTip::showText(helpEvent->globalPos(), tr("Delete file \"%1\"").arg(d->getFileName()));
                    return true;
                }
            }
        }

        QToolTip::hideText();
        event->ignore();
        return true;
    }

    return QObject::eventFilter(obj, event);
}