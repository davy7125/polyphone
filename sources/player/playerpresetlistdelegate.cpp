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

#include "playerpresetlistdelegate.h"
#include <QPainter>
#include "playerpresetlist.h"
#include "contextmanager.h"

PlayerPresetListDelegate::PlayerPresetListDelegate(PlayerPresetList * list) : QStyledItemDelegate(),
    _presetList(list)
{
    _borderColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND);
}

void PlayerPresetListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    // Draw a border
    painter->setPen(QPen(_borderColor, 4));
    painter->drawRect(option.rect);
}

QSize PlayerPresetListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    int columnNumber = _presetList->columnCount();
    double columnWidth = (double)_presetList->viewport()->width() / columnNumber;
    int rowHeight = _presetList->rowHeight();

    return QSize(columnWidth - 1, rowHeight);
}
