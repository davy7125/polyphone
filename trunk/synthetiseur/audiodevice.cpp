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

#include <QDebug>
#include "audiodevice.h"
#include "synth.h"
#include "portaudio.h"

#ifndef Q_OS_WIN
#include "jack.h"

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
#endif

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

AudioDevice::AudioDevice(QObject *parent) : QObject(parent),
    _isStandardRunning(false),
    _standardStream(NULL),
    _jack_client(NULL),
    _initialized(false)
{
    PaError err = Pa_Initialize();
    _initialized = (err == paNoError);
    if (!_initialized)
        printf("Error during initialization: %s\n", Pa_GetErrorText(err));
}

AudioDevice::~AudioDevice()
{
    if (_initialized)
        Pa_Terminate();
}

QList<HostInfo> AudioDevice::getHostInfo()
{
    QList<HostInfo> listRet;

    // No sound
    listRet << HostInfo("-", -1);

    if (_initialized)
    {
        for (int i = 0; i < Pa_GetDeviceCount(); i++)
        {
            const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);
            const PaHostApiInfo * hostInfo  = Pa_GetHostApiInfo(deviceInfo->hostApi);

            if (deviceInfo->maxOutputChannels > 0 &&
                    (hostInfo->type == paDirectSound || hostInfo->type == paMME ||
                     hostInfo->type == paASIO || hostInfo->type == paCoreAudio ||
                     hostInfo->type == paALSA))
            {
                // Host
                int indexHost = -1;
                for (int j = 0; j < listRet.size(); j++)
                    if (listRet[j]._index == hostInfo->type)
                        indexHost = j;
                if (indexHost == -1)
                {
                    listRet << HostInfo(QString::fromLatin1(hostInfo->name), hostInfo->type);
                    indexHost = listRet.size() - 1;
                }

                // Device within an host
                if (i == hostInfo->defaultOutputDevice || hostInfo->type == paASIO)
                    listRet[indexHost]._devices << HostInfo::DeviceInfo(QString::fromLatin1(deviceInfo->name), i,
                                                                        (i == hostInfo->defaultOutputDevice));
            }
        }
    }

    // Default host
    bool found = false;
    for (int i = 0; i < listRet.size(); i++)
    {
        if (listRet[i]._index == paMME || listRet[i]._index == paALSA)
        {
            listRet[i]._isDefault = true;
            found = true;
            break;
        }
    }
    if (!found)
    {
        for (int i = 0; i < listRet.size(); i++)
        {
            if (listRet[i]._index == paDirectSound || listRet[i]._index == paCoreAudio)
            {
                listRet[i]._isDefault = true;
                found = true;
                break;
            }
        }
    }
    if (!found)
        listRet[0]._isDefault = true;

#ifndef Q_OS_WIN
    // Ajout jack
    listRet << HostInfo("Jack", -2);
#endif

    return listRet;
}

void AudioDevice::initAudio(int numDevice, int numIndex, int bufferSize)
{
    // ≥0 : standard
    // -1 : none
    // -2 : jack

    // Arrêt des serveurs son si besoin
    this->closeConnections();

#ifdef Q_OS_WIN
    // Asio cannot work without these two lines
    if (numDevice == paASIO)
    {
        Pa_Terminate();
        Pa_Initialize();
    }
    // No jack support
    if (numDevice == -2)
        numDevice = -1;
#endif

    if (numDevice == -1)
    {
        emit(connectionDone());
        return;
    }
    if (numDevice == -2)
        this->openJackConnection(bufferSize);
    else
        this->openStandardConnection(numDevice, numIndex, bufferSize);

    if (!_jack_client && !_isStandardRunning)
    {
        this->openDefaultConnection(bufferSize);
        if (!_jack_client && !_isStandardRunning)
            emit(connectionProblem());
        else
            emit(connectionDefault());
    }
    else
        emit(connectionDone());
}

void AudioDevice::openDefaultConnection(int bufferSize)
{
    QList<HostInfo> hostInfos = getHostInfo();

    int type = -1;
    int index = -1;

    for (int i = 1; i < hostInfos.size(); i++)
    {
        if (hostInfos[i]._isDefault)
        {
            for (int j = 0; j < hostInfos[i]._devices.count(); j++)
            {
                if (hostInfos[i]._devices[j]._isDefault)
                {
                    type = hostInfos[i]._index;
                    index = hostInfos[i]._devices[j]._index;
                    break;
                }
            }
            break;
        }
    }

    if (type != -1 && index != -1)
        openStandardConnection(type, index, bufferSize);
}

void AudioDevice::openJackConnection(int bufferSize)
{
#ifndef Q_OS_WIN
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

    // Envoi du format au synthé
    _synth->setFormat(_format);

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
#else
    Q_UNUSED(bufferSize)
#endif
}

void AudioDevice::openStandardConnection(int numDevice, int numIndex, int bufferSize)
{
    if (!_initialized)
        return;

    if (numIndex < 0 || numIndex >= Pa_GetDeviceCount())
        return;

    if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(numIndex)->hostApi)->type != numDevice)
        return;

    // Format audio à l'écoute
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(2);
    _format.setSampleSize(32);

    // Sortie audio par défaut, nombre de canaux max
    PaStreamParameters outputParameters;
    outputParameters.device = numIndex;
    const PaDeviceInfo* pdi = Pa_GetDeviceInfo(outputParameters.device);
    outputParameters.channelCount = pdi->maxOutputChannels;
    if (outputParameters.channelCount > 2)
        outputParameters.channelCount = 2;
    _format.setChannelCount(outputParameters.channelCount);
    outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
    if (numDevice == paASIO)
        outputParameters.suggestedLatency = qMin(0.04, pdi->defaultLowOutputLatency);
    else
        outputParameters.suggestedLatency = qMin(0.2, pdi->defaultLowOutputLatency);
    outputParameters.hostApiSpecificStreamInfo = NULL;

    // Ouverture du flux
    PaError err = Pa_OpenStream(&_standardStream,
                                NULL,               // pas d'entrée
                                &outputParameters,  // paramètres
                                SAMPLE_RATE,        // sample rate
                                bufferSize,         // frame par buffer
                                paNoFlag, //paClipOff,     // avec clipping
                                standardProcess,    // callback
                                this);              // instance d'audiodevice

    // Envoi du format au synthé
    _synth->setFormat(_format);

    // Début du son
    if (err == paNoError)
    {
        err = Pa_StartStream(_standardStream); // Début du son
        if (err != paNoError)
        {
            qDebug() << "Error when starting stream: " << Pa_GetErrorText(err);
            Pa_CloseStream(_standardStream);
        }
        else
            _isStandardRunning = true;
    }
    else
        qDebug() << "Error when opening stream:" << Pa_GetErrorText(err);
}

void AudioDevice::closeConnections()
{
    if (_isStandardRunning)
    {
        Pa_StopStream(_standardStream);
        Pa_CloseStream(_standardStream);
        _isStandardRunning = false;
        _standardStream = NULL;
    }

#ifndef Q_OS_WIN
    if (_jack_client)
    {
        jack_client_close(_jack_client);
        _jack_client = NULL;
    }
#endif
}
