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

#ifndef MIDIRECORDER_H
#define MIDIRECORDER_H

#include <QElapsedTimer>
#include <QMutex>
#include <QString>
#include <QVector>

// Records the MIDI events played (MIDI input, virtual keyboard, ...) in a standard MIDI file of format 0. The events come
// before any processing: a key released while the sustain pedal is down is a note off followed later by the pedal up,
// as it has been played. The events have been received by another thread than the one controlling the record.
class MidiRecorder
{
public:
    MidiRecorder();

    // Start a record, possibly waiting for the first event. False if the file cannot be written.
    bool startNewRecord(QString fileName, bool isWaiting);

    // Pause: the time stops and the keys held are released in the file
    void pause(bool isOn);

    // End a record and write its file, false if it cannot be written. A record without notes is not kept.
    bool endRecord();

    // True as long as a record is waiting for the first event
    bool isWaiting();

    // Duration (microseconds) and number of notes of the current or last record
    qint64 getDuration();
    int getNoteCount();

    // Time since the last event (microseconds), 0 if a key is held or if nothing is recorded yet
    qint64 getSilenceDuration();

    // Highest velocity of the notes played since the last call, 0 if there is none
    int takeVelocity();

    // New event (status with a channel below 0xF0 and its data)
    void processMessage(quint8 status, quint8 data1, quint8 data2);

private:
    struct Event
    {
        qint64 time; // Microseconds
        quint8 status;
        quint8 data1;
        quint8 data2;
    };

    qint64 getTime();
    void releaseKeys(qint64 time);
    bool writeFile();

    QMutex _mutex;
    QString _fileName;
    QVector<Event> _events;
    QElapsedTimer _clock;
    qint64 _pausedTime; // Total time of the pauses
    qint64 _pauseStart;
    qint64 _lastEventTime;
    int _noteCount;
    int _heldKeyCount;
    int _velocity;
    bool _heldKeys[16][128];
    bool _isRecording;
    bool _isWaiting;
    bool _isPaused;
};

#endif // MIDIRECORDER_H
