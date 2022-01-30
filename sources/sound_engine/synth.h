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

#ifndef SYNTH_H
#define SYNTH_H

#include "soundengine.h"
#include "audiodevice.h"
#include "calibrationsinus.h"
#include "liveeq.h"
#include <QDataStream>
class SoundfontManager;
class ConfManager;

class Synth : public QObject
{
    Q_OBJECT

public:
    Synth(ConfManager *configuration);
    ~Synth();

    // Executed by the main thread (thread 1)
    int play(EltID id, int key, int velocity);
    void stop();
    void setGain(double gain);

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
    void setFormat(AudioFormat format);

signals:
    void currentPosChanged(quint32 pos);
    void readFinished(int token);
    void sampleRateChanged(quint32 sampleRate);
    void dataWritten(quint32 sampleRate, quint32 number);

public slots:
    void updateConfiguration();

private:
    void playPrst(int idSf2, int idElt, int key, int velocity);
    void playInst(int idSf2, int idElt, int key, int velocity, EltID idPrstInst = EltID(elementUnknown));
    int playSmpl(int idSf2, int idElt, int key, int velocity,
                 EltID idInstSmpl = EltID(elementUnknown), EltID idPrstInst = EltID(elementUnknown));

    void destroySoundEnginesAndBuffers();
    void createSoundEnginesAndBuffers();

    CalibrationSinus _sinus;
    LiveEQ _eq;
    SoundfontManager * _sf2;

    // Liste des sound engines, voix temporaires (pour exclusive class)
    QList<SoundEngine *> _soundEngines;
    QList<Voice *> _listVoixTmp;
    static int s_sampleVoiceTokenCounter;

    // Audio format
    AudioFormat _format;

    // Global parameter
    double _gain;

    // Effects
    int _choLevel, _choDepth, _choFrequency;
    stk::FreeVerb _reverb;
    bool _reverbOn;
    QMutex _mutexReverb, _mutexSynchro;

    // Record management
    QFile * _recordFile;
    QDataStream _recordStream;
    bool _isRecording;
    quint32 _recordLength;
    QMutex _mutexRecord;

    float * _fTmpSumRevL, * _fTmpSumRevR, * _dataWav;
    quint32 _bufferSize;

    ConfManager * _configuration;
};


#endif // SYNTH_H
