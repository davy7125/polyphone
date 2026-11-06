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

#include "recorder.h"
#include <QDataStream>
#include <QFile>
#include <QThread>
#include <QtMath>
#include <cmath>

// Header of the file, sizes being at fixed positions
static const quint32 HEADER_SIZE = 46;
static const quint32 RIFF_SIZE_POSITION = 4;
static const quint32 DATA_SIZE_POSITION = 42;

// Maximal size of a wav file: the sizes in its header have 32 bits
static const quint32 MAX_DATA_SIZE = 0xFFFFFFFFU - HEADER_SIZE;

// Time the disk can take without losing data (seconds), period of the writings (ms) and of the header update (seconds)
static const quint32 RING_DURATION = 8;
static const quint32 WRITING_PERIOD = 20;
static const quint32 SIZES_UPDATE_PERIOD = 2;

// Time kept after the last sound when a silence stops a record (ms)
static const quint32 TAIL_DURATION = 200;

Recorder::Recorder() : QObject(),
    _ring(nullptr),
    _ringLength(0),
    _writePosition(0),
    _readPosition(0),
    _sampleRate(0),
    _channelCount(2),
    _isRecording(false),
    _isPaused(false),
    _isWaiting(false),
    _isRingLocked(false),
    _isOverflowed(false),
    _isSilenceReached(false),
    _isEndRequested(false),
    _threshold(0),
    _stopDuration(0),
    _soundEnd(0),
    _recordFile(nullptr),
    _writtenFrames(0),
    _arePeaksEnabled(false),
    _peakLeft(0),
    _peakRight(0)
{}

Recorder::~Recorder()
{
    this->endRecord();
}

bool Recorder::startNewRecord(QString fileName, quint32 sampleRate, int channelCount, bool isWaiting)
{
    this->endRecord();

    // Unbuffered: a failure is immediately known, and nothing is lost after a crash
    QFile * file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Unbuffered))
    {
        delete file;
        return false;
    }

    // Create header
    QDataStream stream(file);
    stream.setByteOrder(QDataStream::LittleEndian);
    quint32 dwTemp = 0;
    quint16 wTemp;

    // Write it
    stream.writeRawData("RIFF", 4);
    stream << static_cast<quint32>(HEADER_SIZE - 8);
    stream.writeRawData("WAVE", 4);

    ///////////// BLOC FMT /////////////
    stream.writeRawData("fmt ", 4);
    dwTemp = 18;
    stream << dwTemp;
    // Compression code
    wTemp = 3;
    stream << wTemp;
    // Number of channels
    wTemp = static_cast<quint16>(channelCount);
    stream << wTemp;
    // Sample rate
    dwTemp = sampleRate;
    stream << dwTemp;
    // Average byte per second
    dwTemp *= channelCount * sizeof(float);
    stream << dwTemp;
    // Block align
    wTemp = static_cast<quint16>(channelCount * sizeof(float));
    stream << wTemp;
    // Significants bits per smpl
    stream << static_cast<quint16>(32);
    // Extra format bytes
    wTemp = 0;
    stream << wTemp;

    ///////////// BLOC DATA /////////////
    stream.writeRawData("data", 4);
    stream << static_cast<quint32>(0);

    if (stream.status() != QDataStream::Ok)
    {
        file->remove();
        delete file;
        return false;
    }

    // Ring buffer, with a power of 2 for the length
    _ringLength = qNextPowerOfTwo(sampleRate * RING_DURATION);
    _ring = new float[channelCount * _ringLength];
    _writePosition.store(0);
    _readPosition.store(0);
    _sampleRate = sampleRate;
    _channelCount = channelCount;
    _soundEnd = 0;
    _isPaused.store(false);
    _isWaiting.store(isWaiting);
    _isOverflowed.store(false);
    _isSilenceReached.store(false);
    _isEndRequested.store(false);

    // The audio thread and the writing thread can start
    _recordFile = file;
    _writtenFrames = 0;
    _writingThread = std::thread(&Recorder::writeRing, this);
    _isRecording.store(true, std::memory_order_release);
    return true;
}

void Recorder::pause(bool isOn)
{
    _isPaused.store(isOn);
}

