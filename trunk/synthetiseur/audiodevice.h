/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

#include <QAudioOutput>
#include <jack/jack.h>
#include <jack/session.h>
#include "synth.h"
#ifdef WIN32
#include "portaudio.h"
#endif

#define BUFFER_AUDIO          20000
#define SAMPLE_RATE           48000
#define FRAMES_PER_BUFFER     1024

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
    // Accès au format audio par Asio
    QAudioFormat m_format;
public slots:
    void initAudio(int numDevice);
signals:
    void start();
private:
    enum TypeConnection
    {
        CONNECTION_NONE,
        CONNECTION_JACK,
        CONNECTION_ASIO,
        CONNECTION_QAUDIO
    };
    void openJackConnection();
    void openQAudioConnection(int numDevice);
    void closeConnections();
    // Serveur son, sortie standard
    QAudioOutput * m_audioOutput;
#ifdef WIN32
    // Serveur son, asio
    int m_isAsioRunning;
    PaStream * m_asioStream;
    void openAsioConnection();
#endif
    // Serveur son, jack
    jack_client_t * m_jack_client;
    TypeConnection m_typeConnection;
};

#endif // AUDIODEVICE_H
