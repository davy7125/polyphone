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

#ifndef SPINBOXRANGE_H
#define SPINBOXRANGE_H

#include <QAbstractSpinBox>
#include "imidilistener.h"

class SpinBoxRange : public QAbstractSpinBox, public IMidiListener
{
    Q_OBJECT

public:
    SpinBoxRange(QWidget * parent);
    ~SpinBoxRange();

    // QAbstractSpinBox virtual members
    void stepBy(int steps) override;
    QValidator::State validate(QString& input, int& pos) const override;

    bool isNull();
    int getValMin();
    int getValMax();
    void setText(QString text);

    // MIDI signals
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override { Q_UNUSED(channel); Q_UNUSED(key); Q_UNUSED(pressure); return false; }
    bool processMonoPressureChanged(int channel, int value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processControllerChanged(int channel, int num, int value) override { Q_UNUSED(channel); Q_UNUSED(num); Q_UNUSED(value); return false; }
    bool processBendChanged(int channel, float value) override { Q_UNUSED(channel); Q_UNUSED(value); return false; }
    bool processBendSensitivityChanged(int channel, float semitones) override { Q_UNUSED(channel); Q_UNUSED(semitones); return false; }
    bool processProgramChanged(int channel, quint16 bank, quint8 preset) override { Q_UNUSED(channel); Q_UNUSED(bank); Q_UNUSED(preset); return false; }

    static QString SEPARATOR;

public slots:
    void clear() override;

signals:
    void valueChanged();

protected:
    StepEnabled stepEnabled() const override;
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
    void stringToRange(QString input, bool &isNull, int &valMin, int &valMax, QValidator::State &state) const;
    static int MINI;
    static int MAXI;
    bool _isNull;
    int _valMin, _valMax;
    int _firstMidiKey;
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
