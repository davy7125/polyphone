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

#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include "voice.h"

class SoundEngine: public QObject
{
    Q_OBJECT

public:
    SoundEngine(QSemaphore * semRunning, quint32 bufferSize);
    virtual ~SoundEngine();
    static void setInstanceList(SoundEngine ** soundEngines, int count)
    {
        _listInstances = soundEngines;
        _instanceCount = count;
    }

    static void addVoice(Voice **voicesToAdd, int numberOfVoicesToAdd);
    static void stopAllVoices(bool allChannels);

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
    static bool isStereo() { return _isStereo; }
    static void setGainSample(int gain);

    // Data generation
    void stop();
    void prepareData(quint32 len);
    void addRevData(float * dataL, float * dataR, quint32 len);
    void addNonRevData(float * dataL, float * dataR, quint32 len);

public slots:
    void start();

signals:
    void readFinished(int token);

private:
    static void closeAll(int channel, int exclusiveClass, int numPreset);

    void closeAllInstance(int channel, int exclusiveClass, int numPreset);
    void addVoiceInstance(Voice ** voicesToAdd, int numberOfVoicesToAdd);
    void stopAllVoicesInstance(bool allChannels);
    void releaseVoicesInstance(int sf2Id, int presetId, int channel, int key);
    void setGainInstance(double gain);
    void setChorusInstance(int level, int depth, int frequency);
    void setPitchCorrectionInstance(qint16 correction, bool repercute);
    void setStartLoopInstance(quint32 startLoop, bool repercute);
    void setEndLoopInstance(quint32 endLoop, bool repercute);
    void setLoopEnabledInstance(bool isEnabled);
    void setStereoInstance(bool isStereo);
    void setGainSampleInstance(int gain);
    void generateData();

    QAtomicInt _interrupted;
    QSemaphore * _semRunning;
    QMutex _mutexVoices, _mutexSynchro;
    Voice ** _voices;
    int _numberOfVoices;
    static const int MAX_NUMBER_OF_VOICES;
    float * _dataL, * _dataR, * _dataRevL, * _dataRevR;
    float * _dataTmpL, * _dataTmpR;
    volatile quint32 _lenToPrepare;

    static int _gainSmpl;
    static bool _isStereo, _isLoopEnabled;
    static SoundEngine** _listInstances;
    static int _instanceCount;
};

#endif // SOUNDENGINE_H
