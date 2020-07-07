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

#include "spinboxkey.h"
#include "contextmanager.h"

SpinBoxKey::SpinBoxKey(QWidget *parent) : QSpinBox(parent),
    _alwaysShowKeyName(false)
{
    this->setMinimum(0);
    this->setMaximum(127);
    this->setValue(60);

    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), this, SLOT(onKeyPlayed(int,int)));
}

QValidator::State SpinBoxKey::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    // Possibly remove extra information
    QString textToConvert = input;
    if (_alwaysShowKeyName && ContextManager::keyName()->getNameMiddleC() == KeyNameManager::MIDDLE_C_60 &&
            !textToConvert.isEmpty())
        textToConvert = textToConvert.split(' ').first();

    // Get the key number
    int numKey = ContextManager::keyName()->getKeyNum(input);
    if (numKey < 0 || numKey > 127)
        return QValidator::Invalid;
    return QValidator::Acceptable;
}

int SpinBoxKey::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return 0;
    return ContextManager::keyName()->getKeyNum(text.split(' ').first());
}

QString SpinBoxKey::textFromValue(int val) const
{
    QString keyName = ContextManager::keyName()->getKeyName(static_cast<unsigned int>(val));
    if (_alwaysShowKeyName && ContextManager::keyName()->getNameMiddleC() == KeyNameManager::MIDDLE_C_60)
        keyName += " (" + ContextManager::keyName()->getKeyName(static_cast<unsigned int>(val), true, false, false, true) + ")";
    return keyName;
}

void SpinBoxKey::onKeyPlayed(int key, int vel)
{
    if (vel > 0 && this->hasFocus())
        this->setValue(key);
}

void SpinBoxKey::setAlwaysShowKeyName(bool isOn)
{
    _alwaysShowKeyName = isOn;
}
