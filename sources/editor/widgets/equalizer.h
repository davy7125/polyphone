/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QWidget>
#include <QTimer>
#include <QThreadPool>
#include "basetypes.h"
#include "gaincurve.h"
class Synth;
class QSlider;
class QLabel;

namespace Ui {
class Equalizer;
}

// Equalizer of the sample page: 10 bands (gain in dB) at fixed frequencies, or at ratios to the fundamental frequency of
// each sample (note tracking). The preview plays the processed data, computed again after each change.
class Equalizer : public QWidget
{
    Q_OBJECT

public:
    explicit Equalizer(QWidget *parent = nullptr);
    ~Equalizer() override;

    // Allow the user to press on "apply" or not
    void enableApply(bool isEnabled);

    // Set the current ids to work on
    void setCurrentIds(IdList ids);

    // Update the preview and the bands after a change of the samples (loop, root key, pitch correction)
    void updateSamples();

private slots:
    void on_pushEgalRestore_clicked();
    void on_pushEgaliser_clicked();
    void on_checkEqualizerPreview_stateChanged(int arg1);
    void on_checkNoteTracking_toggled(bool checked);
    void on_verticalSliderMoved(int position);
    void computePreview();

private:
    struct PreviewData
    {
        EltID id;
        QVector<float> data;
    };

    GainCurve getCurve();
    QList<EltID> getProcessedIds(); // Selected samples, with their linked samples if the stereo editing is enabled
    void updateBands();
    void schedulePreview();
    void clearPreview();
    void setPreviewPending(bool isPending);
    void applyPreview(int generation, const QList<PreviewData> &previewData);
    void saveEQ();
    void loadEQ();

    Ui::Equalizer *ui;
    Synth * _synth;
    bool _initialization;
    IdList _currentIds;
    QList<QSlider *> _sliders;
    QList<QLabel *> _labels;

    // Preview
    QTimer _previewTimer;
    QThreadPool _previewThreadPool;
    QList<EltID> _previewIds; // Samples whose data is replaced in the synth
    int _previewGeneration;

    static const double FREQUENCIES[10];
    static const double RATIOS[10];
};

#endif // EQUALIZER_H