void Recorder::endRecord()
{
    if (!_writingThread.joinable())
        return;

    // The audio thread stops using the ring buffer, and remains out of it until the end of this function
    _isRecording.store(false);
    while (_isRingLocked.exchange(true, std::memory_order_acquire))
        QThread::msleep(1);

    // The rest of the ring buffer is written and the file is closed
    _isEndRequested.store(true, std::memory_order_release);
    _writingThread.join();
    delete _recordFile;
    _recordFile = nullptr;
    delete [] _ring;
    _ring = nullptr;

    _isRingLocked.store(false, std::memory_order_release);
}

void Recorder::setSoundDetection(float threshold, quint32 stopDuration)
{
    _threshold.store(threshold, std::memory_order_relaxed);
    _stopDuration.store(stopDuration, std::memory_order_relaxed);
}

void Recorder::setSampleRate(quint32 sampleRate)
{
    if (_writingThread.joinable() && sampleRate != _sampleRate)
    {
        this->endRecord();
        emit interrupted(STOP_SAMPLE_RATE_CHANGED);
    }
}

qint64 Recorder::getFileSize()
{
    return HEADER_SIZE + static_cast<qint64>(this->getFrameCount()) * _channelCount * sizeof(float);
}

void Recorder::takePeaks(float &left, float &right)
{
    left = _peakLeft.exchange(0, std::memory_order_relaxed);
    right = _peakRight.exchange(0, std::memory_order_relaxed);
}

void Recorder::updatePeak(std::atomic<float> &peak, float value)
{
    float current = peak.load(std::memory_order_relaxed);
    while (value > current && !peak.compare_exchange_weak(current, value, std::memory_order_relaxed)) {}
}

void Recorder::process(const float * dataL, const float * dataR, quint32 maxlen)
{
    // Peaks, even if nothing is recorded: the level can be checked before a record
    bool isRecording = _isRecording.load(std::memory_order_acquire);
    bool arePeaksEnabled = _arePeaksEnabled.load(std::memory_order_relaxed);
    float peakLeft = 0;
    float peakRight = 0;
    if (arePeaksEnabled || isRecording)
    {
        for (quint32 i = 0; i < maxlen; i++)
        {
            peakLeft = qMax(peakLeft, std::fabs(dataL[i]));
            if (dataR != nullptr)
                peakRight = qMax(peakRight, std::fabs(dataR[i]));
        }
        if (dataR == nullptr)
            peakRight = peakLeft;
    }
    if (arePeaksEnabled)
    {
        updatePeak(_peakLeft, peakLeft);
        updatePeak(_peakRight, peakRight);
    }

    // Nothing is waited for here: the ring buffer is skipped if a record is ending
    if (!isRecording || _isRingLocked.exchange(true, std::memory_order_acquire))
        return;

    if (_isRecording.load(std::memory_order_relaxed) && !_isPaused.load(std::memory_order_relaxed))
    {
        // The first sound starts a waiting record
        bool isSound = qMax(peakLeft, peakRight) >= _threshold.load(std::memory_order_relaxed);
        if (isSound)
            _isWaiting.store(false, std::memory_order_relaxed);
        if (!_isWaiting.load(std::memory_order_relaxed))
            this->push(dataL, dataR, maxlen, isSound);
    }

    _isRingLocked.store(false, std::memory_order_release);
}

void Recorder::push(const float * dataL, const float * dataR, quint32 maxlen, bool isSound)
{
    quint32 position = _writePosition.load(std::memory_order_relaxed);
    if (maxlen > _ringLength - (position - _readPosition.load(std::memory_order_acquire)))
    {
        // The disk is too slow: the record ends here, without holes
        _isRecording.store(false, std::memory_order_relaxed);
        _isOverflowed.store(true, std::memory_order_release);
        return;
    }

    // Interleave
    quint32 mask = _ringLength - 1;
    if (_channelCount == 2)
    {
        for (quint32 i = 0; i < maxlen; i++)
        {
            quint32 index = 2 * ((position + i) & mask);
            _ring[index] = dataL[i];
            _ring[index + 1] = dataR[i];
        }
    }
    else
    {
        for (quint32 i = 0; i < maxlen; i++)
            _ring[(position + i) & mask] = dataL[i];
    }
    position += maxlen;
    _writePosition.store(position, std::memory_order_release);

    // Silence after a sound
    if (isSound)
        _soundEnd = position;
    else
    {
        quint32 stopDuration = _stopDuration.load(std::memory_order_relaxed);
        if (stopDuration > 0 && _soundEnd > 0 && position - _soundEnd >= stopDuration * _sampleRate)
        {
            _isRecording.store(false, std::memory_order_relaxed);
            _isSilenceReached.store(true, std::memory_order_release);
        }
    }
}

