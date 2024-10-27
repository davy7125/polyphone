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

#include "spinboxrange.h"
#include <QLineEdit>
#include "contextmanager.h"

QString SpinBoxRange::SEPARATOR = QString::fromUtf8("-");
int SpinBoxRange::MINI = 0;
int SpinBoxRange::MAXI = 127;

SpinBoxRange::SpinBoxRange(QWidget *parent) : QAbstractSpinBox(parent),
    _isNull(false),
    _valMin(MINI),
    _valMax(MAXI),
    _firstMidiKey(-1)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(updateValue()));
    ContextManager::midi()->addListener(this, 1000);
}

SpinBoxRange::~SpinBoxRange()
{
    ContextManager::midi()->removeListener(this);
}

void SpinBoxRange::stepBy(int steps)
{
    int selection = 0;
    switch (getCurrentSection())
    {
    case SectionMin:
        _valMin += steps;
        if (_valMin < MINI)
            _valMin = MINI;
        else if (_valMin > MAXI)
            _valMin = MAXI;
        if (_valMin > _valMax)
            _valMax = _valMin;

        // Select to the left
        selection = -1;

        break;
    case SectionMax:
        _valMax += steps;
        if (_valMax < MINI)
            _valMax = MINI;
        else if (_valMax > MAXI)
            _valMax = MAXI;
        if (_valMax < _valMin)
            _valMin = _valMax;

        // Select to the right
        selection = 1;

        break;
    case SectionNone:
        break;
    }
    formatText();

    if (selection == -1)
    {
        QString txt = lineEdit()->text();
        int posSeparator = txt.indexOf(QRegularExpression("[0-9]" + SEPARATOR)) + 1;
        lineEdit()->setSelection(0, posSeparator);
    }
    else if (selection == 1)
    {
        QString txt = lineEdit()->text();
        int posSeparator = txt.indexOf(QRegularExpression("[0-9]" + SEPARATOR)) + 1;
        lineEdit()->setSelection(posSeparator + SEPARATOR.size(), txt.size() - posSeparator + SEPARATOR.size());
    }

    emit valueChanged();
}

QValidator::State SpinBoxRange::validate(QString& input, int& pos) const
{
    Q_UNUSED(pos);
    QValidator::State state = QValidator::Invalid;
    bool isNull;
    int valMin, valMax;
    stringToRange(input, isNull, valMin, valMax, state);
    return state;
}

bool SpinBoxRange::isNull()
{
    return _isNull;
}

int SpinBoxRange::getValMin()
{
    return _valMin;
}

int SpinBoxRange::getValMax()
{
    return _valMax;
}

void SpinBoxRange::setText(QString text)
{
    QValidator::State state;
    stringToRange(text, _isNull, _valMin, _valMax, state);
    formatText();

    // Select all
    this->lineEdit()->selectAll();
}

void SpinBoxRange::clear()
{
    _valMin = MINI;
    _valMax = MAXI;
    formatText();
    emit valueChanged();
}

bool SpinBoxRange::processKey(int channel, int key, int vel)
{
    Q_UNUSED(channel)

    if (vel > 0 && this->hasFocus())
    {
        if (_firstMidiKey == -1)
        {
            // Single note for now
            _firstMidiKey = key;
            _valMin = _valMax = key;
        }
        else
        {
            // The second creates a range
            _valMin = qMin(_firstMidiKey, key);
            _valMax = qMax(_firstMidiKey, key);
            _firstMidiKey = -1;
        }
        formatText();
    }
    else
        _firstMidiKey = -1;

    return false;
}

QAbstractSpinBox::StepEnabled SpinBoxRange::stepEnabled() const
{
    StepEnabled stepEnabled = QFlags<StepEnabledFlag>();
    switch (getCurrentSection())
    {
    case SectionMin:
        if (_valMin != MINI)
            stepEnabled |= StepDownEnabled;
        if (_valMin != MAXI)
            stepEnabled |= StepUpEnabled;
        break;
    case SectionMax:
        if (_valMax != MINI)
            stepEnabled |= StepDownEnabled;
        if (_valMax != MAXI)
            stepEnabled |= StepUpEnabled;
        break;
    case SectionNone:
        break;
    }

    return stepEnabled;
}

SpinBoxRange::SpinboxSection SpinBoxRange::getCurrentSection() const
{
    int cursorPos = this->lineEdit()->cursorPosition();
    const QString str = lineEdit()->text();

    int posSeparator = str.indexOf(QRegularExpression("[0-9]" + SEPARATOR)) + 1;
    if (posSeparator == 0)
        return SectionNone;

    if (cursorPos <= posSeparator)
        return SectionMin;
    else if (cursorPos >= posSeparator + SEPARATOR.size())
        return SectionMax;
    return SectionNone;
}

void SpinBoxRange::updateValue()
{
    QValidator::State state = QValidator::Invalid;
    bool isNull;
    int valMin, valMax;
    stringToRange(lineEdit()->text(), isNull, valMin, valMax, state);
    if (state != QValidator::Acceptable)
        return;

    if (valMin == _valMin && valMax == _valMax && isNull == _isNull)
        return;

    _isNull = isNull;
    _valMin = valMin;
    _valMax = valMax;

    formatText();
    emit valueChanged();
}

void SpinBoxRange::stringToRange(QString input, bool &isNull, int &valMin, int &valMax, QValidator::State &state) const
{
    input = input.replace('_', '-');
    int posSeparator = input.indexOf(QRegularExpression("[0-9]" + SEPARATOR)) + 1;

    bool ok = false;
    state = QValidator::Acceptable;
    isNull = false;
    if (posSeparator != 0)
    {
        QString txtMin = input.left(posSeparator);
        QString txtMax = input.right(input.size() - posSeparator - SEPARATOR.size());

        if (txtMin.isEmpty())
            valMin = MINI;
        else
        {
            valMin = getValue(txtMin, ok);
            if (!ok)
                state = QValidator::Intermediate;
        }

        if (txtMax.isEmpty())
            valMax = MAXI;
        else
        {
            valMax = getValue(txtMax, ok);
            if (!ok)
                state = QValidator::Intermediate;
        }

        if (valMin > valMax)
        {
            int iTmp = valMin;
            valMin = valMax;
            valMax = iTmp;
        }
    }
    else
    {
        if (input.isEmpty())
        {
            isNull = true;
            valMin = MINI;
            valMax = MAXI;
        }
        else
        {
            int valUnique = getValue(input, ok);
            valMin = valUnique;
            valMax = valUnique;
            if (!ok)
                state = QValidator::Intermediate;
        }
    }
}

void SpinBoxRange::formatText()
{
    lineEdit()->setText(_isNull ? "" : (getText(_valMin) + SEPARATOR + getText(_valMax)));
}


// PARTICULARISATION //

QString SpinBoxVelocityRange::getText(int value) const
{
    return QLocale::system().toString(value);
}

int SpinBoxVelocityRange::getValue(QString &text, bool &ok) const
{
    int vel = text.toInt(&ok);
    if (ok)
    {
        if (vel < 0)
            return 0;
        if (vel > 127)
            return 127;
        return vel;
    }

    return -1;
}

QString SpinBoxKeyRange::getText(int value) const
{
    return ContextManager::keyName()->getKeyName(value);
}

int SpinBoxKeyRange::getValue(QString &text, bool &ok) const
{
    // If the text is an int, possibly change the result
    int key = text.toInt(&ok);
    if (ok)
    {
        if (key < 0)
            return 0;
        if (key > 127)
            return 127;
        return key;
    }

    key = ContextManager::keyName()->getKeyNum(text);
    ok = (key != -1);
    return key;
}
