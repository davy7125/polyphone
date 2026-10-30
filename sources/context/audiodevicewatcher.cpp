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

#include "audiodevicewatcher.h"

#if defined(Q_OS_MACOS)
#include <CoreAudio/CoreAudio.h>

static const AudioObjectPropertyAddress s_watchedProperties[] = {
    { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain },
    { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain }
};

// Called by CoreAudio in one of its threads
static OSStatus propertyListener(AudioObjectID objectId, UInt32 addressCount,
                                 const AudioObjectPropertyAddress * addresses, void * clientData)
{
    Q_UNUSED(objectId)
    Q_UNUSED(addressCount)
    Q_UNUSED(addresses)
    static_cast<AudioDeviceWatcher *>(clientData)->notify();
    return noErr;
}

#elif defined(Q_OS_WIN)
#include <windows.h>
#include <mmdeviceapi.h>

// Called by the system in one of its threads
class AudioDeviceWatcher::NotificationClient final : public IMMNotificationClient
{
public:
    NotificationClient(AudioDeviceWatcher * watcher) :
        _referenceCount(1),
        _watcher(watcher)
    {}

    // IUnknown
    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return InterlockedIncrement(&_referenceCount);
    }

    ULONG STDMETHODCALLTYPE Release() override
    {
        ULONG referenceCount = InterlockedDecrement(&_referenceCount);
        if (referenceCount == 0)
            delete this;
        return referenceCount;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** object) override
    {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient))
        {
            AddRef();
            *object = static_cast<IMMNotificationClient *>(this);
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }

    // IMMNotificationClient
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR deviceId, DWORD newState) override
    {
        Q_UNUSED(deviceId)
        Q_UNUSED(newState)
        _watcher->notify();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR deviceId) override
    {
        Q_UNUSED(deviceId)
        _watcher->notify();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR deviceId) override
    {
        Q_UNUSED(deviceId)
        _watcher->notify();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR defaultDeviceId) override
    {
        Q_UNUSED(defaultDeviceId)

        // Same default output as RtAudio
        if (flow == eRender && role == eConsole)
            _watcher->notify();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR deviceId, const PROPERTYKEY key) override
    {
        Q_UNUSED(deviceId)
        Q_UNUSED(key)
        return S_OK;
    }

private:
    LONG _referenceCount;
    AudioDeviceWatcher * _watcher;
};
#endif

AudioDeviceWatcher::AudioDeviceWatcher(QObject * parent) : QObject(parent)
#if defined(Q_OS_WIN)
  , _client(nullptr),
    _enumerator(nullptr),
    _coInitialized(false)
#endif
{
#if defined(Q_OS_MACOS)
    for (const AudioObjectPropertyAddress &address : s_watchedProperties)
        AudioObjectAddPropertyListener(kAudioObjectSystemObject, &address, propertyListener, this);
#elif defined(Q_OS_WIN)
    // COM may already be initialized in this thread (by Qt)
    _coInitialized = SUCCEEDED(CoInitialize(nullptr));

    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&_enumerator))))
    {
        _enumerator = nullptr;
        return;
    }

    _client = new NotificationClient(this);
    if (FAILED(_enumerator->RegisterEndpointNotificationCallback(_client)))
    {
        _client->Release();
        _client = nullptr;
    }
#endif
}

AudioDeviceWatcher::~AudioDeviceWatcher()
{
#if defined(Q_OS_MACOS)
    for (const AudioObjectPropertyAddress &address : s_watchedProperties)
        AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &address, propertyListener, this);
#elif defined(Q_OS_WIN)
    if (_client != nullptr)
    {
        _enumerator->UnregisterEndpointNotificationCallback(_client);
        _client->Release();
    }
    if (_enumerator != nullptr)
        _enumerator->Release();
    if (_coInitialized)
        CoUninitialize();
#endif
}

void AudioDeviceWatcher::notify()
{
    QMetaObject::invokeMethod(this, [this]() {
        emit devicesChanged();
    }, Qt::QueuedConnection);
}
