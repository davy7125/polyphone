/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "backgroundwidget.h"
#include <QPaintEvent>
#include <QStyleOption>
#include <QPainter>
#include "contextmanager.h"

const int BackgroundWidget::SIZE = 600;
const int BackgroundWidget::OFFSET_X = 150;
const int BackgroundWidget::OFFSET_Y = 50;
const double BackgroundWidget::COLOR_RATIO = 0.1;

BackgroundWidget::BackgroundWidget(QWidget *parent) : QWidget(parent)
{
    // Color of the decoration
    _withDecoration = ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "decoration", true).toBool();
    if (_withDecoration)
    {
        QMap<QString, QString> replacements;
        replacements["currentColor"] = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                                         ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                                         COLOR_RATIO).name();
        _decoration = ContextManager::theme()->getColoredSvg(":/misc/decoration.svg", QSize(SIZE, SIZE), replacements);
    }
}

void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.fillRect(opt.rect, ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND));
    if (_withDecoration)
        p.drawPixmap(opt.rect.left() - OFFSET_X, opt.rect.bottom() - _decoration.height() + OFFSET_Y, _decoration);

    QWidget::paintEvent(event);
}
