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

#ifndef DIALOG_MAGNETOPHONE_H
#define DIALOG_MAGNETOPHONE_H

#include <QDialog>

namespace Ui {
class DialogMagnetophone;
}

class Synth;

class DialogMagnetophone : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogMagnetophone(QWidget *parent = 0);
    ~DialogMagnetophone();
    void setSynth(Synth * synth) { _synth = synth; }

public slots:
    void avanceSamples(int number);
    void setSampleRate(qint32 sampleRate);
    
private slots:
    void on_pushRecord_toggled(bool checked);
    void on_pushPlayPause_clicked();

private:
    Ui::DialogMagnetophone *ui;
    int _sampleRate;
    int _currentSample;
    bool _isPause;
    Synth * _synth;

    void closeEvent(QCloseEvent *);
    void initGui();
    QString getDefaultPath();
};

#endif // DIALOG_MAGNETOPHONE_H
