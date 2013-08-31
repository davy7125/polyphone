/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
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


#define BUFFER_SYNTH_SIZE     60000
#define BUFFER_SYNTH_AVANCE   22000

#include "pile_sf2.h"
#include "voice.h"
#include "audiodevice.h"

class Synth : public CircularBuffer
{
    Q_OBJECT

public:
    Synth(Pile_sf2 * sf2, QObject *parent = NULL);
    ~Synth();

    // Exécution par mainWindow (thread 1)
    void interruption();
    void play(int type, int idSf2, int idElt, int note, int velocity,
              VoiceParam * voiceParamTmp = NULL);
    void stop();
    void setGain(double gain);
    void setReverb(int level, int size, int width, int damping);
    void setChorus(int level, int depth, int frequency);
    // Paramètres de lecture de samples
    void setGainSample(int gain);
    void setStereo(int isStereo, bool withMutex = true);
    void setStartLoop(int startLoop);
    void setEndLoop(int endLoop);
    void setLoopEnabled(int isEnabled);
    void setRootKey(int rootKey);
    void setSinusEnabled(int isEnabled, bool withMutex = true);
    void setPitchCorrection(int correction);
    // Gestion de l'enregistrement
    void startNewRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);

    // Exécution par le serveur audio (thread 3)
    qint64 readData(char *data1, char *data2, qint64 maxlen);
    void setFormat(AudioFormat format);

public slots:
    // Exécution par synth (thread 2)
    void start();

signals:
    void currentPosChanged(int pos);
    void readFinished();
    void sampleRateChanged(qint32 sampleRate);
    void samplesRead(int number);

protected slots:
    void generateData(qint64 nbData = 0);
    void emitCurrentPosChanged(int pos);

private:
    void clip(double * data1, double * data2, qint64 size);
    void endVoice(Voice * voice);
    bool getInterrupt();

    // Pointeur vers les données
    Pile_sf2 * m_sf2;
    // Liste des voix
    QList<Voice *> m_listeVoix, m_listeVoixTmp;
    // Format audio
    AudioFormat m_format;
    // Paramètre global
    double m_gain;
    // Paramètres lecture sample
    int m_gainSmpl;
    bool m_isStereo;
    bool m_isLoopEnabled;
    bool m_isSinusEnabled;
    // Effets
    int m_choLevel, m_choDepth, m_choFrequency;
    FreeVerb m_reverb;
    // Etat clipping
    double m_clipCoef;
    // Protection des données et déclenchement de la génération de données
    QMutex m_mutexInterrupt;
    QMutex m_mutexVoices;
    QMutex m_mutexCompleted;
    // Interruption de la boucle
    bool m_interrupt;
    // Gestion de l'enregistrement
    QFile * m_recordFile;
    QDataStream m_recordStream;
    bool m_isRecording;
    quint32 m_recordLength;
    QMutex m_mutexRecord;
};


#endif // SYNTH_H
