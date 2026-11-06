/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QString>
#include <atomic>
#include <thread>
class QFile;

// Records an audio stream (output of the synth or audio input) in a 32-bit float wav file. The audio thread only fills a
// ring buffer, emptied by a dedicated thread: a slow disk does not disturb the sound. The sizes in the header are
// regularly updated, a record being still readable after a crash.
// A record can wait for the first sound before starting, and stop by itself after a silence, which is then removed.
class Recorder: public QObject
{
    Q_OBJECT

public:
    // Reasons for stopping a record without being asked
    enum StopReason
    {
        STOP_SILENCE,
        STOP_WRITE_FAILED,
        STOP_DISK_TOO_SLOW,
        STOP_MAX_SIZE_REACHED,
        STOP_SAMPLE_RATE_CHANGED
    };

    Recorder();
    ~Recorder() override;

    // Executed by the main thread, false if the file cannot be written: 1 or 2 channels, the record possibly waiting for
    // the first sound
    bool startNewRecord(QString fileName, quint32 sampleRate, int channelCount, bool isWaiting);
    void pause(bool isOn);
    void endRecord();

    // Level from which there is a sound (linear value) and duration of the silence stopping a record that already has a
    // sound (seconds, 0 for no automatic stop)
    void setSoundDetection(float threshold, quint32 stopDuration);

    // A record stops if the sample rate of the audio output changes, a wav file having only one sample rate
    void setSampleRate(quint32 sampleRate);

    // True as long as a record is waiting for the first sound
    bool isWaiting() { return _isWaiting.load(std::memory_order_relaxed); }

    // Duration and size of the current or last record
    quint32 getRecordSampleRate() { return _sampleRate; }
    quint32 getFrameCount() { return _writePosition.load(std::memory_order_relaxed); }
    qint64 getFileSize();

    // Peaks of the audio output (linear values) since the last call, computed only if enabled
    void setPeaksEnabled(bool isEnabled) { _arePeaksEnabled.store(isEnabled, std::memory_order_relaxed); }
    void takePeaks(float &left, float &right);

    // Executed by the audio server, dataR being null with one channel
    void process(const float * dataL, const float * dataR, quint32 maxlen);

signals:
    // A record stopped by itself (StopReason), the file being complete up to this point: endRecord is still to be called
    void interrupted(int reason);

private:
    void push(const float * dataL, const float * dataR, quint32 maxlen, bool isSound);
    void writeRing();
    bool writeFrames(quint32 position, quint32 count);
    void writeSizes();
    static void updatePeak(std::atomic<float> &peak, float value);

    // Ring buffer of interleaved frames, the positions always increasing
    float * _ring;
    quint32 _ringLength;
    std::atomic<quint32> _writePosition;
    std::atomic<quint32> _readPosition;

    // State
    quint32 _sampleRate;
    int _channelCount;
    std::atomic<bool> _isRecording;
    std::atomic<bool> _isPaused;
    std::atomic<bool> _isWaiting;
    std::atomic<bool> _isRingLocked; // Released by the audio thread while it is not using the ring buffer
    std::atomic<bool> _isOverflowed;
    std::atomic<bool> _isSilenceReached;
    std::atomic<bool> _isEndRequested;

    // Sound detection
    std::atomic<float> _threshold;
    std::atomic<quint32> _stopDuration;
    quint32 _soundEnd; // Position after the last sound, 0 if there is none yet

    // Used by the thread writing the file
    std::thread _writingThread;
    QFile * _recordFile;
    quint32 _writtenFrames;

    // Peaks
    std::atomic<bool> _arePeaksEnabled;
    std::atomic<float> _peakLeft;
    std::atomic<float> _peakRight;
};

#endif // RECORDER_H
