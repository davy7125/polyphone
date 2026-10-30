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

#include "ledbutton.h"
#include "contextmanager.h"
#include <QStylePainter>
#include <QStyleOptionButton>

LedButton::LedButton(QWidget * parent) : QPushButton(parent)
{
    this->setCheckable(true);
    this->setFlat(true);
    this->setCursor(Qt::PointingHandCursor);

    // Same colors as the led of the controller area
    QMap<QString, QString> replacement;
    replacement["border1"] = ContextManager::theme()->getColor(ThemeManager::BORDER).darker(130).name();
    replacement["border2"] = ContextManager::theme()->getColor(ThemeManager::BORDER).name();
    replacement["color1"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).darker(130).name();
    replacement["color2"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _ledOn = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(48, 48), replacement);
    replacement["color1"] = replacement["border1"];
    replacement["color2"] = replacement["border2"];
    _ledOff = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(48, 48), replacement);
    updateIcon();
}

void LedButton::checkStateSet()
{
    QPushButton::checkStateSet();
    updateIcon();
}

void LedButton::nextCheckState()
{
    QPushButton::nextCheckState();
    updateIcon();
}

void LedButton::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    // The state is displayed by the led only, a checked button is not drawn pressed
    QStylePainter painter(this);
    QStyleOptionButton option;
    this->initStyleOption(&option);
    option.state &= ~QStyle::State_On;
    painter.drawControl(QStyle::CE_PushButton, option);
}

void LedButton::updateIcon()
{
    this->setIcon(this->isChecked() ? _ledOn : _ledOff);
}
