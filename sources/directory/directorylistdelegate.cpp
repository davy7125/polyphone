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

const int DirectoryListDelegate::MARGIN = 6;
const int DirectoryListDelegate::ICON_SIZE = 16;

DirectoryListDelegate::DirectoryListDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
    _colorEnabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::NORMAL);
    _colorHighlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::NORMAL);
    _colorDisabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::DISABLED);

    // Icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::YELLOW, ThemeManager::LIST_BACKGROUND).name();
    _iconSample = ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(16, 16), replacement);
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::BLUE, ThemeManager::LIST_BACKGROUND).name();
    _iconInstrument = ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(16, 16), replacement);
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND).name();
    _iconPreset = ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(16, 16), replacement);

    _iconSampleSelected = ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);
    _iconInstrumentSelected = ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);
    _iconPresetSelected = ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);
}

QSize DirectoryListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr)
        return QStyledItemDelegate::sizeHint(option, index);

    QFontMetrics fm(option.font);
    int height = fm.height() + 2 * MARGIN;

    if (d->getStatus() == DirectoryFileData::OK)
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
    QColor textColor;
    if (d->getStatus() == DirectoryFileData::NOT_READABLE || d->getStatus() == DirectoryFileData::NOT_OPENABLE)
        textColor = _colorDisabled;
    else
        textColor = (option.state & QStyle::State_Selected) ?  _colorHighlighted : _colorEnabled;
    painter->setPen(textColor);

    // Font
    QFont font = option.font;
    const QFontMetrics fm(font);

    // First line text
    QString fileName = d->getFileName();
    QString suffix = QString(" (%1 - %2)").arg(Utils::getFormattedSize(d->getFileSize()), d->getLastModified().toString(_dateFormat));
    int suffixWidth = fm.horizontalAdvance(suffix);

    if (d->getStatus() == DirectoryFileData::OK)
    {
        // Bold font
        QFont fontBold = font;
        fontBold.setBold(true);
        const QFontMetrics fmBold(fontBold);

        // First line: counters on the right
        int counterWidth = getCounterWidth(fm, d);
        paintCounters(painter, fm, d, option.rect.right() - MARGIN - counterWidth, option.rect.top() + MARGIN, option.state & QStyle::State_Selected);

        // First line: text on the left
        fileName = fmBold.elidedText(fileName, Qt::ElideMiddle, option.rect.width() - 3 * MARGIN - counterWidth - suffixWidth);
        painter->setFont(fontBold);
        painter->drawText(option.rect.left() + MARGIN, option.rect.top() + MARGIN + fm.ascent(), fileName);
        painter->setFont(font);
        painter->drawText(option.rect.left() + MARGIN + fmBold.horizontalAdvance(fileName), option.rect.top() + MARGIN + fm.ascent(), suffix);

        // Second line
        QFont fontSmall = font;
        fontSmall.setPointSizeF(font.pointSizeF() * 0.9);
        fontSmall.setItalic(true);
        painter->setFont(fontSmall);
        const QFontMetrics fmSmall(fontSmall);

        DirectoryFileData::DetailsData presets = d->getPresetDetails();
        QStringList listTmp;
        for (const QString &s : presets.names)
            listTmp << s.mid(8);
        QString secondLineText = listTmp.join(", ");
        secondLineText = fmSmall.elidedText(secondLineText, Qt::ElideRight, option.rect.width() - 2 * MARGIN);
        painter->drawText(option.rect.left() + MARGIN, option.rect.top() + MARGIN + fm.height() + 0.5 * MARGIN + fmSmall.ascent(), secondLineText);
    }
    else
    {
        // One line only, no counters
        if (d->getStatus() == DirectoryFileData::NOT_READABLE)
            fileName = "[" + tr("NOT READABLE") + "] " + fileName;
        fileName += suffix;
        fileName = fm.elidedText(fileName, Qt::ElideMiddle, option.rect.width() - 2 * MARGIN);
        painter->setFont(font);
        painter->drawText(option.rect.left() + MARGIN, option.rect.top() + MARGIN + fm.ascent(), fileName);
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