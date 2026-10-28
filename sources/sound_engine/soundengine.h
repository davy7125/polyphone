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
#include <atomic>
#include "synthbuses.h"
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

    // True when the data generation is over (offline rendering)
    bool isIdle();

    // Get the result
    void copyData(SynthBuses &buses, quint32 len);
    void addData(SynthBuses &buses, quint32 len);

    // Time spent computing the voices since the last preparation, and number of voices times the number of chunks
    void getComputationTime(double &seconds, uint32_t &voiceChunkCount);

public slots:
    void start();

private:
    void generateData(quint32 len);

    SynthBuses _buses;

    volatile bool _interrupted;
    QAtomicInt _runningState; // 0: ready (waiting), 1: running, 2: pause required, 3: pause and continue
    QMutex _mutexSynchro;
    volatile quint32 _lenToPrepare;
    std::atomic<int64_t> _computationTime; // ns
    std::atomic<uint32_t> _computedVoiceChunkCount;

    static VoiceList * s_voices;
};

#endif // SOUNDENGINE_H
