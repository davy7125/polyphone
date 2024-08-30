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

#include "pageoverviewsampledelegate.h"
#include <QPainter>
#include "contextmanager.h"

const QSize PageOverviewSampleDelegate::IMAGE_SIZE = QSize(35, 18);

PageOverviewSampleDelegate::PageOverviewSampleDelegate() : QStyledItemDelegate()
{
    // Sample mode icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::ColorContext::DISABLED).name();
    _sampleModeIcons[0] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_off.svg", IMAGE_SIZE, replacement);
    _sampleModeIcons[1] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on.svg", IMAGE_SIZE, replacement);
    _sampleModeIcons[2] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_release.svg", IMAGE_SIZE, replacement);
    _sampleModeIcons[3] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on_end.svg", IMAGE_SIZE, replacement);
}

void PageOverviewSampleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStringList textModes = index.data(Qt::DisplayRole).toString().split(",", Qt::SkipEmptyParts);
    QList<int> modes;
    for (int i = 0; i < textModes.count(); i++)
    {
        bool ok;
        int mode = textModes[i].toInt(&ok);
        if (ok && mode >= 0 && mode <= 3)
            modes << mode;
    }

    int imageWidth = modes.count() > 3 ? IMAGE_SIZE.width() - 5 : IMAGE_SIZE.width();
    for (int i = 0; i < modes.count(); i++)
    {
        // Center of the image
        int x = option.rect.left() + option.rect.width() / 2;
        x += imageWidth * (i - 0.5 * (modes.count() - 1));
        int y = option.rect.top() + option.rect.height() / 2;

        QPixmap pix = _sampleModeIcons[modes[i]];
        painter->drawPixmap(x - pix.width() / 2,  y - pix.height() / 2, pix.width(), pix.height(), pix);
    }
}
