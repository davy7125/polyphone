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

#include "midifile.h"
#include <QFile>
#include <cmath>
#include <cstring>
#include <deque>
#include <vector>
#include <algorithm>

static const qint64 MAX_FILE_SIZE = 64 * 1024 * 1024;

static quint32 readBigEndian32(const uchar * data)
{
    return (static_cast<quint32>(data[0]) << 24) | (static_cast<quint32>(data[1]) << 16) |
           (static_cast<quint32>(data[2]) << 8) | data[3];
}

static quint16 readBigEndian16(const uchar * data)
{
    return static_cast<quint16>((data[0] << 8) | data[1]);
}

static quint32 readLittleEndian32(const uchar * data)
{
    return (static_cast<quint32>(data[3]) << 24) | (static_cast<quint32>(data[2]) << 16) |
           (static_cast<quint32>(data[1]) << 8) | data[0];
}

// Variable-length quantity of at most 4 bytes, false if it is truncated or longer
static bool readVariableLength(const uchar * data, int &position, int end, quint32 &value)
{
    value = 0;
    for (int i = 0; i < 4; i++)
    {
        if (position >= end)
            return false;
        uchar byte = data[position++];
        value = (value << 7) | (byte & 0x7F);
        if (!(byte & 0x80))
            return true;
    }
    return false;
}

// Identifier of a chunk: 4 printable characters
static bool isChunkId(const uchar * data)
{
    for (int i = 0; i < 4; i++)
        if (data[i] < 0x20 || data[i] > 0x7E)
            return false;
    return true;
}

// Text of a meta event, until a possible null character, in UTF-8 if it is valid or in Latin-1
static QString readText(const uchar * data, int length)
{
    QByteArray bytes(reinterpret_cast<const char *>(data), length);
    int nullPosition = bytes.indexOf('\0');
    if (nullPosition >= 0)
        bytes.truncate(nullPosition);
    QString text = QString::fromUtf8(bytes);
    if (text.contains(QChar::ReplacementCharacter))
        text = QString::fromLatin1(bytes);
    return text.trimmed();
}

MidiFile::MidiFile() :
    _duration(0),
    _usedChannels(0),
    _lastTick(0),
    _ticksPerQuarter(1),
    _isSmpte(false)
{}

bool MidiFile::read(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        _error = tr("Cannot read file \"%1\".").arg(filePath);
        return false;
    }
    if (file.size() > MAX_FILE_SIZE)
    {
        _error = tr("This is not a MIDI file.");
        return false;
    }
    return read(file.readAll());
}

bool MidiFile::read(const QByteArray &data)
{
    _error.clear();
    _events.clear();
    _rawEvents.clear();
    _tempos.clear();
    _duration = 0;
    _usedChannels = 0;
    _lastTick = 0;
    for (int i = 0; i < 16; i++)
        _channelNames[i].clear();

    int position, size, format;
    if (!readHeader(data, position, size, format))
        return false;

    // All tracks until the end of the data, whatever the number of tracks written in the header
    const uchar * bytes = reinterpret_cast<const uchar *>(data.constData());
    quint64 firstTick = 0;
    while (position + 8 <= size)
    {
        qint64 declaredEnd = position + 8 + static_cast<qint64>(readBigEndian32(bytes + position + 4));
        if (memcmp(bytes + position, "MTrk", 4) == 0)
        {
            quint64 lastTick;
            int parsedEnd = readTrack(data, position + 8, declaredEnd, size, firstTick, lastTick);

            // With the format 2, the tracks are independent sequences played one after the other
            if (format == 2)
                firstTick = lastTick;

            position = getNextChunkPosition(data, size, declaredEnd, parsedEnd);
        }
        else if (isChunkId(bytes + position) && declaredEnd <= size)
        {
            // Other chunk (Yamaha XF for instance), skipped
            position = static_cast<int>(declaredEnd);
        }
        else
        {
            // Not a chunk: next track if any
            position = data.indexOf("MTrk", position + 1);
            if (position < 0)
                break;
        }
    }

    if (_usedChannels == 0)
    {
        _error = tr("This MIDI file contains no notes.");
        _rawEvents.clear();
        _tempos.clear();
        return false;
    }

    sortEvents();
    computeTimes();
    _rawEvents.clear();
    _tempos.clear();
    return true;
}

