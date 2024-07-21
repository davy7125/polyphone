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

#ifndef SPINBOXKEY_H
#define SPINBOXKEY_H

#include <QSpinBox>

class SpinBoxKey : public QSpinBox
{
    Q_OBJECT
public:
    explicit SpinBoxKey(QWidget *parent = nullptr, bool isNullable = false);
    void setAlwaysShowKeyName(bool isOn);
    bool isNull() { return this->text().isEmpty(); }

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    int valueFromText(const QString &text) const override;
    QString textFromValue(int val) const override;

private slots:
    void onKeyPlayed(int key, int vel);

private:
    bool _alwaysShowKeyName;
    bool _isNullable;
};

#endif // SPINBOXKEY_H
