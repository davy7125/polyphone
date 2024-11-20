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

namespace Ui {
class DialogRecorder;
}

class Synth;

class DialogRecorder : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogRecorder(QWidget *parent = nullptr);
    ~DialogRecorder() override;
    void updateState(bool isOn) { _stateUpdate = isOn; }

protected:
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;

private slots:
    void onDataWritten(quint32 sampleRate, quint32 number);
    void on_pushRecord_clicked();
    void on_pushPlayPause_clicked();

private:
    void initialize();
    QString getDefaultPath();

    Ui::DialogRecorder *ui;
    quint32 _currentSample;
    bool _isRecording;
    bool _isPaused;
    Synth * _synth;
    bool _stateUpdate;
};

#endif // DIALOGRECORDER_H
