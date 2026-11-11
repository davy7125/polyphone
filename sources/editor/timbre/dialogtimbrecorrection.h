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


#ifndef DIALOGTIMBRECORRECTION_H
#define DIALOGTIMBRECORRECTION_H

#include <QDialog>
#include <QTimer>
#include <QThreadPool>
#include "timbreactions.h"
class TimbreBarsWidget;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QLabel;
class QRadioButton;
class QPushButton;
class Synth;

// Parameters of the correction of the timbre and of the level of root keys, with the levels before and after and a
// comparison A / B of the original and of the corrected sound (the data of the samples being replaced in the synth)
class DialogTimbreCorrection : public QDialog
{
    Q_OBJECT

public:
    DialogTimbreCorrection(TimbreModel * model, const QList<int> &targets, const TimbreSettings &settings,
                           QWidget * parent = nullptr);
    ~DialogTimbreCorrection() override;

    TimbreCorrectionParameters getParameters();

    // Parameters used the last time, until Polyphone is closed
    static TimbreCorrectionParameters s_lastParameters;
    static bool s_lastParametersSet;

protected:
    void done(int result) override;

private slots:
    void updatePreview();
    void onPlayToggled(bool isOn);
    void onComparisonChanged();
    void computeAudioPreview();

private:
    void clearAudioPreview();
    void applyAudioPreview(int generation, const QList<TimbreCorrectedSample> &samples, double gain);

    TimbreModel * _model;
    QList<int> _targets;
    TimbreSettings _settings;
    TimbreCorrectionPlan _plan;
    Synth * _synth;
    QTimer _previewTimer;
    QThreadPool _pool;
    int _generation;
    QList<EltID> _previewIds;
    int _playedKey;

    QComboBox * _comboPreviewed;
    QComboBox * _comboReference;
    QSpinBox * _spinRegression;
    QComboBox * _comboAligned;
    QCheckBox * _checkTimbre, * _checkLevel;
    QDoubleSpinBox * _spinTolerance, * _spinStrength, * _spinMaximum;
    QComboBox * _comboRanks;
    QSpinBox * _spinRanks;
    QDoubleSpinBox * _spinBrightness, * _spinOddEven;
    TimbreBarsWidget * _bars;
    QLabel * _labelSummary;
    QRadioButton * _radioOriginal, * _radioCorrected;
    QPushButton * _buttonPlay;
};

#endif // DIALOGTIMBRECORRECTION_H
