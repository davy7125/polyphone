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

#include "audiodevice.h"

// Callbacks de jack
int jackProcess(jack_nframes_t nframes, void * arg)
{
    // Récupération de l'instance de AudioDevice
    AudioDevice * instance = static_cast<AudioDevice*>(arg);
    // Envoi de données
    if (instance->m_output_port_R)
    {
        if (instance->m_output_port_L)
        {
            // Stéréo
            char * out1 = (char *)jack_port_get_buffer(instance->m_output_port_L, nframes);
            char * out2 = (char *)jack_port_get_buffer(instance->m_output_port_R, nframes);
            instance->m_synth->readData(out1, out2, 4 * nframes);
        }
        else
        {
            // Mono
            char * out = (char *)jack_port_get_buffer(instance->m_output_port_L, nframes);
            instance->m_synth->readData(out, 4 * nframes);
        }
    }
    return 0;
}
void jack_shutdown(void *arg) {Q_UNUSED(arg); exit(1);}

// Callback d'asio
#ifdef PA_USE_ASIO
int asioProcess(const void* inputBuffer, void* outputBuffer,
                unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags, void* userData)
{
    Q_UNUSED(inputBuffer);
    Q_UNUSED(timeInfo);
    Q_UNUSED(statusFlags);
    // Récupération de l'instance de AudioDevice
    AudioDevice * instance = static_cast<AudioDevice*>(userData);
    char** outputs = (char**)outputBuffer;
    // Envoi de données
    if (instance->m_format.channelCount() == 1)
        instance->m_synth->readData(outputs[0], 4 * framesPerBuffer);
    else if (instance->m_format.channelCount() == 2)
        instance->m_synth->readData(outputs[0], outputs[1], 4 * framesPerBuffer);
    return 0;
}
#endif


AudioDevice::AudioDevice(Synth *synth, QObject *parent) : QObject(parent),
    m_synth(synth),
    m_audioOutput(NULL),
#ifdef PA_USE_ASIO
    m_isAsioRunning(false),
    m_asioStream(NULL),
#endif
    m_jack_client(NULL),
    m_typeConnection(CONNECTION_NONE)
{
    connect(this, SIGNAL(start()), m_synth, SLOT(start()));
}
AudioDevice::~AudioDevice()
{
    this->closeConnections();
#ifdef PA_USE_ASIO
    Pa_Terminate();
#endif
}

