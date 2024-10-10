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

    // An empty input is "Acceptable"
    if (input.isEmpty())
        return QValidator::Acceptable;

    // "Intermediate" if we start writing a negative number
    if (input == "-")
        return QValidator::Intermediate;

    // All values are "Acceptable", the other is invalid
    bool ok;
    input.toInt(&ok);
    return ok ? QValidator::Acceptable : QValidator::Invalid;
}

int NullableSpinBox::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return 0;

    bool ok;
    int value = text.toInt(&ok);
    if (!ok)
        return 0;

    if (value < this->minimum())
        return this->minimum();
    if (value > this->maximum())
        return this->maximum();

    return value;
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

    // An empty input is "Acceptable"
    if (input.isEmpty())
        return QValidator::Acceptable;

    // "Intermediate" if we start writing a negative number
    if (input == "-")
        return QValidator::Intermediate;

    // Coma accepted as a decimal separator
    QString tmp = input; // Work on a copy
    tmp = tmp.replace(",", ".");

    // All values are "Acceptable", the other is invalid
    bool ok;
    tmp.toDouble(&ok);
    return ok ? QValidator::Acceptable : QValidator::Invalid;
}

double NullableDoubleSpinBox::valueFromText(const QString &text) const
{
    if (text.isEmpty())
        return 0.0;

    // Coma accepted as a decimal separator
    QString tmp = text; // Work on a copy
    tmp = tmp.replace(",", ".");

    bool ok;
    double value = tmp.toDouble(&ok);
    if (!ok)
        return 0.0;

    if (value < this->minimum())
        return this->minimum();
    if (value > this->maximum())
        return this->maximum();

    return value;
}

QString NullableDoubleSpinBox::textFromValue(double val) const
{
    if (this->text().isEmpty())
        return "";
    return QDoubleSpinBox::textFromValue(val);
}
