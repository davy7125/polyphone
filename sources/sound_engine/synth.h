/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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
    // Type 0 is sample, 1 is instrument, 2 is preset
    void play(int type, int idSf2, int idElt, int note, int velocity,
              VoiceParam * voiceParamTmp = NULL);
    void stop();
    void setGain(double gain);

    // Parameters for reading samples
    void setGainSample(int gain);
    void setStereo(bool isStereo);
    void setStartLoop(int startLoop, bool repercute);
    void setEndLoop(int endLoop, bool repercute);
    void setLoopEnabled(bool isEnabled);
    void setSinus(bool isOn, int rootKey);
    void setPitchCorrection(int correction, bool repercute);

    // Record
    void startNewRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);

    // Following functions are executed by the audio server (thread 2)
    void readData(float *data1, float *data2, qint64 maxlen);
    void setFormat(AudioFormat format);

signals:
    void currentPosChanged(int pos);
    void readFinished();
    void sampleRateChanged(qint32 sampleRate);
    void dataWritten(qint32 sampleRate, int number);

public slots:
    void updateConfiguration();

private:
    void play_sub(int type, int idSf2, int idElt, int note, int velocity,
                  VoiceParam * voiceParamTmp = NULL);
    void destroySoundEnginesAndBuffers();
    void createSoundEnginesAndBuffers();

    void clip(float *data1, float *data2, qint64 size)
    {
        // Recherche valeur maxi
        float dMax = 0;
        qint32 pos = -1;
        float dTmp;
        for (qint32 i = 0; i < size; i++)
        {
            dTmp = qAbs(data1[i]);
            if (dTmp > dMax)
            {
                dMax = dTmp;
                pos = i;
            }
            dTmp = qAbs(data2[i]);
            if (dTmp > dMax)
            {
                dMax = dTmp;
                pos = i;
            }
        }

        if (dMax > .99)
        {
            float coef = .99 / dMax;
            for (int i = 0; i < pos; i++)
            {
                dTmp = (float)(pos - i) / pos * m_clipCoef
                        + (float)i / pos * coef;
                data1[i] *= dTmp;
                data2[i] *= dTmp;
            }
            m_clipCoef = coef;
            for (int i = pos; i < size; i++)
            {
                data1[i] *= coef;
                data2[i] *= coef;
            }
        }
    }

    CalibrationSinus _sinus;

    // Pointeur vers les données
    SoundfontManager * _sf2;

    // Liste des sound engines, voix temporaires (pour exclusive class)
    QList<SoundEngine *> _soundEngines;
    QList<Voice *> _listVoixTmp;

    // Format audio
    AudioFormat m_format;

    // Paramètre global
    double m_gain;

    // Effets
    int m_choLevel, m_choDepth, m_choFrequency;
    stk::FreeVerb _reverb;
    QMutex _mutexReverb, _mutexSynchro;

    // Etat clipping
    double m_clipCoef;

    // Gestion de l'enregistrement
    QFile * m_recordFile;
    QDataStream m_recordStream;
    bool m_isRecording;
    quint32 m_recordLength;
    QMutex m_mutexRecord;

    float * _fTmpSumRev1, * _fTmpSumRev2, * _dataWav;
    int _bufferSize;
};


#endif // SYNTH_H
