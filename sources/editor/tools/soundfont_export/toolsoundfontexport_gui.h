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

#ifndef TOOLSOUNDFONTEXPORT_GUI_H
#define TOOLSOUNDFONTEXPORT_GUI_H

#include "abstracttoolgui.h"

namespace Ui {
class ToolSoundfontExport_gui;
}

class ToolSoundfontExport_gui : public AbstractToolGui
{
    Q_OBJECT

public:
    explicit ToolSoundfontExport_gui(QWidget *parent = nullptr);
    ~ToolSoundfontExport_gui() override;

    /// Update the interface with the parameters
    void updateInterface(AbstractToolParameters * parameters, IdList ids) override;

    /// Save the parameters based on the interface
    void saveParameters(AbstractToolParameters * parameters) override;

private slots:
    void on_pushTick_clicked();
    void on_pushUntick_clicked();
    void on_pushFolder_clicked();
    void on_pushAnnuler_clicked();
    void on_pushExport_clicked();
    void on_comboFormat_currentIndexChanged(int index);

private:
    Ui::ToolSoundfontExport_gui *ui;
    QMap<int, QList<int> > _presetList;
};

#endif // TOOLSOUNDFONTEXPORT_GUI_H
