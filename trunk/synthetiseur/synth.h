/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SYNTH_H
#define SYNTH_H

#include "pile_sf2.h"
#include "soundengine.h"
#include "audiodevice.h"
#include "calibrationsinus.h"

class Synth : public QObject
{
    Q_OBJECT

public:
    Synth(Pile_sf2 * sf2, int bufferSize);
    ~Synth();

    // Exécution par mainWindow (thread 1)
    void play(int type, int idSf2, int idElt, int note, int velocity,
              VoiceParam * voiceParamTmp = NULL);
    void play_sub(int type, int idSf2, int idElt, int note, int velocity,
                  VoiceParam * voiceParamTmp = NULL);
    void stop();
    void setGain(double gain);
    void setReverb(int level, int size, int width, int damping);
    void setChorus(int level, int depth, int frequency);
    void setBufferSize(int bufferSize);

    // Paramètres de lecture de samples
    void setGainSample(int gain);
    void setStereo(bool isStereo);
    void setStartLoop(int startLoop, bool repercute);
    void setEndLoop(int endLoop, bool repercute);
    void setLoopEnabled(bool isEnabled);
    void setRootKey(int rootKey);
    void setSinusEnabled(bool isEnabled);
    void setPitchCorrection(int correction, bool repercute);

    // Gestion de l'enregistrement
    void startNewRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);

    // Exécution par le serveur audio (thread 2)
    void readData(float *data1, float *data2, qint64 maxlen)
    {
        for (int i = 0; i < maxlen; i++)
            data1[i] = data2[i] = _fTmpSumRev1[i] = _fTmpSumRev2[i] = 0;

        // Fusion des sound engines
        _mutexSynchro.lock();
        for (int i = 0; i < _soundEngines.size(); i++)
            _soundEngines.at(i)->addData(data1, data2, _fTmpSumRev1, _fTmpSumRev2, maxlen);
        _mutexSynchro.unlock();

        // Application réverbération et addition
        _mutexReverb.lock();
        for (int i = 0; i < maxlen; i++)
        {
            data1[i] += _reverb.tick(_fTmpSumRev1[i], _fTmpSumRev2[i]);
            data2[i] += _reverb.lastOut(1);
        }
        _mutexReverb.unlock();

        // Ajout sinus de calibration
        _sinus.addData(data1, data2, maxlen);

        // Clipping
        clip(data1, data2, maxlen);

        // Enregistrement dans un fichier si demandé
        m_mutexRecord.lock();
        if (m_recordFile && m_isRecording)
        {
            // Entrelacement et écriture
            for (int i = 0; i < maxlen; i++)
            {
                _dataWav[2 * i + 1] = data1[i];
                _dataWav[2 * i]     = data2[i];
            }
            m_recordStream.writeRawData((char*)_dataWav, maxlen * 8);

            // Prise en compte de l'avance
            m_recordLength += maxlen * 8;
            this->samplesRead(maxlen);
        }
        m_mutexRecord.unlock();
    }

    void setFormat(AudioFormat format);

signals:
    void currentPosChanged(int pos);
    void readFinished();
    void sampleRateChanged(qint32 sampleRate);
    void samplesRead(int number);

private slots:
    void stopSinus();

private:
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
    bool _isSinusEnabled, _sampleRunning;

    // Pointeur vers les données
    Pile_sf2 * m_sf2;

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
