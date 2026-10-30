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

#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <QCoreApplication>
#include <QString>
#include <QVector>
#include <QByteArray>

// Standard MIDI file (format 0, 1 or 2, possibly in a RIFF-MIDI container) read as a list of channel events sorted by
// time. The usual malformations are accepted:
// * data before the header (MacBinary for instance), unknown chunks, garbage between the tracks,
// * number of tracks of the header not matching the file, track lengths too short or too long, truncated file,
// * missing end of track, data bytes above 127, system messages that are not allowed,
// * running status kept after a meta or a system exclusive event, system exclusive event without a valid length,
// * tempo changes in any track, null tempo,
// * end of track long after the song in a track without notes (tempo track ending at 2^18 - 1 ticks for instance),
// * note off written after the note on of the next note of the same key at the same time.
// A track stops at data bytes without status. The system exclusive events are ignored, as well as the MIDI ports (all
// tracks play the same 16 channels).
class MidiFile
{
    Q_DECLARE_TR_FUNCTIONS(MidiFile)

public:
    struct Event
    {
        qint64 time; // Microseconds
        quint8 status; // Type of the channel message (note off, note on, ..., pitch bend) and channel
        quint8 data1;
        quint8 data2;
    };

    MidiFile();

    // Read a file, false in case of error (see getError)
    bool read(QString filePath);
    bool read(const QByteArray &data);
    QString getError() const { return _error; }

    // Channel events sorted by time, a note on with a velocity of 0 being a note off (status 0x80)
    const QVector<Event> &getEvents() const { return _events; }

    // Duration (microseconds): last event or end of a track having notes
    qint64 getDuration() const { return _duration; }

    // Channels having notes (0 to 15), and their name if a track has the notes of one channel
    bool isChannelUsed(int channel) const { return channel >= 0 && channel < 16 && (_usedChannels & (1 << channel)); }
    QString getChannelName(int channel) const { return channel >= 0 && channel < 16 ? _channelNames[channel] : ""; }

private:
    struct RawEvent
    {
        quint64 tick;
        qint64 order; // Order of the events of a same tick
        quint8 status;
        quint8 data1;
        quint8 data2;
    };

    struct Tempo
    {
        quint64 tick;
        quint32 microsecondsPerQuarter;
    };

    bool readHeader(const QByteArray &data, int &position, int &size, int &format);
    int readTrack(const QByteArray &data, int start, qint64 declaredEnd, int size, quint64 firstTick, quint64 &lastTick);
    int getNextChunkPosition(const QByteArray &data, int size, qint64 declaredEnd, int parsedEnd);
    void sortEvents();
    void computeTimes();

    QString _error;
    QVector<Event> _events;
    qint64 _duration;
    quint16 _usedChannels;
    QString _channelNames[16];

    // Data used while reading
    QVector<RawEvent> _rawEvents;
    QVector<Tempo> _tempos;
    quint64 _lastTick; // End of the song
    double _ticksPerQuarter; // Or, with a SMPTE division, per second
    bool _isSmpte;
};

#endif // MIDIFILE_H
