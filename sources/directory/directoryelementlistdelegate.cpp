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

#include "directoryelementlistdelegate.h"
#include <QPainter>
#include <QApplication>
#include "contextmanager.h"

const int DirectoryElementListDelegate::MARGIN = 6;

DirectoryElementListDelegate::DirectoryElementListDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    _colorEnabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::NORMAL);
    _colorHighlighted = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::NORMAL);
    _colorAlternate = ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND, ThemeManager::NORMAL);
}

void DirectoryElementListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // Background
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

    if (index.row() > 0)
    {
        painter->setPen(QPen(_colorAlternate, 1));
        int y = option.rect.top();
        painter->drawLine(option.rect.left(), y, option.rect.right(), y);
    }

    // Text color
    bool selected = (option.state & QStyle::State_Selected);
    QColor textColor = selected ? _colorHighlighted : _colorEnabled;
    painter->setPen(textColor);

    QRect rect = option.rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);

    // First line
    QString name = index.data(Qt::DisplayRole).toString();
    QFont font = option.font;
    painter->setFont(font);
    const QFontMetrics fm(font);
    name = fm.elidedText(name, Qt::ElideMiddle, rect.width());
    painter->drawText(rect.left(), rect.top() + fm.ascent(), name);

    // Other lines
    QStringList details = index.data(Qt::UserRole + 1).toString().split("\n");
    QFont fontSmall = option.font;
    fontSmall.setPointSizeF(0.9 * fontSmall.pointSizeF());
    painter->setFont(fontSmall);
    const QFontMetrics fmSmall(fontSmall);
    for (int i = 0; i < details.count(); i++)
    {
        QString tmp = fmSmall.elidedText(details[i], Qt::ElideRight, rect.width());
        painter->drawText(rect.left(), rect.top() + fm.height() + (i + 1) * MARGIN / 2 + i * fmSmall.height() + fmSmall.ascent(), tmp);
    }

    painter->restore();
}

QSize DirectoryElementListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    QFontMetrics fm(option.font);
    int lineCount = index.data(Qt::UserRole + 2).toInt();
    return QSize(0, (0.9 * lineCount + 1.0) * fm.height() + (0.5 * lineCount + 2.0) * MARGIN);
}