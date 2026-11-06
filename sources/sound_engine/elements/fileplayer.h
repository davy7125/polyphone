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

#ifndef FILEPLAYER_H
#define FILEPLAYER_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>
#include <sndfile.h>

// Plays an audio file of any size along with the output of the synth. A dedicated thread reads the file and adapts it to
// the sample rate of the output in a ring buffer, the audio thread only adding this buffer to the output: a slow disk
// does not disturb the sound.
class FilePlayer: public QObject
{
    Q_OBJECT

public:
    FilePlayer();
    ~FilePlayer() override;

    // Executed by the main thread: start a file for a given sample rate of the output, false if it cannot be read
    bool start(QString fileName, quint32 sampleRate);
    void stop();

    // A playback stops if the sample rate of the output changes
    void setSampleRate(quint32 sampleRate);

    // Executed by the audio server
    void process(float * dataL, float * dataR, quint32 len);

signals:
    // The playback ended by itself, stop is still to be called
    void finished();

private:
    void readFile();
    void waitForSpace(quint32 frameCount);
    void write(float left, float right);

    // Ring buffer of stereo frames, the positions always increasing
    float * _ring;
    std::atomic<quint32> _writePosition;
    std::atomic<quint32> _readPosition;

    std::atomic<bool> _isPlaying;
    std::atomic<bool> _isRingLocked; // Released by the audio thread while it is not using the ring buffer
    std::atomic<bool> _isStopRequested;

    // Used by the thread reading the file
    std::thread _readingThread;
    SNDFILE * _file;
    int _channelCount;
    double _step; // Frames of the file by frame of the output
    quint32 _sampleRate;
};

#endif // FILEPLAYER_H
