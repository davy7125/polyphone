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

#ifndef DIALOGMIDIPLAYER_H
#define DIALOGMIDIPLAYER_H

#include <QDialog>
#include <memory>
class MidiFile;
class MidiFilePlayer;
class MidiPlayerChannel;
class QTimer;

namespace Ui {
class DialogMidiPlayer;
}

// Plays a MIDI file with the soundfonts that are open, the soundfont, the preset and the volume of each channel being
// chosen. The last file is opened again, and the playback stops when the window is closed.
class DialogMidiPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMidiPlayer(QWidget *parent = nullptr);
    ~DialogMidiPlayer() override;
    void updateState(bool isOn) { _stateUpdate = isOn; }

protected:
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;

private slots:
    void on_pushOpen_clicked();
    void on_pushPlayPause_clicked();
    void on_pushStop_clicked();
    void on_pushLoop_toggled(bool checked);
    void on_sliderPosition_valueChanged(int value);
    void on_sliderPosition_sliderReleased();
    void onTimeout();
    void onMixChanged();
    void onSoundfontLoaded(int sf2Index);
    void onSoundfontClosed(int sf2Index);
    void onEditingDone(QString editingSource, QList<int> sf2Indexes);
    void onInputModulatorChanged(int controllerNumber, bool isBipolar, bool isDescending);

private:
    void loadFile(QString filePath);
    void clearChannels();
    void updatePlayPauseIcon(bool isPlaying);
    void updatePosition(qint64 position);
    static QString getTimeText(qint64 microseconds);

    Ui::DialogMidiPlayer * ui;
    MidiFilePlayer * _player;
    std::shared_ptr<MidiFile> _file;
    QList<MidiPlayerChannel *> _channels;
    QTimer * _timer;
    bool _stateUpdate;
    bool _isPlaying; // As displayed
    bool _updatingSlider;
};

#endif // DIALOGMIDIPLAYER_H
