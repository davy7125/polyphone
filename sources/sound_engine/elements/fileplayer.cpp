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

#include "fileplayer.h"
#include <QThread>
#include <QVector>
#include <cmath>
#include <cstring>

// Length of the ring buffer (power of 2) and frames read at once: at most 24 times more frames are written, from a file
// at 8 kHz to an output at 192 kHz
static const quint32 RING_LENGTH = 1 << 17;
static const quint32 RING_MASK = RING_LENGTH - 1;
static const quint32 READ_LENGTH = 1024;

// Period of the thread when the buffer is full or when it empties at the end of a file (ms)
static const quint32 READING_PERIOD = 10;

FilePlayer::FilePlayer() : QObject(),
    _ring(new float[2 * RING_LENGTH]),
    _writePosition(0),
    _readPosition(0),
    _isPlaying(false),
    _isRingLocked(false),
    _isStopRequested(false),
    _file(nullptr),
    _channelCount(0),
    _step(1),
    _sampleRate(0)
{}

FilePlayer::~FilePlayer()
{
    this->stop();
    delete [] _ring;
}

bool FilePlayer::start(QString fileName, quint32 sampleRate)
{
    this->stop();

    SF_INFO info;
    memset(&info, 0, sizeof(SF_INFO));
    _file = sf_open(fileName.toStdString().c_str(), SFM_READ, &info);
    if (_file == nullptr || sampleRate == 0 || info.samplerate <= 0 || info.channels <= 0)
    {
        if (_file != nullptr)
            sf_close(_file);
        _file = nullptr;
        return false;
    }

    _channelCount = info.channels;
    _step = static_cast<double>(info.samplerate) / sampleRate;
    _sampleRate = sampleRate;
    _writePosition.store(0);
    _readPosition.store(0);
    _isStopRequested.store(false);

    // The audio thread and the reading thread can start
    _readingThread = std::thread(&FilePlayer::readFile, this);
    _isPlaying.store(true, std::memory_order_release);
    return true;
}

void FilePlayer::stop()
{
    if (!_readingThread.joinable())
        return;

    // The audio thread stops using the ring buffer, and remains out of it until the end of this function
    _isPlaying.store(false);
    while (_isRingLocked.exchange(true, std::memory_order_acquire))
        QThread::msleep(1);

    _isStopRequested.store(true);
    _readingThread.join();
    sf_close(_file);
    _file = nullptr;

    _isRingLocked.store(false, std::memory_order_release);
}

void FilePlayer::setSampleRate(quint32 sampleRate)
{
    if (_readingThread.joinable() && sampleRate != _sampleRate)
    {
        this->stop();
        emit finished();
    }
}

void FilePlayer::process(float * dataL, float * dataR, quint32 len)
{
    // Nothing is waited for here: the ring buffer is skipped if a playback is ending
    if (!_isPlaying.load(std::memory_order_acquire) || _isRingLocked.exchange(true, std::memory_order_acquire))
        return;

    if (_isPlaying.load(std::memory_order_relaxed))
    {
        quint32 position = _readPosition.load(std::memory_order_relaxed);
        quint32 count = qMin(len, _writePosition.load(std::memory_order_acquire) - position);
        for (quint32 i = 0; i < count; i++)
        {
            quint32 index = 2 * ((position + i) & RING_MASK);
            dataL[i] = qBound(-1.0f, dataL[i] + _ring[index], 1.0f);
            dataR[i] = qBound(-1.0f, dataR[i] + _ring[index + 1], 1.0f);
        }
        _readPosition.store(position + count, std::memory_order_release);
    }

    _isRingLocked.store(false, std::memory_order_release);
}

void FilePlayer::waitForSpace(quint32 frameCount)
{
    while (!_isStopRequested.load(std::memory_order_relaxed) &&
           RING_LENGTH - (_writePosition.load(std::memory_order_relaxed) - _readPosition.load(std::memory_order_acquire)) < frameCount)
        QThread::msleep(READING_PERIOD);
}

void FilePlayer::write(float left, float right)
{
    quint32 position = _writePosition.load(std::memory_order_relaxed);
    quint32 index = 2 * (position & RING_MASK);
    _ring[index] = left;
    _ring[index + 1] = right;
    _writePosition.store(position + 1, std::memory_order_release);
}

void FilePlayer::readFile()
{
    // Frames read with the last one of the previous reading first, the frames of the output being linearly interpolated
    // between them: the first channel for a mono file, the first two channels otherwise
    QVector<float> data(READ_LENGTH * _channelCount);
    QVector<float> left(READ_LENGTH + 1);
    QVector<float> right(READ_LENGTH + 1);
    double position = 1;
    int rightChannel = _channelCount >= 2 ? 1 : 0;
    while (!_isStopRequested.load(std::memory_order_relaxed))
    {
        sf_count_t count = sf_readf_float(_file, data.data(), READ_LENGTH);
        if (count <= 0)
            break;
        for (sf_count_t i = 0; i < count; i++)
        {
            left[i + 1] = data[i * _channelCount];
            right[i + 1] = data[i * _channelCount + rightChannel];
        }

        waitForSpace(static_cast<quint32>(std::ceil(count / _step)) + 2);
        if (_isStopRequested.load(std::memory_order_relaxed))
            break;

        while (position < count)
        {
            int index = static_cast<int>(position);
            float fraction = static_cast<float>(position - index);
            write(left[index] + fraction * (left[index + 1] - left[index]),
                  right[index] + fraction * (right[index + 1] - right[index]));
            position += _step;
        }
        position -= count;
        left[0] = left[count];
        right[0] = right[count];
    }

    // Last frame of the file
    if (position < 1)
    {
        waitForSpace(1);
        if (!_isStopRequested.load(std::memory_order_relaxed))
            write(left[0], right[0]);
    }

    // The end of the file is reached when the audio thread has read everything
    while (!_isStopRequested.load(std::memory_order_relaxed) &&
           _writePosition.load(std::memory_order_relaxed) != _readPosition.load(std::memory_order_acquire))
        QThread::msleep(READING_PERIOD);

    if (!_isStopRequested.load(std::memory_order_relaxed))
    {
        _isPlaying.store(false, std::memory_order_relaxed);
        emit finished();
    }
}
