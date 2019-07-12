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

#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

#include "circularbuffer.h"
#include "voice.h"

class SoundEngine : public CircularBuffer
{
    Q_OBJECT

public:
    SoundEngine(unsigned int bufferSize);
    virtual ~SoundEngine();

    static void addVoice(Voice * voice, QList<Voice *> friends = QList<Voice*>());
    static void stopAllVoices();
    static void syncNewVoices();
    static void releaseNote(int numNote);
    static void setGain(double gain);
    static void setChorus(int level, int depth, int frequency);
    static void setPitchCorrection(qint16 correction, bool repercute);
    static void setStartLoop(quint32 startLoop, bool repercute);
    static void setEndLoop(quint32 endLoop, bool repercute);
    static void setLoopEnabled(bool isEnabled);
    static void setStereo(bool isStereo);
    static void setGainSample(int gain);

signals:
    void readFinished();

protected:
    // Executed by the circular buffer thread
    void generateData(float *dataL, float *dataR, float *dataRevL, float *dataRevR, quint32 len)
    {
        // Initialize data
        for (quint32 i = 0; i < len; i++)
            dataL[i] = dataR[i] = dataRevL[i] = dataRevR[i] = 0;

        _mutexVoices.lock();

        int nbVoices = _listVoices.size();
        for (int i = nbVoices - 1; i >= 0; i--)
        {
            // Check for started voice (synchronization)
            if (_listVoices.at(i)->isRunning())
            {
                // Get data
                _listVoices.at(i)->generateData(_dataTmpL, _dataTmpR, len);
                float coef1 = _listVoices.at(i)->getReverb() / 100.0f;
                float coef2 = 1.f - coef1;

                // Fusion
                for (quint32 j = 0; j < len; j++)
                {
                    dataL   [j] += coef2 * _dataTmpL[j];
                    dataR   [j] += coef2 * _dataTmpR[j];
                    dataRevL[j] += coef1 * _dataTmpL[j];
                    dataRevR[j] += coef1 * _dataTmpR[j];
                }

                // Voice ended?
                if (_listVoices.at(i)->isFinished())
                {
                    if (_listVoices.at(i)->getKey() == -1)
                        emit(readFinished());
                    delete _listVoices.takeAt(i);
                }
            }
        }
        _mutexVoices.unlock();
    }

private:
    static void closeAll(int exclusiveClass, int numPreset, QList<Voice *> friends);

    int getNbVoices();
    void closeAllInstance(int exclusiveClass, int numPreset, QList<Voice*> friends);
    void addVoiceInstance(Voice * voice);
    void stopAllVoicesInstance();
    void syncNewVoicesInstance(quint32 delay);
    void releaseNoteInstance(int numNote);
    void setGainInstance(double gain);
    void setChorusInstance(int level, int depth, int frequency);
    void setPitchCorrectionInstance(qint16 correction, bool repercute);
    void setStartLoopInstance(quint32 startLoop, bool repercute);
    void setEndLoopInstance(quint32 endLoop, bool repercute);
    void setLoopEnabledInstance(bool isEnabled);
    void setStereoInstance(bool isStereo);
    void setGainSampleInstance(int gain);

    QMutex _mutexVoices;
    QList<Voice *> _listVoices;
    float * _dataTmpL, * _dataTmpR;

    static int _gainSmpl;
    static bool _isStereo, _isLoopEnabled;
    static QList<SoundEngine*> _listInstances;
};

#endif // SOUNDENGINE_H
