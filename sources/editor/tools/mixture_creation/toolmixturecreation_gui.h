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

#ifndef TOOLMIXTURECREATION_GUI_H
#define TOOLMIXTURECREATION_GUI_H

#include "abstracttoolgui.h"
#include "toolmixturecreation_parameters.h"

namespace Ui {
class ToolMixtureCreation_gui;
}

class ToolMixtureCreation_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolMixtureCreation_gui(QWidget *parent = nullptr);
    ~ToolMixtureCreation_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushAddDiv_pressed();
    void on_pushRemoveDiv_pressed();
    void on_spinDivStart_valueChanged(int arg1);
    void on_spinDivEnd_valueChanged(int arg1);
    void on_pushAddRank_clicked();
    void on_pushRemoveRank_clicked();
    void on_comboType1_currentIndexChanged(int index);
    void on_comboType2_currentIndexChanged(int index);
    void on_listDivisions_itemSelectionChanged();
    void on_listRangs_itemSelectionChanged();

private:
    QList<QList<int> > getStoredRanks();
    void storeRanks(QList<QList<int> > val);
    void dispDiv();
    void dispRang(int numDiv = -1);
    static QString getText(int overtoneType, int octave);
    int getNumDiv();
    int getNumRang();
    void dispType2(int index);

    Ui::ToolMixtureCreation_gui *ui;
    QList<DivisionInfo> _divisions;
};

#endif // TOOLMIXTURECREATION_GUI_H
