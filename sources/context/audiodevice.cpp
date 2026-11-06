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

#include "audiodevice.h"
#include "audiodevicewatcher.h"
#include "recorder.h"
#include "synth.h"
#include "confmanager.h"
#include "soundfontmanager.h"
#include "soundfonts.h"

// RtAudio 5 only defines the version as a string, the numbers came with the new API of RtAudio 6
// (device identifiers, errors given to a callback instead of exceptions)
#ifndef RTAUDIO_VERSION_MAJOR
#define RT_AUDIO_5
#else
class RtAudioError {
public:
    void printMessage() {}
};
#endif

QList<AudioDevice::HostInfo> AudioDevice::getAllHosts(bool isInput)
{
    QList<HostInfo> hosts;

    // Browse all hosts
    std::vector<RtAudio::Api> apis;
    RtAudio::getCompiledApi(apis);
    RtAudio::DeviceInfo info;
    for (unsigned int i = 0; i < apis.size(); i++)
    {
        // Host details (ASIO can only be used by one stream, the one of the output)
        RtAudio::Api availableApi = apis[i];
        if (isInput && availableApi == RtAudio::WINDOWS_ASIO)
            continue;
        HostInfo hostInfo;
        hostInfo.identifier = QString::fromStdString(RtAudio::getApiName(availableApi).c_str());
        hostInfo.name = QString::fromStdString(RtAudio::getApiDisplayName(availableApi).c_str());
        hostInfo.canFollowDefault = (availableApi == RtAudio::MACOSX_CORE || availableApi == RtAudio::WINDOWS_WASAPI);

        if (!isInput && availableApi == _currentApi && !_currentDeviceName.isEmpty())
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
            #ifdef RT_AUDIO_5
            for (unsigned int j = 0, end = audio->getDeviceCount(); j < end; ++j)
            #else
            std::vector<unsigned int> deviceIds;
            try {
                deviceIds = audio->getDeviceIds();
            } catch (RtAudioError &error) {
                error.printMessage();
            }

            for (unsigned int j: deviceIds)
            #endif
            {
                try {
                    info = audio->getDeviceInfo(j);
                    if (isInput ? info.inputChannels >= 1 : info.outputChannels >= 2)
                    {
                        // Device details
                        DeviceInfo deviceInfo;
                        deviceInfo.name = QString::fromStdString(info.name.c_str());
                        deviceInfo.channelCount = isInput ? info.inputChannels : info.outputChannels;
                        deviceInfo.isDefault = isInput ? info.isDefaultInput : info.isDefaultOutput;
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

void AudioDevice::getHostAndDeviceFromConfig(QString config, QString &hostIdentifier, QString &hostName, QString &deviceName)
{
    int api;
    getApiAndDeviceNameFromConfig(config, api, deviceName);
    hostIdentifier = QString::fromStdString(RtAudio::getApiName((RtAudio::Api)api).c_str());
    hostName = QString::fromStdString(RtAudio::getApiDisplayName((RtAudio::Api)api).c_str());
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
    _rtAudioInput(nullptr),
    _inputRecorder(new Recorder()),
    _inputSampleRate(0),
    _inputChannelCount(2),
    _configuration(configuration)
{
    _synth = new Synth(SoundfontManager::getInstance()->getSoundfonts(), SoundfontManager::getInstance()->getMutex());
    _synth->configure(_configuration->getSynthConfig());
    initAudio();

    // Changes in the audio outputs, grouped since a single change is notified several times
    _deviceCheckTimer.setSingleShot(true);
    _deviceCheckTimer.setInterval(500);
    connect(&_deviceCheckTimer, SIGNAL(timeout()), this, SLOT(checkDevices()));
    AudioDeviceWatcher * watcher = new AudioDeviceWatcher(this);
    connect(watcher, SIGNAL(devicesChanged()), &_deviceCheckTimer, SLOT(start()));
}

AudioDevice::~AudioDevice()
{
    closeInput();
    delete _inputRecorder;
    closeConnections();
    delete _synth;
}

void AudioDevice::initAudio()
{
    // Stop the possible existing connection
    closeConnections();

    // Polyphony, the voices being allocated while there is no audio
    _synth->setMaxPolyphony(_configuration->getValue(ConfManager::SECTION_AUDIO, "polyphony", DEFAULT_POLYPHONY).toInt());

    // Read the configuration
    QString audioType = _configuration->getValue(ConfManager::SECTION_AUDIO, "type", "").toString();
    if (audioType == "none")
        return;
    getApiAndDeviceNameFromConfig(audioType, _currentApi, _currentDeviceName);

    // Buffer size
    quint32 bufferSize = _configuration->getValue(ConfManager::SECTION_AUDIO, "buffer_size", 0).toUInt();
    if (bufferSize == 0)
    {
        // Default buffer size, depends on the API
        switch (_currentApi)
        {
        case RtAudio::WINDOWS_WASAPI: case RtAudio::WINDOWS_DS:
            bufferSize = 512;
            break;
        default:
            bufferSize = 256;
            break;
        }
    }
    else if (bufferSize < COMPUTATION_CHUNK)
    {
        // Smaller sizes (no longer proposed) would compute a chunk once every several calls
        bufferSize = COMPUTATION_CHUNK;
    }

    // Instantiate an RtAudio object with the selected api
    try {
        #ifdef RT_AUDIO_5
        _rtAudio = new RtAudio((RtAudio::Api)_currentApi);
        #else
        _rtAudio = new RtAudio((RtAudio::Api)_currentApi, [this](RtAudioErrorType type, const std::string &errorText) {
            // The stream has been closed by RtAudio in an audio thread
            if (type == RTAUDIO_DEVICE_DISCONNECT)
                QMetaObject::invokeMethod(this, [this]() {
                    _deviceCheckTimer.start();
                }, Qt::QueuedConnection);
            else
                qWarning("%s", errorText.c_str());
        });
        #endif
    } catch (RtAudioError &error) {
        error.printMessage();
        return;
    }
    #ifdef RT_AUDIO_5
    _rtAudio->showWarnings(false);
    #endif

    // Find the device to open
    std::vector<unsigned int> sampleRates;
    int selectedDevice = findDevice(_openedDeviceName, sampleRates);
    if (selectedDevice == -1)
    {
        // No audio output
        closeConnections();
        return;
    }

    // Open the connection
    openConnection(selectedDevice, bufferSize, getClosestSampleRate(sampleRates));
}

quint32 AudioDevice::getClosestSampleRate(const std::vector<unsigned int> &sampleRates)
{
    quint32 closestSampleRate = 0;
    for (auto const sampleRate : sampleRates)
    {
        // Find anything that is close to 48000
        if (closestSampleRate == 0 || qAbs(sampleRate - 48000) < qAbs(closestSampleRate - 48000))
            closestSampleRate = sampleRate;
    }
    return closestSampleRate == 0 ? 48000 : closestSampleRate;
}

int AudioDevice::findDevice(QString &deviceName, std::vector<unsigned int> &sampleRates)
{
    // Browse the devices
    int defaultDevice = -1;
    #ifdef RT_AUDIO_5
    for (unsigned int i = 0, end = _rtAudio->getDeviceCount(); i < end; ++i)
    #else
    std::vector<unsigned int> deviceIds;
    try {
        deviceIds = _rtAudio->getDeviceIds();
    } catch (RtAudioError &error) {
        error.printMessage();
    }
    for (unsigned int i: deviceIds)
    #endif
    {
        try {
            RtAudio::DeviceInfo deviceInfo = _rtAudio->getDeviceInfo(i);
            if (deviceInfo.outputChannels < 2)
                continue;

            if (QString::fromStdString(deviceInfo.name.c_str()) == _currentDeviceName)
            {
                deviceName = _currentDeviceName;
                sampleRates = deviceInfo.sampleRates;
                return i;
            }
            if (deviceInfo.isDefaultOutput)
            {
                deviceName = QString::fromStdString(deviceInfo.name.c_str());
                sampleRates = deviceInfo.sampleRates;
                if (_currentDeviceName == "")
                    return i;
                defaultDevice = i;
            }
        } catch (RtAudioError &error) {
            error.printMessage();
            return -1;
        }
    }

    // No exact match, the default device is selected (if any)
    return defaultDevice;
}

int AudioDevice::findInputDevice(QString deviceName, RtAudio::DeviceInfo &deviceInfo)
{
    // Browse the devices
    int defaultDevice = -1;
    #ifdef RT_AUDIO_5
    for (unsigned int i = 0, end = _rtAudioInput->getDeviceCount(); i < end; ++i)
    #else
    std::vector<unsigned int> deviceIds;
    try {
        deviceIds = _rtAudioInput->getDeviceIds();
    } catch (RtAudioError &error) {
        error.printMessage();
    }
    for (unsigned int i: deviceIds)
    #endif
    {
        try {
            RtAudio::DeviceInfo info = _rtAudioInput->getDeviceInfo(i);
            if (info.inputChannels < 1)
                continue;

            if (QString::fromStdString(info.name.c_str()) == deviceName)
            {
                deviceInfo = info;
                return i;
            }
            if (info.isDefaultInput || defaultDevice == -1)
            {
                deviceInfo = info;
                defaultDevice = i;
            }
        } catch (RtAudioError &error) {
            error.printMessage();
            return -1;
        }
    }

    // No exact match, the default device is selected (if any)
    return defaultDevice;
}

// RtAudio callback of the audio input
int inputProcess(void *outputBuffer, void *inputBuffer, unsigned int nFrames,
                 double streamTime, RtAudioStreamStatus status, void *userData)
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(status)

    AudioDevice * instance = static_cast<AudioDevice*>(userData);
    const float * input = reinterpret_cast<const float*>(inputBuffer);
    instance->getInputRecorder()->process(&input[0], instance->getInputChannelCount() == 2 ? &input[nFrames] : nullptr, nFrames);
    return 0;
}

bool AudioDevice::openInput()
{
    closeInput();

    // By default, the default input of the host used for the output
    QString config = _configuration->getValue(ConfManager::SECTION_AUDIO, "input", "").toString();
    if (config == "none")
        return false;
    if (config.isEmpty())
        config = _configuration->getValue(ConfManager::SECTION_AUDIO, "type", "").toString().section('|', 0, 0) + "|";
    int api;
    QString deviceName;
    getApiAndDeviceNameFromConfig(config, api, deviceName);

    try {
        #ifdef RT_AUDIO_5
        _rtAudioInput = new RtAudio((RtAudio::Api)api);
        #else
        _rtAudioInput = new RtAudio((RtAudio::Api)api, [](RtAudioErrorType, const std::string &errorText) {
            qWarning("%s", errorText.c_str());
        });
        #endif
    } catch (RtAudioError &error) {
        error.printMessage();
        return false;
    }
    #ifdef RT_AUDIO_5
    _rtAudioInput->showWarnings(false);
    #endif

    RtAudio::DeviceInfo deviceInfo;
    int selectedDevice = findInputDevice(deviceName, deviceInfo);
    if (selectedDevice == -1)
    {
        closeInput();
        return false;
    }

    // Mono or stereo, depending on the device
    _inputChannelCount = qMin(static_cast<int>(deviceInfo.inputChannels), 2);
    _inputSampleRate = getClosestSampleRate(deviceInfo.sampleRates);
    RtAudio::StreamParameters inParam;
    inParam.deviceId = static_cast<unsigned int>(selectedDevice);
    inParam.nChannels = static_cast<unsigned int>(_inputChannelCount);
    inParam.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_NONINTERLEAVED;
    options.streamName = "Polyphone input";
    unsigned int bufferSize = 512;
    try {
        _rtAudioInput->openStream(nullptr, &inParam, RTAUDIO_FLOAT32, _inputSampleRate, &bufferSize, inputProcess, this, &options);
        _rtAudioInput->startStream();
    } catch (RtAudioError &error) {
        error.printMessage();
    }

    if (!_rtAudioInput->isStreamRunning())
    {
        closeInput();
        return false;
    }
    return true;
}

void AudioDevice::closeInput()
{
    delete _rtAudioInput;
    _rtAudioInput = nullptr;
}

void AudioDevice::checkDevices()
{
    bool reopen = true;
    if (_rtAudio != nullptr && _rtAudio->isStreamRunning())
    {
        // Possibly keep the stream if it already uses the right device (ASIO cannot be probed while it is in use)
        QString deviceName;
        std::vector<unsigned int> sampleRates;
        reopen = _currentApi != RtAudio::WINDOWS_ASIO &&
                 (findDevice(deviceName, sampleRates) == -1 || deviceName != _openedDeviceName);
    }

    // The configured device if available, otherwise the default device (the configuration is not changed)
    if (reopen)
        initAudio();

    emit devicesChanged();
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
