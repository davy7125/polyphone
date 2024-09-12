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

#ifndef CONTROLLERAREA_H
#define CONTROLLERAREA_H

#include <QWidget>
#include <QMap>
#include "imidilistener.h"
class QComboBox;
class ComboCC;

namespace Ui {
class ControllerArea;
}

class ControllerArea : public QWidget, public IMidiListener
{
    Q_OBJECT

public:
    explicit ControllerArea(QWidget *parent = nullptr);
    ~ControllerArea();

    void setChannel(int channel);

    // Reaction to MIDI signals
    bool processKey(int channel, int key, int vel) override { Q_UNUSED(channel); Q_UNUSED(key); Q_UNUSED(vel); return false; }
    bool processPolyPressureChanged(int channel, int key, int pressure) override { Q_UNUSED(channel); Q_UNUSED(key); Q_UNUSED(pressure); return false; }
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processBendSensitivityChanged(int channel, float semitones) override;

    void updateBend(int channel, float value, bool stopTimer = true);

private slots:
    void on_sliderPitchWheel_valueChanged(int value);
    void on_sliderSensitivity_valueChanged(int value);
    void on_sliderPressure_valueChanged(int value);
    void on_comboControl1_currentIndexChanged(int index);
    void on_comboControl2_currentIndexChanged(int index);
    void on_comboControl3_currentIndexChanged(int index);
    void on_comboControl4_currentIndexChanged(int index);
    void on_knob1_valueChanged(int value);
    void on_knob2_valueChanged(int value);
    void on_knob3_valueChanged(int value);
    void on_push4_clicked();

private:
    void updateInput4Display();

    Ui::ControllerArea *ui;
    QPixmap _ledOn;
    QPixmap _ledOff;
    bool _ledState;
    int _channel;
    QMap<ComboCC *, int> _comboGroup;
};

#endif // CONTROLLERAREA_H
