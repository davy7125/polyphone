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
**             Date: 01.01.2021                                           **
***************************************************************************/

#include "nullablespinbox.h"
#include <QLineEdit>

NullableSpinBox::NullableSpinBox(QWidget *parent) : QSpinBox(parent)
{
    this->lineEdit()->setText("0");
    this->setValue(0);
}

QValidator::State NullableSpinBox::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    // An empty input is OK
    if (input.isEmpty())
        return QValidator::Acceptable;

    return QSpinBox::validate(input, pos);
}

int NullableSpinBox::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return 0;
    return QSpinBox::valueFromText(text);
}

QString NullableSpinBox::textFromValue(int val) const
{
    if (this->text().isEmpty())
        return "";
    return QSpinBox::textFromValue(val);
}

////////////////////////////////

NullableDoubleSpinBox::NullableDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    this->lineEdit()->setText("0.0");
    this->setValue(0.0);
}

QValidator::State NullableDoubleSpinBox::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    // An empty input is OK
    if (input.isEmpty())
        return QValidator::Acceptable;

    return QDoubleSpinBox::validate(input, pos);
}

double NullableDoubleSpinBox::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return 0.0;
    return QDoubleSpinBox::valueFromText(text);
}

QString NullableDoubleSpinBox::textFromValue(double val) const
{
    if (this->text().isEmpty())
        return "";
    return QDoubleSpinBox::textFromValue(val);
}