bool MidiFile::readHeader(const QByteArray &data, int &position, int &size, int &format)
{
    const uchar * bytes = reinterpret_cast<const uchar *>(data.constData());
    size = static_cast<int>(data.size());
    int start = 0;

    // RIFF-MIDI container: the standard MIDI file is in the chunk "data"
    if (size >= 12 && memcmp(bytes, "RIFF", 4) == 0 && memcmp(bytes + 8, "RMID", 4) == 0)
    {
        qint64 chunkPosition = 12;
        while (chunkPosition + 8 <= size)
        {
            quint32 length = readLittleEndian32(bytes + chunkPosition + 4);
            if (memcmp(bytes + chunkPosition, "data", 4) == 0)
            {
                start = static_cast<int>(chunkPosition) + 8;
                size = static_cast<int>(qMin(static_cast<qint64>(size), start + static_cast<qint64>(length)));
                break;
            }
            chunkPosition += 8 + static_cast<qint64>(length) + (length & 1); // Chunks aligned on 2 bytes
        }
    }

    // Header, possibly after other data (MacBinary header for instance)
    int headerPosition = static_cast<int>(data.indexOf("MThd", start));
    if (headerPosition < 0 || headerPosition + 14 > size)
    {
        _error = tr("This is not a MIDI file.");
        return false;
    }
    quint32 headerLength = readBigEndian32(bytes + headerPosition + 4);
    format = readBigEndian16(bytes + headerPosition + 8);
    quint16 division = readBigEndian16(bytes + headerPosition + 12);

    // Ticks per quarter note or, with a SMPTE division, per second (frames per second, negative and 29 meaning 29.97,
    // then ticks per frame)
    _isSmpte = (division & 0x8000) != 0;
    if (_isSmpte)
    {
        int framesPerSecond = -static_cast<qint8>(division >> 8);
        _ticksPerQuarter = (framesPerSecond == 29 ? 29.97 : framesPerSecond) * (division & 0xFF);
    }
    else
        _ticksPerQuarter = division;
    if (_ticksPerQuarter <= 0)
    {
        _error = tr("The time division of this MIDI file is not valid.");
        return false;
    }

    // First chunk after the header, whose length might be wrong
    qint64 next = headerPosition + 8 + static_cast<qint64>(headerLength);
    if (headerLength >= 6 && next + 8 <= size && isChunkId(bytes + next))
        position = static_cast<int>(next);
    else
        position = headerPosition + 14;
    return true;
}

