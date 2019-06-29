/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include <QString>
#include <QStringList>
#include <QObject>
class ConfManager;
class RtMidiIn;
class PianoKeybdCustom;
class Synth;
class ControllerArea;

class MidiDevice: public QObject
{
    Q_OBJECT

public:
    MidiDevice(ConfManager * configuration, Synth * synth);
    ~MidiDevice();

    // Initialize the midi device
    QStringList getMidiList();
    void openMidiPort(int index);

    // Connect the keyboard
    void setKeyboard(PianoKeybdCustom * keyboard);
    PianoKeybdCustom * keyboard() { return _keyboard; }

    // Connect the controller area
    void setControllerArea(ControllerArea * controllerArea);

    // Stop all keys
    void stopAll();

public slots:
    void processKeyOn(int key, int vel, bool syncKeyboard = false);
    void processKeyOff(int key, bool syncKeyboard = false);
    void processPolyPressureChanged(int key, int pressure, bool syncKeyboard = false);
    void processMonoPressureChanged(int value, bool syncControllerArea = false);
    void processControllerChanged(int num, int value, bool syncControllerArea = false);
    void processBendChanged(int value, bool syncControllerArea = false);
    void processBendSensitivityChanged(double semitones, bool syncControllerArea = false);

signals:
    void keyPlayed(int key, int vel);
    void polyPressureChanged(int key, int pressure);
    void monoPressureChanged(int value);
    void controllerChanged(int num, int value);
    void bendChanged(int value);
    void bendSensitivityChanged(double semitones);

protected:
    void customEvent(QEvent * event);

private:
    PianoKeybdCustom * _keyboard;
    ControllerArea * _controllerArea;
    ConfManager * _configuration;
    RtMidiIn * _midiin;
    Synth * _synth;
    QList<QPair<int, int> > _rpnHistory;

    // Sustain pedal
    QList<int> _listKeysToRelease;
    bool _isSustainOn;
};

#endif // MIDIDEVICE_H
