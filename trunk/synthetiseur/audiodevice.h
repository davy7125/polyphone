/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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
#include "jack.h"
#include <session.h>
#include "portaudio.h"

class Synth;

#define SAMPLE_RATE           44100

class AudioFormat
{
public:
    void setChannelCount(qint32 value)  {m_channelCount = value;}
    void setSampleRate(qint32 value)    {m_sampleRate = value;}
    void setSampleSize(qint32 value)    {m_sampleSize = value;}
    qint32 channelCount()               {return m_channelCount;}
    qint32 sampleRate()                 {return m_sampleRate;}
    qint32 sampleSize()                 {return m_sampleSize;}
private:
    qint32 m_channelCount;
    qint32 m_sampleRate;
    qint32 m_sampleSize;
};

class AudioDevice : public QObject
{
    Q_OBJECT
public:
    AudioDevice(Synth *synth, QObject *parent = NULL);
    ~AudioDevice();

    // Accès aux ports et synthé pour Jack
    jack_port_t * m_output_port_R;
    jack_port_t * m_output_port_L;
    Synth * m_synth;

    // Accès au format audio
    AudioFormat m_format;

public slots:
    void initAudio(int numDevice, int bufferSize);
    void closeConnections();
signals:
    void start();
private:
    enum TypeConnection
    {
        CONNECTION_NONE,
        CONNECTION_JACK,
        CONNECTION_ASIO,
        CONNECTION_STANDARD
    };
    void openJackConnection(int bufferSize);
    void openStandardConnection(int bufferSize, bool isAsio);
    // Serveur son, sortie standard ou asio
    bool m_isStandardRunning;
    PaStream * m_standardStream;
    // Serveur son, jack
    jack_client_t * m_jack_client;
    TypeConnection m_typeConnection;
};

#endif // AUDIODEVICE_H
