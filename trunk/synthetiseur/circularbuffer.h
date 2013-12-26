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

#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QIODevice>
#include <QMutex>

class CircularBuffer : public QIODevice
{
    Q_OBJECT

public:
    CircularBuffer(quint32 bufferSize, quint32 avanceBuffer, QObject * parent = NULL);
    ~CircularBuffer();
    qint64 readData(char *data, qint64 maxlen);
    qint64 readData(char *data1, char *data2, qint64 maxlen);

protected:
    qint64 writeData(const char *data, qint64 len);
    qint64 writeData(const char *data1, const char *data2, qint64 len);
    using QIODevice::bytesAvailable;
    virtual qint64 bytesAvailable();
    qint64 dataNeeded();
    void reinit();
    virtual void generateData(qint64 nbData = 0) = 0;

private:
    // Buffer et positions
    char * m_data;
    char * m_data2;
    quint32 m_bufferSize;
    qint64 m_avance;
    qint64 m_posEcriture, m_posLecture;
    qint64 m_currentBufferLength;
    // Protection des ressources
    QMutex m_mutex;
};

#endif // CIRCULARBUFFER_H
