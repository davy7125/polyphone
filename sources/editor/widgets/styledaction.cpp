/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

StyledAction::StyledAction(QWidget *parent) : QPushButton(parent),
    _isDisabled(false),
    _data(-1)
{
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));

    // Style
    this->setStyleSheet("StyledAction{border:0}");
    this->setIconSize(ICON_SIZE);
    this->setCursor(Qt::PointingHandCursor);
    this->setMinimumWidth(40);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    _checkedBackgroundColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
}

void StyledAction::initialize(QString label, QString iconName)
{
    // Text
    this->setToolTip(label);

    // Icon
    QColor color = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor hoveredColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED);
    QColor disabledColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::DISABLED);

    QMap<QString, QString> replacement;
    replacement["currentColor"] = color.name();
    _icon = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    replacement["currentColor"] = hoveredColor.name();
    _iconHover = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    replacement["currentColor"] = disabledColor.name();
    _iconDisabled = ContextManager::theme()->getColoredSvg(iconName, ICON_SIZE, replacement);
    this->setIcon(_icon);
}

#if QT_VERSION >= 0x060000
void StyledAction::enterEvent(QEnterEvent *event)
#else
void StyledAction::enterEvent(QEvent *event)
#endif
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

    emit(actionClicked());
}

void StyledAction::onClicked(bool unused)
{
    Q_UNUSED(unused)

    if (_isDisabled || this->isCheckable())
        return;

    emit(actionClicked());
}

void StyledAction::setData(int data)
{
    _data = data;
}

int StyledAction::getData()
{
    return _data;
}
