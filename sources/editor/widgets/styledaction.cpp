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

#include "styledaction.h"
#include "contextmanager.h"

const QSize StyledAction::ICON_SIZE = QSize(32, 32);

StyledAction::StyledAction(QString label, QString iconName, QWidget *parent) : QToolButton(parent),
    _isDisabled(false),
    _data(-1)
{
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));

    // Style
    this->setStyleSheet("StyledAction{border:0}");
    this->setIconSize(ICON_SIZE);
    this->setMinimumSize(39, 39);
    this->setCursor(Qt::PointingHandCursor);
    _checkedBackgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);

    // Text
    this->setToolTip(label);

    // Icon
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    _icon = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();
    _iconHover = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::DISABLED).name();
    _iconDisabled = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    this->setIcon(_icon);
}

void StyledAction::enterEvent(QEvent * event)
{
    Q_UNUSED(event)
    if (!_isDisabled)
        this->setIcon(_iconHover);
}

void StyledAction::leaveEvent(QEvent * event)
{
    Q_UNUSED(event)
    if (!_isDisabled)
        this->setIcon(_icon);
}

void StyledAction::disable(bool isDisabled)
{
    _isDisabled = isDisabled;
    if (_isDisabled)
    {
        this->setIcon(_iconDisabled);
        this->setCursor(Qt::ArrowCursor);
    }
    else
    {
        this->setIcon(_icon);
        this->setCursor(Qt::PointingHandCursor);
    }
}

void StyledAction::onToggled(bool isChecked)
{
    if (_isDisabled)
        return;

    if (isChecked)
        this->setStyleSheet("StyledAction{border:0;border-radius:2px;background-color:" + _checkedBackgroundColor.name() + "}");
    else
        this->setStyleSheet("StyledAction{border:0}");
    emit(clicked());
}

void StyledAction::onClicked(bool isClicked)
{
    Q_UNUSED(isClicked)

    if (_isDisabled || this->isCheckable())
        return;
    emit(clicked());
}

void StyledAction::setData(int data)
{
    _data = data;
}

int StyledAction::getData()
{
    return _data;
}
