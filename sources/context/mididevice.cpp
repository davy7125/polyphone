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

#include "mididevice.h"
#include <QApplication>
#include "rtmidi/RtMidi.h"
#include "confmanager.h"
#include "soundfontmanager.h"
#include "imidilistener.h"

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    Q_UNUSED(deltatime);
    static_cast<MidiDevice*>(userData)->processMidiMessage(message->data(), message->size());
}

MidiDevice::MidiDevice(ConfManager * configuration) :
    _configuration(configuration),
    _midiIn(nullptr),
    _dispatcher(_listeners)
{
    // The events are processed before reaching the listeners, the bend sensitivity of the channel -1 is restored
    _processor.setListener(&_dispatcher);
    _processor.setBendSensitivityValue(-1, _configuration->getValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", 2.0f).toFloat());

    // Initialize the connection
    this->openMidiPort(_configuration->getValue(ConfManager::SECTION_MIDI, "index_port", "-1#-1").toString());

    // Link to the soundfont manager to initialize the CC values
    connect(SoundfontManager::getInstance(), SIGNAL(inputModulatorChanged(int,bool,bool)), this, SLOT(onInputModulatorChanged(int,bool,bool)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(check()));
    timer->start(1000);
}

MidiDevice::~MidiDevice()
{
    // Store the wheel sensitivity from channel -1
    _configuration->setValue(ConfManager::SECTION_MIDI, "wheel_sensitivity", _processor.getBendSensitivityValue(-1));

    if (_midiIn != nullptr)
    {
        _midiIn->closePort();
        delete _midiIn;
    }
}

bool MidiDevice::readConfiguration(QString config)
{
    _api = -1;
    _portNumber = -1;
    QStringList split = config.split('#');
    if (split.count() != 2)
        return false;

    // Api number
    bool ok;
    _api = split[0].toInt(&ok);
    if (!ok)
    {
        _api = -1;
        return false;
    }

    // Port number
    _portNumber = split[1].toInt(&ok);
    if (!ok)
    {
        _api = -1;
        _portNumber = -1;
        return false;
    }

    if (_portNumber == -1 || _api == -1)
    {
        _api = -1;
        _portNumber = -1;
        return false;
    }

    return true;
}

void MidiDevice::onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending)
{
    _processor.setDefaultControllerValue(controllerNumber, isBipolar, isDescending);
}

QMap<QString, QString> MidiDevice::getMidiList()
{
    QMap<QString, QString> mapRet;

#if defined(__LINUX_ALSASEQ__)
    getMidiList(RtMidi::LINUX_ALSA, &mapRet);
#endif
#if defined(__UNIX_JACK__)
    getMidiList(RtMidi::UNIX_JACK, &mapRet);
#endif
#if defined(__WINDOWS_MM__)
    getMidiList(RtMidi::WINDOWS_MM, &mapRet);
#endif
#if defined(__MACOSX_CORE__)
    getMidiList(RtMidi::MACOSX_CORE, &mapRet);
#endif

    return mapRet;
}

void MidiDevice::getMidiList(int api, QMap<QString, QString> * map)
{
    try
    {
        RtMidiIn * midiInTmp = new RtMidiIn((RtMidi::Api)api, "Polyphone");
        for (unsigned int i = 0; i < midiInTmp->getPortCount(); i++)
        {
            map->insert(QString::number(api) + "#" + QString::number(i),
                        QString(midiInTmp->getPortName(i).c_str()));
        }
        delete midiInTmp;
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
    }
}

void MidiDevice::openMidiPort(QString source)
{
    // Possibly close an existing midi input
    if (_midiIn)
    {
        _midiIn->closePort();
        delete _midiIn;
        _midiIn = nullptr;
    }

    if (readConfiguration(source))
        openMidiPort();
}

void MidiDevice::openMidiPort()
{
    // Create a MIDI input based on the selected API
    try
    {
        _midiIn = new RtMidiIn(static_cast<RtMidi::Api>(_api), "Polyphone");
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        delete _midiIn;
        _midiIn = nullptr;
        return;
    }

    // Associate a callback
    _midiIn->setCallback(&midiCallback, this);

    // Initialize the midi connection
    if (_portNumber < static_cast<int>(_midiIn->getPortCount()))
    {
        try
        {
            _midiIn->openPort(static_cast<unsigned int>(_portNumber));
        }
        catch (std::exception &error)
        {
            Q_UNUSED(error)
            delete _midiIn;
            _midiIn = nullptr;
        }
    }
}

void MidiDevice::check()
{
    // Nothing to do if there is no configured connection
    if (_api == -1 || _portNumber == -1)
        return;

    // Possibly create a MIDI input
    if (_midiIn == nullptr)
    {
        openMidiPort();
        return;
    }

    if (_midiIn->isPortOpen())
    {
        // Check that the port number is still valid
        if ((unsigned int)_portNumber >= _midiIn->getPortCount())
            _midiIn->closePort();
    }
    else
    {
        // Maybe we can open the port
        if ((unsigned int)_portNumber < _midiIn->getPortCount())
            openMidiPort();
    }
}

void MidiDevice::processMidiMessage(const unsigned char * message, size_t length)
{
    _processor.processMidiMessage(message, length);
}

void MidiDevice::processKeyOn(int channel, int key, int vel)
{
    _processor.processKeyOn(channel, key, vel);
}

void MidiDevice::processKeyOff(int channel, int key)
{
    _processor.processKeyOff(channel, key);
}

void MidiDevice::processPolyPressureChanged(int channel, int key, int pressure)
{
    _processor.processPolyPressureChanged(channel, key, pressure);
}

void MidiDevice::processMonoPressureChanged(int channel, int value)
{
    _processor.processMonoPressureChanged(channel, value);
}

void MidiDevice::processControllerChanged(bool external, int channel, int num, int value)
{
    _processor.processControllerChanged(external, channel, num, value);
}

void MidiDevice::processBendChanged(int channel, float value)
{
    _processor.processBendChanged(channel, value);
}

void MidiDevice::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    _processor.processRPNChanged(channel, parameter, value, isRegistered, trigger);
}

