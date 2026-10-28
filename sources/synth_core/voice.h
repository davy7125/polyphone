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

#include "voiceparam.h"
#include "enveloppevol.h"
#include "osctriangle.h"
#include "isynthlistener.h"

class Voice
{
public:
    // The listener (possibly null) is notified when a sample is played alone
    Voice(VoiceParam * voiceParam, ISynthListener * listener = nullptr);
    ~Voice();

    // Initial key is:
    // * -1 when we use "play" for reading a sample
    // >= 0 otherwise (sample, instrument or preset level)
    // Type is:
    // * 1 for a sample
    // * 2 for its stereo part
    // * 0 otherwise (instrument / preset)
    void initialize(int sf2Id, ISynthInstPrst * prst, ISynthZone * prstZone, ISynthInstPrst * inst, ISynthZone * instZone,
                    ISynthSample * smpl, int8_t channel, int8_t key, int8_t vel, int8_t type, uint32_t audioSmplRate, int token);

    // Get the parameter object, for setting or retrieving values
    VoiceParam * getParam() { return _voiceParam; }

    void release(bool quick = false);
    void finish();
    bool isInRelease() { return _release || _loopMode == 2; }
    bool isFinished() { return _isFinished; }

    void setGain(float gain);

    // Generate data (pan, chorus and reverb not done)
    void generateData(float *data, uint32_t len);

    // Configuration
    static void setReferencePitch(int referencePitch);
    static void setTemperament(float temperament[12], int relativeKey);

    // Maths
    static void prepareTables();

private:
    ISynthListener * _listener;

    // Oscillators and envelopes
    OscTriangle _modLFO, _vibLFO;
    EnveloppeVol _volEnvelope, _modEnvelope;

    // Sound data and parameters
    int16_t * _dataSmpl16;
    uint8_t * _dataSmpl24;
    uint32_t _dataSmplLength;
    uint32_t _smplRate, _audioSmplRate;
    float _audioSmplRateInv;
    float _gain;
    VoiceParam * _voiceParam;
    int _token;
    char _loopMode;

    // Sample playback
    bool _release;
    bool _isFinished;
    unsigned char _sampleRootKey;
    uint32_t _currentSmplPos, _elapsedSmplPos;
    double _time;
    float (*_sincWindow)[RESAMPLING_ORDER];

    // Saved state for resampling
    int16_t _srcData16[RESAMPLING_ORDER];
    uint8_t _srcData24[RESAMPLING_ORDER];
    uint32_t _lastDistanceFraction;
    int32_t _moreAvailable;

    // Save state for the low pass filter
    float _x1, _x2, _y1, _y2;

    // Evolving volume coefficient
    float _volumeCoeff;

    // Arrays
    uint32_t _arrayLength;
    float * _dataVolArray;
    float * _dataModArray;
    float * _modLfoArray;
    float * _vibLfoArray;

    static volatile int s_referencePitch;
    static volatile float s_temperament[12]; // Fine tune in cents from each key from C to B
    static volatile int s_temperamentRelativeKey;
    static float s_sinc_tables[8][RESAMPLING_SUBDIVISION][RESAMPLING_ORDER];
    static float s_floatConversionCoef24;

    void getData(uint32_t goOn, uint32_t loopStart, uint32_t loopEnd, int16_t * &srcData16, uint8_t * &srcData24);
    void getDataWithLoop(uint32_t goOn, uint32_t loopStart, uint32_t loopEnd, int16_t * &srcData16, uint8_t * &srcData24);
    bool biQuadCoefficients(float *coeffs, float freq, float inv_Q);
    static int whichSincTable(unsigned char sampleRootKey, double shift);
};

#endif // VOICE_H