void Recorder::writeRing()
{
    quint32 bytesPerFrame = _channelCount * sizeof(float);
    quint32 maxFrameCount = MAX_DATA_SIZE / bytesPerFrame;
    quint32 readPosition = 0;
    quint32 nextSizesUpdate = _sampleRate * SIZES_UPDATE_PERIOD;
    int stopReason = -1;
    bool isEnding = false;
    while (!isEnding && stopReason == -1)
    {
        // Read first: the frames received before the end, the overflow or the silence are all written
        isEnding = _isEndRequested.load(std::memory_order_acquire);
        bool isOverflowed = _isOverflowed.load(std::memory_order_acquire);
        bool isSilenceReached = _isSilenceReached.load(std::memory_order_acquire);

        quint32 count = _writePosition.load(std::memory_order_acquire) - readPosition;
        if (count > maxFrameCount - readPosition)
        {
            count = maxFrameCount - readPosition;
            stopReason = STOP_MAX_SIZE_REACHED;
        }

        // The frames to write possibly reach the end of the ring buffer
        quint32 start = readPosition & (_ringLength - 1);
        quint32 firstCount = qMin(count, _ringLength - start);
        if (!writeFrames(start, firstCount) || !writeFrames(0, count - firstCount))
            stopReason = STOP_WRITE_FAILED;
        readPosition += count;
        _readPosition.store(readPosition, std::memory_order_release);

        if (stopReason == -1 && isOverflowed)
            stopReason = STOP_DISK_TOO_SLOW;
        if (stopReason == -1 && isSilenceReached)
            stopReason = STOP_SILENCE;

        if (readPosition >= nextSizesUpdate)
        {
            writeSizes();
            nextSizesUpdate = readPosition + _sampleRate * SIZES_UPDATE_PERIOD;
        }

        if (!isEnding && stopReason == -1)
            QThread::msleep(WRITING_PERIOD);
    }

    // The silence that stopped the record is not kept
    if (stopReason == STOP_SILENCE)
    {
        _writtenFrames = qMin(_writtenFrames, _soundEnd + _sampleRate * TAIL_DURATION / 1000);
        _recordFile->resize(HEADER_SIZE + static_cast<qint64>(_writtenFrames) * bytesPerFrame);
        _writePosition.store(_writtenFrames, std::memory_order_relaxed);
    }

    writeSizes();
    _recordFile->close();

    if (stopReason != -1)
    {
        _isRecording.store(false, std::memory_order_relaxed);
        emit interrupted(stopReason);
    }
}

bool Recorder::writeFrames(quint32 position, quint32 count)
{
    if (count == 0)
        return true;

    // Only the complete frames are kept
    qint64 bytesPerFrame = _channelCount * sizeof(float);
    qint64 size = count * bytesPerFrame;
    qint64 written = _recordFile->write(reinterpret_cast<const char *>(&_ring[_channelCount * position]), size);
    if (written > 0)
        _writtenFrames += static_cast<quint32>(written / bytesPerFrame);
    return written == size;
}

void Recorder::writeSizes()
{
    quint32 dataSize = _writtenFrames * _channelCount * sizeof(float);
    qint64 position = HEADER_SIZE + static_cast<qint64>(dataSize);

    QDataStream stream(_recordFile);
    stream.setByteOrder(QDataStream::LittleEndian);
    _recordFile->seek(RIFF_SIZE_POSITION);
    stream << static_cast<quint32>(dataSize + HEADER_SIZE - 8);
    _recordFile->seek(DATA_SIZE_POSITION);
    stream << dataSize;
    _recordFile->seek(position);
}
