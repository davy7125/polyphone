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
****************************************************************************/

#ifndef AUDIO_H
#define AUDIO_H

#include <QAudioOutput>
#include "sf2_types.h"
#include "sound.h"

class Audio : public QIODevice
{
    Q_OBJECT
public:
    Audio(QObject *parent = NULL);
    ~Audio();
protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 readSinus(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;
signals:
    void askForData();
    void setCurrentPos(int index);
    void readFinished();
    void sigFinished();
    void sigFinished2();
public slots:
    void start();
    void stop();
    void finished2();
    void finished3();
    void generateData(int nbData = 0);
    void generateSinus(double frequency);
    // Initialisation
    void initAudio(int numServeur);
    void setData(QByteArray * baDataDroite, QByteArray * baDataGauche, quint32 dwSmplRate);
    void setPlayParameter(int startLoop1, int endLoop1, int typeSmpl, int startLoop2, int endLoop2, bool boucle, bool sinus,
                          int tune, int rootKey, int volume, int stereo);
    // Paramètres
    void setStartLoop(int index);
    void setEndLoop(int index);
    void setBoucle(int iVal);
    void setSinus(int iVal);
    void setTune(int iTune);
    void setStereo(int iVal);
    void setRootKey(int iKey);
    void setVolume(int iVol);
private:
    // Données et échantillonnage
    QByteArray baDataDroite;
    QByteArray baDataGauche;
    quint32 dwSmplRate;
    // Buffer et positions
    QByteArray m_buffer, m_bufferSinus;
    qint64 posEcriture;
    qint64 posLecture, posLectureSinus;
    qint64 currentBufferLength;
    qint64 avance;
    qint64 currentSmpl1;
    qint64 currentSmpl2;
    // Serveur son
    QAudioOutput * audioOutput;
    QAudioFormat format;
    // Paramètres
    int startLoop1, startLoop2, endLoop1, endLoop2;
    int typeSmpl;
    bool boucle, stereo, sinus;
    int tune, rootKey;
    double volume, attenuationFin;
    bool lectureD, lectureG, finDemandee;
    double d; // décalage pour la série de resampling
    qint32 valPrec1, valPrec2; // valeurs précédentes pour resampling
    // Méthodes privées
    static QByteArray takeData(QByteArray baData, qint64 posInit, qint64 nbRead,
                               qint64 loopStart, qint64 loopEnd, bool boucle,
                               bool &end, qint64 &posFinal);
};


class QThreadEx : public QThread
{
protected:
    void run() {exec();}
public:
    static void mSleep(unsigned long mSec) {msleep(mSec);}
};

#endif // AUDIO_H
