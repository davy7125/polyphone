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

#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include <QString>
#include <QObject>
#include "imidivalues.h"
class ConfManager;
class RtMidiIn;

class MidiDevice: public QObject, public IMidiValues
{
    Q_OBJECT

public:
    MidiDevice(ConfManager * configuration);
    ~MidiDevice() override;

    // Initialize the midi device
    QMap<QString, QString> getMidiList();
    void openMidiPort(QString source); // Source in the form "{api type}#{port number}"

    // Stop all keys
    void stopAll();

    // Add / remove MIDI listener
    void addListener(IMidiListener * listener, int priority);
    void removeListener(IMidiListener * listener);

    // Get last values (-1 if not received yet)
    int getControllerValue(int channel, int controllerNumber) override;
    float getBendValue(int channel) override;
    float getBendSensitivityValue(int channel) override;
    int getMonoPressure(int channel) override;
    int getPolyPressure(int channel, int key) override;

    // Process MIDI signals
    void processKeyOn(int channel, int key, int vel);
    void processKeyOff(int channel, int key);
    void processPolyPressureChanged(int channel, int key, int pressure);
    void processMonoPressureChanged(int channel, int value);
    void processControllerChanged(bool external, int channel, int num, int value);
    void processBendChanged(int channel, float value);
    void processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger);
    void processProgramChanged(int channel, quint8 preset);

private slots:
    // When using modulators, default values of controllers might change
    void onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending);
    void check();

private:
    struct MIDI_State
    {
        // Configuration of a channel, the values being written and read by different threads
        volatile int _controllerValues[128];
        volatile bool _controllerValueSpecified[128];
        volatile bool _controllerValueIsRelative[128];
        volatile quint8 _controllerValueIsRelativeCounter[128];
        volatile float _bendValue;
        volatile float _bendSensitivityValue;
        volatile int _monoPressureValue;
        volatile int _polyPressureValues[128];

        // RPN memory
        unsigned char _rpnMSBparameter;
        unsigned char _rpnLSBparameter;
        unsigned char _rpnMSBvalue;
        unsigned char _rpnLSBvalue;
        bool _rpnIsRegistered;
    };

    struct Sustain_State
    {
        bool _currentKeys[128];
        bool _sustainedKeys[128];
        bool _sostenutoMemoryKeys[128];
        bool _isSustainOn;
        bool _isSostenutoOn;
    };

    void openMidiPort();
    bool readConfiguration(QString config);
    void getMidiList(int api, QMap<QString, QString> *map);

    ConfManager * _configuration;
    RtMidiIn * _midiIn;

    // Last values, first is channel -1 (for the editor) then channel 1 to 16
    MIDI_State _midiStates[17];

    // Sustain / Sostenuto pedals
    Sustain_State _sustainStates[17];

    // MIDI listeners
    QVector<IMidiListener *> _listeners;
    QVector<int> _listenerPriorities;

    // Current api and port number
    int _api;
    int _portNumber;
};

#endif // MIDIDEVICE_H
