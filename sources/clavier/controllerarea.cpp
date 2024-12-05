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

#include "controllerarea.h"
#include "ui_controllerarea.h"
#include "contextmanager.h"
#include <QApplication>

ControllerArea::ControllerArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllerArea),
    _ledState(false),
    _channel(-1)
{
    ui->setupUi(this);

    // Led on / off
    QMap<QString, QString> replacement;
    replacement["border1"] = ContextManager::theme()->getColor(ThemeManager::BORDER).darker(130).name();
    replacement["border2"] = ContextManager::theme()->getColor(ThemeManager::BORDER).name();
    replacement["color1"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).darker(130).name();
    replacement["color2"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    _ledOn = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(48, 48), replacement);
    replacement["color1"] = replacement["border1"];
    replacement["color2"] = replacement["border2"];
    _ledOff = ContextManager::theme()->getColoredSvg(":/icons/led.svg", QSize(48, 48), replacement);
    ui->push4->setIcon(_ledOff);

    // Initialize the channel
    setChannel(-1);
}

ControllerArea::~ControllerArea()
{
    ContextManager::midi()->removeListener(this);
    delete ui;
}

void ControllerArea::setChannel(int channel)
{
    ContextManager::midi()->removeListener(this);
    _channel = channel;

    // Initialization of the sensitivity slider
    processBendSensitivityChanged(_channel, ContextManager::midi()->getBendSensitivityValue(_channel));

    // Initialization of the pressure slider
    processMonoPressureChanged(_channel, ContextManager::midi()->getMonoPressure(_channel));

    // Initialization of the wheel
    ui->sliderPitchWheel->setColorFromMiddle(true);
    ui->sliderPitchWheel->setBackToValue(64);
    updateBend(_channel, 0); // Always in the middle

    // Initialization of the controllers
    ui->comboControl1->setGroup(&_comboGroup);
    ui->comboControl2->setGroup(&_comboGroup);
    ui->comboControl3->setGroup(&_comboGroup);
    ui->comboControl4->setGroup(&_comboGroup);

    ui->comboControl1->blockSignals(true);
    ui->comboControl1->selectCC(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "controller_1", 1).toInt());
    on_comboControl1_currentIndexChanged(-1);
    ui->comboControl1->blockSignals(false);
    ui->comboControl2->blockSignals(true);
    ui->comboControl2->selectCC(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "controller_2", 10).toInt());
    on_comboControl2_currentIndexChanged(-1);
    ui->comboControl2->blockSignals(false);
    ui->comboControl3->blockSignals(true);
    ui->comboControl3->selectCC(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "controller_3", 11).toInt());
    on_comboControl3_currentIndexChanged(-1);
    ui->comboControl3->blockSignals(false);
    ui->comboControl4->blockSignals(true);
    ui->comboControl4->selectCC(ContextManager::configuration()->getValue(ConfManager::SECTION_MIDI, "controller_4", 64).toInt());
    on_comboControl4_currentIndexChanged(-1);
    ui->comboControl4->blockSignals(false);

    ContextManager::midi()->addListener(this, 0);
}

void ControllerArea::on_sliderPitchWheel_valueChanged(int value)
{
    float fVal = static_cast<float>(value - 64) / 64.0f;
    updateBend(fVal, false);
    ContextManager::midi()->processBendChanged(_channel, fVal);
}

void ControllerArea::on_sliderSensitivity_valueChanged(int value)
{
    float semitones = 0.01f * value;
    processBendSensitivityChanged(_channel, semitones);

    // Combinaison of 4 controller events
    ContextManager::midi()->processControllerChanged(false, _channel, 101, 0);
    ContextManager::midi()->processControllerChanged(false, _channel, 100, 0);
    ContextManager::midi()->processControllerChanged(false, _channel, 6, value / 100);
    ContextManager::midi()->processControllerChanged(false, _channel, 38, value % 100);
}

void ControllerArea::on_sliderPressure_valueChanged(int value)
{
    processMonoPressureChanged(_channel, value);
    ContextManager::midi()->processMonoPressureChanged(_channel, value);
}

void ControllerArea::on_comboControl1_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "controller_1", ui->comboControl1->getCurrentCC());

    int previousValue = ContextManager::midi()->getControllerValue(_channel, ui->comboControl1->getCurrentCC());
    if (previousValue < 0) // Keep the current one
        on_knob1_valueChanged(ui->knob1->value());
    else
    {
        ui->knob1->blockSignals(true);
        ui->knob1->setValue(previousValue);
        ui->labelValue1->setText(QString::number(previousValue));
        ui->knob1->blockSignals(false);
    }
}

void ControllerArea::on_comboControl2_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "controller_2", ui->comboControl2->getCurrentCC());

    int previousValue = ContextManager::midi()->getControllerValue(_channel, ui->comboControl2->getCurrentCC());
    if (previousValue < 0) // Keep the current one
        on_knob2_valueChanged(ui->knob2->value());
    else
    {
        ui->knob2->blockSignals(true);
        ui->knob2->setValue(previousValue);
        ui->labelValue2->setText(QString::number(previousValue));
        ui->knob2->blockSignals(false);
    }
}

