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
    m_avance(avanceBuffer)
{
    this->reinit();
    m_buffer.resize(bufferSize);
    m_buffer2.resize(bufferSize);
    this->open(QIODevice::ReadOnly);
}

qint64 CircularBuffer::readData(char *data, qint64 maxlen)
{
    m_mutex.lock();
    qint64 writeLen = qMin(maxlen, m_currentBufferLength);
    qint64 total = 0;
    while (writeLen - total > 0)
    {
        const qint64 chunk = qMin((m_buffer.size() - m_posLecture), writeLen - total);
        memcpy(data + total, m_buffer.constData() + m_posLecture, chunk);
        m_posLecture = (m_posLecture + chunk) % m_buffer.size();
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
        const qint64 chunk = qMin((m_buffer.size() - m_posLecture), writeLen - total);
        memcpy(data1 + total, m_buffer.constData() + m_posLecture, chunk);
        memcpy(data2 + total, m_buffer2.constData() + m_posLecture, chunk);
        m_posLecture = (m_posLecture + chunk) % m_buffer.size();
        total += chunk;
    }
    m_currentBufferLength -= total;
    m_mutex.unlock();
    return total;
}

qint64 CircularBuffer::writeData(const char *data, qint64 len)
{
    m_mutex.lock();
    if ((m_posEcriture + len) > m_buffer.size())
    {
        qint64 firstData = m_buffer.size() - m_posEcriture;
        memcpy(m_buffer.data() + m_posEcriture, data, firstData);
        memcpy(m_buffer.data(), data + firstData, len - firstData);
    }
    else
        memcpy(m_buffer.data() + m_posEcriture, data, len);
    m_posEcriture = (m_posEcriture + len) % m_buffer.size();
    m_currentBufferLength += len;
    m_mutex.unlock();
    return len;
}

qint64 CircularBuffer::writeData(const char *data1, const char *data2, qint64 len)
{
    m_mutex.lock();
    if ((m_posEcriture + len) > m_buffer.size())
    {
        qint64 firstData = m_buffer.size() - m_posEcriture;
        memcpy(m_buffer.data() + m_posEcriture, data1, firstData);
        memcpy(m_buffer.data(), data1 + firstData, len - firstData);
        memcpy(m_buffer2.data() + m_posEcriture, data2, firstData);
        memcpy(m_buffer2.data(), data2 + firstData, len - firstData);
    }
    else
    {
        memcpy(m_buffer.data() + m_posEcriture, data1, len);
        memcpy(m_buffer2.data() + m_posEcriture, data2, len);
    }
    m_posEcriture = (m_posEcriture + len) % m_buffer.size();
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
