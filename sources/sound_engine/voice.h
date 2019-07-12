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
#include "oscsinus.h"
#include "Chorus.h"
#include "FreeVerb.h"

class Voice : public QObject
{
    Q_OBJECT

public:
    // Initial key is:
    // * -1 when we use "play" for reading a sample
    // * -2 when we want to read the stereo part of a sample, with "play"
    // >= 0 otherwise (sample, instrument or preset level)
    Voice(QByteArray baData, quint32 smplRate, quint32 audioSmplRate, int initialKey, VoiceParam *voiceParam);
    ~Voice();

    int getKey() { return _initialKey; }
    void release(bool quick = false);
    void setGain(double gain);
    void setChorus(int level, int depth, int frequency);
    bool isFinished() { return _isFinished; }
    bool isRunning() { return _isRunning; }
    void runVoice(quint32 delay) { _isRunning = true; _delayStart = delay; }

    // Access to voiceParam properties
    double getPan();
    int getExclusiveClass();
    int getPresetNumber();
    float getReverb();

    // Modification des propriétés de voiceParam
    void setPan(double val);
    void setLoopMode(quint16 val);
    void setLoopStart(quint32 val);
    void setLoopEnd(quint32 val);
    void setFineTune(qint16 val);

    // Generate data
    void generateData(float *dataL, float *dataR, quint32 len);

signals:
    void currentPosChanged(quint32 pos);

private:
    // Oscillateurs, enveloppes et chorus
    OscSinus _modLFO, _vibLFO;
    EnveloppeVol _enveloppeVol, _enveloppeMod;
    stk::Chorus _chorus;
    int _chorusLevel;

    // Données son et paramètres
    QByteArray _baData;
    quint32 _smplRate, _audioSmplRate;
    double _gain;
    int _initialKey; // Only used to know which key triggered the sound, not for computing data
    VoiceParam * _voiceParam;

    // Lecture du sample
    quint32 _currentSmplPos;
    double _time;
    bool _release;
    quint32 _delayEnd, _delayStart;
    bool _isFinished;
    bool _isRunning;

    // Save state for resampling
    float _deltaPos;
    qint32 _valPrec, _valBase;

    // Save state for low pass filter
    double _x1, _x2, _y1, _y2;

    bool takeData(qint32 *data, quint32 nbRead);
    void biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2, double freq, double Q);

    // Protect parameters
    QMutex _mutexParam;
};

#endif // VOICE_H