int MidiFile::readTrack(const QByteArray &data, int start, qint64 declaredEnd, int size, quint64 firstTick, quint64 &lastTick)
{
    const uchar * bytes = reinterpret_cast<const uchar *>(data.constData());

    // The length is trusted if it reaches the end of the data or another chunk, otherwise the track is read until its end
    // event or the end of the data
    bool isLengthValid = declaredEnd == size || (declaredEnd + 8 <= size && isChunkId(bytes + declaredEnd));
    int end = isLengthValid ? static_cast<int>(declaredEnd) : size;

    quint64 tick = firstTick;
    lastTick = firstTick;
    quint8 runningStatus = 0;
    quint16 channels = 0; // Channels having notes in this track
    QString trackName, instrumentName;
    int position = start;
    while (position < end)
    {
        quint32 delta;
        if (!readVariableLength(bytes, position, end, delta) || position >= end)
            break;
        tick += delta;

        quint8 status = bytes[position];
        if (status < 0x80)
        {
            // Running status, kept after the meta and system exclusive events
            // Without it, the rest of the track cannot be read (the next delta-times would be wrong)
            if (runningStatus == 0)
                break;
            status = runningStatus;
        }
        else
            position++;

        if (status == 0xFF)
        {
            // Meta event
            quint32 length;
            if (position >= end)
                break;
            quint8 type = bytes[position++];
            if (!readVariableLength(bytes, position, end, length))
                break;
            int dataLength = static_cast<int>(qMin(static_cast<qint64>(length), static_cast<qint64>(end - position)));
            const uchar * metaData = bytes + position;
            position += dataLength;

            if (type == 0x2F)
            {
                // End of track, the time after the last event being part of the song if the track has notes (a track
                // without notes, with the tempo for instance, can end long after them)
                lastTick = qMax(lastTick, tick);
                if (channels != 0)
                    _lastTick = qMax(_lastTick, tick);
                break;
            }
            else if (type == 0x51 && dataLength >= 3)
            {
                // Tempo, possibly in any track
                Tempo tempo;
                tempo.tick = tick;
                tempo.microsecondsPerQuarter = (static_cast<quint32>(metaData[0]) << 16) | (metaData[1] << 8) | metaData[2];
                if (tempo.microsecondsPerQuarter > 0)
                    _tempos << tempo;
            }
            else if (type == 0x03 && trackName.isEmpty())
                trackName = readText(metaData, dataLength);
            else if (type == 0x04 && instrumentName.isEmpty())
                instrumentName = readText(metaData, dataLength);
        }
        else if (status == 0xF0 || status == 0xF7)
        {
            // System exclusive event (ignored), until the end of the message if the length is not valid
            quint32 length;
            if (!readVariableLength(bytes, position, end, length))
                break;
            if (position + static_cast<qint64>(length) <= end)
                position += static_cast<int>(length);
            else
            {
                int endOfExclusive = static_cast<int>(data.indexOf('\xF7', position));
                position = (endOfExclusive < 0 || endOfExclusive >= end) ? end : endOfExclusive + 1;
            }
        }
        else if (status > 0xF0)
        {
            // System common or real time message, not allowed in a file: skipped with its data
            position += (status == 0xF2) ? 2 : ((status == 0xF1 || status == 0xF3) ? 1 : 0);
        }
        else
        {
            // Channel message: 1 data byte for a program change or a channel pressure, 2 otherwise
            // The data bytes above 127 are limited (a velocity of 128 written by some software for instance)
            runningStatus = status;
            int dataCount = ((status & 0xF0) == 0xC0 || (status & 0xF0) == 0xD0) ? 1 : 2;
            if (position + dataCount > end)
                break;
            RawEvent event;
            event.tick = tick;
            event.order = 0;
            event.status = status;
            event.data1 = qMin(bytes[position], static_cast<uchar>(127));
            event.data2 = dataCount == 2 ? qMin(bytes[position + 1], static_cast<uchar>(127)) : 0;
            position += dataCount;

            if ((status & 0xF0) == 0x90)
            {
                if (event.data2 == 0)
                    event.status = 0x80 | (status & 0x0F); // Note off
                else
                    channels |= static_cast<quint16>(1 << (status & 0x0F));
            }
            _rawEvents << event;
            lastTick = qMax(lastTick, tick);
            _lastTick = qMax(_lastTick, tick);
        }
    }

    // Name of the channel if this track has the notes of only one channel
    if (channels != 0 && (channels & (channels - 1)) == 0)
    {
        int channel = 0;
        while (!(channels & (1 << channel)))
            channel++;
        if (_channelNames[channel].isEmpty())
            _channelNames[channel] = trackName.isEmpty() ? instrumentName : trackName;
    }
    _usedChannels |= channels;

    return position;
}

