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

DirectoryListDelegate::DirectoryListDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
    _colorEnabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::NORMAL);
    _colorHighlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::NORMAL);
    _colorDisabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::DISABLED);

    // Icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::YELLOW, ThemeManager::LIST_BACKGROUND).name();
    _iconSample = ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(ICON_SIZE, ICON_SIZE), replacement);
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::BLUE, ThemeManager::LIST_BACKGROUND).name();
    _iconInstrument = ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(ICON_SIZE, ICON_SIZE), replacement);
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND).name();
    _iconPreset = ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(ICON_SIZE, ICON_SIZE), replacement);

    _iconSampleSelected = ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(ICON_SIZE, ICON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
    _iconInstrumentSelected = ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(ICON_SIZE, ICON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
    _iconPresetSelected = ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(ICON_SIZE, ICON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);

    _iconRename = ContextManager::theme()->getColoredSvg(":/icons/pen.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::LIST_TEXT);
    _iconRenameSelected = ContextManager::theme()->getColoredSvg(":/icons/pen.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
    _iconDelete = ContextManager::theme()->getColoredSvg(":/icons/trash.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::LIST_TEXT);
    _iconDeleteSelected = ContextManager::theme()->getColoredSvg(":/icons/trash.svg", QSize(BUTTON_SIZE, BUTTON_SIZE), ThemeManager::HIGHLIGHTED_TEXT);
}

QSize DirectoryListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr)
        return QStyledItemDelegate::sizeHint(option, index);

    QFontMetrics fm(option.font);
    int height = fm.height() + 2 * MARGIN;

    if (d->getStatus() == DirectoryFileData::OK || d->getStatus() == DirectoryFileData::NOT_INITIALIZED)
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

    // First line text
    QString fileName = d->getFileName();
    QString suffix = QString(" (%1 - %2)").arg(Utils::getFormattedSize(d->getFileSize()), d->getLastModified().toString(_dateFormat));
    int suffixWidth = fm.horizontalAdvance(suffix);
    int counterWidth = 0;

    // Second line text
    QString secondLineText = "";

    switch (d->getStatus())
    {
    case DirectoryFileData:: NOT_INITIALIZED:
        secondLineText = tr("Analyze in progress...");
        break;
    case DirectoryFileData::NOT_READABLE:
        fileName = "[" + tr("NOT READABLE") + "] " + fileName;
        break;
    case DirectoryFileData::NOT_OPENABLE:
        break;
    case DirectoryFileData::NOT_SCANNABLE:
        break;
    case DirectoryFileData::CANNOT_SCAN:
        secondLineText = tr("Error when reading the file.");
        break;
    case DirectoryFileData::OK:
        // First line: counters on the right
        counterWidth = getCounterWidth(fm, d);
        paintCounters(painter, fm, d, option.rect.right() - MARGIN - counterWidth, option.rect.top() + MARGIN, selected);

        // Second line
        {
            DirectoryFileData::DetailsData presets = d->getPresetDetails();
            QStringList listTmp;
            for (const QString &s : presets.names)
                listTmp << s.mid(8);
            secondLineText = listTmp.join(", ");
        }
        break;
    }

    // First line: text on the left
    QFont fontBold = font;
    fontBold.setBold(true);
    const QFontMetrics fmBold(fontBold);
    fileName = fmBold.elidedText(fileName, Qt::ElideMiddle, option.rect.width() - 3 * MARGIN - counterWidth - suffixWidth - prefixWidth);
    painter->setFont(fontBold);
    painter->drawText(option.rect.left() + MARGIN + prefixWidth, option.rect.top() + MARGIN + fm.ascent(), fileName);
    painter->setFont(font);
    painter->drawText(option.rect.left() + MARGIN + prefixWidth + fmBold.horizontalAdvance(fileName), option.rect.top() + MARGIN + fm.ascent(), suffix);

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

int DirectoryListDelegate::getCounterWidth(const QFontMetrics &fm, const DirectoryFileData * d) const
{
    return ICON_SIZE + MARGIN / 2 +
           fm.horizontalAdvance(QString::number(d->getSampleCount())) +
           MARGIN +
           ICON_SIZE + MARGIN / 2 +
           fm.horizontalAdvance(QString::number(d->getInstrumentCount())) +
           MARGIN +
           ICON_SIZE + MARGIN / 2 +
           fm.horizontalAdvance(QString::number(d->getPresetCount()));
}

void DirectoryListDelegate::paintCounters(QPainter *painter, const QFontMetrics &fm, const DirectoryFileData * d, int x, int y, bool selected) const
{
    int deltaPixmap = (fm.height() - ICON_SIZE) / 2;

    painter->drawPixmap(x, y + deltaPixmap, selected ? _iconSampleSelected : _iconSample);
    x += ICON_SIZE + MARGIN / 2;
    int count = d->getSampleCount();
    painter->drawText(x, y + fm.ascent(), QString::number(count));
    x += MARGIN + fm.horizontalAdvance(QString::number(count));

    painter->drawPixmap(x, y + deltaPixmap, selected ? _iconInstrumentSelected : _iconInstrument);
    x += ICON_SIZE + MARGIN / 2;
    count = d->getInstrumentCount();
    painter->drawText(x, y + fm.ascent(), QString::number(count));
    x += MARGIN + fm.horizontalAdvance(QString::number(count));

    painter->drawPixmap(x, y + deltaPixmap, selected ? _iconPresetSelected : _iconPreset);
    x += ICON_SIZE + MARGIN / 2;
    count = d->getPresetCount();
    painter->drawText(x, y + fm.ascent(), QString::number(count));
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