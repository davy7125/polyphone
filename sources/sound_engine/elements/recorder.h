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
#include <QFile>

class Recorder: public QObject
{
    Q_OBJECT

public:
    Recorder();
    ~Recorder();

    void setBufferSize(quint32 bufferSize);
    void startNewRecord(QString fileName, quint32 sampleRate);
    void pause(bool isOn);
    void endRecord();

    void process(float * dataL, float * dataR, quint32 maxlen);

signals:
    void dataWritten(quint32 sampleRate, quint32 number);

private:
    quint32 _sampleRate;
    QFile * _recordFile;
    QDataStream _recordStream;
    volatile bool _isRecording;
    QAtomicInt _isWritingInStream;
    quint32 _recordLength;
    float * _dataWav;
};

#endif // RECORDER_H
