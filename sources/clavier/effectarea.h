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

#ifndef EFFECTAREA_H
#define EFFECTAREA_H

#include <QWidget>

namespace Ui {
class EffectArea;
}

// Reverb, chorus and general volume of the sound engine, also editable in the sound settings
class EffectArea : public QWidget
{
    Q_OBJECT

public:
    explicit EffectArea(QWidget *parent = nullptr);
    ~EffectArea();

private slots:
    void updateValues();
    void on_pushReverb_toggled(bool checked);
    void on_dialRevLevel_valueChanged(int value);
    void on_dialRevSize_valueChanged(int value);
    void on_dialRevWidth_valueChanged(int value);
    void on_dialRevDamping_valueChanged(int value);
    void on_pushChorus_toggled(bool checked);
    void on_dialChoLevel_valueChanged(int value);
    void on_dialChoDepth_valueChanged(int value);
    void on_dialChoFrequency_valueChanged(int value);
    void on_sliderGain_valueChanged(int value);

private:
    void updateGainLabel(int value);

    Ui::EffectArea *ui;
};

#endif // EFFECTAREA_H
