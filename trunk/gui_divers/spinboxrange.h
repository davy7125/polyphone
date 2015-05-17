/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SPINBOXRANGE_H
#define SPINBOXRANGE_H

#include <QAbstractSpinBox>

class SpinBoxRange : public QAbstractSpinBox
{
    Q_OBJECT

public:
    SpinBoxRange(QWidget * parent);
    ~SpinBoxRange() {}

    // QAbstractSpinBox virtual members
    virtual void stepBy(int steps);
    virtual QValidator::State validate(QString& input, int& pos) const;

    int getValMin();
    int getValMax();
    void setText(QString text);

    static QString SEPARATOR;

public slots:
    virtual void clear();

signals:
    void valueChanged();

protected:
    virtual StepEnabled stepEnabled() const;
    virtual QString getText(int value) const = 0;
    virtual int getValue(QString &text, bool &ok) const = 0;
    void formatText();

private slots:
    void updateValue();

private:
    enum SpinboxSection
    {
        SectionMin,
        SectionMax,
        SectionNone
    };
    SpinboxSection getCurrentSection() const;
    void stringToRange(QString input, int &valMin, int &valMax, QValidator::State &state) const;
    static int MINI;
    static int MAXI;
    int _valMin, _valMax;
};

class SpinBoxVelocityRange : public SpinBoxRange
{
    Q_OBJECT

public:
    SpinBoxVelocityRange(QWidget * parent) : SpinBoxRange(parent) {formatText();}
    ~SpinBoxVelocityRange() {}

protected:
    virtual QString getText(int value) const;
    virtual int getValue(QString &text, bool &ok) const;
};

class SpinBoxKeyRange : public SpinBoxRange
{
    Q_OBJECT

public:
    SpinBoxKeyRange(QWidget * parent) : SpinBoxRange(parent) {formatText();}
    ~SpinBoxKeyRange() {}

protected:
    virtual QString getText(int value) const;
    virtual int getValue(QString &text, bool &ok) const;
};

#endif // SPINBOXRANGE_H