void ControllerArea::on_comboControl3_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "controller_3", ui->comboControl3->getCurrentCC());

    int previousValue = ContextManager::midi()->getControllerValue(_channel, ui->comboControl3->getCurrentCC());
    if (previousValue < 0) // Keep the current one
        on_knob3_valueChanged(ui->knob3->value());
    else
    {
        ui->knob3->blockSignals(true);
        ui->knob3->setValue(previousValue);
        ui->labelValue3->setText(QString::number(previousValue));
        ui->knob3->blockSignals(false);
    }
}

void ControllerArea::on_comboControl4_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ContextManager::configuration()->setValue(ConfManager::SECTION_MIDI, "controller_4", ui->comboControl4->getCurrentCC());

    int previousValue = ContextManager::midi()->getControllerValue(_channel, ui->comboControl4->getCurrentCC());
    if (previousValue < 0) // Keep the current one
    {
        _ledState = !_ledState; // Will be back to the same value next line
        on_push4_clicked();
    }
    else
    {
        _ledState = (previousValue >= 64);
        updateInput4Display();
    }
}

void ControllerArea::on_knob1_valueChanged(int value)
{
    ui->labelValue1->setText(QString::number(value));
    ContextManager::midi()->processControllerChanged(false, _channel, ui->comboControl1->getCurrentCC(), value);
}

void ControllerArea::on_knob2_valueChanged(int value)
{
    ui->labelValue2->setText(QString::number(value));
    ContextManager::midi()->processControllerChanged(false, _channel, ui->comboControl2->getCurrentCC(), value);
}

void ControllerArea::on_knob3_valueChanged(int value)
{
    ui->labelValue3->setText(QString::number(value));
    ContextManager::midi()->processControllerChanged(false, _channel, ui->comboControl3->getCurrentCC(), value);
}

void ControllerArea::on_push4_clicked()
{
    _ledState = !_ledState;
    updateInput4Display();
    ContextManager::midi()->processControllerChanged(false, _channel, ui->comboControl4->getCurrentCC(), _ledState ? 127 : 0);
}

void ControllerArea::updateInput4Display()
{
    if (_ledState)
    {
        ui->labelValue4->setText(tr("on"));
        ui->push4->setIcon(_ledOn);
    }
    else
    {
        ui->labelValue4->setText(tr("off"));
        ui->push4->setIcon(_ledOff);
    }
}

bool ControllerArea::processMonoPressureChanged(int channel, int value)
{
    if (channel != _channel)
        return false;

    if (value < 0)
        value = 0;
    else if (value > 127)
        value = 127;

    ui->sliderPressure->blockSignals(true);
    ui->sliderPressure->setValue(value);
    ui->sliderPressure->blockSignals(false);

    ui->labelPressureValue->setText(QString::number(value));
    return false;
}

bool ControllerArea::processControllerChanged(int channel, int num, int value)
{
    if (channel != _channel)
        return false;

    // Update first input?
    if (ui->comboControl1->getCurrentCC() == num)
    {
        ui->knob1->blockSignals(true);
        ui->knob1->setValue(value);
        ui->labelValue1->setText(QString::number(value));
        ui->knob1->blockSignals(false);
    }

    // Update second input?
    if (ui->comboControl2->getCurrentCC() == num)
    {
        ui->knob2->blockSignals(true);
        ui->knob2->setValue(value);
        ui->labelValue2->setText(QString::number(value));
        ui->knob2->blockSignals(false);
    }

    // Update third input?
    if (ui->comboControl3->getCurrentCC() == num)
    {
        ui->knob3->blockSignals(true);
        ui->knob3->setValue(value);
        ui->labelValue3->setText(QString::number(value));
        ui->knob3->blockSignals(false);
    }

    // Update fourth input?
    if (ui->comboControl4->getCurrentCC() == num)
    {
        _ledState = (value >= 64);
        updateInput4Display();
    }
    return false;
}

bool ControllerArea::processBendChanged(int channel, float value)
{
    updateBend(channel, value, channel != -1);
    return false;
}

void ControllerArea::updateBend(int channel, float value, bool stopTimer)
{
    if (channel != _channel)
        return;

    if (value < -1)
        value = -1;
    else if (value > 1)
        value = 1;

    ui->sliderPitchWheel->blockSignals(true);
    ui->sliderPitchWheel->setValue(static_cast<int>((value + 1) * 64));
    if (stopTimer)
        ui->sliderPitchWheel->stopTimer();
    ui->sliderPitchWheel->blockSignals(false);

    ui->labelWheelValue->setText(QLocale::system().toString(value, 'f', 2));
}

bool ControllerArea::processBendSensitivityChanged(int channel, float semitones)
{
    if (channel != _channel)
        return false;

    if (semitones < 0)
        semitones = 0;
    else if (semitones > 15.0)
        semitones = 15.0;

    ui->sliderSensitivity->blockSignals(true);
    ui->sliderSensitivity->setValue(static_cast<int>(semitones * 100));
    ui->sliderSensitivity->blockSignals(false);

    ui->labelSensitivityValue->setText(QLocale::system().toString(semitones, 'f', 2));
    return false;
}
