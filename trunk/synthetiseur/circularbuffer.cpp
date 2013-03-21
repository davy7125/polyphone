/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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

CircularBuffer::CircularBuffer(quint32 bufferSize, quint32 avanceBuffer, QObject *parent) :
    QIODevice(parent),
    m_data(NULL),
    m_data2(NULL),
    m_bufferSize(bufferSize),
    m_avance(avanceBuffer)
{
    this->reinit();
    if (m_bufferSize)
    {
        m_data = new char [m_bufferSize];
        m_data2 = new char [m_bufferSize];
        for (unsigned int i = 0; i < m_bufferSize; i++)
        {
            m_data[i] = 0;
            m_data2[i] = 0;
        }
    }
    this->open(QIODevice::ReadOnly);
}
CircularBuffer::~CircularBuffer()
{
    delete [] m_data;
    delete [] m_data2;
}

qint64 CircularBuffer::readData(char *data, qint64 maxlen)
{
    m_mutex.lock();
    qint64 writeLen = qMin(maxlen, m_currentBufferLength);
    qint64 total = 0;
    while (writeLen - total > 0)
    {
        const qint64 chunk = qMin((m_bufferSize - m_posLecture), writeLen - total);
        memcpy(&data[total], &m_data[m_posLecture], chunk);
        m_posLecture = (m_posLecture + chunk) % m_bufferSize;
        total += chunk;
    }
    m_currentBufferLength -= total;
    m_mutex.unlock();
    return total;
}

qint64 CircularBuffer::readData(char *data1, char *data2, qint64 maxlen)
{
    m_mutex.lock();
    qint64 writeLen = qMin(maxlen, m_currentBufferLength);
    qint64 total = 0;
    while (writeLen - total > 0)
    {
        const qint64 chunk = qMin((m_bufferSize - m_posLecture), writeLen - total);
        memcpy(&data1[total], &m_data[m_posLecture], chunk);
        memcpy(&data2[total], &m_data2[m_posLecture], chunk);
        m_posLecture = (m_posLecture + chunk) % m_bufferSize;
        total += chunk;
    }
    m_currentBufferLength -= total;
    m_mutex.unlock();
    return total;
}

qint64 CircularBuffer::writeData(const char *data, qint64 len)
{
    m_mutex.lock();
    qint64 total = 0;
    while (len - total > 0)
    {
        const qint64 chunk = qMin(m_bufferSize - m_posEcriture, len - total);
        memcpy(&m_data[m_posEcriture], &data[total], chunk);
        m_posEcriture += chunk;
        if (m_posEcriture >= m_bufferSize) m_posEcriture = 0;
        total += chunk;
    }
    m_currentBufferLength += len;
    m_mutex.unlock();
    return len;
}

qint64 CircularBuffer::writeData(const char *data1, const char *data2, qint64 len)
{
    m_mutex.lock();
    qint64 total = 0;
    while (len - total > 0)
    {
        const qint64 chunk = qMin(m_bufferSize - m_posEcriture, len - total);
        memcpy(&m_data[m_posEcriture], &data1[total], chunk);
        memcpy(&m_data2[m_posEcriture], &data2[total], chunk);
        m_posEcriture += chunk;
        if (m_posEcriture >= m_bufferSize) m_posEcriture = 0;
        total += chunk;
    }
    m_currentBufferLength += len;
    m_mutex.unlock();
    return len;
}

qint64 CircularBuffer::bytesAvailable()
{
    m_mutex.lock();
    qint64 valRet = m_currentBufferLength;
    m_mutex.unlock();
    return valRet;
}

qint64 CircularBuffer::dataNeeded()
{
    m_mutex.lock();
    qint64 valRet = qMax(m_avance - m_currentBufferLength, 0LL);
    m_mutex.unlock();
    return valRet;
}

void CircularBuffer::reinit()
{
    m_mutex.lock();
    m_posEcriture = 0;
    m_posLecture = 0;
    m_currentBufferLength = 0;
    this->close();
    this->open(QIODevice::ReadOnly);
    m_mutex.unlock();
}
