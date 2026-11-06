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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QObject>
#include <QTimer>
#include <vector>
#include "rtaudio/RtAudio.h" // No forward declaration: the classes are in a namespace in RtAudio after 6.0

class Synth;
class ConfManager;
class Recorder;

class AudioDevice : public QObject
{
    Q_OBJECT

public:
    //////////////// INFO ////////////////
    class DeviceInfo
    {
    public:
        QString name;
        int channelCount;
        bool isDefault;
    };

    class HostInfo
    {
    public:
        QString identifier;
        QString name;
        bool canFollowDefault; // The default device of the system is followed when it changes
        QList<DeviceInfo> devices;
    };

    QList<HostInfo> getAllHosts(bool isInput = false);
    static void getHostAndDeviceFromConfig(QString config, QString &hostIdentifier, QString &hostName, QString &deviceName);
    /////////////////////////////////////

    AudioDevice(ConfManager * configuration);
    ~AudioDevice() override;

    Synth * getSynth() { return _synth; }

    // True if the synth is currently heard (no output if none is configured or available)
    bool isRunning() { return _rtAudio != nullptr && _rtAudio->isStreamRunning(); }

    // Audio input of the configuration, open only while it is used, with its own recorder. False if there is none.
    bool openInput();
    void closeInput();
    bool isInputRunning() { return _rtAudioInput != nullptr && _rtAudioInput->isStreamRunning(); }
    quint32 getInputSampleRate() { return _inputSampleRate; }
    int getInputChannelCount() { return _inputChannelCount; } // 1 or 2
    Recorder * getInputRecorder() { return _inputRecorder; }

signals:
    // Audio outputs have been plugged, unplugged or the default output changed
    void devicesChanged();

public slots:
    void initAudio();
    void closeConnections();

private slots:
    void checkDevices();

private:
    static void getApiAndDeviceNameFromConfig(QString config, int &api, QString &deviceName);
    static int getDefaultApi();
    int findDevice(QString &deviceName, std::vector<unsigned int> &sampleRates);
    int findInputDevice(QString deviceName, RtAudio::DeviceInfo &deviceInfo);
    void openConnection(unsigned int deviceNumber, quint32 bufferSize, quint32 sampleRate);
    static quint32 getClosestSampleRate(const std::vector<unsigned int> &sampleRates);

    RtAudio * _rtAudio;
    RtAudio * _rtAudioInput;
    Recorder * _inputRecorder;
    quint32 _inputSampleRate;
    int _inputChannelCount;
    ConfManager * _configuration;
    Synth * _synth;
    int _currentApi;
    QString _currentDeviceName;
    QString _openedDeviceName;
    QTimer _deviceCheckTimer;
};

#endif // AUDIODEVICE_H
