/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "circularbuffer.h"

CircularBuffer::CircularBuffer(int minBuffer, int maxBuffer) : QObject(NULL),
    _minBuffer(minBuffer),
    _maxBuffer(maxBuffer),
    _bufferSize(4 * maxBuffer),
    _posEcriture(0),
    _posLecture(0),
    _currentLengthAvailable(0),
    _interrupted(0)
{

    // Initialisation des buffers
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
    _interrupted = 1;
    _mutexSynchro.tryLock();
    _mutexSynchro.unlock();
}

void CircularBuffer::start()
{
    int avance = _maxBuffer - _minBuffer;

    // Surveillance du buffer après chaque lecture
    while (_interrupted == 0)
    {
        // Génération de données
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
void CircularBuffer::writeData(const float *dataL, const float *dataR, float *dataRevL, float *dataRevR, int &len)
{
    int total = 0;
    while (len - total > 0)
    {
        const int chunk = qMin(_bufferSize - _posEcriture, len - total);
        memcpy(&_dataL   [_posEcriture], &dataL   [total], 4 * chunk);
        memcpy(&_dataR   [_posEcriture], &dataR   [total], 4 * chunk);
        memcpy(&_dataRevL[_posEcriture], &dataRevL[total], 4 * chunk);
        memcpy(&_dataRevR[_posEcriture], &dataRevR[total], 4 * chunk);
        _posEcriture += chunk;
        if (_posEcriture >= _bufferSize)
            _posEcriture = 0;
        total += chunk;
    }

    // Quantité qu'il aurait fallu écrire (mise à jour pour la fois suivante)
    len = _maxBuffer - _currentLengthAvailable;
    if (len < _maxBuffer - _minBuffer)
        len = _maxBuffer - _minBuffer;

    // Mise à jour avance
    _currentLengthAvailable.fetchAndAddAcquire(total);
}

// Read data (audio thread)
void CircularBuffer::addData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, int maxlen)
{
    int writeLen = qMin(maxlen, (int)_currentLengthAvailable);
    int total = 0;
    while (writeLen - total > 0)
    {
        const int chunk = qMin((_bufferSize - _posLecture), writeLen - total);
        for (int i = 0; i < chunk; i++)
        {
            dataL   [total + i] += _dataL   [_posLecture + i];
            dataR   [total + i] += _dataR   [_posLecture + i];
            dataRevL[total + i] += _dataRevL[_posLecture + i];
            dataRevR[total + i] += _dataRevR[_posLecture + i];
        }
        _posLecture = (_posLecture + chunk) % _bufferSize;
        total += chunk;
    }
    for (int i = total; i < maxlen; i++)
        dataL[i] = dataR[i] = dataRevL[i] = dataRevR[i] = 0;

    if (_currentLengthAvailable.fetchAndAddAcquire(-total) - total <= _minBuffer)
    {
        _mutexSynchro.tryLock();
        _mutexSynchro.unlock();
    }
}
