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
class QGridLayout;
class QLabel;
class QComboBox;
class QCheckBox;
class QPushButton;
class ResettableSlider;

// Widgets of a channel of the MIDI player, in a row of a grid: activity, name, soundfont, preset chosen by the file or
// forced, volume, mute and solo
class MidiPlayerChannel : public QObject
{
    Q_OBJECT

public:
    // The programs used by the file (bank * 128 + preset) are for loading the samples beforehand
    MidiPlayerChannel(MidiFilePlayer * player, int channel, QString name, QList<int> usedPrograms,
                      QGridLayout * layout, int row);
    ~MidiPlayerChannel() override;

    // Soundfont playing the channel (-1: none) among the ones that are open
    void updateSoundfontList();
    void setSoundfont(int sf2Index);
    int getSoundfont() { return _sf2Index; }

    // Presets of the soundfont, to update when it is edited
    void updatePresetList();

    // Gain sent to the synth, depending on the volume, mute and solo of all channels
    bool isSolo();
    void updateGain(bool isSoloEnabled);

    // Activity and program chosen by the file, read periodically
    void updateState();

signals:
    void mixChanged(); // Volume, mute or solo

private slots:
    void onSoundfontChanged(int index);
    void onForcedToggled(bool isChecked);
    void onPresetChanged(int index);
    void onVolumeChanged(int value);

private:
    void applySoundfont(int sf2Index);
    int getForcedProgram();
    void displayFileProgram(int program);
    void loadSamples();
    static QString getProgramText(int program);

    MidiFilePlayer * _player;
    int _channel;
    QList<int> _usedPrograms;
    int _sf2Index;
    int _displayedProgram;
    quint32 _noteCount;
    QElapsedTimer _ledTimer;
    bool _isLedOn;
    QPixmap _ledOn, _ledOff, _warning;

    QLabel * _labelLed;
    QLabel * _labelChannel;
    QLabel * _labelName;
    QComboBox * _comboSoundfont;
    QCheckBox * _checkForced;
    QComboBox * _comboPreset;
    QLabel * _labelWarning;
    ResettableSlider * _sliderVolume;
    QLabel * _labelVolume;
    QPushButton * _pushMute;
    QPushButton * _pushSolo;
};

#endif // MIDIPLAYERCHANNEL_H
