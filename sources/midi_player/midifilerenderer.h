/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#ifndef MIDIFILERENDERER_H
#define MIDIFILERENDERER_H

#include <QCoreApplication>
#include <QString>
#include <QMetaObject>
#include <atomic>
#include "midiprocessor.h"
#include "synthcoreconfig.h"
class MidiFile;
class SynthCore;
class SynthChannels;
class ISynthSoundfonts;
class QRecursiveMutex;
class QDataStream;

// Plays a MIDI file with a synth faster than real time and writes a stereo 32-bit float wav file, as the recorder does.
// As in MidiFilePlayer, each channel plays the presets of a soundfont chosen by the program changes of the file or
// forced (SynthChannels), with a gain, a speed and a transposition. The notes still held at the end of the file are released, and the
// file ends once the sound has faded out.
// The renderer is created, configured and deleted by the same thread, render being possibly called by another one.
class MidiFileRenderer
{
    Q_DECLARE_TR_FUNCTIONS(MidiFileRenderer)

public:
    // Created before loading the soundfonts: the default controller values depend on their modulators
    MidiFileRenderer();
    ~MidiFileRenderer();

    // Settings of the synth (the computation time does not limit the voices), maximum number of voices and sample rate,
    // to give before rendering
    void configure(const SynthCoreConfig &configuration, int polyphony, quint32 sampleRate);

    // Configuration of a channel (0 to 15), none playing by default: soundfont (-1 for none), forced program
    // ("bank * 128 + preset", -1 for the programs of the file) and gain (dB)
    void setChannel(int channel, int sf2Index, int forcedProgram, float gain);

    // Speed (1 for the tempo of the file), and transposition in semitones except for the percussions (bank 128)
    void setSpeed(double speed) { _speed = speed; }
    void setTransposition(int semitones);

    // Default values of the controllers taken from another processor (the MIDI player)
    void copyDefaultControllerValues(MidiProcessor &other) { _processor.copyDefaultControllerValues(other); }

    // Render a file, false in case of error (see getError) or cancellation, the incomplete file being then removed
    bool render(const MidiFile * file, QString outputFilePath);
    QString getError() { return _error; }

    // Progress from 0 to 1000 and cancellation, possibly by another thread than the one rendering
    int getProgress() { return _progress; }
    void cancel() { _isCanceled = true; }

private:
    void writeHeader(QDataStream &stream, quint32 frameCount);

    ISynthSoundfonts * _soundfonts;
    QRecursiveMutex * _mutexSoundfonts;
    SynthCore * _core;
    SynthChannels * _channels; // Receiving the events processed by _processor
    MidiProcessor _processor; // Values of the controllers read by the synth
    QMetaObject::Connection _modulatorConnection;
    quint32 _sampleRate;
    double _speed;

    // Rendering
    std::atomic<int> _progress;
    std::atomic<bool> _isCanceled;
    QString _error;
};

#endif // MIDIFILERENDERER_H
