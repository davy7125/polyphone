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

#include "midirecorder.h"
#include <QFile>
#include <QMutexLocker>
#include <QtEndian>
#include <cstring>

// Time base of the file: 960 ticks by quarter note at 120 beats per minute (about 0.5 ms by tick)
static const quint16 TICKS_PER_QUARTER = 960;
static const quint32 MICROSECONDS_PER_QUARTER = 500000;

MidiRecorder::MidiRecorder() :
    _pausedTime(0),
    _pauseStart(0),
    _lastEventTime(0),
    _noteCount(0),
    _heldKeyCount(0),
    _velocity(0),
    _isRecording(false),
    _isWaiting(false),
    _isPaused(false)
{
    memset(_heldKeys, 0, sizeof(_heldKeys));
}

bool MidiRecorder::startNewRecord(QString fileName, bool isWaiting)
{
    // The file is checked now, it is written at the end of the record
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    file.close();

    QMutexLocker locker(&_mutex);
    _fileName = fileName;
    _events.clear();
    memset(_heldKeys, 0, sizeof(_heldKeys));
    _pausedTime = 0;
    _lastEventTime = 0;
    _noteCount = 0;
    _heldKeyCount = 0;
    _isWaiting = isWaiting;
    _isPaused = false;
    _isRecording = true;
    _clock.start();
    return true;
}

qint64 MidiRecorder::getTime()
{
    return (_isPaused ? _pauseStart : _clock.nsecsElapsed() / 1000) - _pausedTime;
}

void MidiRecorder::pause(bool isOn)
{
    QMutexLocker locker(&_mutex);
    if (!_isRecording || isOn == _isPaused)
        return;

    if (isOn)
    {
        releaseKeys(getTime());
        _pauseStart = _clock.nsecsElapsed() / 1000;
    }
    else
        _pausedTime += _clock.nsecsElapsed() / 1000 - _pauseStart;
    _isPaused = isOn;
}

void MidiRecorder::releaseKeys(qint64 time)
{
    for (int channel = 0; channel < 16; channel++)
    {
        for (int key = 0; key < 128; key++)
        {
            if (_heldKeys[channel][key])
            {
                _heldKeys[channel][key] = false;
                _events << Event{time, static_cast<quint8>(0x80 | channel), static_cast<quint8>(key), 0};
                _lastEventTime = time;
            }
        }
    }
    _heldKeyCount = 0;
}

bool MidiRecorder::endRecord()
{
    QMutexLocker locker(&_mutex);
    if (!_isRecording)
        return true;

    if (!_isWaiting)
        releaseKeys(getTime());
    _isRecording = false;
    _isWaiting = false;
    if (_noteCount > 0)
        return writeFile();

    QFile::remove(_fileName);
    return true;
}

bool MidiRecorder::isWaiting()
{
    QMutexLocker locker(&_mutex);
    return _isWaiting;
}

qint64 MidiRecorder::getDuration()
{
    QMutexLocker locker(&_mutex);
    if (_isRecording)
        return _isWaiting ? 0 : getTime();
    return _lastEventTime;
}

int MidiRecorder::getNoteCount()
{
    QMutexLocker locker(&_mutex);
    return _noteCount;
}

qint64 MidiRecorder::getSilenceDuration()
{
    QMutexLocker locker(&_mutex);
    if (!_isRecording || _isWaiting || _isPaused || _heldKeyCount > 0 || _events.isEmpty())
        return 0;
    return getTime() - _lastEventTime;
}

int MidiRecorder::takeVelocity()
{
    QMutexLocker locker(&_mutex);
    int velocity = _velocity;
    _velocity = 0;
    return velocity;
}

void MidiRecorder::processMessage(quint8 status, quint8 data1, quint8 data2)
{
    quint8 type = status & 0xF0;
    bool isNoteOn = type == 0x90 && data2 > 0;
    bool isNoteOff = type == 0x80 || (type == 0x90 && data2 == 0);
    int channel = status & 0x0F;

    QMutexLocker locker(&_mutex);
    if (isNoteOn)
        _velocity = qMax(_velocity, static_cast<int>(data2));
    if (!_isRecording || _isPaused)
        return;

    // A key pressed before the record is ignored when it is released, and doesn't start the record
    if (isNoteOff && !_heldKeys[channel][data1 & 0x7F])
        return;

    if (_isWaiting)
    {
        _isWaiting = false;
        _pausedTime = 0;
        _clock.restart();
    }

    if (isNoteOn)
    {
        _noteCount++;
        if (!_heldKeys[channel][data1 & 0x7F])
            _heldKeyCount++;
        _heldKeys[channel][data1 & 0x7F] = true;
    }
    else if (isNoteOff)
    {
        _heldKeyCount--;
        _heldKeys[channel][data1 & 0x7F] = false;
        status = 0x80 | channel;
    }

    _lastEventTime = getTime();
    _events << Event{_lastEventTime, status, data1, data2};
}

static void appendVariableLength(QByteArray &data, quint32 value)
{
    // Groups of 7 bits, from the most significant one, the last one having its first bit at 0
    quint8 bytes[5];
    int count = 0;
    do
    {
        bytes[count++] = value & 0x7F;
        value >>= 7;
    } while (value > 0);
    while (count > 1)
        data.append(static_cast<char>(bytes[--count] | 0x80));
    data.append(static_cast<char>(bytes[0]));
}

bool MidiRecorder::writeFile()
{
    // Track: tempo, events then end of track
    QByteArray track;
    appendVariableLength(track, 0);
    track.append("\xFF\x51\x03", 3);
    track.append(static_cast<char>(MICROSECONDS_PER_QUARTER >> 16));
    track.append(static_cast<char>((MICROSECONDS_PER_QUARTER >> 8) & 0xFF));
    track.append(static_cast<char>(MICROSECONDS_PER_QUARTER & 0xFF));

    quint64 lastTick = 0;
    foreach (const Event &event, _events)
    {
        quint64 tick = static_cast<quint64>(event.time) * TICKS_PER_QUARTER / MICROSECONDS_PER_QUARTER;
        appendVariableLength(track, static_cast<quint32>(tick - lastTick));
        lastTick = tick;

        track.append(static_cast<char>(event.status));
        track.append(static_cast<char>(event.data1));
        quint8 type = event.status & 0xF0;
        if (type != 0xC0 && type != 0xD0) // Program and channel pressure have only one data byte
            track.append(static_cast<char>(event.data2));
    }
    appendVariableLength(track, 0);
    track.append("\xFF\x2F\x00", 3);

    // Header then track, the numbers being big endian
    QByteArray data;
    data.append("MThd\x00\x00\x00\x06\x00\x00\x00\x01", 12);
    quint16 division = qToBigEndian(TICKS_PER_QUARTER);
    data.append(reinterpret_cast<const char *>(&division), 2);
    data.append("MTrk", 4);
    quint32 trackSize = qToBigEndian(static_cast<quint32>(track.size()));
    data.append(reinterpret_cast<const char *>(&trackSize), 4);
    data.append(track);

    QFile file(_fileName);
    return file.open(QIODevice::WriteOnly) && file.write(data) == data.size();
}
