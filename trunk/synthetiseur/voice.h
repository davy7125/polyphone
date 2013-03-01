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

#ifndef VOICE_H
#define VOICE_H

#define BUFFER_VOICE_SIZE     180000
#define BUFFER_VOICE_AVANCE   40000

#include "sound.h"
#include "circularbuffer.h"
#include "enveloppevol.h"
#include "oscsinus.h"

class Voice : public CircularBuffer
{
    Q_OBJECT

public:
    Voice(QByteArray baData, DWORD smplRate, DWORD audioSmplRate, int note,
          int velocity, VoiceParam *voiceParam, QObject *parent = NULL);
    Voice(DWORD audioSmplRate, VoiceParam *voiceParam, QObject *parent = NULL);
    ~Voice();
    qint64 readData(char *data, qint64 maxlen);
    int getNote()                {return m_note;}
    VoiceParam * getVoiceParam() {return m_voiceParam;}
    void release()               {m_release = true;}
    void setGain(double gain);

signals:
    void currentPosChanged(int pos);

protected slots:
    void generateData(qint64 nbData = 0);

private:
    // Oscillateurs
    OscSinus m_sinusOsc, m_modLFO, m_vibLFO;
    // Données et paramètres
    QByteArray m_baData;
    DWORD m_smplRate, m_audioSmplRate;
    int m_note;
    int m_velocity;
    double m_gain;
    VoiceParam * m_voiceParam;
    // Lecture du sample
    qint64 m_currentSmplPos;
    double m_time;
    bool m_release;
    bool m_finished;
    // enveloppe
    EnveloppeVol m_enveloppeVol;
    EnveloppeVol m_enveloppeMod;
    // sauvegarde état pour le resampling
    double m_deltaPos;
    qint32 m_valPrec, m_valBase;
    // sauvegarde état pour filtre passe bas
    double m_x1, m_x2, m_y1, m_y2;
    // Méthodes privées
    bool takeData(qint32 *data, qint64 nbRead);
    void biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2,
                            double freq, double Q);
};

#endif // VOICE_H
