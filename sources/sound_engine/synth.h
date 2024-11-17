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
#include "stk/Chorus.h"
#include <QDataStream>
#include "imidilistener.h"
#include "recorder.h"
class Soundfonts;
class Soundfont;
class InstPrst;

class SynthConfig
{
public:
    SynthConfig() :
        choLevel(0),
        choDepth(0),
        choFrequency(0),
        revLevel(0),
        revSize(0),
        revWidth(0),
        revDamping(0),
        masterGain(0),
        tuningFork(440),
        temperament(QStringList())
    {}

    void copy(SynthConfig * other)
    {
        choLevel = other->choLevel;
        choDepth = other->choDepth;
        choFrequency = other->choFrequency;
        revLevel = other->revLevel;
        revSize = other->revSize;
        revWidth = other->revWidth;
        revDamping = other->revDamping;
        masterGain = other->masterGain;
        tuningFork = other->tuningFork;
        temperament = other->temperament;
    }

    // Effects
    int choLevel;
    int choDepth;
    int choFrequency;

    int revLevel;
    int revSize;
    int revWidth;
    int revDamping;

    // Master gain / tuning
    double masterGain;
    int tuningFork;
    QStringList temperament;
};

class SynthInternalConfig
{
public:
    SynthInternalConfig() :
        reverbOn(false),
        chorusOn(false),
        sampleRate(44100),
        smplGain(0),
        smplIsStereoEnabled(false),
        smplIsLoopEnabled(true)
    {}

    // Effects
    bool reverbOn;
    bool chorusOn;

    // Global parameters
    quint32 sampleRate;

    // Sample parameters
    double smplGain;
    bool smplIsStereoEnabled;
    bool smplIsLoopEnabled;
};

class Synth : public QObject, public IMidiListener
{
    Q_OBJECT

public:
    Synth(Soundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts);
    ~Synth() override;

    // Executed by the main thread (thread 1)
    void configure(SynthConfig * configuration);
    void setIMidiValues(IMidiValues * midiValues);
    int play(EltID id, int channel, int key, int velocity);
    void stop(bool allChannels);

    // Configuration for reading samples
    void setSampleGain(double gain);
    void setStereo(bool isStereoEnabled);
    bool isStereoEnabled();
    void setLoopEnabled(bool isEnabled);
    void setSinus(bool isOn, int rootKey);
    void activateSmplEq(bool isActivated);
    void setSmplEqValues(int values[10]);

    // Editing parameters of the playing samples
    void setStartLoop(quint32 startLoop, bool withLinkedSample);
    void setEndLoop(quint32 endLoop, bool withLinkedSample);
    void setPitchCorrection(qint16 correction, bool withLinkedSample);

    // Record
    void startNewRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);

    // MIDI signals (thread 2)
    bool processKey(int channel, int key, int vel) override;
    bool processPolyPressureChanged(int channel, int key, int pressure) override;
    bool processMonoPressureChanged(int channel, int value) override;
    bool processControllerChanged(int channel, int num, int value) override;
    bool processBendChanged(int channel, float value) override;
    bool processBendSensitivityChanged(int channel, float semitones) override;

    // Following functions are executed by the audio server (thread 3)
    void setSampleRateAndBufferSize(quint32 sampleRate, quint32 bufferSize);
    void readData(float *dataL, float *dataR, quint32 maxlen);

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);
    void dataWritten(quint32 sampleRate, quint32 number); // For updating the recorder

private:
    void playPrst(Soundfont * soundfont, InstPrst * prst, int channel, int key, int velocity);
    void playInst(Soundfont * soundfont, InstPrst * inst, int channel, int key, int velocity,
                  InstPrst * prst = nullptr, Division * prstDiv = nullptr);
    int playSmpl(Smpl * smpl, int channel, int key, int velocity, bool other,
                 InstPrst * inst = nullptr, Division * instDiv = nullptr, InstPrst * prst = nullptr, Division * prstDiv = nullptr);

    void destroySoundEnginesAndBuffers();
    void createSoundEnginesAndBuffers();
    void gatherSoundEngineData(quint32 maxlen);
    void applyChoRev(float * dataL, float * dataR, quint32 maxlen);

    // Soundfonts
    Soundfonts * _soundfonts;
    QRecursiveMutex * _mutexSoundfonts;

    // Configuration
    SynthConfig _configuration;
    SynthInternalConfig _internalConfiguration;

    // Sound engines
    QSemaphore _semRunningSoundEngines;
    SoundEngine ** _soundEngines;
    int _soundEngineCount;
    VoiceInitializer _voiceInitializers[MAX_NUMBER_OF_VOICES_TO_ADD];
    int _numberOfVoicesToAdd;
    static int s_sampleVoiceTokenCounter;

    // Effects / post-processing
    CalibrationSinus _sinus;
    LiveEQ _eq;
    stk::FreeVerb _reverb;
    stk::Chorus _chorusRevL, _chorusRevR, _chorusL, _chorusR;
    QAtomicInt _effectsInUse;
    Recorder _recorder;

    quint32 _bufferSize;

    // Copy of the sound engine data
    float * _dataL, * _dataR, * _dataChoL, * _dataChoR,
          * _dataRevL, * _dataRevR, * _dataChoRevL, * _dataChoRevR;
};

#endif // SYNTH_H
