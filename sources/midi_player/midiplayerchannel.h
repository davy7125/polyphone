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

#ifndef MIDIPLAYERCHANNEL_H
#define MIDIPLAYERCHANNEL_H

#include <QObject>
#include <QPixmap>
#include <QElapsedTimer>
class MidiFilePlayer;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QPushButton;
class QToolButton;
class QMenu;
class ResettableSlider;

// Channel of the MIDI player in a row of a table (see MidiPlayerDelegate for the columns): number with mute, solo and
// the current MIDI values, soundfont, preset chosen by the file ("auto") or forced, volume and activity
class MidiPlayerChannel : public QObject
{
    Q_OBJECT

public:
    // The programs used by the file (bank * 128 + preset) are for loading the samples beforehand
    MidiPlayerChannel(MidiFilePlayer * player, int channel, QString name, QList<int> usedPrograms,
                      QTableWidget * table, int row);

    // Soundfont playing the channel (-1: none) among the ones that are open, the choices being "name / sf2 index"
    QList<QPair<QString, int> > getSoundfontChoices();
    void setSoundfont(int sf2Index);
    int getSoundfont() { return _sf2Index; }
    void updateSoundfont(); // When a soundfont is loaded, closed or renamed

    // Program forced (bank * 128 + preset) or -1 for the program of the file, the choices being "name / program"
    QList<QPair<QString, int> > getPresetChoices();
    void setForcedProgram(int program);
    int getForcedProgram() { return _forcedProgram; }
    void updatePresetList(); // When the soundfont is edited

    // Gain sent to the synth, depending on the volume, mute and solo of all channels
    bool isSolo();
    void updateGain(bool isSoloEnabled);

    // Activity, program chosen by the file and MIDI values, read periodically
    void updateState();

signals:
    void mixChanged(); // Volume, mute or solo

private slots:
    void onVolumeChanged(int value);
    void updateMidiValues();

private:
    void updatePresetText();
    QString getPresetText(int program);
    bool hasPreset(int program);
    void loadSamples();
    static QString getProgramText(int program);

    MidiFilePlayer * _player;
    int _channel;
    QList<int> _usedPrograms;
    int _sf2Index;
    int _forcedProgram;
    QList<QPair<int, QString> > _presets; // Program and name of the presets of the soundfont
    int _displayedProgram;
    quint32 _noteCount;
    QElapsedTimer _ledTimer;
    bool _isLedOn;
    QPixmap _ledOn, _ledOff, _warning;

    // Widgets and items, owned by the table
    QTableWidgetItem * _itemSoundfont;
    QTableWidgetItem * _itemPreset;
    QPushButton * _pushMute;
    QPushButton * _pushSolo;
    QMenu * _menuMidi;
    QLabel * _labelMidi;
    QLabel * _labelLed;
    ResettableSlider * _sliderVolume;
    QLabel * _labelVolume;
};

#endif // MIDIPLAYERCHANNEL_H
