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

#include "audiodevice.h"
#include "synth.h"

// Callback portaudio
int standardProcess(const void* inputBuffer, void* outputBuffer,
                    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags, void* userData)
{
    Q_UNUSED(inputBuffer);
    Q_UNUSED(timeInfo);
    Q_UNUSED(statusFlags);
    // Récupération de l'instance de AudioDevice
    AudioDevice * instance = static_cast<AudioDevice*>(userData);
    float** outputs = (float**)outputBuffer;
    // Envoi de données
    if (instance->_format.channelCount() == 2)
        instance->_synth->readData(outputs[1], outputs[0], framesPerBuffer);
    return 0;
}

// Callbacks de jack
int jackProcess(jack_nframes_t nframes, void * arg)
{
    // Récupération de l'instance de AudioDevice
    AudioDevice * instance = static_cast<AudioDevice*>(arg);
    // Envoi de données
    if (instance->_output_port_R && instance->_output_port_L)
    {
        // Stéréo
        float * out1 = (float *)jack_port_get_buffer(instance->_output_port_R, nframes);
        float * out2 = (float *)jack_port_get_buffer(instance->_output_port_L, nframes);
        instance->_synth->readData(out1, out2, nframes);
    }
    return 0;
}
void jack_shutdown(void *arg) {Q_UNUSED(arg); exit(1);}


AudioDevice::AudioDevice(Synth *synth, QObject *parent) : QObject(parent),
    _synth(synth),
    _isStandardRunning(false),
    _standardStream(NULL),
    _jack_client(NULL),
    _typeConnection(CONNECTION_NONE)
{
}
AudioDevice::~AudioDevice()
{
}

void AudioDevice::initAudio(int numDevice, int bufferSize)
{
    //  0 : défaut
    // -1 : arrêt
    // -2 : jack
    // -3 : asio

    // Arrêt des serveurs son si besoin
    this->closeConnections();
    if (numDevice == -1)
        return;

    // Ouverture serveur son (doit être fait après la création, une fois dans le bon thread)
    if (numDevice == -2)
    {
        // Lancement Jack
        this->openJackConnection(bufferSize);
        if (_jack_client)
            _typeConnection = CONNECTION_JACK;
        else
        {
            this->openStandardConnection(bufferSize, false);
            if (_isStandardRunning)
                _typeConnection = CONNECTION_STANDARD;
            else
                _typeConnection = CONNECTION_NONE;
        }
    }
    else if (numDevice == -3)
    {
        // Lancement Asio
        this->openStandardConnection(bufferSize, true);
        if (_isStandardRunning)
            _typeConnection = CONNECTION_ASIO;
        else
        {
            this->openStandardConnection(bufferSize, false);
            if (_isStandardRunning)
                _typeConnection = CONNECTION_STANDARD;
            else
                _typeConnection = CONNECTION_NONE;
        }
    }
    else
    {
        this->openStandardConnection(bufferSize, false);
        if (_isStandardRunning)
            _typeConnection = CONNECTION_STANDARD;
        else
        {
            this->openJackConnection(bufferSize);
            if (_jack_client)
                _typeConnection = CONNECTION_JACK;
            else
            {
                this->openStandardConnection(bufferSize, true);
                if (_isStandardRunning)
                    _typeConnection = CONNECTION_ASIO;
                else
                    _typeConnection = CONNECTION_NONE;
            }
        }
    }
}

