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

#include "configtocbutton.h"
#include "contextmanager.h"

QSize ConfigTocButton::ICON_SIZE(42, 42);

ConfigTocButton::ConfigTocButton(QWidget *parent) : QToolButton(parent)
{
    this->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->setCursor(Qt::PointingHandCursor);
    QString styleSheet = "QToolButton:hover{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                ";} QToolButton{padding:12px 10px;border: none;}";
    this->setStyleSheet(styleSheet);
    this->setIconSize(ICON_SIZE);
}

void ConfigTocButton::setIconName(QString iconName)
{
    _icon = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, ThemeManager::LIST_TEXT);
    _iconHovered = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, ThemeManager::HIGHLIGHTED_TEXT);
    this->setIcon(_icon);
}

void ConfigTocButton::enterEvent(QEvent * event)
{
    this->setIcon(_iconHovered);
    QToolButton::enterEvent(event);
}

void ConfigTocButton::leaveEvent(QEvent * event)
{
    this->setIcon(_icon);
    QToolButton::leaveEvent(event);
}
