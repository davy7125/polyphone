/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "circularbuffer.h"

CircularBuffer::CircularBuffer(quint32 minBuffer, quint32 maxBuffer) : QObject(nullptr),
    _minBuffer(minBuffer),
    _maxBuffer(maxBuffer),
    _bufferSize(4 * maxBuffer),
    _posEcriture(0),
    _posLecture(0),
    _currentLengthAvailable(0),
    _interrupted(0)
{

    // Buffer initialization
    _dataL       = new float [_bufferSize];
    _dataR       = new float [_bufferSize];
    _dataTmpL    = new float [_bufferSize];
    _dataTmpR    = new float [_bufferSize];
    _dataRevL    = new float [_bufferSize];
    _dataRevR    = new float [_bufferSize];
    _dataTmpRevL = new float [_bufferSize];
    _dataTmpRevR = new float [_bufferSize];
}

CircularBuffer::~CircularBuffer()
{
    delete [] _dataL;
    delete [] _dataR;
    delete [] _dataTmpL;
    delete [] _dataTmpR;
    delete [] _dataRevL;
    delete [] _dataRevR;
    delete [] _dataTmpRevL;
    delete [] _dataTmpRevR;
}

void CircularBuffer::stop()
{
    _interrupted.store(1);
    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

void CircularBuffer::start()
{
    quint32 avance = (_maxBuffer + _minBuffer) / 2;

    // Generate and copy data into the buffer after each reading
    while (_interrupted.load() == 0)
    {
        // Generate data
        _mutexBuffer.lock();
        generateData(_dataTmpL, _dataTmpR, _dataTmpRevL, _dataTmpRevR, avance);
        writeData(_dataTmpL, _dataTmpR, _dataTmpRevL, _dataTmpRevR, avance);
        _mutexBuffer.unlock();
        _mutexSynchro.lock();
    }

    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

// Sound engine thread => write data in the buffer
// "len" contains at the end the data length that should have been written to meet the buffer requirements
void CircularBuffer::writeData(const float *dataL, const float *dataR, float *dataRevL, float *dataRevR, quint32 &len)
{
    quint32 total = 0;
    while (total < len)
    {
        const quint32 chunk = qMin(_bufferSize - _posEcriture, len - total);
        memcpy(&_dataL   [_posEcriture], &dataL   [total], 4 * chunk);
        memcpy(&_dataR   [_posEcriture], &dataR   [total], 4 * chunk);
        memcpy(&_dataRevL[_posEcriture], &dataRevL[total], 4 * chunk);
        memcpy(&_dataRevR[_posEcriture], &dataRevR[total], 4 * chunk);
        _posEcriture = (_posEcriture + chunk) % _bufferSize;
        total += chunk;
    }

    // Update the quantity of data to read
    if (_currentLengthAvailable > _maxBuffer)
        len = _minBuffer; // Minimum to read
    else
        len = _maxBuffer - _currentLengthAvailable;

    // Update the current amount of data available
    _currentLengthAvailable.fetchAndAddAcquire(total);
}

// Read data (audio thread)
void CircularBuffer::addData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, quint32 maxlen)
{
    quint32 readLen = qMin(maxlen, _currentLengthAvailable.load());
    quint32 total = 0;
    while (total < readLen)
    {
        const quint32 chunk = qMin(_bufferSize - _posLecture, readLen - total);
        for (quint32 i = 0; i < chunk; i++)
        {
            dataL   [total + i] += _dataL   [_posLecture + i];
            dataR   [total + i] += _dataR   [_posLecture + i];
            dataRevL[total + i] += _dataRevL[_posLecture + i];
            dataRevR[total + i] += _dataRevR[_posLecture + i];
        }
        _posLecture = (_posLecture + chunk) % _bufferSize;
        total += chunk;
    }

    // Possibly trigger data generation
    if (_currentLengthAvailable.fetchAndSubAcquire(total) - total <= _minBuffer)
    {
        _mutexSynchro.tryLock();
        _mutexSynchro.unlock();
    }
}
