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

#ifndef TOOLDIVISIONDUPLICATION_GUI_H
#define TOOLDIVISIONDUPLICATION_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolDivisionDuplication_gui;
}

class ToolDivisionDuplication_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolDivisionDuplication_gui(QWidget *parent = nullptr);
    ~ToolDivisionDuplication_gui();

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_pushAdd_clicked();
    void on_pushRemove_clicked();
    void on_spinMinVel_valueChanged(int arg1);
    void on_spinMaxVel_valueChanged(int arg1);
    void on_checkForEachVelocityRange_clicked();
    void on_listVelocites_currentRowChanged(int currentRow);

private:
    void dispVel();
    void storeVelocities(QVector<QPair<int, int> > val);

    Ui::ToolDivisionDuplication_gui *ui;
    bool _isInst;
    QList<QPair<int, int> > _listeVelocites;
};

#endif // TOOLDIVISIONDUPLICATION_GUI_H
