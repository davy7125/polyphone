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

#include "menulabel.h"
#include <QMouseEvent>
#include "contextmanager.h"
#include <QString>

MenuLabel::MenuLabel(QString text) : QLabel(text)
{
    QString textColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name();
    QString backgroundColor = ThemeManager::mix(
                                  ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                  ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.5).name();

    this->setStyleSheet(QString("background: %1; color: %2; padding: 5px").arg(backgroundColor, textColor));
}

void MenuLabel::mousePressEvent(QMouseEvent *event)
{
    // Stop the propagation for not stopping the menu
    event->accept();
}

void MenuLabel::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}