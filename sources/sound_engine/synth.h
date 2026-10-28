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

#include "basetypes.h"
#include "synthcore.h"
#include "soundengine.h"
#include "calibrationsinus.h"
#include "liveeq.h"
#include "imidilistener.h"
#include "recorder.h"

// Synth of the editor: SynthCore with the voices computed by several threads, the effects for
// the samples (calibration sinus, equalizer), the recording and the Qt signals
class Synth : public QObject, public IMidiListener, public ISynthListener
{
    Q_OBJECT

public:
    Synth(ISynthSoundfonts * soundfonts, QRecursiveMutex * mutexSoundfonts);
    ~Synth() override;

    // Executed by the main thread (thread 1)
    void configure(const SynthCoreConfig &configuration);
    void setIMidiValues(IMidiValues * midiValues); // Not deleted by the synth (null: default values)
    int play(EltID id, int channel, int key, int velocity);
    void stop(bool allChannels, bool fast);

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
    bool processProgramChanged(int channel, quint16 bank, quint8 preset) override;
    bool processRPNChanged(int channel, int parameter, int value, bool isRegistered, int trigger) override;

    // Following functions are executed by the audio server (thread 3)
    // Any length can be read: the voices are computed by whole chunks (COMPUTATION_CHUNK), the frames not asked being
    // returned first by the next call
    void setSampleRateAndBufferSize(quint32 sampleRate, quint32 bufferSize);
    void readData(float *dataL, float *dataR, quint32 len);

    // Polyphony, the voices being allocated for it (while the audio server is stopped), and real time (by default) or
    // offline rendering (see SynthCore)
    void setMaxPolyphony(int maxPolyphony);
    void setRealTime(bool isRealTime);

    // Offline rendering: wait until all voices of the next buffer are computed, before readData
    // (in real time, the voices not computed in time are skipped)
    void waitForComputation();

    // ISynthListener implementation (sound engine threads)
    void onSamplePositionChanged(uint32_t position) override;
    void onSampleReadFinished(int token) override;

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);
    void dataWritten(quint32 sampleRate, quint32 number); // For updating the recorder

private:
    void destroySoundEngines();
    void createSoundEngines();
    void appendOutput(quint32 len);

    // Soundfonts and synth
    QRecursiveMutex * _mutexSoundfonts;
    SynthCore _core;

    // Sound engines computing the voices
    SoundEngine ** _soundEngines;
    int _soundEngineCount;
    quint32 _bufferSize;

    // Copy of the sound engine data
    SynthBuses _buses;
    quint32 _preparedLength; // Frames being computed by the sound engines for the next call

    // Data after the effects, aligned as the buses: frames computed and not returned yet
    float * _outputL, * _outputR;
    quint32 _outputCapacity;
    quint32 _outputPosition;
    quint32 _outputAvailable;

    // Post-processing
    CalibrationSinus _sinus;
    LiveEQ _eq;
    Recorder _recorder;
};

#endif // SYNTH_H