int MidiFile::getNextChunkPosition(const QByteArray &data, int size, qint64 declaredEnd, int parsedEnd)
{
    const uchar * bytes = reinterpret_cast<const uchar *>(data.constData());

    // After the length of the track if it reaches the end of the data or another chunk
    if (declaredEnd == size || (declaredEnd + 8 <= size && isChunkId(bytes + declaredEnd)))
        return static_cast<int>(declaredEnd);

    // Otherwise after the events read, or at the next track
    if (parsedEnd + 8 <= size && memcmp(bytes + parsedEnd, "MTrk", 4) == 0)
        return parsedEnd;
    int next = static_cast<int>(data.indexOf("MTrk", parsedEnd));
    return next < 0 ? size : next;
}

void MidiFile::sortEvents()
{
    // Events of all tracks by tick, in the order of the file
    std::stable_sort(_rawEvents.begin(), _rawEvents.end(), [](const RawEvent &a, const RawEvent &b) {
        return a.tick < b.tick;
    });

    // The notes of a key end in the order they started. A note off ending a note started earlier is moved before the
    // note on of the same key written before it at the same tick: otherwise it would stop the new note. The note off of
    // a note started at the same tick stays after its note on.
    std::vector<std::deque<quint64>> startTicks(16 * 128);
    std::vector<quint64> lastNoteOnTicks(16 * 128, static_cast<quint64>(-1));
    std::vector<qint64> lastNoteOnOrders(16 * 128, 0);
    for (int i = 0; i < _rawEvents.size(); i++)
    {
        RawEvent &event = _rawEvents[i];
        event.order = 2 * static_cast<qint64>(i);
        int type = event.status & 0xF0;
        if (type != 0x80 && type != 0x90)
            continue;

        int note = (event.status & 0x0F) * 128 + event.data1;
        std::deque<quint64> &noteStartTicks = startTicks[note];
        if (type == 0x90)
        {
            noteStartTicks.push_back(event.tick);
            if (lastNoteOnTicks[note] != event.tick)
            {
                lastNoteOnTicks[note] = event.tick;
                lastNoteOnOrders[note] = event.order;
            }
        }
        else if (!noteStartTicks.empty())
        {
            if (noteStartTicks.front() < event.tick && lastNoteOnTicks[note] == event.tick)
                event.order = lastNoteOnOrders[note] - 1;
            noteStartTicks.pop_front();
        }
    }
    std::stable_sort(_rawEvents.begin(), _rawEvents.end(), [](const RawEvent &a, const RawEvent &b) {
        return a.tick < b.tick || (a.tick == b.tick && a.order < b.order);
    });
}

void MidiFile::computeTimes()
{
    // Tempo changes of all tracks (or none with a SMPTE division), the default tempo being 120 bpm
    std::stable_sort(_tempos.begin(), _tempos.end(), [](const Tempo &a, const Tempo &b) {
        return a.tick < b.tick;
    });
    double microsecondsPerTick = (_isSmpte ? 1000000.0 : 500000.0) / _ticksPerQuarter;
    quint64 segmentTick = 0;
    double segmentTime = 0;
    int tempoIndex = 0;
    auto getTime = [&](quint64 tick) -> qint64 {
        // Ticks in ascending order
        while (!_isSmpte && tempoIndex < _tempos.size() && _tempos[tempoIndex].tick <= tick)
        {
            segmentTime += static_cast<double>(_tempos[tempoIndex].tick - segmentTick) * microsecondsPerTick;
            segmentTick = _tempos[tempoIndex].tick;
            microsecondsPerTick = _tempos[tempoIndex].microsecondsPerQuarter / _ticksPerQuarter;
            tempoIndex++;
        }
        return std::llround(segmentTime + static_cast<double>(tick - segmentTick) * microsecondsPerTick);
    };

    _events.resize(_rawEvents.size());
    for (int i = 0; i < _rawEvents.size(); i++)
    {
        const RawEvent &rawEvent = _rawEvents[i];
        Event &event = _events[i];
        event.time = getTime(rawEvent.tick);
        event.status = rawEvent.status;
        event.data1 = rawEvent.data1;
        event.data2 = rawEvent.data2;
    }
    _duration = getTime(_lastTick);
}
