/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
    Voice(QByteArray baData, quint32 smplRate, quint32 audioSmplRate, int note,
          int velocity, VoiceParam *voiceParam, QObject *parent = NULL);
    ~Voice();

    int getNote()                       { return m_note; }
    void release(bool quick = false);
    void setGain(double gain);
    void setChorus(int level, int depth, int frequency);
    bool isFinished() { return _isFinished; }
    bool isRunning() { return _isRunning; }
    void runVoice(int delay) { _isRunning = true; _delayStart = delay; }

    // Accès aux propriétés de voiceParam
    double getPan();
    int getExclusiveClass();
    int getPresetNumber();
    double getReverb();

    // Modification des propriétés de voiceParam
    void setPan(double val);
    void setLoopMode(int val);
    void setLoopStart(qint32 val);
    void setLoopEnd(qint32 val);
    void setFineTune(qint32 val);
    void setRootKey(double val);

    // Génération de données
    void generateData(float *dataL, float *dataR, qint64 len);

signals:
    void currentPosChanged(int pos);

private:
    // Oscillateurs, enveloppes et chorus
    OscSinus _modLFO, _vibLFO;
    EnveloppeVol _enveloppeVol, _enveloppeMod;
    stk::Chorus _chorus;

    // Données son et paramètres
    QByteArray _baData;
    quint32 m_smplRate, m_audioSmplRate;
    int m_note;
    int m_velocity;
    double m_gain;
    VoiceParam * _voiceParam;

    // Lecture du sample
    qint64 m_currentSmplPos;
    double m_time;
    bool m_release;
    int _delayEnd, _delayStart;
    bool _isFinished;
    bool _isRunning;

    // Sauvegarde état pour le resampling
    double m_deltaPos;
    qint32 m_valPrec, m_valBase;

    // Sauvegarde état pour filtre passe bas
    double m_x1, m_x2, m_y1, m_y2;

    bool takeData(qint32 *data, qint64 nbRead);
    void biQuadCoefficients(double &a0, double &a1, double &a2, double &b1, double &b2,
                            double freq, double Q);

    // protection des paramètres
    QMutex _mutexParam;
};

#endif // VOICE_H
