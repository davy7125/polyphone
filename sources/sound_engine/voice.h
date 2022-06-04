/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#ifndef VOICE_H
#define VOICE_H

#include <QMutex>
#include "sound.h"
#include "enveloppevol.h"
#include "osctriangle.h"
#include "stk/Chorus.h"
#include "stk/FreeVerb.h"

class Voice : public QObject
{
    Q_OBJECT

public:
    // Initial key is:
    // * -1 when we use "play" for reading a sample
    // * -2 when we want to read the stereo part of a sample, with "play"
    // >= 0 otherwise (sample, instrument or preset level)
    Voice(QVector<float> baData, quint32 smplRate, quint32 audioSmplRate, VoiceParam *voiceParam, int token);
    ~Voice();

    int getChannel() { return _voiceParam->getChannel(); }
    int getSf2Id() { return _voiceParam->getSf2Id(); }
    int getPresetId() { return _voiceParam->getPresetId(); }
    int getKey() { return _voiceParam->getKey(); }
    int getToken() { return _token; }
    void release(bool quick = false);
    void setGain(double gain);
    void setChorus(int level, int depth, int frequency);
    bool isFinished() { return _isFinished; }

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

    static void prepareSincTable();

signals:
    void currentPosChanged(quint32 pos);

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
    VoiceParam * _voiceParam;
    int _token;

    // Sample playback
    quint32 _currentSmplPos;
    double _time;
    bool _release;
    quint32 _delayEnd;
    bool _isFinished;
    bool _isRunning;

    // Save state for resampling
    float _lastFraction;
    qint32 _valPrec3, _valPrec2, _valPrec1;
    float _firstVal[3];

    // Save state for low pass filter
    float _x1, _x2, _y1, _y2;

    bool takeData(float * data, quint32 nbRead);
    void biQuadCoefficients(float &a0, float &a1, float &a2, float &b1, float &b2, float freq, float Q);

    // Protect parameters
    QMutex _mutexParam;

    // Arrays
    float * _dataModArray;
    float * _modLfoArray;
    float * _vibLfoArray;
    float * _modFreqArray;
    quint32 * _pointDistanceArray;
    quint32 _arrayLength;
    float * _srcData;
    quint32 _srcDataLength;

    static volatile int s_tuningFork;
    static volatile float s_temperament[12]; // Fine tune in cents from each key from C to B
    static volatile int s_temperamentRelativeKey;
    static float s_sinc_table7[256][7];
};

#endif // VOICE_H
