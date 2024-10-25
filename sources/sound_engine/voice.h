/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#ifndef VOICE_H
#define VOICE_H

#include "instprst.h"
#include "voiceparam.h"
#include "enveloppevol.h"
#include "osctriangle.h"
#include "stk/Chorus.h"

class VoiceInitializer
{
public:
    InstPrst * prst;
    Division * prstDiv;
    InstPrst * inst;
    Division * instDiv;
    Smpl * smpl;

    int channel;
    int key;
    int vel;

    int choLevel;
    int choDepth;
    int choFrequency;
    float gain;

    quint32 audioSmplRate;
    int token;

    bool loopEnabled;
};

class Voice : public QObject
{
    Q_OBJECT

public:
    Voice();
    ~Voice() override;

    // Initial key is:
    // * -1 when we use "play" for reading a sample
    // * -2 when we want to read the stereo part of a sample, with "play"
    // >= 0 otherwise (sample, instrument or preset level)
    void initialize(VoiceInitializer * voiceInitializer);

    int getChannel() { return _voiceParam.getChannel(); }
    int getSf2Id() { return _voiceParam.getSf2Id(); }
    int getPresetId() { return _voiceParam.getPresetId(); }
    int getKey() { return _voiceParam.getKey(); }
    void release(bool quick = false);
    void setGain(double gain);
    void setChorus(int level, int depth, int frequency);
    bool isFinished() { return _isFinished; }
    void triggerReadFinishedSignal();

    // Access to voiceParam properties
    double getPan();
    int getExclusiveClass();
    int getPresetNumber();
    float getReverb();

    // Update voiceParam properties
    void setPan(double val);
    void setLoopMode(quint16 val);
    void setLoopStart(quint32 val);
    void setLoopEnd(quint32 val);
    void setFineTune(qint16 val);

    // Generate data
    void generateData(float *dataL, float *dataR, quint32 len);

    // Configuration
    static void setTuningFork(int tuningFork);
    static void setTemperament(float temperament[12], int relativeKey);

    static void prepareTables();

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);

private:
    // Oscillators, envelopes and chorus
    OscTriangle _modLFO, _vibLFO;
    EnveloppeVol _enveloppeVol, _enveloppeMod;
    stk::Chorus _chorus;
    int _chorusLevel;

    // Sound data and parameters
    QVector<float> _baData;
    quint32 _smplRate, _audioSmplRate;
    double _gain;
    VoiceParam _voiceParam;
    int _token;

    // Sample playback
    quint32 _currentSmplPos, _elapsedSmplPos;
    double _time;
    bool _release;
    quint32 _delayEnd;
    bool _isFinished;
    bool _isRunning;

    // Save state for resampling
    float _firstVal[6];

    // Save state for low pass filter
    float _x1, _x2, _y1, _y2;

    bool takeData(float * data, quint32 nbRead, qint32 loopMode);
    void biQuadCoefficients(float &a0, float &a1, float &a2, float &b1, float &b2, float freq, float Q);
    float fastSin(float value); // Range [0; 1] for [0; pi]
    float fastCos(float value); // Range [0; 1] for [0; pi]
    float getSinValue(float value); // Range [0; 0.5] for [0; pi / 2]

    // Arrays
    quint32 _arrayLength;
    float * _dataModArray;
    float * _modLfoArray;
    float * _vibLfoArray;
    float * _modFreqArray;
    quint32 * _pointDistanceArray;

    quint32 _srcDataLength;
    float * _srcData;

    static volatile int s_tuningFork;
    static volatile float s_temperament[12]; // Fine tune in cents from each key from C to B
    static volatile int s_temperamentRelativeKey;
    static float s_sinc_table7[256][7];
    static float s_sin_table[256];
};

#endif // VOICE_H
