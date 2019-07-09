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

#include "spinboxcents.h"
#include <QPainter>
#include <QPaintEvent>
#include "contextmanager.h"

SpinBoxCents::SpinBoxCents(QWidget *parent) : QSpinBox(parent)
{
    this->setMaximum(100);
    this->setMinimum(-100);
    this->setPrefix("+");

    // Change the line edit
    this->setLineEdit(new LineEditCents(this));

    // Connection
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

void SpinBoxCents::onValueChanged(int value)
{
    this->setPrefix(value >= 0 ? "+" : "");
}

LineEditCents::LineEditCents(QWidget * parent) : QLineEdit(parent)
{
    // Style
    _textColor = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                                   ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.5);
    _textFont = QFont(this->font().family(), 3 * this->font().pointSize() / 4);
}

void LineEditCents::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);

    // Draw "/100" on the bottom right
    QPainter p(this);
    p.setPen(_textColor);
    p.setFont(_textFont);
    p.drawText(0, 0, this->width() - 2, this->height(), Qt::AlignBottom | Qt::AlignRight, "/ 100");
}
