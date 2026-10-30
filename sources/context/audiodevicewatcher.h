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

#ifndef AUDIODEVICEWATCHER_H
#define AUDIODEVICEWATCHER_H

#include <QObject>

#ifdef Q_OS_WIN
struct IMMDeviceEnumerator;
#endif

// Notified by the system when an audio output is plugged or unplugged, or when the default output changes
// (MacOS and Windows only, the output "default" following the system on Linux)
class AudioDeviceWatcher : public QObject
{
    Q_OBJECT

public:
    AudioDeviceWatcher(QObject * parent = nullptr);
    ~AudioDeviceWatcher() override;

    // Can be called from any thread
    void notify();

signals:
    // Emitted in the thread of the watcher, possibly several times for a single change
    void devicesChanged();

#ifdef Q_OS_WIN
private:
    class NotificationClient;
    NotificationClient * _client;
    IMMDeviceEnumerator * _enumerator;
    bool _coInitialized;
#endif
};

#endif // AUDIODEVICEWATCHER_H
