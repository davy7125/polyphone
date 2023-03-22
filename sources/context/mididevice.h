/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
#include <QObject>
#include "rtmidi/RtMidi.h"
#include "imidivalues.h"
class ConfManager;
class RtMidiIn;
class DialogKeyboard;
class PianoKeybdCustom;
class Synth;

class MidiDevice: public QObject, public IMidiValues
{
    Q_OBJECT

public:
    MidiDevice(ConfManager * configuration, Synth * synth);
    ~MidiDevice() override;

    // Initialize the midi device
    QMap<QString, QString> getMidiList();
    void openMidiPort(QString source); // Source in the form "{api type}#{port number}"

    // Connect the keyboard
    void setKeyboard(DialogKeyboard * dialogKeyboard) { _dialogKeyboard = dialogKeyboard; }
    PianoKeybdCustom * keyboard();

    // Stop all keys
    void stopAll();

    // Get last values (-1 if not received yet)
    int getControllerValue(int channel, int controllerNumber) override;
    float getBendValue(int channel) override;
    float getBendSensitivityValue(int channel) override;
    int getMonoPressure(int channel) override;
    int getPolyPressure(int channel, int key) override;

signals:
    // Emit key ON / OFF for the editor (channel -1)
    void keyPlayed(int key, int vel);

protected:
    void customEvent(QEvent * event) override;

private slots:
    // When using modulators, default values of controllers might change
    void onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending);

private:
    struct MIDI_State
    {
        // Configuration of a channel, the values being written and read by different threads
        volatile int _controllerValues[128];
        volatile bool _controllerValueSpecified[128];
        volatile float _bendValue;
        volatile float _bendSensitivityValue;
        volatile int _monoPressureValue;
        volatile int _polyPressureValues[128];

        // RPN history since we need 4 messages to know what command it is
        int _rpnHistoryControllers[4];
        int _rpnHistoryValues[4];
        quint8 _rpnHistoryPosition;
    };

    void getMidiList(RtMidi::Api api, QMap<QString, QString> *map);
    void processKeyOn(int channel, int key, int vel);
    void processKeyOff(int channel, int key);
    void processPolyPressureChanged(int channel, int key, int pressure);
    void processMonoPressureChanged(int channel, int value);
    void processControllerChanged(int channel, int num, int value);
    void processBendChanged(int channel, float value);
    void processBendSensitivityChanged(int channel, float semitones);

    DialogKeyboard * _dialogKeyboard;
    ConfManager * _configuration;
    RtMidiIn * _midiin;
    Synth * _synth;

    // Last values, first is channel -1 (for the editor) then channel 1 to 16
    MIDI_State _midiStates[17];

    // Sustain / Sostenuto pedals (channel -1 only)
    bool _sustainedKeys[128];
    bool _sostenutoMemoryKeys[128];
    bool _isSustainOn, _isSostenutoOn;
};

#endif // MIDIDEVICE_H
