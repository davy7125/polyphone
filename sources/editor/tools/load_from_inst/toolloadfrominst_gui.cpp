/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "toolloadfrominst_gui.h"
#include "ui_toolloadfrominst_gui.h"
#include "toolloadfrominst_parameters.h"

ToolLoadFromInst_gui::ToolLoadFromInst_gui(QWidget *parent) :
    AbstractToolGui(parent),
    ui(new Ui::ToolLoadFromInst_gui)
{
    ui->setupUi(this);
}

ToolLoadFromInst_gui::~ToolLoadFromInst_gui()
{
    delete ui;
}

void ToolLoadFromInst_gui::updateInterface(AbstractToolParameters * parameters, IdList ids)
{
    Q_UNUSED(ids)
    ToolLoadFromInst_parameters * params = dynamic_cast<ToolLoadFromInst_parameters *>(parameters);

    // Parameter
    switch (params->getParameter())
    {
    case 1:
        ui->radioTuningSemiTones->setChecked(true);
        break;
    case 2:
        ui->radioTuningCents->setChecked(true);
        break;
    case 3:
        ui->radioLoop->setChecked(true);
        break;
    default:
        ui->radioRootKey->setChecked(true);
        break;
    }
}

void ToolLoadFromInst_gui::saveParameters(AbstractToolParameters * parameters)
{
    ToolLoadFromInst_parameters * params = dynamic_cast<ToolLoadFromInst_parameters *>(parameters);

    // Parameter
    if (ui->radioTuningSemiTones->isChecked())
        params->setParameter(1);
    else if (ui->radioTuningCents->isChecked())
        params->setParameter(2);
    else if (ui->radioLoop->isChecked())
        params->setParameter(3);
    else
        params->setParameter(0);
}

void ToolLoadFromInst_gui::on_pushOk_clicked()
{
    emit(this->validated());
}

void ToolLoadFromInst_gui::on_pushCancel_clicked()
{
    emit(this->canceled());
}
