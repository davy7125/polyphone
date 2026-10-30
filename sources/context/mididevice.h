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
#include <QVector>
#include "midiprocessor.h"
class ConfManager;
class RtMidiIn;

// MIDI input of the editor: the events are processed by MidiProcessor (synth core) and given to the listeners
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

    // Value of a controller as long as it has not been received
    int getDefaultControllerValue(int channel, int controllerNumber);

    // Process MIDI signals
    void processMidiMessage(const unsigned char * message, size_t length);
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
    // Gives the processed events to all listeners, by priority (an event is consumed if a listener consumes it)
    class ListenerDispatcher: public IMidiListener
    {
    public:
        ListenerDispatcher(QVector<IMidiListener *> &listeners) : _listeners(listeners) {}
        bool processKey(int channel, int key, int vel) override;
        bool processKeyRestarted(int channel, int key, int vel) override;
        bool processPolyPressureChanged(int channel, int key, int pressure) override;
        bool processMonoPressureChanged(int channel, int value) override;
        bool processControllerChanged(int channel, int num, int value) override;
        bool processBendChanged(int channel, float value) override;
        bool processProgramChanged(int channel, uint16_t bank, uint8_t preset) override;
        bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

    private:
        QVector<IMidiListener *> &_listeners;
    };

    void openMidiPort();
    bool readConfiguration(QString config);
    void getMidiList(int api, QMap<QString, QString> *map);

    ConfManager * _configuration;
    RtMidiIn * _midiIn;

    // MIDI listeners
    QVector<IMidiListener *> _listeners;
    QVector<int> _listenerPriorities;

    // Channel states, pedals and (N)RPN
    MidiProcessor _processor;
    ListenerDispatcher _dispatcher;

    // Current api and port number
    int _api;
    int _portNumber;
};

#endif // MIDIDEVICE_H
