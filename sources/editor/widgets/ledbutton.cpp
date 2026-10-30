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

    // Border and switched off led mixed with the text, so that they are contrasted with both light and dark backgrounds
    QColor windowColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND);
    QColor textColor = ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT);
    QColor borderColor = ThemeManager::mix(windowColor, textColor, 0.5);
    QColor offColor = ThemeManager::mix(windowColor, textColor, 0.3);
    QColor onColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    QMap<QString, QString> replacement;
    replacement["border1"] = borderColor.darker(130).name();
    replacement["border2"] = borderColor.name();
    replacement["color1"] = onColor.darker(130).name();
    replacement["color2"] = onColor.name();
    _ledOn = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(48, 48), replacement);
    replacement["color1"] = offColor.darker(130).name();
    replacement["color2"] = offColor.name();
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
