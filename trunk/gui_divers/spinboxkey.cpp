/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "spinboxkey.h"
#include "keynamemanager.h"

SpinBoxKey::SpinBoxKey(QWidget *parent) :
    QSpinBox(parent)
{
    this->setMinimum(0);
    this->setMaximum(127);
    this->setValue(60);
}

QValidator::State SpinBoxKey::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);
    int numKey = KeyNameManager::getInstance()->getKeyNum(input);
    if (numKey < 0 || numKey > 127)
        return QValidator::Invalid;
    return QValidator::Acceptable;
}

int SpinBoxKey::valueFromText(const QString &text) const
{
    return KeyNameManager::getInstance()->getKeyNum(text);
}

QString SpinBoxKey::textFromValue(int val) const
{
    return KeyNameManager::getInstance()->getKeyName(val);
}