void AudioDevice::initAudio(int numDevice)
{
    // -1 : arrêt
    // -2 : jack
    // -3 : asio
    // autre : via QAudio
    if (numDevice == -2 && m_jack_client)
        return;
    // Arrêt des serveurs son si besoin
    this->closeConnections();
    if (numDevice == -1) return;
    // Ouverture serveur son (doit être fait après la création, une fois dans le bon thread)
    if (numDevice == -2)
    {
        // Lancement Jack
        this->openJackConnection();
        if (m_jack_client)
            m_typeConnection = CONNECTION_JACK;
        else
        {
            this->openQAudioConnection(0);
            if (m_audioOutput)
                m_typeConnection = CONNECTION_QAUDIO;
            else
                m_typeConnection = CONNECTION_NONE;
        }
    }
#ifdef PA_USE_ASIO
    else if (numDevice == -3)
    {
        // Lancement Asio
        this->openAsioConnection();
        if (m_isAsioRunning)
            m_typeConnection = CONNECTION_ASIO;
        else
        {
            this->openQAudioConnection(0);
            if (m_audioOutput)
                m_typeConnection = CONNECTION_QAUDIO;
            else
                m_typeConnection = CONNECTION_NONE;
        }
    }
#endif
    else
    {
        if (numDevice < 0 || numDevice >= QAudioDeviceInfo::availableDevices(QAudio::AudioOutput).count())
            return;
        this->openQAudioConnection(numDevice);
        if (m_audioOutput)
            m_typeConnection = CONNECTION_QAUDIO;
        else
        {
            this->openJackConnection();
            if (m_jack_client)
                m_typeConnection = CONNECTION_JACK;
            else
            {
#ifdef PA_USE_ASIO
                this->openAsioConnection();
                if (m_isAsioRunning)
                    m_typeConnection = CONNECTION_ASIO;
                else
#endif
                    m_typeConnection = CONNECTION_NONE;
            }
        }
    }
}
void AudioDevice::openJackConnection()
{
    // Format audio à l'écoute
    m_format.setSampleRate(SAMPLE_RATE);
    m_format.setChannelCount(2);
    m_format.setSampleSize(32);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::Float);
    // Nom du serveur
    const char *client_name = "Polyphone";
    jack_status_t status;
    // Ouverture d'une session cliente au serveur Jack
    m_jack_client = jack_client_open (client_name, JackNullOption, &status);
    if (m_jack_client == NULL)
    {
        printf ("jack_client_open() failed, status = 0x%2.0x\n", status);
        if (status & JackServerFailed)
            printf("Unable to connect to JACK server\n");
        return;
    }
    if (status & JackServerStarted)
        printf("JACK server started\n");
    if (status & JackNameNotUnique)
    {
        client_name = jack_get_client_name(m_jack_client);
        printf("unique name `%s' assigned\n", client_name);
    }
    // Callback de jack pour la récupération de données et l'arrêt
    jack_set_process_callback(m_jack_client, jackProcess, this);
    jack_on_shutdown(m_jack_client, jack_shutdown, 0);
    // Enregistrement fréquence d'échantillonnage
    this->m_format.setSampleRate((int)jack_get_sample_rate(m_jack_client));
    // Nombre de sorties audio
    const char ** ports = jack_get_ports(m_jack_client, NULL, NULL,
            JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL)
    {
        printf("no physical playback ports\n");
        return;
    }
    bool mono = ports[1] == NULL;
    // Création ports de sortie
    if (mono)
    {
        m_format.setChannelCount(1);
        m_output_port_L = jack_port_register(m_jack_client, "output mono",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput, 0);
        m_output_port_R = NULL;
        if (!m_output_port_L)
        {
            printf("no more JACK ports available\n");
            return;
        }
    }
    else
    {
        m_output_port_L = jack_port_register(m_jack_client, "output L",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput, 0);
        m_output_port_R = jack_port_register(m_jack_client, "output R",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput, 0);
        if (!m_output_port_R || !m_output_port_L)
        {
            printf("no more JACK ports available\n");
            return;
        }
    }
    // Envoi du format au synthé et activation
    m_synth->setFormat(m_format, true);
    emit(start());
    // Activation du serveur et connexion du port de sortie avec les hauts parleurs
    if (jack_activate(m_jack_client))
    {
        printf("cannot activate client\n");
        return;
    }

    if (jack_connect(m_jack_client, jack_port_name(m_output_port_L), ports[0]))
        printf("cannot connect output port L\n");
    if (!mono)
    {
        if (jack_connect(m_jack_client, jack_port_name(m_output_port_R), ports[1]))
            printf("cannot connect output port R\n");
    }
    free(ports);
}
#ifdef PA_USE_ASIO
void AudioDevice::openAsioConnection()
{
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("Asio error during initialization: %s\n", Pa_GetErrorText(err));
        return;
    }
    // Format audio à l'écoute
    m_format.setSampleRate(SAMPLE_RATE);
    m_format.setChannelCount(2);
    m_format.setSampleSize(32);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::Float);
    // Sortie audio par défaut, nombre de canaux max
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if (outputParameters.device == paNoDevice)
    {
      printf("Asio error: No default output device.\n");
      return;
    }
    const PaDeviceInfo* pdi = Pa_GetDeviceInfo(outputParameters.device);
    outputParameters.channelCount = pdi->maxOutputChannels;
    if (outputParameters.channelCount > 2)
        outputParameters.channelCount = 2;
    m_format.setChannelCount(outputParameters.channelCount);
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    outputParameters.suggestedLatency = pdi->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    // Ouverture du flux
    err = Pa_OpenStream(&m_asioStream,
                        NULL,               // pas d'entrée
                        &outputParameters,  // paramètres
                        SAMPLE_RATE,        // sample rate
                        FRAMES_PER_BUFFER,  // frame par buffer
                        paClipOff,          // no clipping
                        asioProcess,        // callback
                        this);              // instance d'audiodevice
    // Envoi du format au synthé et activation
    m_synth->setFormat(m_format, true);
    emit(start());
    // Début du son
    if (err == paNoError)
    {
        err = Pa_StartStream(m_asioStream); // Début du son
        if (err != paNoError)
        {
            printf("Asio error when starting stream: %s\n", Pa_GetErrorText(err));
            Pa_CloseStream(m_asioStream);
        }
        else
            m_isAsioRunning = true;
    }
    else
        printf("Asio error when opening stream: %s\n", Pa_GetErrorText(err));
}
#endif
void AudioDevice::openQAudioConnection(int numDevice)
{
    // Format audio à l'écoute
    m_format.setSampleRate(SAMPLE_RATE);
    m_format.setChannelCount(2);
    m_format.setSampleSize(24);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);
    // Serveur son
    QAudioDeviceInfo m_audioOutputDevice;
    QList<QAudioDeviceInfo> listeAudio = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if (listeAudio.count())
    {
        if (listeAudio.count() > numDevice)
            m_audioOutputDevice = listeAudio.takeAt(numDevice);
        else
            m_audioOutputDevice = QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
        if (!m_audioOutputDevice.isFormatSupported(m_format))
            m_format = m_audioOutputDevice.nearestFormat(m_format);
        // Envoi du format au synthé et activation
        m_synth->setFormat(m_format, false);
        emit(start());
        // Création output
        m_audioOutput = new QAudioOutput(m_audioOutputDevice, m_format);
        m_audioOutput->setBufferSize(BUFFER_AUDIO);
        // Début du son, mode pull
        m_audioOutput->start(this->m_synth);
    }
    else
        m_audioOutput = NULL;
}

void AudioDevice::closeConnections()
{
    if (m_audioOutput)
    {
        m_audioOutput->stop();
        m_audioOutput->reset();
        delete m_audioOutput;
        m_audioOutput = NULL;
    }
    if (m_jack_client)
    {
        jack_client_close(m_jack_client);
        m_jack_client = NULL;
    }
#ifdef PA_USE_ASIO
    if (m_isAsioRunning)
    {
        Pa_StopStream(m_asioStream);
        Pa_CloseStream(m_asioStream);
        m_isAsioRunning = false;
    }
#endif
}
