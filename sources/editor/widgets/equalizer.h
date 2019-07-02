/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QWidget>
#include "basetypes.h"
#include "pagesmpl.h"
class Synth;

namespace Ui {
class Equalizer;
}

class Equalizer : public QWidget
{
    Q_OBJECT

public:
    explicit Equalizer(QWidget *parent = nullptr);
    ~Equalizer();

    // Allow the user to press on "apply" or not
    void enableApply(bool isEnabled);

    // Set the current ids to work on
    void setCurrentIds(IdList ids) { _currentIds = ids; }

    // Return true if the preview is enabled
    bool isPreviewEnabled();

private slots:
    void on_pushEgalRestore_clicked();
    void on_pushEgaliser_clicked();
    void on_checkEqualizerPreview_stateChanged(int arg1);
    void on_verticalSlider_1_sliderMoved(int position);
    void on_verticalSlider_2_sliderMoved(int position);
    void on_verticalSlider_3_sliderMoved(int position);
    void on_verticalSlider_4_sliderMoved(int position);
    void on_verticalSlider_5_sliderMoved(int position);
    void on_verticalSlider_6_sliderMoved(int position);
    void on_verticalSlider_7_sliderMoved(int position);
    void on_verticalSlider_8_sliderMoved(int position);
    void on_verticalSlider_9_sliderMoved(int position);
    void on_verticalSlider_10_sliderMoved(int position);

private:
    void saveEQ();
    void loadEQ();
    QVector<int> gatherEqVariables();

    Ui::Equalizer *ui;
    Synth * _synth;
    bool _initialization;
    IdList _currentIds;
};

#endif // EQUALIZER_H
