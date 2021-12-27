/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2021 Davy Triponney                                **
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
**             Date: 01.01.2021                                           **
***************************************************************************/

#ifndef NULLABLESPINBOX_H
#define NULLABLESPINBOX_H

#include <QSpinBox>

class NullableSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    NullableSpinBox(QWidget *parent = nullptr);
    bool isNull() { return this->text().isEmpty(); }

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    int valueFromText(const QString &text) const override;
    QString textFromValue(int val) const override;
};

class NullableDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    NullableDoubleSpinBox(QWidget *parent = nullptr);
    bool isNull() { return this->text().isEmpty(); }

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    double valueFromText(const QString &text) const override;
    QString textFromValue(double val) const override;
};

#endif // NULLABLESPINBOX_H
