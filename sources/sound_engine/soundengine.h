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

#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include <QMutex>
#include <QObject>
class VoiceList;

class SoundEngine: public QObject
{
    Q_OBJECT

public:
    SoundEngine(quint32 bufferSize);
    virtual ~SoundEngine() override;
    static void initialize(VoiceList * voices) { s_voices = voices; }

    // Data generation
    void stop();
    void prepareData(quint32 len);
    void endCurrentProcessing();

    // Get the result
    void setData(float * dataL, float * dataR, float * dataChoL, float * dataChoR,
                 float * dataRevL, float * dataRevR, float * dataChoRevL, float * dataChoRevR, quint32 len);
    void addData(float * dataL, float * dataR, float * dataChoL, float * dataChoR,
                 float * dataRevL, float * dataRevR, float * dataChoRevL, float * dataChoRevR, quint32 len);

public slots:
    void start();

private:
    void generateData(quint32 len);

    float * _voiceData;
    float * _dataL, * _dataR, * _dataRevL, * _dataRevR, * _dataChoL, * _dataChoR, * _dataChoRevL, * _dataChoRevR;

    volatile bool _interrupted;
    QAtomicInt _runningState; // 0: ready (waiting), 1: running, 2: pause required, 3: pause and continue
    QMutex _mutexSynchro;
    volatile quint32 _lenToPrepare;

    static VoiceList * s_voices;
};

#endif // SOUNDENGINE_H
