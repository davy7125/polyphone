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
    // Type is:
    // * 1 for a sample
    // * 2 for its stereo part
    // * 0 otherwise (instrument / preset)
    void initialize(InstPrst * prst, Division * prstDiv, InstPrst * inst, Division * instDiv, Smpl * smpl,
                    qint8 channel, qint8 key, qint8 vel, qint8 type, quint32 audioSmplRate, int token);

    // Get the parameter object, for setting or retrieving values
    VoiceParam * getParam() { return _voiceParam; }

    void release(bool quick = false);
    void finish() { _isFinished = true; }
    bool isInRelease() { return _release; }
    bool isFinished() { return _isFinished; }
    void triggerReadFinishedSignal();

    void setGain(float gain);

    // Generate data (pan, chorus and reverb not done)
    void generateData(float *data, quint32 len);

    // Configuration
    static void setReferencePitch(int referencePitch);
    static void setTemperament(float temperament[12], int relativeKey);

    // Maths
    static void prepareTables();

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);

private:
    inline float multiply(const float * __restrict coeffs, const float * __restrict srcData)
    {
        coeffs = (float*)__builtin_assume_aligned(coeffs, 32);
        srcData = (float*)__builtin_assume_aligned(srcData, 32);
        return coeffs[0] * srcData[0] +
               coeffs[1] * srcData[1] +
               coeffs[2] * srcData[2] +
               coeffs[3] * srcData[3] +
               coeffs[4] * srcData[4] +
               coeffs[5] * srcData[5] +
               coeffs[6] * srcData[6];
    }

    // Oscillators and envelopes
    OscTriangle _modLFO, _vibLFO;
    EnveloppeVol _volEnvelope, _modEnvelope;

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
    bool _isFinished;
    bool _isRunning;

    // Saved state for resampling
    float _srcData[7];
    quint32 _lastDistanceFraction;
    qint32 _moreAvailable;

    // Save state for the low pass filter
    float _x1, _x2, _y1, _y2;

    // Evolving volume coefficient
    float _volumeCoeff;

    float * getData(quint32 goOn, quint32 loopStart, quint32 loopEnd);
    float * getDataWithLoop(quint32 goOn, quint32 loopStart, quint32 loopEnd);
    bool biQuadCoefficients(float *coeffs, float freq, float inv_Q);

    // Arrays
    quint32 _arrayLength;
    float * _dataVolArray;
    float * _dataModArray;
    float * _modLfoArray;
    float * _vibLfoArray;

    static volatile int s_referencePitch;
    static volatile float s_temperament[12]; // Fine tune in cents from each key from C to B
    static volatile int s_temperamentRelativeKey;
    static float s_sinc_table7[2048][7];
};

#endif // VOICE_H
