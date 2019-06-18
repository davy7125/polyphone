/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#ifndef TOOLSOUNDSPATIALIZATION_GUI_H
#define TOOLSOUNDSPATIALIZATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
  class ToolSoundSpatialization_gui;
}

class ToolSoundSpatialization_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolSoundSpatialization_gui(QWidget *parent = nullptr);
    ~ToolSoundSpatialization_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_comboPattern_currentIndexChanged(int index);
    void on_spinNbDivision_valueChanged(int arg1);
    void on_spinFilling_valueChanged(int arg1);
    void on_spinSpreading_valueChanged(int arg1);
    void on_spinOffset_valueChanged(int arg1);
    void on_checkFlip_stateChanged(int arg1);
    void on_checkFlop_stateChanged(int arg1);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    double spaceLinear(int note, int nbDiv, int etalement, int occupation, int offset, int sens, bool isAscending);
    double spaceCurve(int note, int nbDiv, int etalement, int occupation, int offset, int sens, int sens2, bool isHollow);
    void updateRenversements();
    void updateGraph();
    void computeData(QVector<double> &x, QVector<int> &y);

    static double getAxe(int note, int nbDiv, bool sens);

    Ui::ToolSoundSpatialization_gui *ui;
    bool _isInst;
    int _minKey, _maxKey;
};

#endif // TOOLSOUNDSPATIALIZATION_GUI_H
