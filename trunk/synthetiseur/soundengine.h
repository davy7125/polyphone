/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2014 Davy Triponney                                **
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

#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#define BUFFER_ENGINE_MIN_DATA   500
#define BUFFER_ENGINE_MAX_DATA   1500

#include "circularbuffer.h"
#include "voice.h"

class SoundEngine : public CircularBuffer
{
    Q_OBJECT

public:
    SoundEngine(QObject *parent = NULL);
    virtual ~SoundEngine();
    bool isEmpty();

    static void addVoice(Voice * voice, QList<Voice *> friends = QList<Voice*>());
    static void stopAllVoices();
    static void releaseNote(int numNote);
    static void setGain(double gain);
    static void setChorus(int level, int depth, int frequency);
    static void setRootKey(int rootKey);
    static void setPitchCorrection(int correction, bool repercute);
    static void setStartLoop(int startLoop, bool repercute);
    static void setEndLoop(int endLoop, bool repercute);
    static void setSinusEnabled(bool isEnabled);
    static void setLoopEnabled(bool isEnabled);
    static void setStereo(bool isStereo);
    static void setGainSample(int gain);

signals:
    void readFinished();

protected:
    void generateData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, int len)
    {
        // Initialisation des données
        for (int i = 0; i < len; i++)
        {
            dataL[i] = 0;
            dataR[i] = 0;
            dataRevL[i] = 0;
            dataRevR[i] = 0;
        }

        _mutexVoices.lock();
        int nbVoices = _listVoices.size();
        for (int i = nbVoices - 1; i >= 0; i--)
        {
            // Récupération des données
            _listVoices.at(i)->generateData(_dataTmpL, _dataTmpR, len);
            float coef1 = _listVoices.at(i)->getReverb() / 100.;
            float coef2 = 1. - coef1;

            // Fusion
            for (int j = 0; j < len; j++)
            {
                dataL   [j] += coef2 * _dataTmpL[j];
                dataR   [j] += coef2 * _dataTmpR[j];
                dataRevL[j] += coef1 * _dataTmpL[j];
                dataRevR[j] += coef1 * _dataTmpR[j];
            }

            // Voix terminée ?
            if (_listVoices.at(i)->isFinished())
            {
                if (_listVoices.at(i)->getNote() == -1)
                    emit(readFinished());
                delete _listVoices.takeAt(i);
            }
        }
        _mutexVoices.unlock();
    }

private:
    static void closeAll(int exclusiveClass, int numPreset, QList<Voice *> friends);
    void closeAllInstance(int exclusiveClass, int numPreset, QList<Voice*> friends);
    int getNbVoices();
    void addVoiceInstance(Voice * voice);
    void stopAllVoicesInstance();
    void releaseNoteInstance(int numNote);
    void setGainInstance(double gain);
    void setChorusInstance(int level, int depth, int frequency);
    void setRootKeyInstance(int rootKey);
    void setPitchCorrectionInstance(int correction, bool repercute);
    void setStartLoopInstance(int startLoop, bool repercute);
    void setEndLoopInstance(int endLoop, bool repercute);
    void setSinusEnabledInstance(bool isEnabled);
    void setLoopEnabledInstance(bool isEnabled);
    void setStereoInstance(bool isStereo);
    void setGainSampleInstance(int gain);

    QMutex _mutexVoices;
    QList<Voice *> _listVoices;
    float * _dataTmpL, * _dataTmpR;

    static int _gainSmpl;
    static bool _isStereo;
    static QList<SoundEngine*> _listInstances;
};

#endif // SOUNDENGINE_H
