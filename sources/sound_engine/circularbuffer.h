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

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QMutex>
#include <QObject>

class CircularBuffer : public QObject
{
    Q_OBJECT

public:
    CircularBuffer(quint32 minBuffer, quint32 maxBuffer);
    ~CircularBuffer();

    void addData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, quint32 maxlen);
    quint32 currentLengthAvailable() { return _currentLengthAvailable.load(); }
    void stop();

public slots:
    void start();

protected:
    virtual void generateData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, quint32 len) = 0;
    QMutex _mutexBuffer;

private:
    // Sound engine thread => write data in the buffer
    // "len" contains at the end the data length that should have been written to meet the buffer requirements
    void writeData(const float *dataL, const float *dataR, float *dataRevL, float *dataRevR, quint32 &len);

    // Buffer et positions
    float * _dataL, * _dataR, * _dataRevL, * _dataRevR;
    float * _dataTmpL, * _dataTmpR, * _dataTmpRevL, * _dataTmpRevR;
    const quint32 _minBuffer, _maxBuffer, _bufferSize;
    quint32 _posEcriture, _posLecture;
    QAtomicInteger<quint32> _currentLengthAvailable;

    // Gestion interruption
    QAtomicInt _interrupted;

    // Synchronisation
    QMutex _mutexSynchro;
};

#endif // CIRCULARBUFFER_H
