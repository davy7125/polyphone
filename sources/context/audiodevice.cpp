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

#include <QDebug>
#include "audiodevice.h"
#include "rtaudio/RtAudio.h"
#include "confmanager.h"
#include "soundfontmanager.h"

#ifndef RT_AUDIO_5_2
class RtAudioError {
public:
    void printMessage() {}
};
#endif

QList<AudioDevice::HostInfo> AudioDevice::getAllHosts()
{
    QList<HostInfo> hosts;

    // Browse all hosts
    std::vector<RtAudio::Api> apis;
    RtAudio::getCompiledApi(apis);
    RtAudio::DeviceInfo info;
    for (unsigned int i = 0; i < apis.size(); i++)
    {
        // Host details
        RtAudio::Api availableApi = apis[i];
        HostInfo hostInfo;
        hostInfo.identifier = QString(RtAudio::getApiName(availableApi).c_str());
        hostInfo.name = QString(RtAudio::getApiDisplayName(availableApi).c_str());

        if (availableApi == _currentApi && !_currentDeviceName.isEmpty())
        {
            // ASIO cannot be probed while it is in use
            if (_currentApi == RtAudio::Api::WINDOWS_ASIO)
            {
                DeviceInfo deviceInfo;
                deviceInfo.name = _currentDeviceName;
                deviceInfo.channelCount = 2;
                deviceInfo.isDefault = true;
                hostInfo.devices << deviceInfo;

                hosts << hostInfo;
                continue;
            }
        }

        // No sound with WASAPI if we scan ASIO at the same time
        if (_currentApi == RtAudio::Api::WINDOWS_WASAPI && availableApi == RtAudio::Api::WINDOWS_ASIO)
            closeConnections();

        // Browse devices
        RtAudio * audio = nullptr;
        try {
            audio = new RtAudio(apis[i]);
        } catch (RtAudioError &error) {
            error.printMessage();
        }
        if (audio)
        {
            #ifdef RT_AUDIO_5_2
            for (unsigned int j = 0, end = audio->getDeviceCount(); j < end; ++j)
            #else
            std::vector<unsigned int> deviceIds = audio->getDeviceIds();
            for (unsigned int j: deviceIds)
            #endif
            {
                try {
                    info = audio->getDeviceInfo(j);
                    if (info.outputChannels >= 2)
                    {
                        // Device details
                        DeviceInfo deviceInfo;
                        deviceInfo.name = QString(info.name.c_str());
                        deviceInfo.channelCount = info.outputChannels;
                        deviceInfo.isDefault = info.isDefaultOutput;
                        hostInfo.devices << deviceInfo;
                    }
                } catch (RtAudioError &error) {
                    error.printMessage();
                }
            }
            delete audio;
        }

        // Possibly restore the sound
        if (_currentApi == RtAudio::Api::WINDOWS_WASAPI && availableApi == RtAudio::Api::WINDOWS_ASIO)
            initAudio();

        if (!hostInfo.devices.empty())
            hosts << hostInfo;
    }

    return hosts;
}

int AudioDevice::getCurrentDeviceIndex(QList<AudioDevice::HostInfo> hosts, QString config)
{
    if (config == "none")
        return -1;

    // Find the host identifier and device name
    int api;
    QString deviceName;
    getApiAndDeviceNameFromConfig(config, api, deviceName);
    QString hostIdentifier = QString(RtAudio::getApiName((RtAudio::Api)api).c_str());

    int currentIndex = 0;
    int exactMatch = -1;
    int defaultMatch = -1;
    HostInfo hostInfo;
    DeviceInfo deviceInfo;
    for (int i = 0; i < hosts.count(); i++)
    {
        hostInfo = hosts[i];
        if (hostInfo.identifier == hostIdentifier)
        {
            for (int j = 0; j < hostInfo.devices.count(); j++)
            {
                deviceInfo = hostInfo.devices[j];
                if (deviceInfo.name == deviceName)
                {
                    exactMatch = currentIndex;
                    break;
                }
                if (deviceInfo.isDefault)
                {
                    if (deviceName == "")
                    {
                        exactMatch = currentIndex;
                        break;
                    }
                    defaultMatch = currentIndex;
                }
                currentIndex++;
            }
            break;
        }
        else
            currentIndex += hostInfo.devices.count();
    }

    return exactMatch != -1 ? exactMatch : defaultMatch;
}

void AudioDevice::getApiAndDeviceNameFromConfig(QString config, int &api, QString &deviceName)
{
    // Find the host identifier and device name
    QString hostIdentifier = "";
    deviceName = "";
    QStringList configParts = config.split('|');
    if (configParts.count() == 2)
    {
        hostIdentifier = configParts[0];
        deviceName = configParts[1];
    }

    // Take the default API if the host identifier is not valid
    api = RtAudio::getCompiledApiByName(hostIdentifier.toStdString());
    if (api == RtAudio::UNSPECIFIED)
#if defined(Q_OS_MACOS)
        api = RtAudio::MACOSX_CORE;
#elif defined(Q_OS_WIN)
        api = RtAudio::WINDOWS_WASAPI;
#else
        api = RtAudio::LINUX_ALSA;
#endif
}

