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

#include "spinboxkey.h"
#include "contextmanager.h"
#include <QLineEdit>

SpinBoxKey::SpinBoxKey(QWidget *parent, bool isNullable) : QSpinBox(parent),
    _alwaysShowKeyName(false),
    _isNullable(isNullable)
{
    this->setMinimum(0);
    this->setMaximum(127);
    this->lineEdit()->setText("60");
    this->setValue(60);

    connect(ContextManager::midi(), SIGNAL(keyPlayed(int,int)), this, SLOT(onKeyPlayed(int,int)));
}

QValidator::State SpinBoxKey::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    if (input.isEmpty() && _isNullable)
        return QValidator::Acceptable;

    // Possibly remove extra information
    QString textToConvert = input;
    if (_alwaysShowKeyName && ContextManager::keyName()->getNameMiddleC() == KeyNameManager::MIDDLE_C_60 &&
            !textToConvert.isEmpty())
        textToConvert = textToConvert.split(' ').first();

    // Get the key number
    int numKey = ContextManager::keyName()->getKeyNum(input);
    if (numKey < 0 || numKey > 127)
        return QValidator::Intermediate;
    return QValidator::Acceptable;
}

int SpinBoxKey::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return _isNullable ? -1 : 60;
    int result = ContextManager::keyName()->getKeyNum(text.split(' ').first());
    return result;
}

QString SpinBoxKey::textFromValue(int val) const
{
    if (this->text().isEmpty() && _isNullable)
        return "";

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
