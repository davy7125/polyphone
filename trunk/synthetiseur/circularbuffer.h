/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QMutex>
#include <QObject>

class CircularBuffer : public QObject
{
    Q_OBJECT

public:
    CircularBuffer(int minBuffer, int maxBuffer, QObject *parent = NULL);
    ~CircularBuffer();

    void addData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, int maxlen)
    {
        int total = 0;
        if (!_mutexData.tryLock())
            return;

        int writeLen = qMin(maxlen, _currentLengthAvailable);

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

        _currentLengthAvailable -= total;

        if (_currentLengthAvailable <= _minBuffer)
        {
            _mutexSynchro.tryLock();
            _mutexSynchro.unlock();
        }

        _mutexData.unlock();
    }

    void stop();
    bool isInterrupted();   // Interruption demandée
    bool isFinished();      // Boucle terminée

    QThread * getThread() { return _thread; }

public slots:
    void start()
    {
        _mutexData.lock();
        int avance = _maxBuffer - _minBuffer;
        _mutexData.unlock();

        // Surveillance du buffer après chaque lecture
        while (!isInterrupted())
        {
            // Génération de données
            generateData(_dataTmpL, _dataTmpR, _dataTmpRevL, _dataTmpRevR, avance);
            writeData(_dataTmpL, _dataTmpR, _dataTmpRevL, _dataTmpRevR, avance);
            _mutexSynchro.lock();
        }
        _mutexInterrupt.lock();
        _isFinished = true;
        _mutexInterrupt.unlock();
    }

protected:
    virtual void generateData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, int len) = 0;

private:
    void writeData(const float *dataL, const float *dataR, float *dataRevL, float *dataRevR, int &len)
    {
        int total = 0;
        _mutexData.lock();

        // Quantité qu'il aurait fallu écrire (mise à jour pour la fois suivante)
        int newLen = _maxBuffer - _currentLengthAvailable;
        if (newLen < _maxBuffer - _minBuffer)
            newLen = _maxBuffer - _minBuffer;

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
        _currentLengthAvailable += len;
        _mutexData.unlock();

        // Mise à jour avance
        len = newLen;
    }

    // Buffer et positions
    float * _dataL, * _dataR, * _dataRevL, * _dataRevR;
    float * _dataTmpL, * _dataTmpR, * _dataTmpRevL, * _dataTmpRevR;
    int _minBuffer, _maxBuffer, _maxTailleBuffer, _bufferSize;
    int _posEcriture, _posLecture;
    int _currentLengthAvailable;

    // Gestion interruption
    bool _interrupted, _isFinished;

    // Protection des ressources
    QMutex _mutexData, _mutexInterrupt, _mutexSynchro;

    // Thread du buffer
    QThread * _thread;
};

#endif // CIRCULARBUFFER_H
