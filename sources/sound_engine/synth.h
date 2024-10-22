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

#ifndef SYNTH_H
#define SYNTH_H

#include "soundengine.h"
#include "calibrationsinus.h"
#include "liveeq.h"
#include "stk/FreeVerb.h"
#include <QDataStream>
class Soundfonts;
class Soundfont;
class InstPrst;

class SynthConfig
{
public:
    int choLevel;
    int choDepth;
    int choFrequency;
    int revLevel;
    int revSize;
    int revWidth;
    int revDamping;
    int gain;
    int tuningFork;
    QStringList temperament;
};

class Synth : public QObject
{
    Q_OBJECT

public:
    Synth(Soundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts);
    ~Synth() override;

    // Executed by the main thread (thread 1)
    void configure(SynthConfig * configuration);
    void setIMidiValues(IMidiValues * iMidiValues);
    int play(EltID id, int channel, int key, int velocity);
    void stop(bool allChannels);

    // Parameters for reading samples
    void setGainSample(int gain);
    void setStereo(bool isStereo);
    bool isStereo();
    void setStartLoop(quint32 startLoop, bool repercute);
    void setEndLoop(quint32 endLoop, bool repercute);
    void setLoopEnabled(bool isEnabled);
    void setSinus(bool isOn, int rootKey);
    void setPitchCorrection(qint16 correction, bool repercute);
    void activateSmplEq(bool isActivated);
    void setSmplEqValues(int values[10]);

    // Record
    void startNewRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);

    // Following functions are executed by the audio server (thread 2)
    void readData(float *dataL, float *dataR, quint32 maxlen);
    void setSampleRateAndBufferSize(quint32 sampleRate, quint32 bufferSize);

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);
    void dataWritten(quint32 sampleRate, quint32 number); // For updating the recorder

private:
    void playPrst(Soundfont * soundfont, InstPrst * prst, int channel, int key, int velocity);
    void playInst(Soundfont * soundfont, InstPrst * inst, int channel, int key, int velocity,
                  InstPrst * prst = nullptr, Division * prstDiv = nullptr);
    int playSmpl(Smpl * smpl, int channel, int key, int velocity,
                 InstPrst * inst = nullptr, Division * instDiv = nullptr, InstPrst * prst = nullptr, Division * prstDiv = nullptr);

    void destroySoundEnginesAndBuffers();
    void createSoundEnginesAndBuffers();

    CalibrationSinus _sinus;
    LiveEQ _eq;
    Soundfonts * _soundfonts;
    QRecursiveMutex * _mutexSoundfonts;

    // Sound engines
    QSemaphore _semRunningSoundEngines;
    SoundEngine ** _soundEngines;
    int _soundEngineCount;
    VoiceInitializer _voiceInitializers[MAX_NUMBER_OF_VOICES_TO_ADD];
    int _numberOfVoicesToAdd;
    static int s_sampleVoiceTokenCounter;

    // Global parameter
    quint32 _sampleRate;
    double _gain;

    // Effects
    int _choLevel, _choDepth, _choFrequency;
    stk::FreeVerb _reverb;
    bool _reverbOn;
    QMutex _mutexReverb;

    // Record management
    QFile * _recordFile;
    QDataStream _recordStream;
    volatile bool _isRecording;
    QAtomicInt _isWritingInStream;
    quint32 _recordLength;

    float * _dataWav;
    quint32 _bufferSize;
};


#endif // SYNTH_H