// RtAudio callback
int standardProcess(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                    double streamTime, RtAudioStreamStatus status, void *userData)
{
    Q_UNUSED(inputBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(status)

    // Get the AudioDevice instance
    AudioDevice * instance = static_cast<AudioDevice*>(userData);
    float* output = reinterpret_cast<float*>(outputBuffer);

    // Read data from the synth and write it in the buffer
    instance->getSynth()->readData(&output[0], &output[nFrames], nFrames);

    return 0;
}

AudioDevice::AudioDevice(ConfManager *configuration) : QObject(nullptr),
    _rtAudio(nullptr),
    _configuration(configuration)
{
    _synth = new Synth(SoundfontManager::getInstance()->getSoundfonts(), SoundfontManager::getInstance()->getMutex());
    _synth->configure(_configuration->getSynthConfig());
    initAudio();
}

AudioDevice::~AudioDevice()
{
    closeConnections();
    delete _synth;
}

void AudioDevice::initAudio()
{
    // Stop the possible existing connection
    closeConnections();

    // Read the configuration
    QString audioType = _configuration->getValue(ConfManager::SECTION_AUDIO, "type", "").toString();
    if (audioType == "none")
        return;
    getApiAndDeviceNameFromConfig(audioType, _currentApi, _currentDeviceName);

    // Buffer size
    quint32 bufferSize = _configuration->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 0).toUInt();
    if (bufferSize == 0)
        bufferSize = 256;

    // Instanciate an RtAudio object with the selected api
    try {
        _rtAudio = new RtAudio((RtAudio::Api)_currentApi);
    } catch (RtAudioError &error) {
        error.printMessage();
        return;
    }
    #ifdef RT_AUDIO_5_2
    _rtAudio->showWarnings(false);
    #endif

    // Browse the devices
    int selectedDevice = -1;
    int defaultDevice = -1;
    std::vector<unsigned int> sampleRates;
    #ifdef RT_AUDIO_5_2
    for (unsigned int i = 0, end = _rtAudio->getDeviceCount(); i < end; ++i)
    #else
    std::vector<unsigned int> deviceIds = _rtAudio->getDeviceIds();
    for (unsigned int i: deviceIds)
    #endif
    {
        try {
            RtAudio::DeviceInfo deviceInfo = _rtAudio->getDeviceInfo(i);
            if (deviceInfo.outputChannels < 2)
                continue;

            if (QString(deviceInfo.name.c_str()) == _currentDeviceName)
            {
                selectedDevice = i;
                sampleRates = deviceInfo.sampleRates;
                break;
            }
            if (deviceInfo.isDefaultOutput)
            {
                sampleRates = deviceInfo.sampleRates;
                if (_currentDeviceName == "")
                {
                    selectedDevice = i;
                    break;
                }
                defaultDevice = i;
            }
        } catch (RtAudioError &error) {
            error.printMessage();
            return;
        }
    }

    // No exact match, the default device is selected
    if (selectedDevice == -1)
    {
        selectedDevice = defaultDevice;

        // No default device either, so no audio output
        if (selectedDevice == -1)
        {
            closeConnections();
            return;
        }
    }

    quint32 closestSampleRate = 0;
    for (auto const sampleRate : sampleRates)
    {
        // Find anything that is close to 48000
        if (closestSampleRate == 0 || qAbs(sampleRate - 48000) < qAbs(closestSampleRate - 48000))
            closestSampleRate = sampleRate;
    }
    if (closestSampleRate == 0)
        closestSampleRate = 48000;

    // Open the connection
    openConnection(selectedDevice, bufferSize, closestSampleRate);
}

void AudioDevice::openConnection(unsigned int deviceNumber, quint32 bufferSize, quint32 sampleRate)
{
    RtAudio::StreamParameters outParam;
    outParam.deviceId = deviceNumber;
    outParam.nChannels = 2;
    outParam.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;
    options.streamName = "Polyphone";
    try {
        _rtAudio->openStream(&outParam, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferSize, standardProcess, this, &options);
    } catch (RtAudioError &error) {
        closeConnections();
        return;
    }

    // Configure the synth
    _synth->setSampleRateAndBufferSize(sampleRate, bufferSize);

    try {
        _rtAudio->startStream();
    } catch (RtAudioError &error) {
        closeConnections();
    }
}

void AudioDevice::closeConnections()
{
    delete _rtAudio;
    _rtAudio = nullptr;
}
