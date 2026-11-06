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

#ifndef DIALOGRECORDER_H
#define DIALOGRECORDER_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class DialogRecorder;
}

class FilePlayer;
class MidiRecorder;
class QCheckBox;
class QLabel;
class Recorder;
class ResettableSlider;
class Synth;

// Records the output of Polyphone or the audio input in a wav file, or the MIDI events played in a MIDI file. A record
// can start with the first sound or MIDI event and stop after a silence, the next one being then awaited.
// The last audio files and the last MIDI files are listed in a history.
class DialogRecorder : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogRecorder(QWidget *parent = nullptr);
    ~DialogRecorder() override;
    void updateState(bool isOn) { _stateUpdate = isOn; }

    // Stop a record in progress after asking the user, false being returned if it must continue
    bool confirmStop();

public slots:
    // A soundfont editor is displayed or not, in which the audio files can be imported
    void onEditorDisplayed(bool isDisplayed);

signals:
    // A MIDI file of the history is to be played
    void midiFileOpened(QString filePath);

protected:
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;
    void closeEvent(QCloseEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;

private slots:
    void onTimeout();
    void onInterrupted(int reason);
    void onInputChanged();
    void onPlaybackFinished();
    void onRecordShortcut();
    void onAutoStartToggled(bool isChecked);
    void onAutoStopChanged(int value);
    void onThresholdChanged(int value);
    void onAskFileNameToggled(bool isChecked);
    void onChangeFolderTriggered();
    void onListenRequested(QString filePath);
    void onFolderRequested(QString filePath);
    void onDeleteRequested(QString filePath);
    void onImportRequested(QString filePath);
    void on_comboMode_activated(int index);
    void on_pushRecord_clicked();
    void on_pushPlayPause_clicked();

private:
    enum Mode
    {
        MODE_OUTPUT = 0,
        MODE_INPUT = 1,
        MODE_MIDI = 2
    };

    void initializeSettingsMenu();
    void updateSource();
    void updateSoundDetection();
    void startRecord(bool isChained);
    void stopRecord();
    void stopAfterSilence();
    void stopPlayback();
    void updateInterface();
    void updateProgress();
    void updateDisplayButtons(bool isChecked);
    void loadHistory();
    void saveHistory();
    void updateHistory();
    QString getDetail(QString filePath);
    bool isWaiting();
    bool hasContent();
    QString getDefaultPath();
    QString getHistoryKey() { return _mode == MODE_MIDI ? "recorder_midi" : "recorder_audio"; }

    Ui::DialogRecorder *ui;
    Synth * _synth;
    Recorder * _recorder; // Of the output or the input, not used with the MIDI mode
    MidiRecorder * _midiRecorder;
    FilePlayer * _filePlayer;
    QTimer _timer;
    Mode _mode;
    QStringList _files; // History of the current mode, from the most recent file
    QString _filePath; // Current record
    QString _playingFile; // Empty if nothing is played
    bool _isRecording;
    bool _isPaused;
    bool _isInHistory; // The current record appears in the history as soon as it has a content
    bool _stateUpdate;

    QCheckBox * _checkAutoStart;
    ResettableSlider * _sliderAutoStop;
    QLabel * _labelAutoStop;
    QLabel * _labelThresholdTitle;
    ResettableSlider * _sliderThreshold;
    QLabel * _labelThreshold;
};

#endif // DIALOGRECORDER_H
