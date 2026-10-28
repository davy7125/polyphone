/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef OFFLINERENDERER_H
#define OFFLINERENDERER_H

#include <QString>
#include <QList>
#include <QVector>
#include <QMetaObject>
class MidiProcessor;
class SynthCoreConfig;

/// Play a list of timed events with the synth, faster than real time, and write the result in a wav file.
/// Used for comparing the sound before and after a change of the sound engine (see tests/render).
class OfflineRenderer
{
public:
    /// Created before loading the soundfont: the default controller values depend on its modulators
    OfflineRenderer();
    ~OfflineRenderer();

    /// Read the settings and the events, return false in case of error (see getError)
    bool readEvents(QString filePath);

    /// Synth playing the events:
    /// * SYNTH_CORE: SynthCore computing in the calling thread,
    /// * SYNTH_TWO_CORES: the same, next to a second SynthCore with other settings whose rendering must not change the
    ///   first one (as two instances of a plugin) and is not written,
    /// * SYNTH_THREADED: Synth (editor) with its threads.
    enum SynthType
    {
        SYNTH_CORE,
        SYNTH_TWO_CORES,
        SYNTH_THREADED
    };

    /// Play the events with the soundfont loaded at sf2Index and write a stereo 32-bit float wav file
    bool render(int sf2Index, QString outputFilePath, SynthType synthType);

    QString getError() { return _error; }

private:
    enum EventType
    {
        EVENT_PROGRAM,
        EVENT_NOTE_ON,
        EVENT_NOTE_OFF,
        EVENT_CONTROLLER,
        EVENT_BEND,
        EVENT_BEND_RANGE,
        EVENT_PRESSURE,
        EVENT_POLY_PRESSURE,
        EVENT_INSTRUMENT,
        EVENT_SAMPLE
    };

    struct Event
    {
        double time;
        quint64 frame;
        EventType type;
        int channel;
        int number; // Bank, key or controller
        int value;  // Preset, velocity, controller value or pressure
        float floatValue; // Bend or bend range
        int index; // Instrument or sample played alone
        int line;
    };

    // Change of the soundfont made after loading it: a muted zone, or an element always played
    struct ModelChange
    {
        bool mute;
        bool isPreset;
        int index;
        int zone;
        int line;
    };

    bool readSetting(QStringList words, int line);
    bool readEvent(double time, QStringList words, int line);
    bool applyModelChanges(int sf2Index);
    bool resolveElements(int sf2Index);
    SynthCoreConfig getSynthConfig();
    bool renderWithCore(int sf2Index, QVector<float> &data, quint32 blockCount, bool withOtherCore);
    void renderWithThreads(int sf2Index, QVector<float> &data, quint32 blockCount);
    template <class T> void processEvents(T * synth, MidiProcessor * midiProcessor, int sf2Index, quint64 blockEnd, int &eventIndex);
    void storeBlock(QVector<float> &data, quint32 block, const float * dataL, const float * dataR);
    bool writeWavFile(QString filePath, const QVector<float> &data, quint32 frameCount);
    bool readInt(QString word, int min, int max, int &value, int line);
    bool readFloat(QString word, float min, float max, float &value, int line);

    QString _error;
    MidiProcessor * _midiProcessor; // Events processed as MidiDevice does, values read by the modulators
    QMetaObject::Connection _modulatorConnection;

    // Settings
    quint32 _sampleRate;
    quint32 _blockSize;
    double _duration;
    double _masterGain;
    int _referencePitch;
    int _reverb[4]; // Level, size, width, damping
    int _chorus[3]; // Level, depth, frequency
    bool _sampleLoop, _sampleStereo; // Samples played alone
    double _sampleGain;
    int _polyphony;
    int _cpuBudget; // 0: offline, the computation time not limiting the voices
    QList<ModelChange> _modelChanges;

    QList<Event> _events;
};

#endif // OFFLINERENDERER_H
