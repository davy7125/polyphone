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

#include "voice.h"
class Synth;

class SoundEngine: public QObject
{
    Q_OBJECT

public:
    SoundEngine(QSemaphore * semRunning, quint32 bufferSize);
    virtual ~SoundEngine() override;

    static void initialize(Synth *synth);
    static void addVoices(VoiceInitializer *voiceInitializers, int numberOfVoicesToAdd);
    static void stopAllVoices(bool allChannels);
    static void finalize();

    // sf2Id: -1 (no filter) or specific sf2 id
    // presetId: -1 (no filter) or specific preset id
    // channel: -2 (all channels), -1 (GUI channel) or [0 - 15] (MIDI channel)
    // key: -2 (all keys), -1 (all keys < 0) or a specific key
    static void releaseVoices(int sf2Id, int presetId, int channel, int key);

    // Configuration
    static void setGain(double gain);
    static void setChorus(int level, int depth, int frequency);
    static void setPitchCorrection(qint16 correction, bool repercute);
    static void setStartLoop(quint32 startLoop, bool repercute);
    static void setEndLoop(quint32 endLoop, bool repercute);
    static void setLoopEnabled(bool isEnabled);
    static void setStereo(bool isStereo);
    static bool isStereo() { return s_isStereo; }
    static void setGainSample(int gain);

    // Data generation
    void stop();
    static void prepareComputation();
    void prepareData(quint32 len);
    void generateData(quint32 len);
    static void endComputation();
    void addRevData(float * dataL, float * dataR, quint32 len);
    void addNonRevData(float * dataL, float * dataR, quint32 len);

public slots:
    void start();

private:
    static void closeAll(int channel, int exclusiveClass, int numPreset);
    static Voice * getNextVoiceToCompute();
    static void configureStereoVoice1(Voice * voice1);
    static void configureStereoVoice2(Voice * voice2);

    volatile bool _interrupted;
    QSemaphore * _semRunning;
    QMutex _mutexSynchro;
    float * _dataL, * _dataR, * _dataRevL, * _dataRevR;
    float * _dataTmpL, * _dataTmpR;
    volatile quint32 _lenToPrepare;

    static Voice * s_voices[MAX_NUMBER_OF_VOICES];
    static int s_numberOfVoices;
    static QAtomicInt s_indexVoice;
    static QMutex s_mutexVoices;

    static int s_gainSmpl;
    static bool s_isStereo, s_isLoopEnabled;
};

#endif // SOUNDENGINE_H
