/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <QObject>

class Synth;
typedef void PaStream;
typedef struct _jack_port jack_port_t;
typedef struct _jack_client jack_client_t;

#define SAMPLE_RATE           44100

class AudioFormat
{
public:
    void setChannelCount(qint32 value)  {_channelCount = value;}
    void setSampleRate(qint32 value)    {_sampleRate = value;}
    void setSampleSize(qint32 value)    {_sampleSize = value;}
    qint32 channelCount()               {return _channelCount;}
    qint32 sampleRate()                 {return _sampleRate;}
    qint32 sampleSize()                 {return _sampleSize;}

private:
    qint32 _channelCount;
    qint32 _sampleRate;
    qint32 _sampleSize;
};

class HostInfo
{
public:
    class DeviceInfo
    {
    public:
        DeviceInfo(QString name, int index, bool isDefault) :
            _name(name),
            _index(index),
            _isDefault(isDefault)
        {}

        QString _name;
        int _index;
        bool _isDefault;
    };

    HostInfo(QString name, int index) :
        _name(name),
        _index(index),
        _isDefault(false)
    {}

    QString _name;
    int _index;
    bool _isDefault;
    QList<DeviceInfo> _devices;
};

class AudioDevice : public QObject
{
    Q_OBJECT

public:
    AudioDevice(QObject *parent = NULL);
    void initSynth(Synth * synth) { _synth = synth; }
    ~AudioDevice();

    QList<HostInfo> getHostInfo();

    // Accès aux ports et synthé pour Jack
    jack_port_t * _output_port_R;
    jack_port_t * _output_port_L;
    Synth * _synth;

    // Accès au format audio
    AudioFormat _format;

public slots:
    void initAudio(int numDevice, int numIndex, int bufferSize);
    void closeConnections();

signals:
    void connectionDone();
    void connectionDefault();
    void connectionProblem();

private:
    void openDefaultConnection(int bufferSize);
    void openJackConnection(int bufferSize);
    void openStandardConnection(int numDevice, int numIndex, int bufferSize);

    // Serveur son, sortie standard ou asio
    bool _isStandardRunning;
    PaStream * _standardStream;

    // Serveur son, jack
    jack_client_t * _jack_client;

    bool _initialized;
};

#endif // AUDIODEVICE_H
