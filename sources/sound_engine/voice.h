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

class Voice : public QObject
{
    Q_OBJECT

public:
    Voice(VoiceParam * voiceParam);
    ~Voice() override;

    // Initial key is:
    // * -1 when we use "play" for reading a sample
    // >= 0 otherwise (sample, instrument or preset level)
    void initialize(InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv, Smpl * smpl,
                    qint8 channel, qint8 key, qint8 vel, qint8 type, quint32 audioSmplRate, int token);

    // Get the parameter object, for setting or retrieving values
    VoiceParam * getParam() { return _voiceParam; }

    void release(bool quick = false);
    bool isInRelease() { return _release; }
    bool isFinished() { return _isFinished; }
    void triggerReadFinishedSignal();

    void setGain(float gain);

    // Generate data (pan, chorus and reverb not done)
    void generateData(float *data, quint32 len);

    // Configuration
    static void setTuningFork(int tuningFork);
    static void setTemperament(float temperament[12], int relativeKey);

    // Maths
    static void prepareTables();
    static float fastSin(float value); // Range [0; 1] for [0; pi]
    static float fastCos(float value); // Range [0; 1] for [0; pi]
    static float fastPow10(float value); // Range [-102.4; 102.3]

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);

private:
    // Oscillators and envelopes
    OscTriangle _modLFO, _vibLFO;
    EnveloppeVol _enveloppeVol, _enveloppeMod;

    // Sound data and parameters
    float * _dataSmpl;
    quint32 _dataSmplLength;
    quint32 _smplRate, _audioSmplRate;
    float _audioSmplRateInv;
    float _gain;
    VoiceParam * _voiceParam;
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
    quint32 _lastDistanceFraction;

    // Save state for low pass filter
    float _x1, _x2, _y1, _y2;

    bool takeData(float * data, quint32 nbRead, qint32 loopMode);
    void biQuadCoefficients(float &a0, float &a1, float &a2, float &b1, float &b2, float freq, float inv_Q);
    static float getSinValue(float value); // Range [0; 0.5] for [0; pi / 2]

    // Arrays
    quint32 _arrayLength;
    float * _dataModArray;
    float * _modLfoArray;
    float * _vibLfoArray;
    quint32 * _pointDistanceArray;

    quint32 _srcDataLength;
    float * _srcData;

    static volatile int s_tuningFork;
    static volatile float s_temperament[12]; // Fine tune in cents from each key from C to B
    static volatile int s_temperamentRelativeKey;
    static float s_sinc_table7[256][7];
    static float s_sin_table[2048];
    static float s_pow10_table[2048];
};

#endif // VOICE_H