void AudioDevice::openJackConnection(int bufferSize)
{
    // Format audio à l'écoute
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(2);
    _format.setSampleSize(32);

    // Nom du serveur
    const char *client_name = "Polyphone";
    jack_status_t status;

    // Ouverture d'une session cliente au serveur Jack
    _jack_client = jack_client_open(client_name, JackNullOption, &status);
    if (_jack_client == NULL)
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
        client_name = jack_get_client_name(_jack_client);
        printf("unique name '%s' assigned\n", client_name);
    }

    // Callback de jack pour la récupération de données et l'arrêt
    jack_set_process_callback(_jack_client, jackProcess, this);
    jack_on_shutdown(_jack_client, jack_shutdown, 0);

    // Enregistrement fréquence d'échantillonnage
    this->_format.setSampleRate((int)jack_get_sample_rate(_jack_client));

    // Modification taille buffer
    jack_set_buffer_size(_jack_client, bufferSize);

    // Nombre de sorties audio
    const char ** ports = jack_get_ports(_jack_client, NULL, NULL,
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
        _format.setChannelCount(1);
        _output_port_L = jack_port_register(_jack_client, "output mono",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsOutput, 0);
        _output_port_R = NULL;
        if (!_output_port_L)
        {
            printf("no more JACK ports available\n");
            return;
        }
    }
    else
    {
        _output_port_L = jack_port_register(_jack_client, "output L",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsOutput, 0);
        _output_port_R = jack_port_register(_jack_client, "output R",
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsOutput, 0);
        if (!_output_port_R || !_output_port_L)
        {
            printf("no more JACK ports available\n");
            return;
        }
    }

    // Envoi du format au synthé et activation
    _synth->setFormat(_format);
    emit(start());

    // Activation du serveur et connexion du port de sortie avec les hauts parleurs
    if (jack_activate(_jack_client))
    {
        printf("cannot activate client\n");
        return;
    }

    if (jack_connect(_jack_client, jack_port_name(_output_port_L), ports[0]))
        printf("cannot connect output port L\n");
    if (!mono)
    {
        if (jack_connect(_jack_client, jack_port_name(_output_port_R), ports[1]))
            printf("cannot connect output port R\n");
    }
    free(ports);
}
void AudioDevice::openStandardConnection(int bufferSize, bool isAsio)
{
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        printf("Error during initialization: %s\n", Pa_GetErrorText(err));
        return;
    }

    // Recherche du device à utiliser
    int numDevice = -1;
    QList<int> listDevices;
    double maxLatency = 0.2;
    for (int i = 0; i < Pa_GetDeviceCount(); i++)
    {
        const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);
        const PaHostApiInfo * hostInfo  = Pa_GetHostApiInfo(deviceInfo->hostApi);
        if (deviceInfo->maxOutputChannels)
        {
            if (isAsio)
            {
                // Recherche d'asio
                if (QString(hostInfo->name).toLower().indexOf("asio") != -1)
                {
                    if(Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice == i)
                        numDevice = i;
                    listDevices << i;
                    maxLatency = 0.04;
                }
            }
            else
            {
                // Recherche d'alsa ou mme
                if (QString(hostInfo->name).toLower().indexOf("alsa") != -1 ||
                        QString(hostInfo->name).toLower().indexOf("mme") != -1)
                {
                    if(Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice == i)
                        numDevice = i;
                    listDevices << i;
                }
            }
        }
    }
    if (numDevice == -1)
    {
        if (listDevices.size())
            numDevice = listDevices.at(0);
        else
        {
            if (Pa_GetDefaultOutputDevice() != paNoDevice)
                numDevice = Pa_GetDefaultOutputDevice();
            else
            {
                printf("Error: No default output device.\n");
                return;
            }
        }
    }

    // Format audio à l'écoute
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(2);
    _format.setSampleSize(32);

    // Sortie audio par défaut, nombre de canaux max
    PaStreamParameters outputParameters;
    outputParameters.device = numDevice;
    const PaDeviceInfo* pdi = Pa_GetDeviceInfo(outputParameters.device);
    outputParameters.channelCount = pdi->maxOutputChannels;
    if (outputParameters.channelCount > 2)
        outputParameters.channelCount = 2;
    _format.setChannelCount(outputParameters.channelCount);
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    outputParameters.suggestedLatency = qMin(maxLatency, pdi->defaultLowOutputLatency);
    outputParameters.hostApiSpecificStreamInfo = NULL;

    // Ouverture du flux
    err = Pa_OpenStream(&_standardStream,
                        NULL,               // pas d'entrée
                        &outputParameters,  // paramètres
                        SAMPLE_RATE,        // sample rate
                        bufferSize,         // frame par buffer
                        0, //paClipOff,     // avec clipping
                        standardProcess,    // callback
                        this);              // instance d'audiodevice

    // Envoi du format au synthé et activation
    _synth->setFormat(_format);
    emit(start());

    // Début du son
    if (err == paNoError)
    {
        err = Pa_StartStream(_standardStream); // Début du son
        if (err != paNoError)
        {
            printf("Error when starting stream: %s\n", Pa_GetErrorText(err));
            Pa_CloseStream(_standardStream);
        }
        else
            _isStandardRunning = true;
    }
    else
        printf("Error when opening stream: %s\n", Pa_GetErrorText(err));
}

void AudioDevice::closeConnections()
{
    if (_isStandardRunning)
    {
        Pa_StopStream(_standardStream);
        Pa_CloseStream(_standardStream);
        Pa_Terminate();
        _isStandardRunning = false;
        _standardStream = NULL;
    }
    if (_jack_client)
    {
        jack_client_close(_jack_client);
        _jack_client = NULL;
    }
}