void MidiDevice::processProgramChanged(int channel, quint8 preset)
{
    _processor.processProgramChanged(channel, preset);
}

void MidiDevice::stopAll()
{
    // Release all keys sustained or held by the sostenuto
    _processor.stopAll();
}

int MidiDevice::getControllerValue(int channel, int controllerNumber)
{
    return _processor.getControllerValue(channel, controllerNumber);
}

float MidiDevice::getBendValue(int channel)
{
    return _processor.getBendValue(channel);
}

float MidiDevice::getBendSensitivityValue(int channel)
{
    return _processor.getBendSensitivityValue(channel);
}

int MidiDevice::getMonoPressure(int channel)
{
    return _processor.getMonoPressure(channel);
}

int MidiDevice::getPolyPressure(int channel, int key)
{
    return _processor.getPolyPressure(channel, key);
}

void MidiDevice::addListener(IMidiListener * listener, int priority)
{
    // Possibly insert a listener before another one
    for (int i = 0; i < _listenerPriorities.size(); i++)
    {
        if (_listenerPriorities[i] < priority)
        {
            _listenerPriorities.insert(i, priority);
            _listeners.insert(i, listener);
            return;
        }
    }

    // Or add it at the end of the list
    _listenerPriorities.append(priority);
    _listeners.append(listener);
}

void MidiDevice::removeListener(IMidiListener * listener)
{
    int pos = _listeners.indexOf(listener);
    if (pos != -1)
    {
        _listenerPriorities.removeAt(pos);
        _listeners.removeAt(pos);
    }
}

bool MidiDevice::ListenerDispatcher::processKey(int channel, int key, int vel)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processKey(channel, key, vel);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processKeyRestarted(int channel, int key, int vel)
{
    // Each listener stops the key, then plays it again
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
    {
        _listeners[i]->processKey(channel, key, 0);
        consumed |= _listeners[i]->processKey(channel, key, vel);
    }
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processPolyPressureChanged(int channel, int key, int pressure)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processPolyPressureChanged(channel, key, pressure);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processMonoPressureChanged(int channel, int value)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processMonoPressureChanged(channel, value);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processControllerChanged(int channel, int num, int value)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processControllerChanged(channel, num, value);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processBendChanged(int channel, float value)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processBendChanged(channel, value);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processProgramChanged(int channel, uint16_t bank, uint8_t preset)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processProgramChanged(channel, bank, preset);
    return consumed;
}

bool MidiDevice::ListenerDispatcher::processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger)
{
    bool consumed = false;
    for (int i = 0; i < _listeners.size(); ++i)
        consumed |= _listeners[i]->processRPNChanged(channel, parameter, value, isRegistered, trigger);
    return consumed;
}
