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

class Synth;
class ConfManager;
class RtAudio;

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

    QList<HostInfo> getAllHosts();
    static void getHostAndDeviceFromConfig(QString config, QString &hostIdentifier, QString &hostName, QString &deviceName);
    /////////////////////////////////////

    AudioDevice(ConfManager * configuration);
    ~AudioDevice() override;

    Synth * getSynth() { return _synth; }

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
    void openConnection(unsigned int deviceNumber, quint32 bufferSize, quint32 sampleRate);

    RtAudio * _rtAudio;
    ConfManager * _configuration;
    Synth * _synth;
    int _currentApi;
    QString _currentDeviceName;
    QString _openedDeviceName;
    QTimer _deviceCheckTimer;
};

#endif // AUDIODEVICE_H
